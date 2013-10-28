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

#include <Alembic/Util/Dimensions.h>

int main( int, char** )
{
    using namespace Alembic::Util;

    //
    // Test fundamental BaseDimensions class
    //
    {
        typedef  BaseDimensions<int>   IntScalarDimension;

        IntScalarDimension rank0;

        IntScalarDimension rank0_val1( (int) 1);

        IntScalarDimension rank1;
        rank1.setRank( 1 );
    }

    //
    // Test Dimensions class
    //
    {
        Dimensions rank00;
        Dimensions rank0;

        Dimensions rank1( 1 );
        assert(rank1.rank() == 1);
        assert(rank1[0] == 1);
    }
    {
        Dimensions rank2;
        rank2.setRank(2);
        assert(rank2.rank() == 2);

        Alembic::Util::uint64_t *ptr = rank2.rootPtr();
        ptr[0] = 11;
        ptr[1] = 12;
        assert( rank2[0] == 11 );
        assert( rank2[1] == 12 );

        Dimensions rank2_copy(rank2);
        assert(rank2_copy.rank() == 2);

        assert( rank2_copy[0] == 11 );
        assert( rank2_copy[1] == 12 );

        assert( rank2_copy == rank2 );

        Dimensions rank3;
        rank3.setRank(3);
        ptr = rank3.rootPtr();
        ptr[0] = 20;
        ptr[1] = 21;
        ptr[2] = 22;

        rank2_copy = rank3;
        assert( rank2_copy == rank3 );
    }

    std::cout << "Success!" << std::endl;

    return 0;
}
