//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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


// Alembic Includes
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreOgawa/All.h>

// Other includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

using namespace Alembic::AbcGeom; // Contains Abc, AbcCoreAbstract

//-*****************************************************************************
void Example1_MeshOut( const std::string &arkiveFile )
{
    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), arkiveFile );

    // Create a PolyMesh class.
    OPolyMesh pmeshyObj( OObject( archive, kTop ), "polymeshy" );

    // Create a subd
    OSubD smeshyObj( archive.getTop(), "subdmeshy" );

    TESTING_ASSERT( OSubD::matches( smeshyObj.getHeader() ) );
}

//-*****************************************************************************
void Example1_MeshIn( const std::string &arkiveFile )
{
    IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), arkiveFile  );

    IPolyMesh pmeshyObj( IObject( archive, kTop ), "polymeshy" );

    IObject smeshyObj( archive.getTop(), "subdmeshy" );

    // you can construct a Schema from a regular IObject simply by passing
    // in that Object's CompoundProperty as the parent in the Schema's
    // constructor.
    ISubDSchema sds( smeshyObj.getProperties() );

    TESTING_ASSERT( ISubDSchema::matches( sds.getMetaData() ) );

    ISubD subdObjNM( smeshyObj, kWrapExisting );

    ISubD subdObj( archive.getTop(), "subdmeshy", kStrictMatching );

    // if this function doesn't throw an exception, this test passes
    IPolyMesh fakePolyMesh( archive.getTop(), "subdmeshy", kNoMatching );

    TESTING_ASSERT( ISubD::matches( smeshyObj.getMetaData() ) );

    TESTING_ASSERT( ISubD::matches( subdObj.getMetaData() ) );

    TESTING_ASSERT( ! IPolyMesh::matches( subdObj.getHeader() ) );

    TESTING_ASSERT( ! IPolyMesh::matches( smeshyObj.getHeader() ) );
}

//-*****************************************************************************
void SchemaValid()
{
    ICameraSchema ics;
    OCameraSchema ocs;
    TESTING_ASSERT( ! ics.valid() );
    TESTING_ASSERT( ! ocs.valid() );

    ICurvesSchema icus;
    OCurvesSchema ocus;
    TESTING_ASSERT( ! icus.valid() );
    TESTING_ASSERT( ! ocus.valid() );

    IFaceSetSchema ifs;
    OFaceSetSchema ofs;
    TESTING_ASSERT( ! ifs.valid() );
    TESTING_ASSERT( ! ofs.valid() );

    ILightSchema ils;
    OLightSchema ols;
    TESTING_ASSERT( ! ils.valid() );
    TESTING_ASSERT( ! ols.valid() );

    INuPatchSchema inps;
    ONuPatchSchema onps;
    TESTING_ASSERT( ! inps.valid() );
    TESTING_ASSERT( ! onps.valid() );

    IPointsSchema ips;
    OPointsSchema ops;
    TESTING_ASSERT( ! ips.valid() );
    TESTING_ASSERT( ! ops.valid() );

    IPolyMeshSchema ipms;    
    OPolyMeshSchema opms;
    TESTING_ASSERT( ! ipms.valid() );
    TESTING_ASSERT( ! opms.valid() );

    ISubDSchema iss;
    OSubDSchema oss;
    TESTING_ASSERT( ! iss.valid() );
    TESTING_ASSERT( ! oss.valid() );
}
//-*****************************************************************************
int main( int argc, char *argv[] )
{
    std::string arkive( "schemaMatchingTest1.abc" );

    Example1_MeshOut( arkive );

    Example1_MeshIn( arkive );

    SchemaValid();

    return 0;
}
