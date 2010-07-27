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

#ifndef _Alembic_TakoAbstract_HDFReaderNode_h_
#define _Alembic_TakoAbstract_HDFReaderNode_h_

#include <Alembic/TakoAbstract/Foundation.h>
#include <Alembic/TakoAbstract/HDFNode.h>

namespace Alembic {

namespace TakoAbstract {

namespace TAKO_ABSTRACT_LIB_VERSION_NS {

//-*****************************************************************************
class HDFReaderNode
{
public:
    struct PropInfo
    {
        ScopeType scope;
        ArbAttrType inputType;
        ArbAttrType outputType;
        bool isSampled;
    };

    typedef std::map< std::string, PropInfo > PropNameInfoMap;

    virtual ~HDFReaderNode();

    //-*************************************************************************
    // These functions exist on every Tako primitive except, trivially,
    // the GenericNode. I'll make this a base-class function of HDFReaderNode
    // that allows this to be treated polymorphically.
    virtual void getFrames( std::set<float>& ioFrames ) const = 0;
    virtual bool hasFrames() const = 0;

    //-*************************************************************************
    // These functions can exist for every Tako primitive. If the bounds
    // are not defined, an empty bounds works.
    // MaxBounds is the maximum spatial bounds this could have at the given
    // frame - in the case of a shape, that's just the bounds of the shape.
    // In the case of a transform, that's the bounds of ALL the children,
    // even if they're not loaded. It is assumed this is written as part of
    // the writing process.
    //virtual Box3d getMaxBounds( float iFrame ) = 0;
    //virtual Box3d getStaticMaxBounds() = 0;

    //-*************************************************************************
    // Property management
    virtual void readProperties( float iFrame ) = 0;
    
    virtual PropertyType readProperty(
        const std::string & iName,
        float iFrame,
        PropertyPair & oProp ) = 0;

    virtual void getPropertyNamesAndInfo(
        PropNameInfoMap & oMap ) = 0;
    
    virtual PropertyType getProperty(
        const std::string &iName,
        PropertyPair & oProp ) = 0;
    
    virtual void clearNonSampledProperties() = 0;

    virtual PropertyMap::const_iterator
    beginNonSampledProperties() const = 0;
    
    virtual PropertyMap::const_iterator
    endNonSampledProperties() const = 0;

    virtual PropertyMap::const_iterator
    beginSampledProperties() const = 0;

    virtual PropertyMap::const_iterator
    endSampledProperties() const = 0;

    virtual void getPropertyFrames(
        std::set<float> & ioFrames ) const = 0;

    virtual bool hasPropertyFrames() const = 0;

    virtual const std::string & getName() const = 0;
};

} // End namespace TAKO_ABSTRACT_LIB_VERSION_NS

} // End namespace TakoAbstract

} // End namespace Alembic

#endif  // _Alembic_TakoAbstract_HDFReaderNode_h_
