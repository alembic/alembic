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

#ifndef _Alembic_Tako_Tests_Foundation_h_
#define _Alembic_Tako_Tests_Foundation_h_

#include <Alembic/HDF5/HDF5.h>

#include <boost/format.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <iostream>
#include <stdexcept>
#include <string>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

//-*****************************************************************************
#define TESTING_ASSERT( TEST )                                          \
do                                                                      \
{                                                                       \
    if ( !( TEST ) )                                                    \
    {                                                                   \
        std::string failedTest = BOOST_PP_STRINGIZE( TEST );            \
        throw std::runtime_error(                                       \
            ( boost::format( "ERROR: Failed Test: %s, File: %d, Line: %d" ) \
              % failedTest                                              \
              % __FILE__                                                \
              % __LINE__ ).str() );                                     \
    }                                                                   \
}                                                                       \
while( 0 )

//-*****************************************************************************
#define TESTING_ASSERT_THROW( TEST, EXCEPT )                            \
do                                                                      \
{                                                                       \
    bool passed = false;                                                \
    try                                                                 \
    {                                                                   \
        TEST ;                                                          \
    }                                                                   \
    catch ( EXCEPT )                                                    \
    {                                                                   \
        passed = true;                                                  \
    }                                                                   \
                                                                        \
    if ( !passed )                                                      \
    {                                                                   \
        std::string failedTest = BOOST_PP_STRINGIZE( TEST );            \
        throw std::runtime_error( "ERROR: Failed Throw: " + failedTest ); \
    }                                                                   \
}                                                                       \
while( 0 )

//-*****************************************************************************
#define TESTING_ASSERT_DOUBLES_EQUAL( FIRST, SECOND, TOLERANCE )        \
do                                                                      \
{                                                                       \
    const double first = ( double )( FIRST );                           \
    const double second = ( double )( SECOND );                         \
    const double diff = fabs( first - second );                         \
    const double tol = ( double )( TOLERANCE );                         \
    if ( diff > tol )                                                   \
    {                                                                   \
        throw std::runtime_error(                                       \
            ( boost::format(                                            \
                  "ERROR: Doubles %f, %f are not within tolerance: %f" ) \
              % first                                                   \
              % second                                                  \
              % tol ).str() );                                          \
    }                                                                   \
}                                                                       \
while( 0 )
        

#endif
