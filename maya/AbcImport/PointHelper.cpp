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
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnParticleSystem.h>
#include <maya/MDagModifier.h>
#include <maya/MItDependencyNodes.h>
#include <map>

PointsSampleData getSampleInfo( size_t particleNumber, unsigned int sampleSize, int sampleExtent, std::string sampleName )
{
	PointsSampleData out;

	// If sample size doesn't match our particle number, the sample is not valid
	out.isValidSample = false;
	out.extent = sampleExtent;
	out.name = sampleName;

	if (sampleSize == 1)
		out.isValidSample = true;

	DISPLAY_INFO("\tsampleSize; " << sampleSize);

	if ( sampleSize == particleNumber )
	{
		out.isValidSample = true;
	}
	else if ( sampleExtent != 2 && sampleSize == particleNumber * 2 )
	{
		// We are certainly dealing with a 2d array, we will assume a ababababab ordering
		DISPLAY_INFO("\tsampleSize is 2 times particle Size, extent is 2");
		out.extent = 2;
		out.isValidSample = true;
	}
	else if ( sampleExtent != 3 && sampleSize == particleNumber * 3 )
	{
		// We are certainly dealing with a 3d array, we will assume a abcabcabc ordering
		DISPLAY_INFO("\tsampleSize is 3 times particle Size, extent is 3");
		out.extent = 3;
		out.isValidSample = true;
	}

	else if (sampleExtent > 3)
	{
		// can't deal with that
		out.isValidSample = false;
	}

	return out;
}


MStatus getPointArbGeomParamsInfos( const Alembic::AbcGeom::IPoints & iNode, MObject & iObject,
		PointSampleDataList & iData )
{
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

		DISPLAY_INFO( "##\n" << propName << ": Comparing at index: " << compareSampleSelector.getRequestedIndex() << " with particle size: " << particleSize );

		Alembic::AbcCoreAbstract::ArraySamplePtr samp;

		arrayProp.get(samp, compareSampleSelector);
			unsigned int sampleSize = (unsigned int)samp->size();

		PointsSampleData sampleInfo = getSampleInfo(particleSize, sampleSize, propExtent, propName);
		DISPLAY_INFO("\tCreating new PointsSampleData:");
		DISPLAY_INFO("\t\t abc sampleSize: " << sampleSize);
		DISPLAY_INFO("\t\t abc extent: " << propExtent);
		DISPLAY_INFO("\t\t sampleInfo.attributeName: " << sampleInfo.name);
		DISPLAY_INFO("\t\t sampleInfo.extent: " << sampleInfo.extent);
		DISPLAY_INFO("\t\t sampleInfo.isValidSample: " << sampleInfo.isValidSample);

		MFnDependencyNode fnparticle(iObject);
		if ( fnparticle.hasAttribute( sampleInfo.name.c_str() ) )
		{

			// Skip attribute Creation
			DISPLAY_INFO( "\tAttribute " << sampleInfo.name << " already exists" );

			MPlug attrPlug = fnparticle.findPlug(sampleInfo.name.c_str());
			MFnAttribute attr( attrPlug.attribute(&status));

			if ( sampleInfo.extent == 1 &&  attr.accepts(MFnData::kDoubleArray, &status))
			{
				// We can skip attribute creation
				DISPLAY_INFO("\tAdd sample to iData[" << propName.c_str() << "]:");
				iData[propName.c_str()] = sampleInfo;
				continue;
			}
			else if (attr.accepts(MFnData::kVectorArray, &status))
			{
				// We can skip attribute creation
				DISPLAY_INFO("\tAdd sample to iData[" << propName.c_str() << "]:");
				iData[propName.c_str()] = sampleInfo;
				continue;
			}
			else
			{
				// Attribute exists but is of the wrong type, we need to rename it
				std::string abcPrefix("abc_");
				sampleInfo.name = abcPrefix + sampleInfo.name;
			}
		}
		iData[propName.c_str()] = sampleInfo;
	}
	return status;
}

