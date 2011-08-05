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

#include <CMakeHelloWorldUtil/HelloUtil.h>
#include <sstream>
#include <iostream>

//-*****************************************************************************
class HelloWorldTester : public CMakeHelloWorldUtil::HelloUtil
{
public:
    HelloWorldTester() : CMakeHelloWorldUtil::HelloUtil() {}

    void testPrint( std::ostream &ostr ) const
    {
        this->print( ostr );
    }
};

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    // There really isn't all that much to test about the HelloWorld class,
    // all it does is print hello world to a stream. So, let's make a
    // stringstream and compare what gets printed into it to what we're
    // expecting.
    HelloWorldTester hwt;
    std::string prototype = "Hello, World!";

    // First test.
    {
        std::stringstream sstr;
        hwt.testPrint( sstr );
        std::string test = sstr.str();
        if ( test != prototype )
        {
            std::cerr << "ERROR: Test failed. Expected: \""
                      << prototype << "\", but got: \""
                      << test << "\"" << std::endl;
            exit( -1 );
        }
    }

    // Second test.
    {
        std::stringstream sstr;
        sstr << hwt;
        std::string test = sstr.str();
        if ( test != prototype )
        {
            std::cerr << "ERROR: Test failed. Expected: \""
                      << prototype << "\", but got: \""
                      << test  << "\"" << std::endl;
            exit( -1 );
        }
    }

    std::cout << "All CMakeHelloWorldUtil_Test testing complete"
              << std::endl;

    // All is well.
    return 0;
}
