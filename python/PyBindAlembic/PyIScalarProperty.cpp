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
#include <PyIBaseProperty.h>
#include <PyIPropertyUtil.h>
#include <PyTypeBindingTraits.h>

using namespace py;

//-*****************************************************************************
template<class TPTraits>
static object getPODValue( Abc::IScalarProperty &p,
                           const Abc::ISampleSelector &iSS )
{
    typedef TypeBindingTraits<TPTraits> binding_traits;
    typedef typename binding_traits::python_value_type T;
    typedef typename binding_traits::native_value_type U;

    // Return the scalar property's value of type T.
    U val;
    p.get( reinterpret_cast<void*>( &val ), iSS );

    //typename return_by_value::apply<T>::type converter;
    return py::object( py::cast( val, return_value_policy::automatic ) );
}

//-*****************************************************************************
template<class TPTraits>
static object getSmallArrayValue( Abc::IScalarProperty &p,
                                  const Abc::ISampleSelector &iSS,
                                  size_t iExtent )
{
    typedef typename TPTraits::value_type U;
    typedef Abc::TypedArraySample<TPTraits> samp_type;
    typedef AbcU::shared_ptr<samp_type>     samp_ptr_type;

    // Get the scalar property's array value of type U as an ArraySample.
    AbcU::Dimensions dims( iExtent );
    AbcA::ArraySamplePtr sampPtr =
        AbcA::AllocateArraySample( TPTraits::dataType(), dims );
    p.get( const_cast<void*>( sampPtr->getData() ), iSS );

    samp_ptr_type typedSampPtr =
        AbcU::static_pointer_cast<samp_type>( sampPtr );
    //typename return_by_value::apply<samp_ptr_type>::type converter;

    return py::object( py::cast( typedSampPtr, return_value_policy::automatic ) );
}

//-*****************************************************************************
#define RETURN_POD_VALUE( BaseType )                        \
{                                                             \
  std::vector< BaseType > val(extent);                        \
  p.get( &val.front(), iSS);                                  \
  size_t width = size_t(1);                                   \
  size_t height = size_t(extent) ;                            \
  return getNumPyArray<BaseType>(height, width, val.data());  \
}                                                             \

//-*****************************************************************************
#define RETURN_POD_ARRAY_VALUE( BaseType ) \
{                                           \
  std::vector< BaseType > val(extent);      \
  p.get( &val.front(), iSS);                \
  return py::array(py::cast(val));          \
}                                           \

//-*****************************************************************************
#define RETURN_POD_STRING_ARRAY_VALUE( BaseType ) \
{                                           \
  std::vector< BaseType > val(extent);      \
  p.get( &val.front(), iSS);                \
  return py::array(py::cast(val));          \
}                                           \


//-*****************************************************************************
#define CASE_RETURN_POD_VALUE( TPTraits, PROP, SELECTOR ) \
case TPTraits::pod_enum:                                  \
    return getPODValue<TPTraits>( PROP, SELECTOR );

//-*****************************************************************************
#define CASE_RETURN_ARRAY_VALUE( TPTraits, PROP, SELECTOR, EXTENT ) \
case TPTraits::pod_enum:                                            \
    return getSmallArrayValue<TPTraits>( PROP, SELECTOR, EXTENT );

//-*****************************************************************************
template<>
object getValue<>( Abc::IScalarProperty &p,
                   const Abc::ISampleSelector &iSS,
                   const ReturnTypeEnum iReturnType )
{
    // Determine the type & extent of the scalar property and return its value.
    const AbcA::DataType &dt = p.getDataType();
    const AbcU::PlainOldDataType pod = dt.getPod();
    const AbcU ::uint8_t extent = dt.getExtent();

    // POD data types
    if ( pod < 0 || pod >= AbcU::kNumPlainOldDataTypes )
    {
        std::stringstream stream;
        stream << "ERROR: Unhandled type " << AbcU::PODName (pod)
               << " with extent " << (int)extent;
        throwPythonException( stream.str().c_str() );
        return object(); // Returns None object
    }

    // U val;
    // p.get( reinterpret_cast<void*>( &val ), iSS );

    if ( iReturnType != kReturnArray )
    {switch ( pod )
    {
      // case Abc::BooleanTPTraits::pod_enum:
      //   RETURN_ARRAY_VALUE( bool );

      case Abc::Uint8TPTraits::pod_enum:
        RETURN_POD_ARRAY_VALUE( uint8_t );

      case Abc::Int8TPTraits::pod_enum:
        RETURN_POD_VALUE( int8_t );

      case Abc::Uint16TPTraits::pod_enum:
        RETURN_POD_VALUE( uint16_t );

      case Abc::Int16TPTraits::pod_enum:
        RETURN_POD_VALUE( int16_t );

      case Abc::Uint32TPTraits::pod_enum:
        RETURN_POD_VALUE( uint32_t );

      case Abc::Int32TPTraits::pod_enum:
        RETURN_POD_VALUE( int32_t );

      case Abc::Uint64TPTraits::pod_enum:
        RETURN_POD_VALUE( uint64_t );

      case Abc::Int64TPTraits::pod_enum:
        RETURN_POD_VALUE( int64_t );

      case Abc::Float16TPTraits::pod_enum:
        RETURN_POD_VALUE( half );

      case Abc::Float32TPTraits::pod_enum:
        RETURN_POD_VALUE( float );

      case Abc::Float64TPTraits::pod_enum:
        RETURN_POD_VALUE( double );

      case Abc::StringTPTraits::pod_enum:
        RETURN_POD_ARRAY_VALUE( std::string );

      case Abc::WstringTPTraits::pod_enum:
        RETURN_POD_ARRAY_VALUE( std::wstring );
      default:
      break;
    }

    }

    // Ok, this is a small sized array of pod values stored as an scalar value.
    // Let's return a python array.
    if ( iReturnType != kReturnScalar )
    {
        switch ( pod )
        {
          case Abc::BooleanTPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( byte_t );

          case Abc::Uint8TPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( uint8_t );

          case Abc::Int8TPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( int8_t );

          case Abc::Uint16TPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( uint16_t );

          case Abc::Int16TPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( int16_t );

          case Abc::Uint32TPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( uint32_t );

          case Abc::Int32TPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( int32_t );

          case Abc::Uint64TPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( uint64_t );

          case Abc::Int64TPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( int64_t );

          case Abc::Float16TPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( half );

          case Abc::Float32TPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( float );

          case Abc::Float64TPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( double );

          case Abc::StringTPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( std::string );

          case Abc::WstringTPTraits::pod_enum:
            RETURN_POD_ARRAY_VALUE( std::wstring );
          default:
          break;
        }
    }

    std::stringstream stream;
    stream << "ERROR: Unhandled type " << AbcU::PODName (pod)
           << " with extent " << (int)extent;
    throwPythonException( stream.str().c_str() );

    return object(); // Returns None object
}

