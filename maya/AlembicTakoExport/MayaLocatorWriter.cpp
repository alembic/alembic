//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include <AlembicTakoExport/MayaUtility.h>
#include <AlembicTakoExport/MayaLocatorWriter.h>

namespace Alembic {

MayaLocatorWriter::MayaLocatorWriter(
    float iFrame, MDagPath & iDag, TransformWriterPtr iParent,
    bool iWriteVisibility) :
    mIsAnimated(false), mLocatorDag(iDag)
{
    MStatus stat;

    MFnDagNode fnLocator(mLocatorDag);

    mWriter = GenericNodeWriterPtr(
        new Tako::GenericNodeWriter(fnLocator.name().asChar(), *iParent));

    MObject locator = iDag.node();
    if (util::isAnimated(locator))
    {
        fillLocatorProps(1);
        mIsAnimated = true;
    }
    else
    {
        fillLocatorProps(0);
        iFrame = FLT_MAX;
    }

    mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, *mWriter,
        fnLocator, iWriteVisibility));

    mWriter->write("locator");
}

void MayaLocatorWriter::setter(int iType, const char * iName,
    Tako::PropertyPair & iProp)
{
    switch (iType)
    {
        case 0:
            mWriter->setNonSampledProperty(iName, iProp);
        break;
        case 1:
            mWriter->setSampledProperty(iName, iProp);
        break;
        default:
            mWriter->updateSample(iName, iProp);
        break;
    }
}

void MayaLocatorWriter::fillLocatorProps(int iType)
{
    Tako::PropertyPair prop;

    MFnDagNode fnLocator(mLocatorDag);

    double px = fnLocator.findPlug("localPositionX").asDouble();
    double py = fnLocator.findPlug("localPositionY").asDouble();
    double pz = fnLocator.findPlug("localPositionZ").asDouble();
    std::vector <double> pos(3);
    pos[0] = px;
    pos[1] = py;
    pos[2] = pz;
    prop.first = pos;
    prop.second.inputType = Tako::ARBATTR_VECTOR3;
    setter(iType, "localPosition", prop);

    double sx = fnLocator.findPlug("localScaleX").asDouble();
    double sy = fnLocator.findPlug("localScaleY").asDouble();
    double sz = fnLocator.findPlug("localScaleZ").asDouble();
    std::vector <double> scale(3);
    scale[0] = sx;
    scale[1] = sy;
    scale[2] = sz;
    prop.first = scale;
    prop.second.inputType = Tako::ARBATTR_VECTOR3;
    setter(iType, "localScale", prop);
}

void MayaLocatorWriter::write(float iFrame)
{
    // only update if animated
    if (mIsAnimated)
        fillLocatorProps(2);

    mAttrs->write(iFrame);
}


bool MayaLocatorWriter::isAnimated() const
{
    return  mIsAnimated || (mAttrs != NULL && mAttrs->isAnimated());
}

} // End namespace Alembic
