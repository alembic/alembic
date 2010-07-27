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

#ifndef _Alembic_Tako_HDFNode_h_
#define _Alembic_Tako_HDFNode_h_

#include <Alembic/Tako/Foundation.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {


/**
\brief A typedef to a boost::variant that may contain numeric, string,
and vector types.

Property is a typedef for boost::variant &lt several sized ints, float, double,
std::string (and vectors of these) &gt and is used to
manage holding these various types for the property map.
*/
typedef boost::variant< int8_t, int16_t, int32_t, int64_t, float, double,
    std::string,  std::vector<int16_t>, std::vector<int32_t>,
    std::vector<int64_t>, std::vector<float>, std::vector<double>,
    std::vector<std::string> >
    Property;

/**
\brief The ArbitraryAttr style scope type.

This enumeration represents the scope of the attribute.  It maps directly
to ArbitraryAttrs various scopes.
*/
enum ScopeType
{
    /**
    Indicates that the attribute applies to the entire primitive.
    */
    SCOPE_PRIMITIVE,

    /**
    Indicates that the attribute applies to each face in the primitive.
    */
    SCOPE_FACE,

    /**
    Indicates that the attribute applies to each vertex in the primitive.
    */
    SCOPE_POINT,

    /**
    Indicates that the attribute applies to each point of each face
    (face-varying points) in the primitive.
    */
    SCOPE_VERTEX
};

/**
\brief The ArbitraryAttr style type.

This enumeration is used to list the inputType or outputType of an ArbitraryAttr
style attribute.
*/
enum ArbAttrType
{
    /**
    Indicates that the in or out type is not used.
    */
    ARBATTR_NONE,

    /**
    Indicates that the in or out type is an integer.
    */
    ARBATTR_INT,

    /**
    Indicates that the in or out type is a 32 bit floating point number.
    (IEEE 754)
    */
    ARBATTR_FLOAT,

    /**
    Indicates that the in or out type is a 64 bit floating point number.
    (IEEE 754)
    */
    ARBATTR_DOUBLE,

    /**
    Indicates that the in or out type is a string.
    */
    ARBATTR_STRING,

    /**
    Indicates that the in or out type is a 3-channel red, green, blue color.
    */
    ARBATTR_COLOR3,

    /**
    Indicates that the in or out type is a 4-channel red, green, blue,
    alpha color.
    */
    ARBATTR_COLOR4,

    /**
    Indicates that the in or out type is a 2 dimensional normal.
    */
    ARBATTR_NORMAL2,

    /**
    Indicates that the in or out type is a 3 dimensional normal.
    */
    ARBATTR_NORMAL3,

    /**
    Indicates that the in or out type is a 2 dimensional vector.
    */
    ARBATTR_VECTOR2,

    /**
    Indicates that the in or out type is a 3 dimensional vector.
    */
    ARBATTR_VECTOR3,

    /**
    Indicates that the in or out type is a 4 dimensional vector.
    */
    ARBATTR_VECTOR4,

    /**
    Indicates that the in or out type is a 2 dimensional point.
    */
    ARBATTR_POINT2,

    /**
    Indicates that the in or out type is a 3 dimensional point.
    */
    ARBATTR_POINT3,

    /**
    Indicates that the in or out type is a 4 dimensional point.
    */
    ARBATTR_POINT4
};

/**
\brief The extra ArbitraryAttrInfo for a property.

This struct holds all the additional information necessary to describe a
property in ArbitraryAttr terms.  This information is optional.
*/
struct ArbAttrInfo
{
    /**
    \brief The ArbitraryAttr style index array.

    This array contains indices into the array of the corresponding property.
    It is used when there is a lot similiar data that can be shared.  (Such
    as a handful of colors that are repeated for every vertex in a complex
    geometry).
    */
    std::vector<int32_t> index;

    /**
    \brief The scope of the property.

    This contains a hint as to what level the given property applies to
    on the primitive.  (For example, a color may apply to the whole shape,
    or every vertex in the shape could have its own color.)
    */
    ScopeType scope;

    /**
    \brief The input type hint.

    This is a hint about what the incoming data represents.  For example an
    array of floats could really represent a 3 channel color.
    */
    ArbAttrType inputType;

    /**
    \brief The output type hint.

    This is a hint about how the data is to be presented.  For example the data
    we may only have data for an array of 3 channel colors, but we want to
    represent it as an array of 4 channel colors (implicitly filling in the
    last channel with a sensible default value).
    */
    ArbAttrType outputType;

    /**
    \brief Default constructor.

    By default, the scope is set to primitive, and the inputType and outputType
    are set to not be used.
    */
    ArbAttrInfo() {
        scope = SCOPE_PRIMITIVE;
        inputType = ARBATTR_NONE;
        outputType = ARBATTR_NONE;
    }

    /**
    \brief Constructor which sets the scope, input type, and output type.
    \param iScope The scope of this attribute.
    \param iInType The ArbitraryAttr style input type.
    \param iOutType The ArbitraryAttr style output type.
    */
    ArbAttrInfo(ScopeType iScope, ArbAttrType iInType, ArbAttrType iOutType) {
        scope = iScope;
        inputType = iInType;
        outputType = iOutType;
    }
};

/**
\brief A typedef to a std::pair that contains the property and arbitrary attr
information.

PropertyPair is a typedef for std::pair &lt Property, ArbAttrInfo &gt.
*/
typedef std::pair< Property, ArbAttrInfo> PropertyPair;

/**
\brief A typedef to a std::map that maps a std::string to a property.

PropertyMap is a typedef for std::map &lt std::string, PropertyPair &gt.
*/
typedef std::map< std::string, PropertyPair > PropertyMap;

/**
\brief Hint about a specific property.

This is the return value for a couple of the convenience functions
which reads or gets just one property.
*/
enum PropertyType
{
    /** Indicates that the property doesn't exist. */
    PROPERTY_NONE,

    /** Indicates that property is not sampled. */
    PROPERTY_STATIC,

    /** Indicates that the property is sampled. */
    PROPERTY_ANIMATED
};
}

using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_HDFNode_h_