MStatus readArbGeomParams(Alembic::AbcCoreAbstract::index_t index, Alembic::AbcCoreAbstract::index_t ceilIndex,
		double alpha, size_t pSize, const Alembic::Abc::ICompoundProperty & props,
		MFnArrayAttrsData & dynDataFn, PointSampleDataList & iData)
{
	MStatus status(MS::kSuccess);

	size_t numProps = props.getNumProperties();
	DISPLAY_INFO( "Found " << numProps << " readArbGeomParams" );
	DISPLAY_INFO( "particleCount: " << pSize );

	DISPLAY_INFO("iData has: " << iData.size() << " items");
	for (unsigned int i = 0; i < numProps; ++ i)
	{
		const Alembic::Abc::PropertyHeader & propHeader =
				props.getPropertyHeader(i);
		const std::string propName =  propHeader.getName();

		if (!propHeader.isArray())
			// we are dealing with PP attributes,they must be arrays
			continue;

		Alembic::Abc::IArrayProperty arrayProp( props, propName );

		Alembic::AbcCoreAbstract::ArraySamplePtr samp;

		arrayProp.get(samp, Alembic::Abc::ISampleSelector(index));
			unsigned int sampSize = (unsigned int)samp->size();

		Alembic::AbcCoreAbstract::DataType dtype = arrayProp.getDataType();
		int extent = (int)dtype.getExtent();
		Alembic::Util::PlainOldDataType pod = dtype.getPod();
		std::string interp = arrayProp.getMetaData().get("interpretation");

		DISPLAY_INFO("\tgetting sample info from iData[" << propName.c_str() << "]")

		DISPLAY_INFO("\tTesting for iData Validity");
		if ( iData.empty() )
		{
			DISPLAY_INFO("\t\tiData is empty, skipping");
			continue;
		}
		if ( iData.find(propName.c_str()) == iData.end() )
		{
			DISPLAY_INFO("\t\tiData[ " << propName.c_str() << "] not found, skipping");
			continue;
		}
		DISPLAY_INFO("\tEverything looks fine");

		PointsSampleData sampleInfo = iData[ propName.c_str() ];

		DISPLAY_INFO( " Current Prop: [" << propName << "]: "  << ", has data: " << dtype );
		DISPLAY_INFO( "\t\t interpretation: " << interp );
		DISPLAY_INFO( "\t\t sampleSize: " << sampSize );
		DISPLAY_INFO( "\t\t orig extent: " << extent );
		DISPLAY_INFO( "\t\t POD: " << Alembic::Util::PODName(pod) );
		DISPLAY_INFO( "\t sampleInfo[" << propName << "]");
		DISPLAY_INFO( "\t\t\t attributeName: " << sampleInfo.name );
		DISPLAY_INFO( "\t\t\t extent: " << sampleInfo.extent );
		DISPLAY_INFO( "\t\t\t isValid: " << sampleInfo.isValidSample );


		if ( ! sampleInfo.isValidSample )
		{
			DISPLAY_INFO("Sample is not valid");
			continue;
		}

		// We will cast everything to double
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
				// Single value
				if (sampSize == 1)
				{
					DISPLAY_INFO( "\t\t\tfloat with single value" );
					MDoubleArray arr = dynDataFn.doubleArray( sampleInfo.name.c_str(), &status);

					DISPLAY_INFO( "\t\t\t Setting particle attribute: " << propName << " with 1 double value" );

					arr.append( ((double *) samp->getData())[0] );
					continue;
				}
				// 1d array
				else if ( sampleInfo.extent == 1 )
				{
					DISPLAY_INFO( "\t\t\tfloat with extent 1 and no fake extent" );
					MDoubleArray doubleArray = dynDataFn.doubleArray( sampleInfo.name.c_str(), &status);

					DISPLAY_INFO( "\t\t\t Setting particle attribute: " << propName << " with double array value" );
					MDoubleArray arr((double *) samp->getData(),
						static_cast<unsigned int>(samp->size()));

					doubleArray = arr;
					continue;
				}
				else // extent is 2 or 3
				{
					DISPLAY_INFO( "\t\t\tfloat with extent: " << extent );

					MVectorArray arr = dynDataFn.vectorArray( sampleInfo.name.c_str(), &status);

					DISPLAY_INFO( "\t\t\t Setting particle attribute: " << propName << " with vector array value" );

					arr.setLength(pSize);

					MVector vec;

					double * vals = (double *) samp->getData();
					for (unsigned int i = 0; i < pSize; ++i)
					{
						vec.x = vals[extent*i];
						vec.y = vals[extent*i+1];

						if (extent == 3)
						{
							vec.z = vals[extent*i+2];
						}
						arr[i] = vec;
					}
					continue;
				}
			} break;
			case Alembic::Util::kStringPOD:
			{
				// ?
			} break;
		}
	}
	return status;
}

