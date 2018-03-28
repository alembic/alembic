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

bool getSampleInfo( size_t particleNumber, unsigned int sampleSize, int sampleExtent, std::string sampleName, PointsSampleData & out )
{
	// PerParticle attribute can only be as big as the number of particle
	// If sample size doesn't match our particle number, the sample is not valid
	// If the sample size is a perfect multiple of our particule number, we assume a mistake from the exporter

	bool isValidSample = false;
	out.extent = sampleExtent;
	out.origName = sampleName;
	out.name = sampleName;

	DISPLAY_INFO("\tsampleSize; " << sampleSize);

	if ( sampleSize == particleNumber )
	{
		isValidSample = true;
	}
	else if ( sampleSize == particleNumber * 2 )
	{
		// We are certainly dealing with a 2d array, we will assume ababababab ordering
		DISPLAY_INFO("\tsampleSize is 2 times particle Size, extent is 2");
		out.extent = 2;
		isValidSample = true;
	}
	else if ( sampleSize == particleNumber * 3 )
	{
		// We are certainly dealing with a 3d array, we will assume abcabcabc ordering
		DISPLAY_INFO("\tsampleSize is 3 times particle Size, extent is 3");
		out.extent = 3;
		isValidSample = true;
	}
	else if (sampleExtent > 3)
	{
		// can't deal with that
		isValidSample = false;
	}

	return isValidSample;
}


