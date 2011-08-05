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

//-*****************************************************************************
//! \file lib/Alembic/Util/Tests/TokenMapTest.cpp
//! \brief Test functions for \ref Alembic::Util::TokenMap class
//-*****************************************************************************

#include <Alembic/Util/TokenMap.h>

namespace Alembic {
namespace Util {

//-*****************************************************************************
// Test that it reads a config correctly.
// Test that it errors on a bad config.
// Test that set works
// Test that setUnique works, and throws exception on non-unique
// Test that get works
// Test that get throws an exception if 'check' is indicated and there
// are token problems.
// Test that value/setValue work together.
//-*****************************************************************************

#define FAIL ALEMBIC_THROW( "TokenMap test failed, file: " \
                            << __FILE__ << ", line: "      \
                            << __LINE__ )

//-*****************************************************************************
// Test that it reads config.
static void Test1( void )
{
    std::string config =
        "george=washington;"
        "thomas=jefferson;"
        "james=madison;"
        "alexander=hamilton;"
        "benjamin=franklin";

    TokenMap tm( config );

    if ( tm.size() != 5 ) { FAIL; }

    if ( tm["george"] != "washington" ) { FAIL; }
    if ( tm["thomas"] != "jefferson" ) { FAIL; }
    if ( tm["james"] != "madison" ) { FAIL; }
    if ( tm["alexander"] != "hamilton" ) { FAIL; }
    if ( tm["benjamin"] != "franklin" ) { FAIL; }

    tm.setValue( "stanley", "sherman" );
    if ( tm["stanley"] != "sherman" ) { FAIL; }

    bool fail = false;
    try
    {
        tm.setUnique( config, ';', '=', false );
        // Should throw.
        fail = true;
    }
    catch ( ... )
    {
        // Pass
        fail = false;
    }
    if ( fail ) { FAIL; }

    std::string configOut =
        "alexander=hamilton;"
        "benjamin=franklin;"
        "george=washington;"
        "james=madison;"
        "stanley=sherman;"
        "thomas=jefferson";

    if ( tm.get() != configOut ) { FAIL; }

    fail = false;
    try
    {
        tm.get( 'x', '=', true );
        // Should throw.
        fail = true;
    }
    catch ( ... )
    {
        // PASS
        fail = false;
    }
    if ( fail ) { FAIL; }
}

} // End namespace Util
} // End namespace Alembic

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    try
    {
        Alembic::Util::Test1();
    }
    catch ( std::exception &exc )
    {
        std::cerr << "EXCEPTION: " << exc.what() << std::endl;
        return -1;
    }
    catch ( ... )
    {
        std::cerr << "UNKNOWN EXCEPTION" << std::endl;
        return -1;
    }
    
    return 0;
}

