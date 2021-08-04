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
#include <PyTypeBindingUtil.h>

using namespace py;

//-*****************************************************************************
#define RETURN_ARRAY_VALUE( TPTraits)                                     \
{                                                                         \
  std::vector< TPTraits::value_type > samp(dims.numPoints() * extent);    \
  p.getAs( &samp.front(), iSS);                                           \
  size_t width = size_t(dims.numPoints());                                \
  size_t height = size_t(extent) ;                                        \
  return getNumPyArray<TPTraits::value_type>(width, height, samp.data()); \
}

//-*****************************************************************************
#define CASE_RETURN_ARRAY_VALUE( TPTraits)                                \
case TPTraits::pod_enum:                                                  \
{                                                                         \
  RETURN_ARRAY_VALUE(TPTraits)                                            \
}

//-*****************************************************************************
#define CASE_RETURN_STRING_ARRAY_VALUE( TPTraits)             \
case TPTraits::pod_enum:                                      \
{                                                             \
  std::vector< TPTraits::value_type > samp(extent);           \
  p.getAs( &samp.front(), iSS);                               \
  return py::array(py::cast(samp));                           \
}

//-*****************************************************************************
template<>
py::object getValue ( Abc::IArrayProperty &p,
                         const Abc::ISampleSelector &iSS,
                         const ReturnTypeEnum returnType )
{
    // Determine the type & extent of the array property and return its value.
    const AbcA::DataType &dt = p.getDataType();
    AbcU::PlainOldDataType pod = dt.getPod();
    const AbcU::uint8_t extent = dt.getExtent();
    AbcU::Dimensions dims;
    p.getDimensions( dims, iSS );

    // POD data types
    if( pod < 0 || pod >= AbcU::kNumPlainOldDataTypes )
    {
        std::stringstream stream;
        stream << "ERROR: Unhandled type " << AbcU::PODName (pod)
               << " with extent " << (int)extent;
        throwPythonException( stream.str().c_str() );
        return object(); // Returns None object
    }

    switch ( pod )
    {
        CASE_RETURN_ARRAY_VALUE( Abc::BooleanTPTraits);
        CASE_RETURN_ARRAY_VALUE( Abc::Uint8TPTraits );
        CASE_RETURN_ARRAY_VALUE( Abc::Int8TPTraits );
        CASE_RETURN_ARRAY_VALUE(Abc::Uint16TPTraits );
        CASE_RETURN_ARRAY_VALUE( Abc::Int16TPTraits );
        CASE_RETURN_ARRAY_VALUE(Abc::Uint32TPTraits );
        CASE_RETURN_ARRAY_VALUE(Abc::Int32TPTraits );
        CASE_RETURN_ARRAY_VALUE(Abc::Uint64TPTraits );
        CASE_RETURN_ARRAY_VALUE(Abc::Int64TPTraits );
        CASE_RETURN_ARRAY_VALUE(Abc::Float16TPTraits );
        CASE_RETURN_ARRAY_VALUE(Abc::Float32TPTraits );
        CASE_RETURN_ARRAY_VALUE(Abc::Float64TPTraits );
        CASE_RETURN_STRING_ARRAY_VALUE(Abc::StringTPTraits );
        CASE_RETURN_STRING_ARRAY_VALUE(Abc::WstringTPTraits );
        default:
        break;
    };

    std::stringstream stream;
    stream << "ERROR: Unhandled type " << AbcU::PODName (pod)
           << " with extent " << (int)extent;
    throwPythonException( stream.str().c_str() );

    return object(); // Returns None object
}

//-*****************************************************************************
static object getDimension( Abc::IArrayProperty& p,
                            const Abc::ISampleSelector& iSS )
{
    AbcU::Dimensions oDim;
    p.getDimensions( oDim, iSS );

    //return_by_value::apply<AbcU::Dimensions>::type converter;

    return py::object( py::cast( oDim, return_value_policy::automatic  ) );
}