MStatus read(double iFrame, const Alembic::AbcGeom::IPoints & iNode,
	MFnArrayAttrsData & dynDataFn, PointSampleDataList & iData)
{
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
    DISPLAY_INFO( "\t\t pSize: " << samp.getPositions()->size() );
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
		DISPLAY_INFO("Width Param is valid");
		Alembic::AbcGeom::IFloatGeomParam::Sample widthSamp;
		widthProp.getExpanded(widthSamp, index);
		radiusArray = dynDataFn.doubleArray("radiusPP", &status);
		radiusArray.setLength(pSize);
		fptr = widthSamp.getVals();
	}

	MVector vec;

	// All other items have can be done in one loop
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

	DISPLAY_INFO(" dynDataFn.count: " << dynDataFn.count()) ;

	Alembic::Abc::ICompoundProperty props = schema.getArbGeomParams();

	if ( !props.valid() )
	{
		DISPLAY_INFO("ArbGeomParams is not valid, skip attribute reading");
		return status;
	}
	status = readArbGeomParams( index, ceilIndex, alpha, pSize, props, dynDataFn, iData );
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


	// We need to create a custom attribute for each abcGeomParam found under the PointSchema
    // nParticle object can only deal with doubleArray or vectorArray, so the only thing we need to know is the extent
    // But because sometimes the writer does not specify it explicitly, we need to investigate the header
	// we compare the sample position size to any geomParam sample size, if is equal to a twice or three time the position size,
	// we assume having an extent of 2 or 3

    // Now connect the nParticle to an existing or a new nucleus node
    DISPLAY_INFO("Add Partile to nSolver");

    MItDependencyNodes dgIt( MFn::kNucleus );
    MObject obj;
    MFnDagNode nParticleFn( iObject );
    while ( !dgIt.isDone() )
    {
        obj = dgIt.thisNode();
        dgIt.next();
    }
    MString nSolver("\"\"");
    if (!obj.isNull() && obj.hasFn( MFn::kNucleus))
    {
		MFnDependencyNode nucleusFn(obj);
		nSolver = nucleusFn.name();
    }

    DISPLAY_INFO("nSolver is : " << nSolver );

    MString cmd;
    cmd += "select ";
	cmd += nParticleFn.fullPathName();
	cmd += ";\n";

    cmd += "assignNSolver ";
	cmd += nSolver;
	cmd += ";\n";

    cmd += "select -clear;";

    DISPLAY_INFO("executingCommand:\n" << cmd);
    MGlobal::executeCommand( cmd, true, false );
    MCHECKERROR(status);

    // Handle radius
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

		// Set particle shading to blobby surface to show radius
		MPlug renderTypePlug = MFnDependencyNode(iObject).findPlug("particleRenderType");
		status = modifier.newPlugValueInt(renderTypePlug, 7);
		status = modifier.doIt();
		MCHECKERROR(status);
	}


    PointSampleDataList iData;

	status = getPointArbGeomParamsInfos(iNode, iObject, iData);


	size_t numProps = iData.size();
	DISPLAY_INFO("Found " << numProps << " valid arbGeomProperties for nParticles");

	if ( numProps == 0 )
	{
		DISPLAY_INFO("No param found, skipping");
		return status;
	}

    Alembic::Abc::ICompoundProperty props = schema.getArbGeomParams();

    for (unsigned int i = 0; i < numProps; ++ i)
	{
		const Alembic::Abc::PropertyHeader & propHeader =
				props.getPropertyHeader(i);
		const std::string propName =  propHeader.getName();

		const PointsSampleData & sampleInfo = iData[ propName.c_str() ];

		MFnDependencyNode fnparticle(iObject);
		if ( fnparticle.hasAttribute( sampleInfo.name.c_str() ) )
		{

			// Skip attribute Creation
			DISPLAY_INFO( "\tAttribute " << sampleInfo.name << " already exists" );
//			continue;

//			DISPLAY_INFO( "\tAttr " << propName << " already exists" );
			MPlug attrPlug = fnparticle.findPlug(sampleInfo.name.c_str());
			MFnAttribute attr( attrPlug.attribute(&status));

			if ( sampleInfo.extent == 1 &&  attr.accepts(MFnData::kDoubleArray, &status))
			{
				// We can skip attribute creation
				DISPLAY_INFO("\tAdd sample to iData[" << propName.c_str() << "]:");
				continue;
			}
			else if (attr.accepts(MFnData::kVectorArray, &status))
			{
				// We can skip attribute creation
				DISPLAY_INFO("\tAdd sample to iData[" << propName.c_str() << "]:");
				continue;
			}
			else
			{
				// Attribute exists but is of the wrong type, we need to create a new one
//				std::string abcPrefix("abc_");
//				sampleInfo.name = abcPrefix + sampleInfo.name;
				continue;
			}
		}

		if (sampleInfo.extent == 1)
		{
			DISPLAY_INFO( "\tCreating attribute: " << sampleInfo.name << " (orig: " << propName << ")" );
			DISPLAY_INFO( "\twith type: kDoubleArray" );
			attrObj = tAttr.create(sampleInfo.name.c_str(), sampleInfo.name.c_str(), MFnData::kDoubleArray, &status);
			MCHECKERROR(status);
			status = modifier.addAttribute(iObject, attrObj);
			MCHECKERROR(status);
		}
		else
		{
			DISPLAY_INFO( "\tCreating attribute: " << sampleInfo.name << " (orig: " << propName << ")" );
			DISPLAY_INFO( "\twith type: kVectorArray" );
			attrObj = tAttr.create(sampleInfo.name.c_str(), sampleInfo.name.c_str(), MFnData::kVectorArray, &status);
			MCHECKERROR(status);
			status = modifier.addAttribute(iObject, attrObj);
			MCHECKERROR(status);
		}
	}

    status = modifier.doIt();

    return status;
}
