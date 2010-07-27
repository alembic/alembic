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

#include <AlembicTakoSPIExport/MayaLightWriter.h>
#include <AlembicTakoSPIExport/MayaUtility.h>

namespace Alembic {

MayaLightWriter::MayaLightWriter(
    float iFrame, MDagPath & iDag, TransformWriterPtr iParent,
    bool iWriteVisibility) :
    mIsAnimated(false), mLightDag(iDag)
{
    MStatus stat;

    MFnLight fnLight(mLightDag);

    mWriter = iParent->createGenericNodeChild(fnLight.name().asChar());

    MObject light = iDag.node();
    if (util::isAnimated(light))
    {
        fillLightProps(1);
        mIsAnimated = true;
    }
    else
    {
        fillLightProps(0);
        iFrame = FLT_MAX;
    }

    mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, *mWriter,
        fnLight, iWriteVisibility));

    mWriter->write("light");
}

void MayaLightWriter::setter(int iType, const char * iName,
    TakoSPI::PropertyPair & iProp)
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

void MayaLightWriter::fillLightProps(int iType)
{
    if (mLightDag.hasFn(MFn::kLight))
    {
        MFnLight light(mLightDag);

        double intense = light.intensity();
        TakoSPI::PropertyPair prop;
        prop.first = intense;
        setter(iType, "intensity", prop);

        double coi = light.centerOfIllumination();
        prop.first = coi;
        setter(iType, "centerOfIllumination", prop);

        MColor color = light.color();
        std::vector <float> vcolor(3);
        vcolor[0] = color.r;
        vcolor[1] = color.g;
        vcolor[2] = color.b;
        prop.first = vcolor;
        prop.second.inputType = TakoSPI::ARBATTR_COLOR3;
        setter(iType, "color", prop);
    }

    if (mLightDag.hasFn(MFn::kNonAmbientLight))
    {
        MFnNonAmbientLight light(mLightDag);
        double decayRate = light.decayRate();

        TakoSPI::PropertyPair prop;
        prop.first = decayRate;
        setter(iType, "decayRate", prop);
    }

    if (mLightDag.hasFn(MFn::kSpotLight))
    {
        MFnSpotLight light(mLightDag);

        TakoSPI::PropertyPair prop;
        double val = light.coneAngle();
        prop.first = val;
        setter(iType, "coneAngle", prop);

        val = light.penumbraAngle();
        prop.first = val;
        setter(iType, "penumbraAngle", prop);

        val = light.dropOff();
        prop.first = val;
        setter(iType, "dropoff", prop);

        if (light.barnDoors())
        {
            val = light.barnDoorAngle(MFnSpotLight::kLeft);
            prop.first = val;
            setter(iType, "leftBarnDoor", prop);

            val = light.barnDoorAngle(MFnSpotLight::kRight);
            prop.first = val;
            setter(iType, "rightBarnDoor", prop);

            val = light.barnDoorAngle(MFnSpotLight::kTop);
            prop.first = val;
            setter(iType, "topBarnDoor", prop);

            val = light.barnDoorAngle(MFnSpotLight::kBottom);
            prop.first = val;
            setter(iType, "bottomBarnDoor", prop);
        }

        // decay region
        if (light.useDecayRegions())
        {
            val = light.startDistance(MFnSpotLight::kFirst);
            prop.first = val;
            setter(iType, "startDistance1", prop);

            val = light.endDistance(MFnSpotLight::kFirst);
            prop.first = val;
            setter(iType, "endDistance1", prop);

            val = light.startDistance(MFnSpotLight::kSecond);
            prop.first = val;
            setter(iType, "startDistance2", prop);

            val = light.endDistance(MFnSpotLight::kSecond);
            prop.first = val;
            setter(iType, "endDistance2", prop);

            val = light.startDistance(MFnSpotLight::kThird);
            prop.first = val;
            setter(iType, "startDistance3", prop);

            val = light.endDistance(MFnSpotLight::kThird);
            prop.first = val;
            setter(iType, "endDistance3", prop);
        }
    }
}

void MayaLightWriter::write(float iFrame)
{
    // only update if animated
    if (mIsAnimated)
        fillLightProps(2);

    mAttrs->write(iFrame);
}


bool MayaLightWriter::isAnimated() const
{
    return  mIsAnimated || (mAttrs != NULL && mAttrs->isAnimated());
}

} // End namespace Alembic


