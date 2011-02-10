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

#include <Alembic/MD5Hash/All.h>
#include <boost/format.hpp>
#include <boost/timer.hpp>
#include <iostream>

//-*****************************************************************************
using namespace Alembic::MD5Hash;

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// TIME TESTING!!!
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

// size of test block 
#define TEST_BLOCK_SIZE 1000

// number of blocks to process
#define TEST_BLOCKS 10000

// number of test bytes = TEST_BLOCK_SIZE * TEST_BLOCKS 
static size_t TEST_BYTES = (( size_t )TEST_BLOCK_SIZE) *
    (( size_t )TEST_BLOCKS);

//-*****************************************************************************
class TimeTrialDataProvider
{
public:
    TimeTrialDataProvider()
    {
        for ( int i = 0; i < TEST_BLOCK_SIZE; ++i )
        {
            m_data[i] = ( UCHAR )( i & 0xFF );
        }
        m_numBlocksLeft = TEST_BLOCKS;
    }

    const UCHAR *operator()( size_t &bytes )
    {
        if ( m_numBlocksLeft <= 0 )
        {
            bytes = 0;
            return NULL;
        }

        bytes = TEST_BLOCK_SIZE;
        --m_numBlocksLeft;
        return ( const UCHAR * )m_data;
    }

protected:
    UCHAR m_data[TEST_BLOCK_SIZE];
    int m_numBlocksLeft;
};  

//-*****************************************************************************
// A time trial routine, to measure the speed of MD5.
// Measures wall time required to digest TEST_BLOCKS * TEST_BLOCK_SIZE
// characters.
template <class MD5CLASS>
bool TimeTrial( std::ostream &ostr )
{
    boost::timer timer;

    // Initialize test data.
    UCHAR data[TEST_BLOCK_SIZE];
    for ( int i = 0; i < TEST_BLOCK_SIZE; ++i )
    {
        data[i] = ( UCHAR )( i & 0xFF );
    }

    // Start 'er up
    ostr << "MD5 time trial. Processing " << TEST_BYTES << " characters..."
         << std::endl;
    timer.restart();

    // Do it!
    MD5CLASS md5;
    for ( int block = 0; block < TEST_BLOCKS; ++block )
    {
        md5.update( ( const UCHAR * )data, ( size_t )TEST_BLOCK_SIZE );
    }
    MD5Digest digest = md5.digest();

    // Read timer.
    double elapsed = timer.elapsed();
    double bytesPerSecond = (( double )TEST_BYTES)/elapsed;
    ostr << digest << " is digest of test input." << std::endl
         << "Seconds to process test input: " << elapsed << std::endl
         << "Bytes processed per second: " << bytesPerSecond << std::endl;

    if ( elapsed < 1.0 )
    {
        ostr << "TIME TRIAL PASSED." << std::endl;
        return true;
    }
    else
    {
        ostr << "ERROR: TIME TRIAL TOOK TOO LONG" << std::endl;
        return false;
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// STANDARD TESTS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// Checks a single thing.
template <class MD5CLASS>
static bool TestString( std::ostream &ostr,
                        const std::string &inStr,
                        const std::string &expectedHash )
{
    MD5CLASS md5;
    std::string calcHash = ( md5 << inStr ).digest().str();
    if ( calcHash == expectedHash )
    {
        ostr << "PASS: MD5(\"" << inStr << "\") = \"" << calcHash
             << "\"" << std::endl;
        return true;
    }
    else
    {
        ostr << "ERROR: MD5(\"" << inStr << "\"), expected: \"" << expectedHash
             << "\", got: \"" << calcHash << "\"" << std::endl;
        return false;
    }
}

//-*****************************************************************************
// Runs a standard suite of test data.
template <class MD5CLASS>
bool TestSuite( std::ostream &ostr )
{
    ostr << "MD5 test suite results: " << std::endl;
    
    bool pass = true;
    pass = pass && TestString<MD5CLASS>( ostr,
                               "",
                               "d41d8cd98f00b204e9800998ecf8427e" );
    pass = pass && TestString<MD5CLASS>( ostr,
                               "a",
                               "0cc175b9c0f1b6a831c399e269772661" );
    
    pass = pass && TestString<MD5CLASS>( ostr,
                               "abc",
                               "900150983cd24fb0d6963f7d28e17f72" );
    
    pass = pass && TestString<MD5CLASS>( ostr,
                               "message digest",
                               "f96b697d7cb7938d525a2f31aaf161d0" );
    
    pass = pass && TestString<MD5CLASS>( ostr,
                               "abcdefghijklmnopqrstuvwxyz",
                               "c3fcd3d76192e4007dfb496cca67e13b" );
    
    pass = pass && TestString<MD5CLASS>( ostr,
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz0123456789",
                               "d174ab98d277d9f5a5611c2c9f419d9f" );
    
    pass = pass && TestString<MD5CLASS>( ostr,
                               "12345678901234567890"
                               "12345678901234567890"
                               "12345678901234567890"
                               "12345678901234567890",
                               "57edf4a22be3c955ac49da2e2107b67a" );

    return pass;
}

//-*****************************************************************************
// A.5 Test suite
//
//   The MD5 test suite (driver option "-x") should print the following
//   results:
//
// MD5 test suite:
// MD5("") = d41d8cd98f00b204e9800998ecf8427e
// MD5("a") = 0cc175b9c0f1b6a831c399e269772661
// MD5("abc") = 900150983cd24fb0d6963f7d28e17f72
// MD5("message digest") = f96b697d7cb7938d525a2f31aaf161d0
// MD5("abcdefghijklmnopqrstuvwxyz") = c3fcd3d76192e4007dfb496cca67e13b
// MD5("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") =
//       d174ab98d277d9f5a5611c2c9f419d9f
// MD5("123456789012345678901234567890123456789012345678901234567890123456
//       78901234567890") = 57edf4a22be3c955ac49da2e2107b67a
//-*****************************************************************************

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    bool pass = true;

    pass = pass && TimeTrial<MD5>( std::cout );
    pass = pass && TestSuite<MD5>( std::cout );

    return pass ? 0 : -1;
}
