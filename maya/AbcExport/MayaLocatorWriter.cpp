//-*****************************************************************************
//
// Copyright (c) 2009-2012,
//  Sony Pictures Imageworks Inc. and
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
// Industrial Light & Magic, nor the names of their contributors may be used
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

#include "MayaLocatorWriter.h"
#include "MayaUtility.h"

MayaLocatorWriter::MayaLocatorWriter(MDagPath & iDag,
    Alembic::Abc::OObject & iParent,
    Alembic::Util::uint32_t iTimeIndex,
    const JobArgs & iArgs) :
    mIsAnimated(false), mDagPath(iDag)
{
    MStatus status = MS::kSuccess;
    MFnDagNode fnLocator(mDagPath, &status);
    if ( !status )
    {
        MGlobal::displayError(
            "Failed to initialize MFnDagNode object for locator" );
    }

    MObject locator = iDag.node();

    MString name = fnLocator.name();
    name = util::stripNamespaces(name, iArgs.stripNamespace);

    mXform = Alembic::AbcGeom::OXform(iParent, name.asChar(), iTimeIndex);

    Alembic::Abc::OCompoundProperty cp = mXform.getProperties();

    // of type double[6]
    Alembic::AbcCoreAbstract::DataType dType(Alembic::Util::kFloat64POD, 6);

    MPlug posX = fnLocator.findPlug("localPositionX");
    MPlug posY = fnLocator.findPlug("localPositionY");
    MPlug posZ = fnLocator.findPlug("localPositionZ");
    MPlug pos = fnLocator.findPlug("localPosition");

    MPlug scaleX = fnLocator.findPlug("localScaleX");
    MPlug scaleY = fnLocator.findPlug("localScaleY");
    MPlug scaleZ = fnLocator.findPlug("localScaleZ");
    MPlug scale = fnLocator.findPlug("localScale");

    if ( iTimeIndex != 0 && (util::getSampledType(posX) != 0 ||
        util::getSampledType(posY) != 0 ||
        util::getSampledType(posZ) != 0 ||
        util::getSampledType(pos) != 0 ||
        util::getSampledType(scaleX) != 0 ||
        util::getSampledType(scaleY) != 0 ||
        util::getSampledType(scaleZ) != 0 ||
        util::getSampledType(scale) != 0) )
    {
        mIsAnimated = true;
        mSp = Alembic::Abc::OScalarProperty(cp,
            "locator", dType, iTimeIndex);
    }
    else
    {
        mSp = Alembic::Abc::OScalarProperty(cp,
            "locator", dType);
    }

    double val[6];
    val[0] = posX.asDouble();
    val[1] = posY.asDouble();
    val[2] = posZ.asDouble();
    val[3] = scaleX.asDouble();
    val[4] = scaleY.asDouble();
    val[5] = scaleZ.asDouble();

    if (!mIsAnimated || iArgs.setFirstAnimShape)
    {
        mSp.set(val);
    }

    Alembic::Abc::OCompoundProperty arbGeom;
    Alembic::Abc::OCompoundProperty userProps;
    if (AttributesWriter::hasAnyAttr(fnLocator, iArgs))
    {
        arbGeom = mXform.getSchema().getArbGeomParams();
        userProps = mXform.getSchema().getUserProperties();
    }

    mAttrs = AttributesWriterPtr(new AttributesWriter(arbGeom, userProps,
                                                      mXform, fnLocator,
                                                      iTimeIndex, iArgs, true));
}


MayaLocatorWriter::~MayaLocatorWriter()
{
    mSp.reset();
}

void MayaLocatorWriter::write()
{
    MStatus status = MS::kSuccess;
    MFnDagNode fnLocator(mDagPath, &status);
    if ( !status )
    {
        MGlobal::displayError(
            "Failed to initialize MFnDagNode object for locator" );
    }

    double val[6];
    val[0] = fnLocator.findPlug("localPositionX").asDouble();
    val[1] = fnLocator.findPlug("localPositionY").asDouble();
    val[2] = fnLocator.findPlug("localPositionZ").asDouble();
    val[3] = fnLocator.findPlug("localScaleX").asDouble();
    val[4] = fnLocator.findPlug("localScaleY").asDouble();
    val[5] = fnLocator.findPlug("localScaleZ").asDouble();

    mSp.set(val);
}

bool MayaLocatorWriter::isAnimated() const
{
    return mIsAnimated;
}
