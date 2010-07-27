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

#include <AlembicTakoImport/LocatorHelper.h>
#include <AlembicTakoImport/Util.h>
#include <AlembicTakoImport/NodeIteratorVisitorHelper.h>

namespace Alembic {

MStatus createLocator(float iFrame, const Tako::GenericNodeReaderPtr & iNode,
    MObject & iParent, MObject & iObject,
    std::vector<std::string> & iSampledPropNameList)
{
    static const std::string sLocalPosition("localPosition");
    static const std::string sLocalScale("localScale");

    MStatus status = MS::kSuccess;

    iNode->readProperties(iFrame);

    iSampledPropNameList.clear();

    MDagModifier modifier;
    iObject = modifier.createNode("locator", iParent, &status);
    status = modifier.doIt();
    if (status != MS::kSuccess)
    {
        printError("Failed to create locator object");
        return status;
    }

    MFnDagNode mFn(iObject, &status);
    if (status != MS::kSuccess)
    {
        printError("Failed to initialize locator object");
        return status;
    }

    mFn.setName(iNode->getName());

    // find the values of the locator attributes
    bool found = false;
    // check non-sampled properties first
    Tako::PropertyMap::const_iterator propIter =
        iNode->beginNonSampledProperties();
    while (propIter != iNode->endNonSampledProperties())
    {
        std::string propName = propIter->first;
        Tako::PropertyPair pPair = propIter->second;
        if (propName == sLocalPosition || propName == sLocalScale)
        {
            MPlug plug = mFn.findPlug(propName.c_str(), true, &status);
            if (status == MS::kSuccess)
            {
                ReadPropAttrVisitor visitor(iFrame);
                visitor.setArbAttrInfo(pPair.second);
                visitor.setPlug(plug);
                boost::apply_visitor(visitor, pPair.first);
            }
            found = true;
        }
        else
        {
            AddAttrVisitor attrVisitor(propName, iObject, pPair.second);
            boost::apply_visitor(attrVisitor, pPair.first);
        }

        propIter++;
    }
    // if not found, check the sampled properties next
    if (!found)
    {
        propIter = iNode->beginSampledProperties();
        while (propIter != iNode->endSampledProperties())
        {
            std::string propName = propIter->first;
            Tako::PropertyPair pPair = propIter->second;
            if (propName == sLocalPosition || propName == sLocalScale)
            {
                MPlug plug = mFn.findPlug(propName.c_str(), true, &status);
                if (status == MS::kSuccess)
                {
                    ReadPropAttrVisitor visitor(iFrame);
                    visitor.setArbAttrInfo(pPair.second);
                    visitor.setPlug(plug);
                    boost::apply_visitor(visitor, pPair.first);
                }
                found = true;
                if (propName == sLocalPosition)
                {
                    iSampledPropNameList.push_back("localPositionX");
                    iSampledPropNameList.push_back("localPositionY");
                    iSampledPropNameList.push_back("localPositionZ");
                }
                else
                {
                    iSampledPropNameList.push_back("localScaleX");
                    iSampledPropNameList.push_back("localScaleY");
                    iSampledPropNameList.push_back("localScaleZ");
                }
            }
            else
            {
                AddAttrVisitor attrVisitor(propName, iObject, pPair.second);
                boost::apply_visitor(attrVisitor, pPair.first);
                iSampledPropNameList.push_back(propName);
            }

            propIter++;
        }
    }

    if (!found)
    {
        MString theWarning("Attribute localPosition and localScale not found.");
        theWarning += "";
        printWarning(theWarning);
    }

    return status;
}

} // End namespace Alembic