//-*****************************************************************************
void register_iscalarproperty( py::module_& module_handle )
{
    // IBaseProperty
    register_IBaseProperty<AbcA::ScalarPropertyReaderPtr>(module_handle,
                                                    "IBaseProperty_Scalar" );

    // Overloads
    struct Overloads
    {
        static object getAllValue( Abc::IScalarProperty& iProp,
                                   const Abc::ISampleSelector &iSS )
        {
            return getValue<Abc::IScalarProperty>( iProp, iSS, kReturnAll );
        }
        static object getScalarValue( Abc::IScalarProperty& iProp,
                                      const Abc::ISampleSelector &iSS )
        {
            return getValue<Abc::IScalarProperty>( iProp, iSS, kReturnScalar );
        }
        static object getArrayValue( Abc::IScalarProperty& iProp,
                                     const Abc::ISampleSelector &iSS )
        {
            return getValue<Abc::IScalarProperty>( iProp, iSS, kReturnArray );
        }

        static SampleList<Abc::IScalarProperty>
        getAllSampleList( Abc::IScalarProperty& iProp )
        {
            return getSampleList<Abc::IScalarProperty>( iProp, kReturnAll );
        }

        static SampleList<Abc::IScalarProperty>
        getScalarSampleList( Abc::IScalarProperty& iProp )
        {
            return getSampleList<Abc::IScalarProperty>( iProp, kReturnScalar );
        }

        static SampleList<Abc::IScalarProperty>
        getArraySampleList( Abc::IScalarProperty& iProp )
        {
            return getSampleList<Abc::IScalarProperty>( iProp, kReturnArray );
        }
    };

    // IScalarProperty
    //
    class_<Abc::IScalarProperty,
           Abc::IBasePropertyT<AbcA::ScalarPropertyReaderPtr> >(
        module_handle,
        "IScalarProperty",
        "The IScalarProperty class is a scalar property reader")
        .def( init<>(), "Create an empty IScalarProperty" )
        .def( init<Abc::ICompoundProperty, const std::string&,
                   Abc::Argument&>(),
                  arg( "parent" ), arg( "name" ), arg ( "argument" ),
                  "Create a new IScalarProperty with the given parent "
                  "ICompoundProperty, name and optional argument which can be "
                  "used to override the ErrorHandlingPolicy" )
        .def( "getTimeSampling",
              &Abc::IScalarProperty::getTimeSampling,
              "Return the TimeSampling of this property" )
        .def( "getNumSamples",
              &Abc::IScalarProperty::getNumSamples,
              "Return the number of samples contained in this property" )
        .def( "isConstant",
              &Abc::IScalarProperty::isConstant,
              "Return True if there's no change in value amongst samples" )
        .def( "getParent",
              &Abc::IScalarProperty::getParent,
              "Return the parent ICompoundProperty" )
        .def( "getValue",
              Overloads::getAllValue,
              arg( "iSS" ) = Abc::ISampleSelector(),
              "Return the sample with the given ISampleSelector" )
        .def( "getScalarValue",
              Overloads::getScalarValue,
              arg( "iSS" ) = Abc::ISampleSelector(),
              "Return the scalar sample with the given ISampleSelector" )
        .def( "getArrayValue",
              Overloads::getArrayValue,
              arg( "iSS" ) = Abc::ISampleSelector(),
              "Return the array sample with the given ISampleSelector" )
        .def( "samples", Overloads::getAllSampleList )
        .def( "scalarSamples", Overloads::getScalarSampleList )
        .def( "arraySamples", Overloads::getArraySampleList )
        ;

    // List and Iterator for scalar samples
    //
    class_<SampleList<Abc::IScalarProperty> >
        ( module_handle, "ScalarSampleList")
        .def( "__len__", &SampleList<Abc::IScalarProperty>::len )
        .def( "__getitem__", &SampleList<Abc::IScalarProperty>::getItem )
        .def( "__iter__",
              &SampleList<Abc::IScalarProperty>::getIterator,
              py::return_value_policy::take_ownership )
        ;

    class_<SampleIterator<Abc::IScalarProperty> >
        ( module_handle, "ScalarSampleIterator")
        .def( ALEMBIC_PYTHON_NEXT_NAME, &SampleIterator<Abc::IScalarProperty>::next )
        ;
 }