MStatus getPointArbGeomParamsInfos( const Alembic::AbcGeom::IPoints & iNode, MObject & iObject,
		PointSampleDataList & iData )
{
	DISPLAY_INFO("#######\n" << "getPointArbGeomParamsInfos()");
	MStatus status( MS::kSuccess);
	Alembic::AbcGeom::IPointsSchema schema = iNode.getSchema();
	Alembic::AbcGeom::IPointsSchema::Sample samp;

	Alembic::Abc::ICompoundProperty props = schema.getArbGeomParams();

	if ( !props.valid() )
	{
		DISPLAY_INFO("   ArbGeomParams is not valid, skip attribute reading");
		return status;
	}

	// We find the index where the particles are the most
	// I found that sometimes, the exporter doesn't specify the correct extent
	size_t particleSize(0);
	size_t sampleSize = schema.getNumSamples();
	Alembic::AbcCoreAbstract::index_t index;
	Alembic::Abc::ISampleSelector compareSampleSelector;
	DISPLAY_INFO("Starting looping over samples to find the index with the most particles");
	for ( index = 0 ; index < sampleSize; ++index )
	{
		Alembic::Abc::ISampleSelector curSampleSelector(index);
		schema.get( samp, curSampleSelector );
		size_t curSize = samp.getPositions()->size();
		if ( particleSize < curSize)
		{
			particleSize = curSize;
			compareSampleSelector = curSampleSelector;
		}
	};
	DISPLAY_INFO("particleSize: " << particleSize);
	DISPLAY_INFO("compareSampleSelector: " << compareSampleSelector.getRequestedIndex());

	size_t numProps = props.getNumProperties();
	DISPLAY_INFO("Found " << numProps << " arbGeomProperties");
	MFnTypedAttribute tAttr;
	MObject attrObj;
	for (unsigned int i = 0; i < numProps; ++ i)
	{
		const Alembic::Abc::PropertyHeader & propHeader =
				props.getPropertyHeader(i);
		const std::string propName =  propHeader.getName();
		int propExtent = propHeader.getDataType().getExtent();

		Alembic::Abc::IArrayProperty arrayProp( props, propName );

		Alembic::AbcCoreAbstract::DataType dtype = arrayProp.getDataType();
			Alembic::Util::PlainOldDataType pod = dtype.getPod();

		DISPLAY_INFO( "##\n" << propName << ": Comparing at index: " << compareSampleSelector.getRequestedIndex() << " with particle size: " << particleSize );
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
				DISPLAY_INFO("\t attribute is not numerica, skipping");
				continue;
			}
		}


		DISPLAY_INFO( "##\n" << propName << ": Comparing at index: " << compareSampleSelector.getRequestedIndex() << " with particle size: " << particleSize );

		Alembic::AbcCoreAbstract::ArraySamplePtr samp;

		arrayProp.get(samp, compareSampleSelector);
			unsigned int sampleSize = (unsigned int)samp->size();

		PointsSampleData sampleInfo;
		if ( !getSampleInfo(particleSize, sampleSize, propExtent, propName, sampleInfo) )
			continue;

		sampleInfo.arrayProp = arrayProp;

		DISPLAY_INFO("\tCreating new PointsSampleData:");
		DISPLAY_INFO("\t\t abc sampleSize: " << sampleSize);
		DISPLAY_INFO("\t\t abc extent: " << propExtent);
		DISPLAY_INFO("\t\t sampleInfo.origName: " << sampleInfo.origName);
		DISPLAY_INFO("\t\t sampleInfo.attributeName: " << sampleInfo.name);
		DISPLAY_INFO("\t\t sampleInfo.extent: " << sampleInfo.extent);


		MFnDependencyNode fnparticle(iObject);
		if ( fnparticle.hasAttribute( sampleInfo.name.c_str() ) )
		{
			DISPLAY_INFO( "\tAttribute " << sampleInfo.name << " already exists" );

			MPlug attrPlug = fnparticle.findPlug(sampleInfo.name.c_str());
			MFnAttribute attr( attrPlug.attribute(&status));

			if ( sampleInfo.extent == 1 &&  attr.accepts(MFnData::kDoubleArray, &status))
			{
				// We can skip attribute creation
				DISPLAY_INFO("\tAttribute has the correct data: extent 1, kDoubleArray");
				iData.push_back( sampleInfo );
				continue;
			}
			else if (attr.accepts(MFnData::kVectorArray, &status))
			{
				// We can skip attribute creation
				DISPLAY_INFO("\tAttribute has the correct data: extent 3, kVectorArray");
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

MStatus readArbGeomParams(Alembic::AbcCoreAbstract::index_t index,
		MFnArrayAttrsData & dynDataFn, PointSampleDataList & iData)
{
	MStatus status(MS::kSuccess);

	size_t numProps = iData.size();

	if ( iData.empty() )
	{
		DISPLAY_INFO("\tiData is empty, skipping");
		return status;
	}

	DISPLAY_INFO("iData has: " << iData.size() << " items");
	for (unsigned int i = 0; i < numProps; ++ i)
	{
		PointsSampleData sampleInfo = iData[ i ];

		Alembic::Util::Dimensions dim;
		DISPLAY_INFO("\t" << "Get array dimension");
		sampleInfo.arrayProp.getDimensions(dim, Alembic::Abc::ISampleSelector(index));

		unsigned int sampSize( dim.numPoints() );
		DISPLAY_INFO("\t" << "Dimensions.numPoints: " << sampSize);

		DISPLAY_INFO("\t initialise vector with: " << sampSize << " x " << sampleInfo.extent << " = " <<  sampSize * sampleInfo.extent << " elements");
		// We don't forget the extent when alocating the array
		std::vector< double > samp(sampSize * sampleInfo.extent);

		// Read everything as double
		DISPLAY_INFO("\t" << "Get propData as kFloat64 and feed the std::vector");
		sampleInfo.arrayProp.getAs( &samp.front(), Alembic::Util::kFloat64POD, Alembic::Abc::ISampleSelector(index));

		DISPLAY_INFO("\tgetting sample info from iData[" << sampleInfo.origName.c_str() << "]")

		DISPLAY_INFO( "\t sampleInfo[" << sampleInfo.origName << "]");
		DISPLAY_INFO( "\t\t\t attributeName: " << sampleInfo.name );
		DISPLAY_INFO( "\t\t\t extent: " << sampleInfo.extent );
		DISPLAY_INFO( "\t\t\t sampleSize: " << sampSize );

		// Single value
		if (sampSize == 1)
		{
			DISPLAY_INFO( "\t\t\tfloat with single value" );
			MDoubleArray arr = dynDataFn.doubleArray( sampleInfo.name.c_str(), &status);

			DISPLAY_INFO( "\t\t\t Setting particle attribute: " << sampleInfo.origName << " with 1 double value" );

			arr.append( samp[0] );
			continue;
		}
		// 1d array
		else if ( sampleInfo.extent == 1 )
		{
			DISPLAY_INFO( "\t\t\tfloat with extent 1 and no fake extent" );
			MDoubleArray doubleArray = dynDataFn.doubleArray( sampleInfo.name.c_str(), &status);

			doubleArray.setLength( sampSize );

			uint count(0);
			for (unsigned int i = 0; i < sampSize; ++i )
			{
				doubleArray[i] = samp[i];
				count++;
			}
			DISPLAY_INFO("\t\t\t" << "looped over: " << count << " items");

			continue;
		}
		else // extent is 2 or 3
		{
			DISPLAY_INFO( "\t\t\tfloat with extent: " << sampleInfo.extent );

			MVectorArray arr = dynDataFn.vectorArray( sampleInfo.name.c_str(), &status);

			DISPLAY_INFO( "\t\t\t Setting particle attribute: " << sampleInfo.origName << " with " << sampSize << " vector array value" );

			arr.setLength(sampSize);

			MVector vec;

			uint count(0);
			for (unsigned int i = 0; i < sampSize; ++i)
			{
				vec.x = samp[sampleInfo.extent*i];
				vec.y = samp[sampleInfo.extent*i+1];

				if (sampleInfo.extent == 3)
				{
					vec.z = samp[sampleInfo.extent*i+2];
				}
				arr[i] = vec;
				count++;
			}
			DISPLAY_INFO("\t\t\t" << "looped over: " << count << " items");

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
		DISPLAY_INFO( "\tCreating attribute: radiusPP" );
		DISPLAY_INFO( "\twith type: kDoubleArray" );
		attrObj = tAttr.create("radiusPP", "radiusPP", MFnData::kDoubleArray, &status);
		MCHECKERROR(status);
		status = modifier.addAttribute(iObject, attrObj);
		status = modifier.doIt();
		MCHECKERROR(status);

		// particleRenderType is an enum, 7 is for blobby surface
		MPlug renderTypePlug = MFnDependencyNode(iObject).findPlug("particleRenderType");
		status = modifier.newPlugValueInt(renderTypePlug, 7);
		status = modifier.doIt();
		MCHECKERROR(status);
	}

	PointSampleDataList iData;
	status = getPointArbGeomParamsInfos(iNode, iObject, iData);

	size_t numValidProps = iData.size();
	DISPLAY_INFO("Found " << numValidProps << " valid arbGeomProperties for nParticles");

	if ( numValidProps == 0 )
	{
		DISPLAY_INFO("No param found, skipping");
		return status;
	}


	DISPLAY_INFO( "Looping over iData:" );
	for (unsigned int i = 0; i < numValidProps; ++ i)
	{
		PointsSampleData & sampleInfo = iData[ i ];
		DISPLAY_INFO("\t" << "currentAttr: " << sampleInfo.origName << ", " << sampleInfo.name);

		if ( MFnDependencyNode(iObject).hasAttribute(sampleInfo.name.c_str() ))
		{
			DISPLAY_INFO("\t attribute " << sampleInfo.name << " already exists");
			continue;
		}

		if (sampleInfo.extent == 1)
		{
			DISPLAY_INFO( "\t Creating attribute: " << sampleInfo.name << " (orig: " << sampleInfo.origName << ")" );
			DISPLAY_INFO( "\t with type: kDoubleArray" );
			attrObj = tAttr.create(sampleInfo.name.c_str(), sampleInfo.name.c_str(), MFnData::kDoubleArray, &status);
			MCHECKERROR(status);
			status = modifier.addAttribute(iObject, attrObj);
			MCHECKERROR(status);
		}
		else
		{
			DISPLAY_INFO( "\t Creating attribute: " << sampleInfo.name << " (orig: " << sampleInfo.origName << ")" );
			DISPLAY_INFO( "\t with type: kVectorArray" );
			attrObj = tAttr.create(sampleInfo.name.c_str(), sampleInfo.name.c_str(), MFnData::kVectorArray, &status);
			MCHECKERROR(status);
			status = modifier.addAttribute(iObject, attrObj);
			MCHECKERROR(status);
		}
	}

	return modifier.doIt();
}

MStatus read(double iFrame, const Alembic::AbcGeom::IPoints & iNode,
	MFnArrayAttrsData & dynDataFn, PointSampleDataList & iData)
{
	// We feed the MFnArrayAttrsData with all the nescessary doubleArray and vectorArray
	// it is then used to feed the outDataPlug of the alembic node

	DISPLAY_INFO( "Reading Abc Data for frame: " << iFrame );

    MStatus status = MS::kSuccess;

    Alembic::AbcGeom::IPointsSchema schema = iNode.getSchema();

    DISPLAY_INFO( "\tSchema: " << schema.getName() );

    // Get IPoints Samples
	Alembic::AbcGeom::IPointsSchema::Sample samp, ceilsSamp;

		Alembic::AbcCoreAbstract::index_t index, ceilIndex;
			double alpha = getWeightAndIndex(iFrame, schema.getTimeSampling(),
					schema.getNumSamples(), index, ceilIndex);

	schema.get(samp, index);

	size_t pSize = samp.getPositions()->size();
	size_t idSize = samp.getIds()->size();
    DISPLAY_INFO( "\t\t pSize:  " << samp.getPositions()->size() );
    DISPLAY_INFO( "\t\t idSize: " << samp.getIds()->size() );

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
	if ( widthProp.valid() )
	{
// TODO, shouldn't width be 2*radius ?
		DISPLAY_INFO("Width Param is valid");
		Alembic::AbcGeom::IFloatGeomParam::Sample widthSamp;
		widthProp.getExpanded(widthSamp, index);
		radiusArray = dynDataFn.doubleArray("radiusPP", &status);
		radiusArray.setLength(pSize);
		fptr = widthSamp.getVals();
		DISPLAY_INFO( "\t\t widthSize: " << fptr->size() );
	}

	MVector vec;
	for (unsigned int i = 0; i < pSize; ++i )
	{
		idArray[i] = (*idPtr)[i];

		vec.x = (*p3ptr)[i].x;
		vec.y = (*p3ptr)[i].y;
		vec.z = (*p3ptr)[i].z;
		positionArray[i] = vec;

		vec.x = (*v3fptr)[i].x;
		vec.y = (*v3fptr)[i].y;
		vec.z = (*v3fptr)[i].z;
		velocityArray[i] = vec;

		if ( widthProp.valid() )
			radiusArray[i] = (*fptr)[i];
	}

	Alembic::Abc::ICompoundProperty props = schema.getArbGeomParams();
	if ( !props.valid() )
	{
		DISPLAY_INFO("ArbGeomParams is not valid, skip attribute reading");
		return status;
	}
	status = readArbGeomParams( index, dynDataFn, iData );
	MCHECKERROR(status);
	DISPLAY_INFO(" dynDataFn.count: " << dynDataFn.count()) ;

    return status;
}

MStatus create(double iFrame, const Alembic::AbcGeom::IPoints & iNode,
    MObject & iParent, MObject & iObject)
{
	MFnTypedAttribute tAttr;
	MObject attrObj;

    MStatus status = MS::kSuccess;
    Alembic::AbcGeom::IPointsSchema schema = iNode.getSchema();

    DISPLAY_INFO( " Create new Particle object" );

    // object has no samples, bail early
    if (schema.getNumSamples() == 0)
    {
        return status;
    }

    MDagModifier modifier;

    DISPLAY_INFO( " Create nParticle" );
    iObject = modifier.createNode("nParticle", iParent, &status);
    MCHECKERROR(status);
    status = modifier.renameNode(iObject, iNode.getName().c_str());
    DISPLAY_INFO( " rename to " << iNode.getName().c_str() );

    status = modifier.doIt();

    // To call the DG evaluation, the nParticle needs to be attached to a nucleus node
    DISPLAY_INFO("Adding Particle to nSolver");

    MItDependencyNodes dgIt( MFn::kNucleus );
    MObject obj;
    MFnDagNode nParticleFn( iObject );
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

    DISPLAY_INFO("nSolver is : " << nSolver );

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
    DISPLAY_INFO("executingCommand:\n" << cmd);
    MGlobal::executeCommand( cmd, true, false );
    MCHECKERROR(status);

    // Assign default particle shader initialParticleSE to correctly display them in the viewport
    MSelectionList sel;
    sel.add( "initialParticleSE" );
    MObject particleSG;
    sel.getDependNode(0, particleSG);
    MFnSet fnSG( particleSG );
    fnSG.addMember(iObject);
    DISPLAY_INFO("assign SG: " << fnSG.name() << " to nParticle.");


/*    // Handle radius
    // IF we have information for the radius, set the shading to blobby surface
    Alembic::AbcGeom::IFloatGeomParam widthProp = schema.getWidthsParam();
	if ( widthProp.valid() )
	{
		DISPLAY_INFO( "\tCreating attribute: radiusPP" );
		DISPLAY_INFO( "\twith type: kDoubleArray" );
		attrObj = tAttr.create("radiusPP", "radiusPP", MFnData::kDoubleArray, &status);
		MCHECKERROR(status);
		status = modifier.addAttribute(iObject, attrObj);
		status = modifier.doIt();
		MCHECKERROR(status);

		// particleRenderType is an enum, 7 is for blobby surface
		MPlug renderTypePlug = MFnDependencyNode(iObject).findPlug("particleRenderType");
		status = modifier.newPlugValueInt(renderTypePlug, 7);
		status = modifier.doIt();
		MCHECKERROR(status);
	}*/

	status = createPerParticleAttributes(iNode, iObject);

	return status;
/*

	// We need to create a custom attribute for each abcGeomParam found under the PointSchema
    // nParticle object can only deal with doubleArray or vectorArray, so the only thing we need to know is the extent
    // But because sometimes the writer does not specify it explicitly, we need to investigate the header
	// we compare the sample position size to any geomParam sample size, if it is equal to twice or three time the position size,
	// we assume having an extent of 2 or 3
    PointSampleDataList iData;
	status = getPointArbGeomParamsInfos(iNode, iObject, iData);

	size_t numValidProps = iData.size();
	DISPLAY_INFO("Found " << numValidProps << " valid arbGeomProperties for nParticles");

	if ( numValidProps == 0 )
	{
		DISPLAY_INFO("No param found, skipping");
		return status;
	}

	DISPLAY_INFO( "Looping over iData:" );
    for (unsigned int i = 0; i < numValidProps; ++ i)
	{
		PointsSampleData & sampleInfo = iData[ i ];
		DISPLAY_INFO("\t" << "currentAttr: " << sampleInfo.origName << ", " << sampleInfo.name);

		if (sampleInfo.extent == 1)
		{
			DISPLAY_INFO( "\tCreating attribute: " << sampleInfo.name << " (orig: " << sampleInfo.origName << ")" );
			DISPLAY_INFO( "\twith type: kDoubleArray" );
			attrObj = tAttr.create(sampleInfo.name.c_str(), sampleInfo.name.c_str(), MFnData::kDoubleArray, &status);
			MCHECKERROR(status);
			status = modifier.addAttribute(iObject, attrObj);
			MCHECKERROR(status);
		}
		else
		{
			DISPLAY_INFO( "\tCreating attribute: " << sampleInfo.name << " (orig: " << sampleInfo.origName << ")" );
			DISPLAY_INFO( "\twith type: kVectorArray" );
			attrObj = tAttr.create(sampleInfo.name.c_str(), sampleInfo.name.c_str(), MFnData::kVectorArray, &status);
			MCHECKERROR(status);
			status = modifier.addAttribute(iObject, attrObj);
			MCHECKERROR(status);
		}
	}

    status = modifier.doIt();

    return status;
    */
}
