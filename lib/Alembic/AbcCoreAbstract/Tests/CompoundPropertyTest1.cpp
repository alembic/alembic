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
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/Util/All.h>

#include <iostream>
#include <vector>

//-*****************************************************************************
namespace AO = Alembic::AbcCoreOgawa;

namespace ABCA = Alembic::AbcCoreAbstract;

using ABCA::chrono_t;
using Alembic::Util::float32_t;
using Alembic::Util::int32_t;
using Alembic::Util::byte_t;
using Alembic::Util::Dimensions;

//-*****************************************************************************
void WriteTestArchive( const std::string &iArchiveName )
{
    ABCA::MetaData md;
    md.setUnique( "name", "arkive" );
    ABCA::ObjectWriterPtr finalChild;

    AO::WriteArchive aw;

    ABCA::ArchiveWriterPtr archive = aw( iArchiveName, md );

    ABCA::ObjectWriterPtr topObj = archive->getTop();
    // at this point, the archive has a single H5 Group under the root
    // group called "ABC".

    {
        // as things go out of scope, they automatically clean themselves up.
        // when the archive goes out of scope, it writes itself to disk.

        md.set( "name", "childObject0" );
        ABCA::ObjectWriterPtr archiveChild = topObj->createChild(
            ABCA::ObjectHeader( "archiveChild", md ) );
        // the hdf5 layout of the archive is now "/ABC/archiveChild/"

        md.set( "name", "childObject1" );
        ABCA::ObjectWriterPtr archiveChildChild = archiveChild->createChild(
            ABCA::ObjectHeader( "archiveChildChild", md ) );
        // "/ABC/archiveChild/archiveChildChild"

        md.set( "name", "childObject2" );
        finalChild = archiveChildChild->createChild(
            ABCA::ObjectHeader( "archiveChildChildChild", md ) );
        // "/ABC/archiveChild/archiveChildChild/archiveChildChildChild"
    }
    // At this point, only the final child and the archive are still in scope,
    // but we should still be able to write some properties to the final child,
    // even though its parents are "gone", and all access needs to be done
    // hierarchically.

}

//-*****************************************************************************
int main( int, char** )
{
    const std::string firstArchive( "firstArchive.abc" );

    WriteTestArchive( firstArchive );

}
