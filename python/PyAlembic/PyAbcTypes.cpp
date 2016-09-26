//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <Foundation.h>

#include <boost/python/implicit.hpp>

using namespace boost::python;

//-*****************************************************************************
void register_abctypes()
{
    // Argument
    //
    class_<Abc::Argument>( "Argument", no_init )
        .def( init<AbcU::uint32_t>() )
        .def( init<const AbcA::MetaData&>() )
        .def( init<const AbcA::TimeSamplingPtr&>() )
        .def( init<Abc::SchemaInterpMatching>() )
        .def( init<Abc::SparseFlag>() )
        ;

    // Implicit conversions for Argument
    //
    implicitly_convertible<Abc::SparseFlag, Abc::Argument>();
    implicitly_convertible<Abc::SchemaInterpMatching, Abc::Argument>();
    implicitly_convertible<AbcU::uint32_t, Abc::Argument>();
    implicitly_convertible<const AbcA::MetaData&, Abc::Argument>();
    implicitly_convertible<const AbcA::TimeSamplingPtr&, Abc::Argument>();

    // SchemaInterpMatching Enum
    //
    enum_<Abc::SchemaInterpMatching>( "SchemaInterpMatching" )
        .value( "kStrictMatching", Abc::kStrictMatching )
        .value( "kNoMatching", Abc::kNoMatching )
        .value( "kSchemaTitleMatching", Abc::kSchemaTitleMatching )
        ;

    // WrapExistingFlag Enum
    enum_<Abc::WrapExistingFlag>( "WrapExistingFlag" )
        .value( "kWrapExisting", Abc::kWrapExisting )
        ;

    // SparseFlag Enum
    enum_<Abc::SparseFlag>( "SparseFlag" )
        .value( "kFull", Abc::kFull )
        .value( "kSparse", Abc::kSparse )
        ;
}
