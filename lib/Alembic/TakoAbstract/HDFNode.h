//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#ifndef _Alembic_TakoAbstract_HDFNode_h_
#define _Alembic_TakoAbstract_HDFNode_h_

#include <Alembic/TakoAbstract/Foundation.h>

namespace Alembic {
namespace TakoAbstract {
namespace TAKO_ABSTRACT_LIB_VERSION_NS {

//-*****************************************************************************
// This property manifestation represents a copy-by-value approach.
typedef boost::variant< int8_t,
                        int16_t,
                        int32_t,
                        int64_t,
                        float,
                        double,
                        
                        std::string,

                        std::vector< int16_t >,
                        std::vector< int32_t >,
                        std::vector< int64_t >,
                        std::vector< float >,
                        std::vector< double >,
                        std::vector< std::string > >

                        Property;

//-*****************************************************************************
enum ScopeType
{
    SCOPE_PRIMITIVE,
    SCOPE_FACE,
    SCOPE_POINT,
    SCOPE_VERTEX
};

//-*****************************************************************************
enum ArbAttrType
{
    ARBATTR_NONE,
    ARBATTR_INT,
    ARBATTR_FLOAT,
    ARBATTR_DOUBLE,
    ARBATTR_STRING,
    ARBATTR_COLOR3,
    ARBATTR_COLOR4,
    ARBATTR_NORMAL2,
    ARBATTR_NORMAL3,
    ARBATTR_VECTOR2,
    ARBATTR_VECTOR3,
    ARBATTR_VECTOR4,
    ARBATTR_POINT2,
    ARBATTR_POINT3,
    ARBATTR_POINT4
};

//-*****************************************************************************
struct ArbAttrInfo
{
    std::vector<index_t> index;
    ScopeType scope;
    ArbAttrType inputType;
    ArbAttrType outputType;

    ArbAttrInfo()
    {
        scope = SCOPE_PRIMITIVE;
        inputType = ARBATTR_NONE;
        outputType = ARBATTR_NONE;
    }

    ArbAttrInfo( ScopeType iScope,
                 ArbAttrType iInType,
                 ArbAttrType iOutType )
    {
        scope = iScope;
        inputType = iInType;
        outputType = iOutType;
    }
};

//-*****************************************************************************
typedef std::pair< Property, ArbAttrInfo > PropertyPair;

//-*****************************************************************************
typedef std::map< std::string, PropertyPair > PropertyMap;

//-*****************************************************************************
enum PropertyType
{
    PROPERTY_NONE,
    PROPERTY_STATIC,
    PROPERTY_ANIMATED
};

} // End namespace TAKO_ABSTRACT_LIB_VERSION_NS

} // End namespace TakoAbstract

} // End namespace Alembic

#endif  // _Alembic_TakoAbstract_HDFNode_h_
