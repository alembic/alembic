//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/Util/All.h>

#include <iostream>

//-*****************************************************************************
namespace A5 = Alembic::AbcCoreHDF5;

namespace AA = Alembic::AbcCoreAbstract;

//-*****************************************************************************
void WriteTestArchive( const std::string &iArchiveName )
{
    std::cout << "Entered WriteTestArchive()" << std::endl;

    AA::MetaData md;

    A5::WriteArchive aw;

    // declaring the object writer prior to opening the archive causes it to
    // stay open when the archive goes out of scope, throwing an exception.
    AA::ObjectWriterPtr finalChild;

    AA::ArchiveWriterPtr arkive = aw( iArchiveName, md );

    // interestingly, declaring topObj prior to opening the archive for
    // writing doesn't crash it.
    AA::ObjectWriterPtr topObj = arkive->getTop();

    finalChild = topObj->createChild(
        AA::ObjectHeader( "ac0", md ) );

    std::cout << "About to leave WriteTestArchive()." << std::endl;
}

//-*****************************************************************************
void ReadTestArchive( const std::string &iArchiveName )
{
    std::cout << "Entered ReadTestArchive()" << std::endl;

    A5::ReadArchive ar;

    // declaring the object reader before opening the archive causes it to stay
    // open when the archive tries to close.
    AA::ObjectReaderPtr finalChild;

    AA::ArchiveReaderPtr arkive = ar( iArchiveName );

    AA::ObjectReaderPtr topObj = arkive->getTop();

    finalChild = topObj->getChild( 0 );

    std::cout << "finalChild->getFullName(): " << finalChild->getFullName()
              << std::endl;

    std::cout << "About to leave ReadTestArchive()" << std::endl;
}

//-*****************************************************************************
int main( int, char** )
{
    const std::string firstArchive( "octessence58.abc" );

    WriteTestArchive( firstArchive );
    ReadTestArchive( firstArchive );

    return 0;
}
