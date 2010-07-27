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

#include <AlembicTakoImport/LightHelper.h>
#include <AlembicTakoImport/Util.h>
#include <AlembicTakoImport/NodeIteratorVisitorHelper.h>

namespace Alembic {

MStatus createLight(float iFrame, const Tako::GenericNodeReaderPtr & iNode,
    MObject & iParent, MObject & iObject,
    std::vector<std::string> & iSampledPropNameList)
{
    // sBaseLightAttrs and sSpotLightAttrs will be set up only once.
    // They are used to identify the type of the current light (point or spot)
    static std::set < std::string > sBaseLightAttrs;
    static std::set < std::string >::iterator sBaseLightEnd;
    if (sBaseLightAttrs.empty())
    {
        sBaseLightAttrs.insert("intensity");
        sBaseLightAttrs.insert("centerOfIllumination");
        sBaseLightAttrs.insert("color");
        sBaseLightAttrs.insert("decayRate");
        sBaseLightEnd = sBaseLightAttrs.end();
    }

    static std::set < std::string > sSpotLightAttrs;
    static std::set < std::string >::iterator sSpotLightEnd;
    if (sSpotLightAttrs.empty())
    {
        sSpotLightAttrs.insert("coneAngle");
        sSpotLightAttrs.insert("penumbraAngle");
        sSpotLightAttrs.insert("dropoff");
        sSpotLightAttrs.insert("leftBarnDoor");
        sSpotLightAttrs.insert("rightBarnDoor");
        sSpotLightAttrs.insert("topBarnDoor");
        sSpotLightAttrs.insert("bottomBarnDoor");
        sSpotLightAttrs.insert("startDistance1");
        sSpotLightAttrs.insert("endDistance1");
        sSpotLightAttrs.insert("startDistance2");
        sSpotLightAttrs.insert("endDistance2");
        sSpotLightAttrs.insert("startDistance3");
        sSpotLightAttrs.insert("endDistance3");
        sSpotLightEnd = sSpotLightAttrs.end();
    }

    MStatus status = MS::kSuccess;

    iNode->readProperties(iFrame);

    bool isSpotLight = false;
    iSampledPropNameList.clear();

    Tako::PropertyMap::const_iterator propIter;
    Tako::PropertyPair pPair;

    // go through the props to see what type of lights to create(point or spot)

    // check non-sampled properties first
    propIter  = iNode->beginNonSampledProperties();
    while (propIter != iNode->endNonSampledProperties())
    {
        if ( sSpotLightAttrs.find(propIter->first) != sSpotLightEnd)
        {
            isSpotLight = true;
            break;
        }

        propIter++;
    }
    // if no property for spot light is found, check the sampled properties
    if (!isSpotLight)
    {
        propIter = iNode->beginSampledProperties();
        while (propIter != iNode->endSampledProperties())
        {
            if ( sSpotLightAttrs.find(propIter->first) != sSpotLightEnd)
            {
                isSpotLight = true;
                break;
            }

            propIter++;
        }
    }

    if (isSpotLight)
    {
        MFnSpotLight mFn;
        iObject = mFn.create(iParent, true, false, &status);
        mFn.setName(iNode->getName());

        propIter  = iNode->beginNonSampledProperties();
        while (propIter != iNode->endNonSampledProperties())
        {
            pPair = propIter->second;
            std::string propName = propIter->first;
            if ( sSpotLightAttrs.find(propName) == sSpotLightEnd &&
                sBaseLightAttrs.find(propName) == sBaseLightEnd)
            {
                AddAttrVisitor attrVisitor(propName, iObject,
                    pPair.second);
                boost::apply_visitor(attrVisitor, pPair.first);
            }
            else
            {
                MPlug plug = mFn.findPlug(propName.c_str(), true, &status);
                if (status == MS::kSuccess)
                {
                    ReadPropAttrVisitor visitor(iFrame);
                    visitor.setArbAttrInfo(pPair.second);
                    visitor.setPlug(plug);
                    boost::apply_visitor(visitor, pPair.first);
                }
            }

            propIter++;
        }

        // Adding sampled attributes to node if any
        propIter = iNode->beginSampledProperties();
        while (propIter != iNode->endSampledProperties())
        {
            pPair = propIter->second;
            std::string propName = propIter->first;
            if (sSpotLightAttrs.find(propName) == sSpotLightEnd &&
                sBaseLightAttrs.find(propName) == sBaseLightEnd)
            {
                AddAttrVisitor attrVisitor(propName, iObject,
                    pPair.second);
                boost::apply_visitor(attrVisitor, pPair.first);
                iSampledPropNameList.push_back(propName);
            }
            else
            {
                if (propName != "color")
                    iSampledPropNameList.push_back(propName);
                else
                {
                    iSampledPropNameList.push_back("colorR");
                    iSampledPropNameList.push_back("colorG");
                    iSampledPropNameList.push_back("colorB");
                }
            }

            propIter++;
        }
    }
    // create a pointlight by default
    else
    {
        MFnPointLight mFn;
        iObject = mFn.create(iParent, true, false, &status);
        mFn.setName(iNode->getName());

        propIter  = iNode->beginNonSampledProperties();
        while (propIter != iNode->endNonSampledProperties())
        {
            pPair = propIter->second;
            std::string propName = propIter->first;
            if ( sBaseLightAttrs.find(propName) == sBaseLightEnd )
            {
                AddAttrVisitor attrVisitor(propName, iObject, pPair.second);
                boost::apply_visitor(attrVisitor, pPair.first);
            }
            else
            {
                MPlug plug = mFn.findPlug(propName.c_str(), true, &status);
                if (status == MS::kSuccess)
                {
                    ReadPropAttrVisitor mVisitor(iFrame);
                    mVisitor.setArbAttrInfo(pPair.second);
                    mVisitor.setPlug(plug);
                    boost::apply_visitor(mVisitor, pPair.first);
                }
            }

            propIter++;
        }

        // Adding sampled attributes to node if any
        propIter = iNode->beginSampledProperties();
        while (propIter != iNode->endSampledProperties())
        {
            pPair = propIter->second;
            std::string propName = propIter->first;
            if ( sBaseLightAttrs.find(propName) == sBaseLightEnd)
            {
                AddAttrVisitor attrVisitor(propName, iObject, pPair.second);
                boost::apply_visitor(attrVisitor, pPair.first);
                iSampledPropNameList.push_back(propName);
            }
            else
            {
                if (propName != "color")
                    iSampledPropNameList.push_back(propName);
                else
                {
                    iSampledPropNameList.push_back("colorR");
                    iSampledPropNameList.push_back("colorG");
                    iSampledPropNameList.push_back("colorB");
                }
            }

            propIter++;
        }
    }

    return status;
}

} // End namespace Alembic

