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

#include <Alembic/Asset/Base/BaseAll.h>
#include <Alembic/MD5Hash/MD5.h>
#include <boost/random.hpp>
#include <boost/detail/endian.hpp>
#include <iostream>

namespace Abc = Alembic::Asset;

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    static const size_t NUM_VALUES = 1024 * 1024 * 8;

    // Allocate a slab
    Abc::DataType dtype( Abc::kFloat64POD, 1 );
    Abc::Slab sbuf( dtype, NUM_VALUES );
    double *data = ( double * )sbuf.rawData();
    std::cout << "Created double slab with: " << NUM_VALUES << " entries."
              << std::endl;

    // Use Boost Random to make some random numbers to fill the slab with.
    boost::mt19937 rng;
    boost::uniform_real<Abc::float64_t> rdist( -100.0, 100.0 );
    boost::variate_generator<boost::mt19937&,
        boost::uniform_real<Abc::float64_t> >
        randGen( rng, rdist );
    std::cout << "Created random number generator." << std::endl;

    for ( size_t i = 0; i < NUM_VALUES; ++i )
    {
        data[i] = randGen();
    }
    std::cout << "Filled slab with random numbers." << std::endl;

    // Use the slab buffer's type identification to compute a hashID.
    Abc::Slab::HashID hashID = sbuf.calculateHashID();
    std::cout << "Computed HashID: " << hashID << std::endl;

    // Now compute the hash ID manually.
    Alembic::MD5Hash::MD5 md5;
    md5.update( data, NUM_VALUES );
    
    Alembic::MD5Hash::MD5Digest md5Digest = md5.digest();
    std::cout << "Manual HashID: " << md5Digest << std::endl;

    // Expected output is:
    // Created double slab with: 8388608 entries.
    // Created random number generator.
    // Filled slab with random numbers.
    // Computed HashID: 1578be20ecdd3c1a53c58d93f0b632e5
    // Manual HashID: 1578be20ecdd3c1a53c58d93f0b632e5
    std::string expectedHashStr( "1578be20ecdd3c1a53c58d93f0b632e5" );
    if ( ( hashID.str() != md5Digest.str() ) ||
         ( hashID.str() != expectedHashStr ) )
    {
        std::cerr << "ERROR: FAILED test. Expected md5 hash: "
                  << expectedHashStr << " but got: "
                  << hashID.str() << std::endl;
        return -1;
    }

    // All's well that ends well.
    std::cout << "PASS. SlabBuffer Tests passed." << std::endl;

    return 0;
}
