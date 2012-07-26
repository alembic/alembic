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
#include <PyTypedArraySampleConverter.h>
#include <PyTypeBindingTraits.h>

using namespace boost::python;

//-*****************************************************************************
template<class TPTraits>
struct FixedArrayToTypedArraySample
{
    typedef TypeBindingTraits<TPTraits>                 binding_traits;
    typedef typename binding_traits::python_array_type  array_type;
    typedef typename binding_traits::native_value_type  value_type;
    typedef Abc::TypedArraySample<TPTraits>             samp_type;
    typedef AbcU::shared_ptr<samp_type>                 samp_type_ptr;

    FixedArrayToTypedArraySample()
    {
        converter::registry::push_back( &convertible,
                                        &construct,
                                        type_id<samp_type>() );
    }

    static void * convertible( PyObject* obj_ptr )
    {
        extract<array_type *> x( obj_ptr );

        return x.check() ? obj_ptr : 0;
    }

    static void construct( PyObject* obj_ptr,
                           converter::rvalue_from_python_stage1_data *data )
    {

        extract<array_type *> x( obj_ptr );

        assert( x.check() );

        array_type* fixedArray( x() );

        void *storage = ( (converter::rvalue_from_python_storage<samp_type>*)
                           data)->storage.bytes;

        converterMemcopyable<TPTraits> converter;
        converter( storage, *fixedArray, obj_ptr );

        data->convertible = storage;
    }
};

//-*****************************************************************************
template<class TPTraits>
struct FixedArrayToTypedArraySamplePtr
{
    typedef TypeBindingTraits<TPTraits>                 binding_traits;
    typedef typename binding_traits::python_array_type  array_type;
    typedef typename binding_traits::native_value_type  value_type;
    typedef Abc::TypedArraySample<TPTraits>             samp_type;
    typedef AbcU::shared_ptr<samp_type>                 samp_type_ptr;

    FixedArrayToTypedArraySamplePtr()
    {
        converter::registry::push_back( &convertible,
                                        &construct,
                                        type_id<samp_type_ptr>() );
    }

    static void * convertible( PyObject* obj_ptr )
    {
        extract<array_type *> x( obj_ptr );

        return x.check() ? obj_ptr : 0;
    }

    static void construct( PyObject* obj_ptr,
                           converter::rvalue_from_python_stage1_data *data )
    {
        extract<array_type *> x( obj_ptr );

        assert( x.check() );

        array_type* fixedArray( x() );

        void *storage = ( (converter::rvalue_from_python_storage<samp_type>*)
                           data)->storage.bytes;

        converterNonMemCopyable<TPTraits> converter;
        converter( storage, *fixedArray, obj_ptr );

        data->convertible = storage;
    }
};

//-*****************************************************************************
template<class TPTraits>
struct TypedArraySampleToFixedArray
{
    typedef TypeBindingTraits<TPTraits>                 binding_traits;
    typedef typename binding_traits::python_array_type  array_type;
    typedef typename binding_traits::native_value_type  value_type;
    typedef Abc::TypedArraySample<TPTraits>             samp_type;
    typedef AbcU::shared_ptr<samp_type>                 samp_type_ptr;

    // Conversion from ArraySample to FixedArray
    static PyObject* convert( const samp_type &iSamp )
    {
        using namespace boost::mpl;
        typename if_<bool_<binding_traits::memCopyable>,
                     converterMemcopyable<TPTraits>,
                     converterNonMemCopyable<TPTraits> >::type  converter;

        return converter( iSamp );
    }

    // Conversion from ArraySamplePtr to FixedArray
    static PyObject* convert( const samp_type_ptr &iSampPtr )
    {
        if ( !iSampPtr )
        {
            Py_RETURN_NONE;
        }

        using namespace boost::mpl;
        typename if_<bool_<binding_traits::memCopyable>,
                     converterMemcopyable<TPTraits>,
                     converterNonMemCopyable<TPTraits> >::type  converter;

        return converter( iSampPtr );
    }
};

//-*****************************************************************************
template<class TPTraits>
struct StringArraySampleToFixedArray
{
     typedef TypeBindingTraits<TPTraits>                 binding_traits;
     typedef typename binding_traits::python_array_type array_type;
     typedef Abc::TypedArraySample<TPTraits>             samp_type;
     typedef AbcU::shared_ptr<samp_type>                 samp_type_ptr;

