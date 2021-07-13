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

template<class TPTraits>
static py::object getPythonArray( AbcA::ArraySamplePtr& iSampPtr )
{
    typedef Abc::TypedArraySample<TPTraits> samp_type;
    typedef AbcU::shared_ptr<samp_type>     samp_type_ptr;

    samp_type_ptr typedSampPtr =
        AbcU::static_pointer_cast<samp_type>( iSampPtr );

    //typename return_by_value::apply<samp_type_ptr>::type converter;


    return py::object( py::cast( typedSampPtr, return_value_policy::automatic ) );
}

//-*****************************************************************************
#define CASE_RETURN_ARRAY_VALUE( TPTraits, iSampPtr )      \
case TPTraits::pod_enum:                                   \
{                                                          \
    return getPythonArray<TPTraits>( iSampPtr );           \
}

//-*****************************************************************************
#define RETURN_ARRAY_VALUE( BaseType )                        \
{                                                             \
  std::vector< BaseType > samp(dims.numPoints() * extent);    \
  p.getAs( &samp.front(), iSS);                               \
  size_t width = size_t(dims.numPoints());                    \
  size_t height = size_t(extent) ;                            \
  return getNumPyArray<BaseType>(width, height, samp.data()); \
}                                                             \

//-*****************************************************************************
#define RETURN_STRING_ARRAY_VALUE( BaseType )      \
{                                                  \
  std::vector< BaseType > samp(dims.numPoints());  \
  p.getAs( &samp.front(), iSS);                    \
  return py::array(py::cast(samp));                \
}                                                  \


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

    AbcA::ArraySamplePtr ptr;
    p.get( ptr, iSS );

    switch ( pod )
    {
      // case Abc::BooleanTPTraits::pod_enum:
      //   RETURN_ARRAY_VALUE( bool );

      case Abc::Uint8TPTraits::pod_enum:
        RETURN_ARRAY_VALUE( uint8_t );

      case Abc::Int8TPTraits::pod_enum:
        RETURN_ARRAY_VALUE( int8_t );

      case Abc::Uint16TPTraits::pod_enum:
        RETURN_ARRAY_VALUE( uint16_t );

      case Abc::Int16TPTraits::pod_enum:
        RETURN_ARRAY_VALUE( int16_t );

      case Abc::Uint32TPTraits::pod_enum:
        RETURN_ARRAY_VALUE( uint32_t );

      case Abc::Int32TPTraits::pod_enum:
        RETURN_ARRAY_VALUE( int32_t );

      case Abc::Uint64TPTraits::pod_enum:
        RETURN_ARRAY_VALUE( uint64_t );

      case Abc::Int64TPTraits::pod_enum:
        RETURN_ARRAY_VALUE( int64_t );

      case Abc::Float16TPTraits::pod_enum:
        RETURN_ARRAY_VALUE( half );

      case Abc::Float32TPTraits::pod_enum:
        RETURN_ARRAY_VALUE( float );

      case Abc::Float64TPTraits::pod_enum:
        RETURN_ARRAY_VALUE( double );

      case Abc::StringTPTraits::pod_enum:
        RETURN_STRING_ARRAY_VALUE( std::string );

      case Abc::WstringTPTraits::pod_enum:
        RETURN_STRING_ARRAY_VALUE( std::wstring );
      default:
      break;
    }

    // return pylist
    // auto pylist = py::list();
    // for (size_t index = 0; index < samp.size(); index++)
    //   pylist.append(samp[index]);
    // return py::array(pylist);

    // return contigous arrays
    // return py::array_t<float>(
    //                 { samp.size() },
    //                 { sizeof(float) },
    //                 samp.data());

      // return 3f vectors array.
        // return py::array(py::buffer_info(
        //     samp.data(),                               /* Pointer to buffer */
        //     sizeof(float),                          /* Size of one scalar */
        //     py::format_descriptor<float>::format(), /* Python struct-style format descriptor */
        //     size_t(2),                                      /* Number of dimensions */
        //     { size_t(samp.size()/extent), size_t(extent) },                   /* Buffer dimensions */
        //     { sizeof(float) * extent, sizeof(float) }  /* Strides (in bytes) for each index */
        //   ));

    if (extent == 1)
    {
        switch ( pod )
        {
            CASE_RETURN_ARRAY_VALUE( Abc::BooleanTPTraits, ptr )
            CASE_RETURN_ARRAY_VALUE( Abc::Uint8TPTraits,   ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Int8TPTraits,    ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Uint16TPTraits,  ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Int16TPTraits,   ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Uint32TPTraits,  ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Int32TPTraits,   ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Uint64TPTraits,  ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Int64TPTraits,   ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Float16TPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Float32TPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Float64TPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::StringTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::WstringTPTraits, ptr );
            default:
            break;
        };
    }
    else if (extent == 2)
    {
        switch ( pod )
        {

            CASE_RETURN_ARRAY_VALUE( Abc::V2sTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::V2iTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::V2fTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::V2dTPTraits, ptr );
            default:
            break;
        };
    }
    else if (extent == 3)
    {
        switch ( pod )
        {
            CASE_RETURN_ARRAY_VALUE( Abc::C3cTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::V3sTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::V3iTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::C3hTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::V3dTPTraits, ptr );
            case AbcU::kFloat32POD:
            {
                std::string interp (p.getMetaData().get ("interpretation"));
                if (!interp.compare (Abc::C3fTPTraits::interpretation()))
                    return getPythonArray<Abc::C3fTPTraits> (ptr);
                else
                    return getPythonArray<Abc::V3fTPTraits> (ptr);
            }
            default:
            break;
        };
    }
    else if (extent == 4)
    {
        switch ( pod )
        {
            CASE_RETURN_ARRAY_VALUE( Abc::C4cTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::C4hTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Box2sTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Box2iTPTraits, ptr );
            case AbcU::kFloat32POD:
            {
                std::string interp (p.getMetaData().get ("interpretation"));
                if (!interp.compare (Abc::C4fTPTraits::interpretation()))
                    return getPythonArray<Abc::C4fTPTraits>( ptr );
                else if (!interp.compare (Abc::QuatfTPTraits::interpretation()))
                    return getPythonArray<Abc::QuatfTPTraits>( ptr );
                else if (!interp.compare (Abc::Box2fTPTraits::interpretation()))
                    return getPythonArray<Abc::Box2fTPTraits>( ptr );
            }
            case AbcU::kFloat64POD:
            {
                std::string interp (p.getMetaData().get ("interpretation"));
                if (!interp.compare (Abc::QuatdTPTraits::interpretation()))
                    return getPythonArray<Abc::QuatdTPTraits>( ptr );
                else if (!interp.compare (Abc::Box2dTPTraits::interpretation()))
                    return getPythonArray<Abc::Box2dTPTraits>( ptr );
            }
            default:
            break;
        };
    }
    else if (extent == 6)
    {
        switch ( pod )
        {
            CASE_RETURN_ARRAY_VALUE( Abc::Box3sTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Box3iTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Box3fTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::Box3dTPTraits, ptr );
            default:
            break;
        }
    }
    else if (extent == 9)
    {
        switch ( pod )
        {
            CASE_RETURN_ARRAY_VALUE( Abc::M33fTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::M33dTPTraits, ptr );
            default:
            break;
        }
    }
    else if (extent == 16)
    {
        switch ( pod )
        {
            CASE_RETURN_ARRAY_VALUE( Abc::M44fTPTraits, ptr );
            CASE_RETURN_ARRAY_VALUE( Abc::M44dTPTraits, ptr );
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
                    "ICompoundProperty, namd and optionial arguments which can "
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
