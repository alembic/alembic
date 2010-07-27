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

#include <Alembic/Core/FileWriter.h>
#include <Alembic/Core/PageWriter.h>
#include <Alembic/Core/FileReader.h>

namespace AB = Alembic::Core;

int main( int argc, char *argv[] )
{
    std::cout << "Test1. Testing simple file write with no pages."
              << std::endl;

    std::string fileName = "AlembicCoreTest1.abc";

    //-*************************************************************************
    // WRITE
    {
        // File
        AB::SharedFileWriter ofile = AB::MakeSharedFileWriter( fileName );
        std::cout << "File: " << fileName << " created." << std::endl;

        // Top Page
        AB::SharedPageWriter opage = AB::MakeSharedPageWriter( ofile );
        std::cout << "Top page created" << std::endl;

        // Write metadata for opage
        opage->writeMetaData( "NOBODY steps on a church in my town!" );

        // Second page
        AB::SharedPageWriter opage2 = AB::MakeSharedPageWriter( opage,
                                                                "page2" );
        std::cout << "Second page created" << std::endl;
        opage2->writeMetaData( "Let's show this prehistoric bitch how we "
                               "do things downtown." );

        opage2->close();
        opage->close();
        ofile->close();
        std::cout << "File closed successfully." << std::endl;
    }

    //-*************************************************************************
    // READ
    {
        // File
        AB::SharedFileReader ifile = AB::MakeSharedFileReader( fileName );
        std::cout << "File: " << fileName << " read." << std::endl;
        
        ifile->close();
        std::cout << "File closed successfully." << std::endl;
    }

    return 0;
}
