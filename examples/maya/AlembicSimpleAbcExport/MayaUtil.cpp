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

#include "MayaUtil.h"

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
void CheckMayaStatus( MStatus &status,
                      const std::string &file,
                      size_t line )
{
    if ( !status )
    {
        MGlobal::displayError(
            MString(
                ( boost::format( "AlembicSimpleAbcExport ERROR: %s:%d %s\n" )
                  % file % line % status.errorString().asChar() )
                .str().c_str() ) );
    }
}

//-*****************************************************************************
Abc::V3f GetVectorAttr( MObject &node, const std::string &attr )
{
    MStatus status;
    MFnDependencyNode dn( node, &status );
    CHECK_MAYA_STATUS;
    MPlug plug = dn.findPlug( attr.c_str(), true, &status );
    CHECK_MAYA_STATUS;

    double x = 0;
    double y = 0;
    double z = 0;
    if ( plug.isCompound() )
    {
        MPlug plug_x = plug.child( 0 );
        MPlug plug_y = plug.child( 1 );
        MPlug plug_z = plug.child( 2 );

        plug_x.getValue( x );
        plug_y.getValue( y );
        plug_z.getValue( z );
    }
    
    return Abc::V3f( x, y, z );
}

//-*****************************************************************************
char GetEnumAttr( MObject node, const std::string &attr )
{
    MStatus status;
    MFnDependencyNode dn( node, &status );
    CHECK_MAYA_STATUS;
    MPlug plug = dn.findPlug( attr.c_str(), true, &status );
    CHECK_MAYA_STATUS;
    
    char value = 0;
    status = plug.getValue( value );
    CHECK_MAYA_STATUS;
    
    return value;
}

}  //  End namespace AlembicSimpleAbcExport
