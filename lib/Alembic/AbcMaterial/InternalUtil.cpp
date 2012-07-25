//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include "InternalUtil.h"

#include <Alembic/Util/All.h>

#include <sstream>

namespace Alembic {
namespace AbcMaterial {
namespace ALEMBIC_VERSION_NS {
namespace Util {

std::string
buildTargetName( const std::string & iTarget,
                 const std::string & iShaderType,
                 const std::string & iSuffix )
{
    std::string name = iTarget;
    name += ".";
    name += iShaderType;

    if (!iSuffix.empty())
    {
        name += "." + iSuffix;
    }

    return name;
}


void validateName( const std::string & iName,
                   const std::string & iExceptionVariableName )
{
    if ( iName.find('.') != std::string::npos ||
         iName.find('/') != std::string::npos )
    {
        ABC_THROW( "invalid name for " << iExceptionVariableName <<
                   ":" << iName );
    }
}


void split_tokens( const std::string & iValue,
                   std::vector<std::string> & oResult,
                   size_t iMaxSplit )
{
    oResult.clear();

    if ( iValue.empty() )
    {
        oResult.push_back("");
        return;
    }

    size_t pos = 0;

    for ( size_t i = 0;
        ( iMaxSplit == 0 || i < iMaxSplit ) && pos < iValue.size(); ++i )
    {
        size_t nextDotIndex = iValue.find('.', pos);

        if ( nextDotIndex == std::string::npos )
        {
            break;
        }

        oResult.push_back( iValue.substr( pos, nextDotIndex-pos ) );

        if ( nextDotIndex == iValue.size() - 1 )
        {
            oResult.push_back( "" );
        }

        pos = nextDotIndex + 1;
    }

    if ( pos < iValue.size() )
    {
        oResult.push_back( iValue.substr( pos ) );
    }
}

} // End namespace Util
} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcGeom
} // End namespace Alembic