//-*****************************************************************************
static std::string getKey( Abc::IArrayProperty &p,
                           const Abc::ISampleSelector &iSS )
{
    AbcA::ArraySampleKey oKey;
    if ( p.getKey( oKey, iSS ) ) {
        return oKey.digest.str();
    };
    return std::string();
}

//-*****************************************************************************
void register_iarrayproperty(py::module_& module_handle)
{
    // IBaseProperty
    register_IBaseProperty<AbcA::ArrayPropertyReaderPtr>(module_handle,
                                                "IBaseProperty_Array" );

    // Overloads
    struct Overloads
    {
        static object getAllValue( Abc::IArrayProperty& iProp,
                                const Abc::ISampleSelector &iSS )
        {
            return getValue<Abc::IArrayProperty>( iProp, iSS, kReturnAll );
        }

        static SampleList<Abc::IArrayProperty>
        getAllSampleList( Abc::IArrayProperty& iProp )
        {
            return getSampleList<Abc::IArrayProperty>( iProp, kReturnAll );
        }

        static object serialize( Abc::IArrayProperty& iProp,
                                      const Abc::ISampleSelector &iSS )
        {
            std::vector<std::string> valueStrings;

            for( size_t i = 0; i < iProp.getNumSamples(); ++i ) {
                std::stringstream buff;
                buff << getValue<Abc::IArrayProperty>( iProp, iSS, kReturnAll ).cast<std::stringstream*>();
                valueStrings[i] = buff.str();
            };

            return ConvertStrings( valueStrings );
        }

    };

    // IArrayProperty
    //
    class_<Abc::IArrayProperty,
          Abc::IBasePropertyT<AbcA::ArrayPropertyReaderPtr> >(
        module_handle,
        "IArrayProperty",
        "The IArrayProperty class is a array property reader" )
        .def( init<>(), "Create an empty IArrayProperty" )
        .def( init<Abc::ICompoundProperty,
                   const std::string&,
                   Abc::Argument, Abc::Argument >(),
                    arg( "parent" ), arg( "name" ), arg( "argument" ),
                    arg( "argument" ),
                    "Create a new IArrayProperty with the given parent "
                    "ICompoundProperty, named and optional arguments which can "
                    "be used to override the ErrorHandlingPolicy, to specify "
                    "protocol matching policy" )
        .def( "getNumSamples",
              &Abc::IArrayProperty::getNumSamples,
              "Return the number of samples contained in this property" )
        .def( "isConstant",
              &Abc::IArrayProperty::isConstant,
              "Return True if there's no change in value amongst samples" )
        .def( "isScalarLike",
              &Abc::IArrayProperty::isScalarLike,
              "Return True if 1 and only 1 element exist per sample" )
        .def( "getTimeSampling",
              &Abc::IArrayProperty::getTimeSampling,
              "Return the TimeSampling of this property" )
        .def( "getValue",
              Overloads::getAllValue,
              ( arg( "iSS" ) = Abc::ISampleSelector() ),
              "Return the sample with the given ISampleSelector" )
        .def( "getDimension", &getDimension )
        .def( "getParent",
              &Abc::IArrayProperty::getParent,
              "Return the parent ICompoundProperty" )
        .def( "getKey", &getKey )
        .def( "serialize",
              Overloads::serialize,
              ( arg( "iSS" ) = Abc::ISampleSelector() ),
              "Return the sample with the given ISampleSelector as a string" )
        .def( "samples", Overloads::getAllSampleList )
        ;

    // List and Iterator for array samples
    //
    class_< SampleList<Abc::IArrayProperty> >
        ( module_handle, "ArraySampleList" )
        .def( "__len__", &SampleList<Abc::IArrayProperty>::len )
        .def( "__getitem__", &SampleList<Abc::IArrayProperty>::getItem )
        .def( "__iter__", &SampleList<Abc::IArrayProperty>::getIterator,
            py::return_value_policy::take_ownership )
        ;

    class_<SampleIterator<Abc::IArrayProperty> >
        ( module_handle, "ArraySampleIterator" )
        .def ( ALEMBIC_PYTHON_NEXT_NAME, &SampleIterator<Abc::IArrayProperty>::next )
        ;
 }
