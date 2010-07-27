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

#ifndef _Alembic_Asset_Base_PropertyInfo_h_
#define _Alembic_Asset_Base_PropertyInfo_h_

#include <Alembic/Asset/Base/FoundationBase.h>
#include <Alembic/Asset/Base/DataType.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
enum PropertyType
{
    kSingularProperty,
    kMultiProperty,
    kUnknownPropertyType
};

//-*****************************************************************************
//-*****************************************************************************
// PROPERTY INFO
//-*****************************************************************************
//-*****************************************************************************
struct PropertyInfoBody
{
    PropertyInfoBody() throw()
      : name( "UNKNOWN" ), protocol( "UNKNOWN" ),
        ptype( kUnknownPropertyType ), dtype( kUnknownPOD, 0 ) {}
    
    PropertyInfoBody( const std::string &n,
                      const std::string &p,
                      PropertyType pt,
                      const DataType &dt ) throw()
      : name( n ), protocol( p ), ptype( pt ), dtype( dt ) {}
    
    std::string name;
    std::string protocol;
    PropertyType ptype;
    DataType dtype;
};

typedef boost::shared_ptr<PropertyInfoBody> PropertyInfo;

inline PropertyInfo MakePropertyInfo( void )
{
    return boost::make_shared<PropertyInfoBody>();
}

inline PropertyInfo MakePropertyInfo( const std::string &n,
                                      const std::string &p,
                                      PropertyType pt,
                                      const DataType &dt )
{
    return boost::make_shared<PropertyInfoBody>( n, p, pt, dt );
}

} // End namespace Asset
} // End namespace Alembic

#endif
