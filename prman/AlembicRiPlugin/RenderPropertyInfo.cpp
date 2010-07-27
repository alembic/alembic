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

#include "Foundation.h"
#include "RenderPropertyInfo.h"
#include <boost/format.hpp>

namespace Abc = AlembicAsset;

namespace AlembicRiPlugin {

//-*****************************************************************************
std::string GetRManAttrType( Abc::PropertyInfo pi )
{
    std::string attrSig;
    std::string attrName;
    std::string attrType;
    std::string attrScope;
    std::string attrNumType;
    boost::smatch match;

    std::string pname = pi->name;
    size_t extent = size_t( pi->dtype.extent );
    std::string prot = pi->protocol;
    std::string dtname = Abc::PODName( pi->dtype.pod );

    if ( boost::regex_match( pname, match, GENERIC_TYPE_BAR_NAME_RE ) )
    {
        attrType = match[1];
        attrName = match[2];
    }
    else
    {
        attrName = pname;
    }

    // renderman attribute scope
    if ( attrType == "perVertex" )
    {
        attrScope = "vertex";
    }
    else if ( attrType == "perFace" )
    {
        attrScope = "uniform";
    }
    else if ( attrType == "perVertexPerFace" )
    {
        attrScope = "facevarying";
    }
    else
    {
        attrScope = "constant";
    }

    // RSL numerical type
    if ( prot == "vec" )
    {
        attrNumType = "point";
        extent = 0;
    }
    else if ( boost::regex_match( dtname, INT_POD_RE )
              || boost::regex_match( dtname, FLOAT_POD_RE ) )
    {
        attrNumType = "float";
    }
    else
    {
        attrNumType = "float";
    }

    // cardinality of data
    if ( extent > 1 )
    {
        attrNumType = ( boost::format( "%s[%d]" )
                        % attrNumType
                        % extent ).str();
    }

    // put it all together
    attrSig = ( boost::format( "%s %s %s" )
                % attrScope
                % attrNumType
                % attrName ).str();


    return attrSig;
}

//-*****************************************************************************
std::string GetRManPropertyNameName( Abc::PropertyInfo pi )
{
    std::string pname = pi->name;
    boost::smatch match;

    if ( ! boost::regex_match( pname, VALID_PROPERTY_NAME_RE ) )
    {
        return pname;
    }
    else if ( boost::regex_match( pname, match, GENERIC_TYPE_BAR_NAME_RE ) )
    {
        return std::string( match[2] );
    }
    else
    {
        return pname;
    }
}

} // End namespace AlembicRiPlugin
