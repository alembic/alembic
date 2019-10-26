//-*****************************************************************************
//
// Copyright (c) 2009-2011,
//  Sony Pictures Imageworks, Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#include "util.h"
#include "PointHelper.h"
#include "NodeIteratorVisitorHelper.h"

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MGlobal.h>
#include <maya/MVectorArray.h>
#include <maya/MSelectionList.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnParticleSystem.h>
#include <maya/MDagModifier.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MFnSet.h>

MStatus getPointArbGeomParamsInfos( const Alembic::AbcGeom::IPoints & iNode, MObject & iObject,
        PointSampleDataList & iData )
{
    MStatus status( MS::kSuccess);
    Alembic::AbcGeom::IPointsSchema schema = iNode.getSchema();
    Alembic::AbcGeom::IPointsSchema::Sample samp;

    Alembic::Abc::ICompoundProperty props = schema.getArbGeomParams();

    if ( !props.valid() )
    {
        return status;
    }

    size_t numProps = props.getNumProperties();
    MFnTypedAttribute tAttr;
    MObject attrObj;
    for (unsigned int i = 0; i < numProps; ++ i)
    {
        const Alembic::Abc::PropertyHeader & propHeader =
                props.getPropertyHeader(i);
        const std::string propName =  propHeader.getName();
        int propExtent = propHeader.getDataType().getExtent();


        Alembic::Abc::IArrayProperty arrayProp( props, propName );

        // Get geometry scope
        Alembic::AbcGeom::GeometryScope propScope = Alembic::AbcGeom::GetGeometryScope(arrayProp.getMetaData());

        Alembic::AbcCoreAbstract::DataType dtype = arrayProp.getDataType();
            Alembic::Util::PlainOldDataType pod = dtype.getPod();

        // Ignore all but numerical
        switch (pod)
        {
            case Alembic::Util::kBooleanPOD:
            case Alembic::Util::kUint8POD:
            case Alembic::Util::kInt8POD:
            case Alembic::Util::kInt16POD:
            case Alembic::Util::kInt32POD:
            case Alembic::Util::kInt64POD:
            case Alembic::Util::kFloat16POD:
            case Alembic::Util::kFloat32POD:
            case Alembic::Util::kFloat64POD:
            {
                break;
            }
            default:
            {
                continue;
            }
        }

        Alembic::AbcCoreAbstract::ArraySamplePtr samp;


        // Storing property information for fast lookup when reading each frame
        PointsSampleData sampleInfo;
        sampleInfo.origName = propName;
        sampleInfo.scope = propScope;
        sampleInfo.name = propName;
        sampleInfo.extent = propExtent;
        sampleInfo.arrayProp = arrayProp;

        MFnDependencyNode fnparticle(iObject);
        if ( fnparticle.hasAttribute( sampleInfo.name.c_str() ) )
        {

            MPlug attrPlug = fnparticle.findPlug(sampleInfo.name.c_str(), true);
            MFnAttribute attr( attrPlug.attribute(&status));

            if ( sampleInfo.extent == 1 &&  attr.accepts(MFnData::kDoubleArray, &status))
            {
                // We can skip attribute creation
                iData.push_back( sampleInfo );
                continue;
            }
            else if (attr.accepts(MFnData::kVectorArray, &status))
            {
                // We can skip attribute creation
                iData.push_back( sampleInfo );
                continue;
            }
            else
            {
                // Attribute exists but is of the wrong type, we need to rename it
                std::string abcPrefix("abc_");
                sampleInfo.name = abcPrefix + sampleInfo.name;
            }
        }
        iData.push_back( sampleInfo );
    }
    return status;
}

