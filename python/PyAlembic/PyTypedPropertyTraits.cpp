//-*****************************************************************************
//
// Copyright (c) 2012,
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
#include <PyTypeBindingTraits.h>

using namespace boost::python;

//-*****************************************************************************
template<class ArrayType>
static ArrayType* createArray( size_t iSize )
{
    return new ArrayType( iSize );
}

template <>
StringArray* createArray<StringArray>( size_t iSize )
{
    return StringArray::createDefaultArray( iSize );
}

template <>
WstringArray* createArray<WstringArray>( size_t iSize )
{
    return WstringArray::createDefaultArray( iSize );
}

//-*****************************************************************************
template<class TPTraits>
object getArrayType()
{
    typedef TypeBindingTraits<TPTraits>                 binding_traits;
    typedef typename binding_traits::python_array_type  array_type;

    typename manage_new_object::apply<array_type*>::type converter;

    array_type* array = createArray<array_type>( 1 ); 

    return object( handle<>( converter( array ) ) ).attr( "__class__" );
}

//-*****************************************************************************
#define REGISTER_TPTRAITS( TPTraits )                        \
{                                                            \
scope s = class_<Abc::TPTraits> ( #TPTraits, init<>() )      \
    .def( "interpretation", &Abc::TPTraits::interpretation,  \
    "Return the interpretation string" )                     \
    .staticmethod( "interpretation" )                        \
    .def( "dataType", &Abc::TPTraits::dataType,              \
    "Return the DataType of this property type" )            \
    .staticmethod( "dataType" )                              \
    ;                                                        \
s.attr( "arrayType" ) = getArrayType<Abc::TPTraits>();       \
}

//-*****************************************************************************
void register_typedpropertytraits()
{
    // Typed Property Traits
    REGISTER_TPTRAITS( BooleanTPTraits )
    REGISTER_TPTRAITS( Uint8TPTraits )
    REGISTER_TPTRAITS( Int8TPTraits )
    REGISTER_TPTRAITS( Uint16TPTraits )
    REGISTER_TPTRAITS( Int16TPTraits )
    REGISTER_TPTRAITS( Uint32TPTraits )
    REGISTER_TPTRAITS( Int32TPTraits )
    REGISTER_TPTRAITS( Uint64TPTraits )
    REGISTER_TPTRAITS( Int64TPTraits )
    REGISTER_TPTRAITS( Float16TPTraits )
    REGISTER_TPTRAITS( Float32TPTraits )
    REGISTER_TPTRAITS( Float64TPTraits )
    REGISTER_TPTRAITS( StringTPTraits )
    REGISTER_TPTRAITS( WstringTPTraits )

    REGISTER_TPTRAITS( V2sTPTraits )
    REGISTER_TPTRAITS( V2iTPTraits )
    REGISTER_TPTRAITS( V2fTPTraits )
    REGISTER_TPTRAITS( V2dTPTraits )
    REGISTER_TPTRAITS( P2sTPTraits )
    REGISTER_TPTRAITS( P2iTPTraits )
    REGISTER_TPTRAITS( P2fTPTraits )
    REGISTER_TPTRAITS( P2dTPTraits )
    REGISTER_TPTRAITS( N2fTPTraits )
    REGISTER_TPTRAITS( N2dTPTraits )

    REGISTER_TPTRAITS( C3cTPTraits )
    REGISTER_TPTRAITS( C3hTPTraits )
    REGISTER_TPTRAITS( C3fTPTraits )
    REGISTER_TPTRAITS( V3sTPTraits )
    REGISTER_TPTRAITS( V3iTPTraits )
    REGISTER_TPTRAITS( V3fTPTraits )
    REGISTER_TPTRAITS( V3dTPTraits )
    REGISTER_TPTRAITS( P3sTPTraits )
    REGISTER_TPTRAITS( P3iTPTraits )
    REGISTER_TPTRAITS( P3fTPTraits )
    REGISTER_TPTRAITS( P3dTPTraits )
    REGISTER_TPTRAITS( N3fTPTraits )
    REGISTER_TPTRAITS( N3dTPTraits )

    REGISTER_TPTRAITS( C4cTPTraits )
    REGISTER_TPTRAITS( C4hTPTraits )
    REGISTER_TPTRAITS( C4fTPTraits )
    
    REGISTER_TPTRAITS( Box2sTPTraits )
    REGISTER_TPTRAITS( Box2iTPTraits )
    REGISTER_TPTRAITS( Box2fTPTraits )
    REGISTER_TPTRAITS( Box2dTPTraits )
    REGISTER_TPTRAITS( QuatfTPTraits )
    REGISTER_TPTRAITS( QuatdTPTraits )

    REGISTER_TPTRAITS( Box3sTPTraits )
    REGISTER_TPTRAITS( Box3iTPTraits )
    REGISTER_TPTRAITS( Box3fTPTraits )
    REGISTER_TPTRAITS( Box3dTPTraits )

    REGISTER_TPTRAITS( M33fTPTraits )
    REGISTER_TPTRAITS( M33dTPTraits )

    REGISTER_TPTRAITS( M44fTPTraits )
    REGISTER_TPTRAITS( M44dTPTraits )
}
