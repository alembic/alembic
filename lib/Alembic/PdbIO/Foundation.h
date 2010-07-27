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

#ifndef _Alembic_PdbIO_Foundation_h_
#define _Alembic_PdbIO_Foundation_h_

#include <boost/static_assert.hpp>
#include <boost/cstdint.hpp>
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>

#include <fstream>
#include <sys/types.h>
#include <float.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace Alembic {
namespace PdbIO {

//-*****************************************************************************
// CONDITIONS
// These will generate compiler errors if they are false.
namespace Conditions {

BOOST_STATIC_ASSERT( sizeof( float ) == 4 );
BOOST_STATIC_ASSERT( sizeof( boost::int32_t ) == 4 );

} // End namespace Conditions

//-*****************************************************************************
// Promotion of types
// This always seems redundant at first, but it's not. It's a way of saying,
// "in this library, in this namespace, when we say "Float", we mean a
// 32-bit IEEE float. When we say "Int", we mean a 32-bit signed integer.
// This is necessary due to the lack of standardization for the format of
// Float and Int.
typedef float float32_t;
typedef float Float;
typedef boost::int32_t Int;


//-*****************************************************************************
enum AttributeType
{
    k_VectorAttribute = 1,
    k_FloatAttribute = 2,
    k_IntAttribute = 3
};

//-*****************************************************************************
inline size_t bytesPerElement( AttributeType typ )
{
    return typ == k_VectorAttribute ? 12 : 4;
}

//-*****************************************************************************
inline size_t elementWidth( AttributeType typ )
{
    return typ == k_VectorAttribute ? 3 : 1;
}

//-*****************************************************************************
struct ObjectInfo
{
    size_t numParticles;
    size_t numAttributes;
    
    // This indicates whether the stream version of this object was or
    // should be byte-swapped from the native version.
    // This is necessary because PDB files don't have a fixed endianness.
    bool swapped;
};

//-*****************************************************************************
struct AttributeInfo
{
    std::string name;
    AttributeType type;
    size_t index;
};

//-*****************************************************************************
//-*****************************************************************************
// Base class for error handling.
//-*****************************************************************************
//-*****************************************************************************
class ErrorHandler
{
public:
    ErrorHandler() {}
    virtual ~ErrorHandler() {}

    virtual void handle( const std::string &err )
    {
        std::cerr << "PDB IO ERROR: " << err << std::endl;
        abort();
    }
};

} // End namespace PdbIO
} // End namespace Alembic

#endif