MStatus readArbGeomParams(size_t pSize, Alembic::AbcCoreAbstract::index_t index,
        MFnArrayAttrsData & dynDataFn, PointSampleDataList & iData)
{
    MStatus status(MS::kSuccess);

    size_t numProps = iData.size();

    if ( iData.empty() || pSize == 0  )
    {
        // iData is empty, feeding empty array");
        return status;
    }

    for (unsigned int i = 0; i < numProps; ++ i)
    {
        PointsSampleData sampleInfo = iData[ i ];

        Alembic::Util::Dimensions dim;
        sampleInfo.arrayProp.getDimensions(dim, Alembic::Abc::ISampleSelector(index));

        unsigned int sampSize( dim.numPoints() );

        if ( sampleInfo.scope == Alembic::AbcGeom::kVaryingScope && sampSize != pSize)
        {
            MString warn = "Ignoring malformed kVarying attribute: ";
            warn += sampleInfo.name.c_str();
            warn += ", the number of point in the sample doesn't match the number of particle";
            MGlobal::displayWarning(warn);
            continue;
        }

        // We don't forget the extent when alocating the array
        std::vector< double > samp(pSize * sampleInfo.extent);

        // Read everything as double
        sampleInfo.arrayProp.getAs( &samp.front(), Alembic::Util::kFloat64POD, Alembic::Abc::ISampleSelector(index));

        // 1d array
        if ( sampleInfo.extent == 1 )
        {
            MDoubleArray doubleArray = dynDataFn.doubleArray( sampleInfo.name.c_str(), &status);

            doubleArray.setLength( pSize );

            if ( sampleInfo.scope <= Alembic::AbcGeom::kUniformScope)
            {
                for (unsigned int i = 0; i < pSize; ++i )
                {
                    doubleArray[i] = samp[0];
                }
            }
            else
            {
                for (unsigned int i = 0; i < pSize; ++i )
                {
                    doubleArray[i] = samp[i];
                }
            }

            continue;
        }
        else // 2d or 3d array
        {
            MVectorArray arr = dynDataFn.vectorArray( sampleInfo.name.c_str(), &status);

            arr.setLength(pSize);

            MVector vec;

            if ( sampleInfo.scope <= Alembic::AbcGeom::kUniformScope)
            {
                for (unsigned int i = 0; i < pSize; ++i)
                {
                    vec.x = samp[0];
                    vec.y = samp[1];

                    if (sampleInfo.extent == 3)
                    {
                        vec.z = samp[2];
                    }
                    arr[i] = vec;
                }
            }
            else
            {
                for (unsigned int i = 0; i < pSize; ++i)
                {
                    vec.x = samp[sampleInfo.extent*i];
                    vec.y = samp[sampleInfo.extent*i+1];

                    if (sampleInfo.extent == 3)
                    {
                        vec.z = samp[sampleInfo.extent*i+2];
                    }
                    arr[i] = vec;
                }
            }
            continue;
        }

    }
    return status;
}

