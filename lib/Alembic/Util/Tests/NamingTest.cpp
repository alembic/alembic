//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#include <Alembic/Util/Naming.h>
#include <Alembic/Util/Foundation.h>

#include <string>
#include <vector>
#include <assert.h>

int main( int argc, char* argv[] )
{
    using namespace Alembic::Util;

    std::vector<std::string> stdNames;
    stdNames.push_back("abc");
    stdNames.push_back("abc8675309");
    stdNames.push_back("a");
    stdNames.push_back("under_score");
    stdNames.push_back("_underscore");
    stdNames.push_back("_underscore12345");
    stdNames.push_back("_");

    std::vector<std::string> nonStdNames;
    nonStdNames.push_back("");
    nonStdNames.push_back("1");
    nonStdNames.push_back("42abc");
    nonStdNames.push_back("Name With Spaces");
    nonStdNames.push_back("@#(#@$)(*");
    nonStdNames.push_back("Xform No. 0x1c # not 0xc1");
    nonStdNames.push_back("#");

    for (std::vector<std::string>::iterator it = stdNames.begin();
         it != stdNames.end();
         ++it)
    {
        assert(isStandardName(*it));
    }

    for (std::vector<std::string>::iterator it = nonStdNames.begin();
         it != nonStdNames.end();
         ++it)
    {
        std::string name = *it;

        assert(!isStandardName(name));

        makeStandardName(name);
        assert(isStandardName(name));
    }

    std::cout << "Success!" << std::endl;
    return 0;
}
