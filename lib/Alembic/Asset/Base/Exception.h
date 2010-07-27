//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#ifndef _Alembic_Asset_Base_Exception_h_
#define _Alembic_Asset_Base_Exception_h_

#include <Alembic/Asset/Base/FoundationBase.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
class Exception : public Alembic::Util::Exception
{
public:
    Exception() throw()
      : Alembic::Util::Exception( "AlembicAsset EXCEPTION: " ) {}
    Exception( const std::string &str ) throw()
      : Alembic::Util::Exception( "AlembicAsset EXCEPTION: " )
    {
        this->append( str );
    }
};

//-*****************************************************************************
//-*****************************************************************************
// HANDY MACROS
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
#define AAST_THROW( TEXT )                      \
do                                              \
{                                               \
    std::stringstream sstr;                     \
    sstr << TEXT;                               \
    Alembic::Asset::Exception exc( sstr.str() );        \
    throw exc;                                  \
}                                               \
while( 0 )

//-*****************************************************************************
#define ALEMBIC_ASSET_THROW( TEXT ) AAST_THROW( TEXT )

} // End namespace Asset
} // End namespace Alembic

#endif
