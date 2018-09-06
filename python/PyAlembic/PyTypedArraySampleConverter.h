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

#ifndef PyAlembic_PyTypedArraySampleConverter_h
#define PyAlembic_PyTypedArraySampleConverter_h

#include <Foundation.h>
#include <PyTypeBindingTraits.h>

namespace {

using namespace boost::python;

//-*****************************************************************************
template<class TPTraits>
struct converterMemcopyable
{
    typedef TypeBindingTraits<TPTraits>                 binding_traits;
    typedef typename binding_traits::python_array_type  array_type;
    typedef typename binding_traits::native_value_type  value_type;
    typedef Abc::TypedArraySample<TPTraits>             samp_type;
    typedef AbcU::shared_ptr<samp_type>                 samp_type_ptr;

    // Conversion from FixedArray to ArraySample
    void operator()( void *storage, array_type &iFixedArray, PyObject* obj_ptr )
    {
        BOOST_STATIC_ASSERT ( binding_traits::memCopyable );

        new (storage) samp_type( &iFixedArray.direct_index( 0 ),
                                 iFixedArray.len() );
    }

    // Converion from ArraySample to FixedArray
    PyObject* operator()( const samp_type& iSamp )
    {
        BOOST_STATIC_ASSERT ( binding_traits::memCopyable );

        // Create a fixed array.
        typename manage_new_object::apply<array_type *>::type converter;
        array_type* fixedArray = new array_type( iSamp.size() );
        object obj( handle<>( converter( fixedArray ) ) );

        // Memory layout is transparent between the arraysample type
        // and the fixedarray type, we just do a memcopy.
        memcpy( &( *fixedArray )[0],
                iSamp.get(),
                iSamp.size() * sizeof( value_type ) );

        return incref(obj.ptr());
    }

    // Converion from ArraySamplePtr to FixedArray
    PyObject* operator()( const samp_type_ptr& iSampPtr )
    {
        BOOST_STATIC_ASSERT ( binding_traits::memCopyable );

        if ( !iSampPtr )
        {
            Py_RETURN_NONE;
        }

        const samp_type& iSamp = *iSampPtr;

        // Create a fixed array.
        // The memcopy is avoided by storing a samp_type_ptr(i.e. a shared_ptr )
        // as a resource handle in a fixedArray.
        typename manage_new_object::apply<array_type *>::type converter;
        array_type* fixedArray = new array_type(
            static_cast<typename array_type::BaseType*>(
                const_cast<value_type*>( iSamp.get() ) ),
            iSamp.size(), 1, iSampPtr );

        object obj( handle<>( converter( fixedArray ) ) );

        return incref(obj.ptr());
    }
};

template<class T, class U>
inline void copyArray( const T& iFixedArray, U* oArray )
{
    for ( Py_ssize_t i = 0; i < iFixedArray.len(); ++i )
        oArray[i] = iFixedArray.direct_index( i );
}

inline void copyArray( const PyImath::StringArray& iFixedArray,
                       std::string* oArray )
{
    for (Py_ssize_t i = 0; i < iFixedArray.len(); ++i)
        oArray[i] = iFixedArray.getitem_string(i);
}

inline void copyArray( const PyImath::WstringArray& iFixedArray,
                       std::wstring* oArray )
{
    for (Py_ssize_t i = 0; i < iFixedArray.len(); ++i)
        oArray[i] = iFixedArray.getitem_string(i);
}

//-*****************************************************************************
template<class TPTraits>
struct converterNonMemCopyable
{

    typedef TypeBindingTraits<TPTraits>                 binding_traits;
    typedef typename binding_traits::python_array_type  array_type;
    typedef typename TPTraits::value_type               value_type;
    typedef Abc::TypedArraySample<TPTraits>             samp_type;
    typedef AbcU::shared_ptr<samp_type>                 samp_type_ptr;

    // Conversion from FixedArray to ArraySamplePtr
    void operator()( void *storage, array_type &iFixedArray, PyObject* obj_ptr )
    {
        BOOST_STATIC_ASSERT ( !binding_traits::memCopyable );

        const size_t len( iFixedArray.len() );

        value_type* array = new value_type[len];
        copyArray( iFixedArray, array );

        new ( storage ) samp_type_ptr( new samp_type( array, len ),
                                       AbcA::TArrayDeleter<value_type>() );
    }

    // Converion from ArraySample to FixedArray
    PyObject* operator()( const samp_type& iSamp )
    {
        BOOST_STATIC_ASSERT ( !binding_traits::memCopyable );

        // Create a fixed array.
        typename manage_new_object::apply<array_type *>::type converter;
        array_type* fixedArray = new array_type( iSamp.size() );
        object obj( handle<>( converter( fixedArray ) ) );

        // Memory layout is not transparent between the arraysample type
        // and the fixedarray type, we need to do explicit conversions
        // for array elements.
        for( size_t i = 0; i < iSamp.size(); ++i )
            fixedArray->direct_index( i ) = iSamp[i];

        return incref(obj.ptr());
    }

    // Converion from ArraySamplePtr to FixedArray
    PyObject* operator()( const samp_type_ptr& iSampPtr )
    {
        BOOST_STATIC_ASSERT ( !binding_traits::memCopyable );

        if ( !iSampPtr )
        {
            Py_RETURN_NONE;
        }

        return ( *this )( *iSampPtr );
    }
};

} // namespace

#endif