MStatus createPerParticleAttributes( const Alembic::AbcGeom::IPoints & iNode, MObject & iObject )
{
    // We need to create a custom attribute for each abcGeomParam found under the PointSchema
    // nParticle object can only deal with doubleArray or vectorArray, so the only thing we need to know is the extent
    // But because sometimes the writer does not specify it explicitly, we need to investigate the header
    // we compare the sample position size to any geomParam sample size, if it is equal to twice or three time the position size,
    // we assume having an extent of 2 or 3

    // Initialise maya object
    MStatus status;
    MFnTypedAttribute tAttr;
    MDagModifier modifier;
    MObject attrObj;

    // Handle radius
    // IF we have information for the radius, set the shading to blobby surface
    Alembic::AbcGeom::IFloatGeomParam widthProp = iNode.getSchema().getWidthsParam();
    if ( widthProp.valid() )
    {
        attrObj = tAttr.create("radiusPP", "radiusPP", MFnData::kDoubleArray,
                               MObject::kNullObj, &status);
        MCHECKERROR(status);
        status = modifier.addAttribute(iObject, attrObj);
        status = modifier.doIt();
        MCHECKERROR(status);

        // particleRenderType is an enum, 7 is for blobby surface
        MPlug renderTypePlug = MFnDependencyNode(iObject).findPlug("particleRenderType", true);
        status = modifier.newPlugValueInt(renderTypePlug, 7);
        status = modifier.doIt();
        MCHECKERROR(status);
    }

    PointSampleDataList iData;
    status = getPointArbGeomParamsInfos(iNode, iObject, iData);

    size_t numValidProps = iData.size();

    if ( numValidProps == 0 )
    {
        // No param found, skipping
        return status;
    }

    for (unsigned int i = 0; i < numValidProps; ++ i)
    {
        PointsSampleData & sampleInfo = iData[ i ];

        if ( MFnDependencyNode(iObject).hasAttribute(sampleInfo.name.c_str() ))
        {
            continue;
        }

        if (sampleInfo.extent == 1)
        {
            attrObj = tAttr.create(sampleInfo.name.c_str(), sampleInfo.name.c_str(),
                                   MFnData::kDoubleArray, MObject::kNullObj, &status);
            MCHECKERROR(status);
            status = modifier.addAttribute(iObject, attrObj);
            MCHECKERROR(status);
        }
        else
        {
            attrObj = tAttr.create(sampleInfo.name.c_str(), sampleInfo.name.c_str(),
                                   MFnData::kVectorArray, MObject::kNullObj, &status);
            MCHECKERROR(status);
            status = modifier.addAttribute(iObject, attrObj);
            MCHECKERROR(status);
        }
    }

    return modifier.doIt();
}

MStatus read(double iFrame, const Alembic::AbcGeom::IPoints & iNode,  int & isInitializedConstant,
    MFnArrayAttrsData & dynDataFn, PointSampleDataList & iData)
{
    // We feed the MFnArrayAttrsData with all the nescessary doubleArray and vectorArray
    // it is then used to feed the outDataPlug of the alembic node
    MStatus status = MS::kSuccess;

    Alembic::AbcGeom::IPointsSchema schema = iNode.getSchema();

    // Get IPoints Samples
    Alembic::AbcGeom::IPointsSchema::Sample samp, ceilsSamp;

    Alembic::AbcCoreAbstract::index_t index, ceilIndex;
    // TODO, might want to interpolate the points when we can
    getWeightAndIndex(iFrame, schema.getTimeSampling(),
        schema.getNumSamples(), index, ceilIndex);

    schema.get(samp, index);

    size_t pSize = samp.getPositions()->size();
    //size_t idSize = samp.getIds()->size();

    MDoubleArray countArray = dynDataFn.doubleArray("count", &status);
    countArray.append( (double)pSize );

    // ID
    Alembic::Abc::UInt64ArraySamplePtr idPtr = samp.getIds();
    MDoubleArray idArray = dynDataFn.doubleArray("id", &status);
    idArray.setLength( pSize );

    // POSITION
    Alembic::Abc::P3fArraySamplePtr p3ptr = samp.getPositions();
    MVectorArray positionArray = dynDataFn.vectorArray("position", &status);
    positionArray.setLength( pSize );

    // VELOCITY
    Alembic::Abc::V3fArraySamplePtr v3fptr= samp.getVelocities();
    MVectorArray velocityArray = dynDataFn.vectorArray("velocity", &status);
    velocityArray.setLength( pSize );

    // RADIUS
    Alembic::AbcGeom::IFloatGeomParam widthProp = schema.getWidthsParam();
    Alembic::Abc::FloatArraySamplePtr fptr;
    MDoubleArray radiusArray;
    Alembic::AbcGeom::GeometryScope widthScope( Alembic::AbcGeom::kUnknownScope );
    if ( widthProp.valid() )
    {
// TODO, shouldn't width be 2*radius ?

        Alembic::AbcGeom::IFloatGeomParam::Sample widthSamp;
        widthProp.getExpanded(widthSamp, index);
        radiusArray = dynDataFn.doubleArray("radiusPP", &status);
        radiusArray.setLength(pSize);
        fptr = widthSamp.getVals();
        widthScope = widthSamp.getScope();
    }

    MVector vec;
    for (unsigned int i = 0; i < pSize; ++i )
    {
        idArray[i] = (*idPtr)[i];

        vec.x = (*p3ptr)[i].x;
        vec.y = (*p3ptr)[i].y;
        vec.z = (*p3ptr)[i].z;
        positionArray[i] = vec;

        if ( v3fptr )
        {
            vec.x = (*v3fptr)[i].x;
            vec.y = (*v3fptr)[i].y;
            vec.z = (*v3fptr)[i].z;
            velocityArray[i] = vec;
        }

        if ( widthProp.valid() )
        {
            if ( widthScope < Alembic::AbcGeom::kVaryingScope )
            {
                // Special Case, we got only one value, populate all particles with this value
                radiusArray[i] = (*fptr)[0];
            }
            else
            {
                radiusArray[i] = (*fptr)[i];
            }
        }
    }

    if ( schema.isConstant() )
   {
        isInitializedConstant = true;
   }

    Alembic::Abc::ICompoundProperty props = schema.getArbGeomParams();
    if ( !props.valid() )
    {
        return status;
    }
    status = readArbGeomParams( pSize, index, dynDataFn, iData );
    MCHECKERROR(status);

    return status;
}

