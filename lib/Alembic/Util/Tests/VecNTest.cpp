//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#include <Alembic/Util/VecN.h>

using namespace Alembic::Util;

typedef VecN<float,7> Float7;
typedef VecN<double,7> Double7;

int main( int argc, char *argv[] )
{
    Float7 a;
    Float7 b( 2.0 );

    Float7 c = a * b;
    if ( a != c )
    {
        std::cerr << "C should be all zeros, as should A" << std::endl
                  << "C = " << c << std::endl
                  << "A = " << a << std::endl;
        exit( -1 );
    }

    float fval1[] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f };
    Float7 d( fval1 );

    float fval2[] = { 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f };
    Float7 e( fval2 );
    Float7 f = b + d;
    if ( e != f )
    {
        std::cerr << "E should be F, but they're not. " << std::endl
                  << "E = " << e << std::endl
                  << "F = " << f << std::endl;
        exit( -1 );
    }

    Float7 g = f / d;

    Double7 h( g );
    Float7 i( h );
    Float7 j( b );

    std::cout << "A = " << a << std::endl
              << "B = " << b << std::endl
              << "C = " << c << std::endl
              << "D = " << d << std::endl
              << "E = " << e << std::endl
              << "F = " << f << std::endl
              << "G = " << g << std::endl
              << "H = " << h << std::endl
              << "I = " << i << std::endl;

    std::cout << "Lame test, but passed for now." << std::endl;

    return 0;
}
