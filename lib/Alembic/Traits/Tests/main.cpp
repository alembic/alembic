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

#include <Alembic/Asset/AssetAll.h>
#include <Alembic/Traits/TraitsAll.h>
#include <Alembic/Traits/Test.h>
#include <boost/algorithm/string.hpp>
#include <sstream>

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    Alembic::Asset::Init();
    
    std::string idealTestOutput =
        "Created Bones Brigade Test asset\n"
        "Wrote Steve.\n"
        "Wrote Tommy.\n"
        "Wrote Tony.\n"
        "Wrote Mike.\n"
        "Wrote Lance.\n"
        "Closed Bones Brigade Test asset\n"
        "Read Bones Brigade Test asset\n"
        "Steve (should be 5): 5\n"
        "Tommy exists. (As he should)\n"
        "Is he animated (should be yes)? 1\n"
        "Tommy rest (should be 17.0): 17\n"
        "Tommy sample 0 (should be 16.0): 16\n"
        "Tommy sample 1 (should be 15.0): 15\n"
        "Tommy sample 2 (should be 14.0): 14\n"
        "Tony\'s data:\n"
        "(0 0 0) (1 1 1) (2 2 2) (3 3 3)\n"
        "Mike exists. (As he should)\n"
        "Mike\'s data:\n"
        "(0 0) (1 1)\n"
        "Lance: (-1 -1 -1) to (1 1 1)\n"
        "Closed Bones Brigade Test asset\n";
    boost::trim( idealTestOutput );
    std::string idealNoWhite = idealTestOutput;
    boost::erase_all( idealNoWhite, "\n" );
    boost::erase_all( idealNoWhite, "\t" );
    boost::erase_all( idealNoWhite, "\r" );
    boost::erase_all( idealNoWhite, " " );

    std::stringstream sstr;
    int ret = AlembicTraits::BonesBrigadeTest( sstr );
    if ( ret != 0 )
    {
        return ret;
    }
    
    std::string actualTestOutput = sstr.str();
    boost::algorithm::trim( actualTestOutput );
    std::string actualNoWhite = actualTestOutput;
    boost::erase_all( actualNoWhite, "\n" );
    boost::erase_all( actualNoWhite, "\t" );
    boost::erase_all( actualNoWhite, "\r" );
    boost::erase_all( actualNoWhite, " " );
    if ( actualNoWhite != idealNoWhite )
    {
        std::cerr << "ERROR: Test produced bad results. Wanted:\n\n\n"
                  << std::endl
                  << "------>"
                  << idealTestOutput
                  << "<------"
                  << "\n\n\nBut Got:\n\n\n"
                  << std::endl
                  << "------>"
                  << actualTestOutput
                  << "<------"
                  << std::endl
                  << "length of ideal: "
                  << idealNoWhite.length() << std::endl
                  << "length of actual: "
                  << actualNoWhite.length() << std::endl;
        return -1;
    }

    std::cout << actualTestOutput << std::endl;

    return 0;
}
