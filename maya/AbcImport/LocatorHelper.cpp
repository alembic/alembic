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
#include "LocatorHelper.h"
#include "NodeIteratorVisitorHelper.h"

#include <maya/MObject.h>
#include <maya/MFnTransform.h>
#include <maya/MString.h>
#include <maya/MVector.h>
#include <maya/MGlobal.h>
#include <maya/MDagModifier.h>


MObject create(Alembic::AbcGeom::IXform & iLocator,
               MObject & iParent,
               Alembic::Abc::IScalarProperty & iLocProp)
{
    MStatus status = MS::kSuccess;
    MObject locatorObj = MObject::kNullObj;

    MString name(iLocator.getName().c_str());

    MDagModifier modifier;
    locatorObj = modifier.createNode("locator", iParent, &status);
    status = modifier.doIt();
    if (status != MS::kSuccess)
    {
        MString theError("Failed to create locator node ");
        theError += name;
        printError(theError);
        return locatorObj;
    }

    MFnDagNode fnLocator(locatorObj);
    fnLocator.setName(name);

    if (iLocProp.isConstant())
    {
        // read the value from iLocProp
        double oSample[6];
        iLocProp.get(oSample, 0);

        // set the plugs and be done
        MPlug dstPlug;
        dstPlug = fnLocator.findPlug("localPositionX");
        dstPlug.setValue(oSample[0]);
        dstPlug = fnLocator.findPlug("localPositionY");
        dstPlug.setValue(oSample[1]);
        dstPlug = fnLocator.findPlug("localPositionZ");
        dstPlug.setValue(oSample[2]);
        dstPlug = fnLocator.findPlug("localScaleX");
        dstPlug.setValue(oSample[3]);
        dstPlug = fnLocator.findPlug("localScaleY");
        dstPlug.setValue(oSample[4]);
        dstPlug = fnLocator.findPlug("localScaleZ");
        dstPlug.setValue(oSample[5]);
    }

    return locatorObj;
}

void read(double iFrame,
          Alembic::AbcGeom::IXform & iLocator,
          std::vector< double > & oArray)
{
    oArray.resize(6);

    Alembic::Abc::ICompoundProperty props = iLocator.getProperties();
    Alembic::Abc::IScalarProperty locProp(props, "locator");

    Alembic::AbcCoreAbstract::index_t index, ceilIndex;
    double alpha = getWeightAndIndex(iFrame,
                                     locProp.getTimeSampling(),
                                     locProp.getNumSamples(),
                                     index,
                                     ceilIndex);
    double samp[6];
    locProp.get(samp, index);

    if (fabs(alpha) > 1e-6)  // interpolation
    {
        double ceilSamp[6];
        locProp.get(ceilSamp, ceilIndex);
        for (unsigned int i = 0; i < 6; i++)
            oArray[i] = simpleLerp< double >(alpha, samp[i], ceilSamp[i]);
    }
    else
    {
        for (unsigned int i = 0; i < 6; i++)
            oArray[i] = samp[i];
    }
}