    static PyObject* convert( const samp_type &iSamp )
    {
        typename manage_new_object::apply<array_type *>::type converter;

        return incref( object( handle<>( converter ( 
                                            array_type::createFromRawArray(
                                            iSamp.get(),
                                            iSamp.size() ) ) ) ).ptr()
                     );    
    }
    static PyObject* convert( const samp_type_ptr &iSampPtr )
    {
        if ( !iSampPtr )
        {
            Py_RETURN_NONE;
        }

       return convert( *iSampPtr );
    }
};

//-*****************************************************************************
template<>
struct TypedArraySampleToFixedArray<Abc::StringTPTraits> :
    public StringArraySampleToFixedArray<Abc::StringTPTraits> {};

template<>
struct TypedArraySampleToFixedArray<Abc::WstringTPTraits> :
    public StringArraySampleToFixedArray<Abc::WstringTPTraits> {};

//-*****************************************************************************
template<class TPTraits>
static void register_()
{
    // Import case
    // to-python converter
    // ArraySample to FixedArray
    to_python_converter<
        Abc::TypedArraySample<TPTraits>,
        TypedArraySampleToFixedArray<TPTraits> >();

    // to-python converter
    // ArraySamplePtr to FixedArray
    to_python_converter<
        AbcU::shared_ptr<Abc::TypedArraySample<TPTraits> >,
        TypedArraySampleToFixedArray<TPTraits> >();

    // Export case
    // from-python converter
    // FixedArray to ArraySample or ArraySamplePtr
    using namespace boost::mpl;
    typedef TypeBindingTraits<TPTraits> binding_traits;

    typename if_<bool_<binding_traits::memCopyable>,
                       FixedArrayToTypedArraySample<TPTraits>,
                       FixedArrayToTypedArraySamplePtr<TPTraits> >::type from_pyton_converter;
}

//-*****************************************************************************
void register_typedarraysampleconverters()
{
    register_<Abc::BooleanTPTraits>();
    register_<Abc::Uint8TPTraits>();
    register_<Abc::Int8TPTraits>();
    register_<Abc::Uint16TPTraits>();
    register_<Abc::Int16TPTraits>();
    register_<Abc::Uint32TPTraits>();
    register_<Abc::Int32TPTraits>();
    register_<Abc::Uint64TPTraits>();
    register_<Abc::Int64TPTraits>();
    register_<Abc::Float16TPTraits>();
    register_<Abc::Float32TPTraits>();
    register_<Abc::Float64TPTraits>();
    register_<Abc::StringTPTraits>();
    register_<Abc::WstringTPTraits>();

    register_<Abc::V2sTPTraits>();
    register_<Abc::V2iTPTraits>();
    register_<Abc::V2fTPTraits>();
    register_<Abc::V2dTPTraits>();
    register_<Abc::P2sTPTraits>();
    register_<Abc::P2iTPTraits>();
    register_<Abc::P2fTPTraits>();
    register_<Abc::P2dTPTraits>();
    register_<Abc::N2fTPTraits>();
    register_<Abc::N2dTPTraits>();

    register_<Abc::C3cTPTraits>();
    register_<Abc::C3hTPTraits>();
    register_<Abc::C3fTPTraits>();
    register_<Abc::V3sTPTraits>();
    register_<Abc::V3iTPTraits>();
    register_<Abc::V3fTPTraits>();
    register_<Abc::V3dTPTraits>();
    register_<Abc::P3sTPTraits>();
    register_<Abc::P3iTPTraits>();
    register_<Abc::P3fTPTraits>();
    register_<Abc::P3dTPTraits>();
    register_<Abc::N3fTPTraits>();
    register_<Abc::N3dTPTraits>();

    register_<Abc::C4cTPTraits>();
    register_<Abc::C4hTPTraits>();
    register_<Abc::C4fTPTraits>();
    
    register_<Abc::Box2sTPTraits>();
    register_<Abc::Box2iTPTraits>();
    register_<Abc::Box2fTPTraits>();
    register_<Abc::Box2dTPTraits>();
    register_<Abc::QuatfTPTraits>();
    register_<Abc::QuatdTPTraits>();

    register_<Abc::Box3sTPTraits>();
    register_<Abc::Box3iTPTraits>();
    register_<Abc::Box3fTPTraits>();
    register_<Abc::Box3dTPTraits>();

    register_<Abc::M33fTPTraits>();
    register_<Abc::M33dTPTraits>();

    register_<Abc::M44fTPTraits>();
    register_<Abc::M44dTPTraits>();
}