MStatus create(double iFrame, const Alembic::AbcGeom::IPoints & iNode,
    MObject & iParent, MObject & iObject)
{
    MFnTypedAttribute tAttr;
    MObject attrObj;

    MStatus status = MS::kSuccess;
    Alembic::AbcGeom::IPointsSchema schema = iNode.getSchema();

    // object has no samples, bail early
    if (schema.getNumSamples() == 0)
    {
        return status;
    }

    MDagModifier modifier;

    iObject = modifier.createNode("nParticle", iParent, &status);
    MCHECKERROR(status);
    status = modifier.renameNode(iObject, iNode.getName().c_str());

    status = modifier.doIt();

    // To call the DG evaluation, the nParticle needs to be attached to a nucleus node
    MItDependencyNodes dgIt( MFn::kNucleus );
    MObject obj;
    MFnDagNode nParticleFn( iObject );

    // Look for a nucleus node
    while ( !dgIt.isDone() )
    {
        obj = dgIt.thisNode();
        break;
    }
    MString nSolver("\"\"");
    if (!obj.isNull() && obj.hasFn( MFn::kNucleus))
    {
        MFnDependencyNode nucleusFn(obj);
        nSolver = nucleusFn.name();
    }

    // I found no way to do this from the API, so we call a dirty simple mel command that need to have an active selection
    MString cmd;
    cmd += "select ";
    cmd += nParticleFn.fullPathName();
    cmd += ";\n";
    cmd += "assignNSolver ";
    cmd += nSolver;
    cmd += ";\n";

    cmd += "select -clear;";

    // If there is no nSolver in the scene, the nSolver string will be empty and it will trigger the creation
    // of a new one
    MGlobal::executeCommand( cmd, true, false );
    MCHECKERROR(status);

    // Set attribute "isDynamic" to off, it could crash if maya tries to compute collision
    // against another nParticleShape
    // It is not related to alembic. It crashes also with nCached particle collision
    // In maya Attribute Editor, the attribute is called "enable"
    MPlug enablePlug = nParticleFn.findPlug("isDynamic", true);
    status = modifier.newPlugValueBool(enablePlug, false);

    // Assign default particle shader initialParticleSE to correctly display them in the viewport
    MSelectionList sel;
    sel.add( "initialParticleSE" );
    MObject particleSG;
    sel.getDependNode(0, particleSG);
    MFnSet fnSG( particleSG );
    fnSG.addMember(iObject);

    status = createPerParticleAttributes(iNode, iObject);

    return status;
}
