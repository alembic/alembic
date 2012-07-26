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

using namespace boost::python;

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

    typename return_by_value::apply<T>::type converter;

    return object( handle<>( converter( val ) ) );
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

    typename return_by_value::apply<samp_ptr_type>::type converter;

    return object( handle<>( converter( typedSampPtr ) ) );
}

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

    if ( iReturnType != kReturnArray )
    {

    if ( extent == 1 )
    {
        switch ( pod )
        {
            CASE_RETURN_POD_VALUE( Abc::BooleanTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Uint8TPTraits,   p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Int8TPTraits,    p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Uint16TPTraits,  p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Int16TPTraits,   p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Uint32TPTraits,  p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Int32TPTraits,   p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Uint64TPTraits,  p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Int64TPTraits,   p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Float16TPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Float32TPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Float64TPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::StringTPTraits,  p, iSS );
            CASE_RETURN_POD_VALUE( Abc::WstringTPTraits, p, iSS );
            default:
            break;
        };
    }
    if ( extent == 2 )
    {
        switch ( pod )
        {
            CASE_RETURN_POD_VALUE( Abc::V2sTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::V2iTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::V2fTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::V2dTPTraits, p, iSS );
            default:
            break;
        };
    }
    else if (extent == 3)
    {
        switch ( pod )
        {
            CASE_RETURN_POD_VALUE( Abc::C3cTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::V3sTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::V3iTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::C3hTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::V3dTPTraits, p, iSS );
            case AbcU::kFloat32POD:
            {
                std::string interp (p.getMetaData().get ("interpretation"));
                if (!interp.compare (Abc::C3fTPTraits::interpretation()))
                    return getPODValue< Abc::C3fTPTraits >( p, iSS ); 
                else
                    return getPODValue< Abc::V3fTPTraits >( p, iSS );
            }
            default:
            break;
        };
    }
    else if (extent == 4)
    {
        switch ( pod )
        {
            CASE_RETURN_POD_VALUE( Abc::C4cTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::C4hTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Box2sTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Box2iTPTraits, p, iSS );
            case AbcU::kFloat32POD:
            {
                std::string interp (p.getMetaData().get ("interpretation"));
                if (!interp.compare (Abc::C4fTPTraits::interpretation()))
                    return getPODValue< Abc::C4fTPTraits >( p, iSS ); 
                else if (!interp.compare (Abc::QuatfTPTraits::interpretation()))
                    return getPODValue< Abc::QuatfTPTraits >( p, iSS ); 
                else if (!interp.compare (Abc::Box2fTPTraits::interpretation()))
                    return getPODValue< Abc::Box2fTPTraits >( p, iSS ); 
            }
            case AbcU::kFloat64POD:
            {
                std::string interp (p.getMetaData().get ("interpretation"));
                if (!interp.compare (Abc::QuatdTPTraits::interpretation()))
                    return getPODValue< Abc::QuatdTPTraits >( p, iSS ); 
                else if (!interp.compare (Abc::Box2dTPTraits::interpretation()))
                    return getPODValue< Abc::Box2dTPTraits >( p, iSS ); 
            }
            default:
            break;
        };
    }
    else if (extent == 6)
    {
        switch ( pod )
        {
            CASE_RETURN_POD_VALUE( Abc::Box3sTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Box3iTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Box3fTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::Box3dTPTraits, p, iSS );
            default:
            break;
        };
    }
    else if (extent == 9)
    {
        switch ( pod )
        {
            CASE_RETURN_POD_VALUE( Abc::M33fTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::M33dTPTraits, p, iSS );
            default:
            break;
        }
    }
    else if (extent == 16)
    {
        switch ( pod )
        {
            CASE_RETURN_POD_VALUE( Abc::M44fTPTraits, p, iSS );
            CASE_RETURN_POD_VALUE( Abc::M44dTPTraits, p, iSS );
            default:
            break;
        }
    }

    }

    // Ok, this is a small sized array of pod values stored as an scalar value.
    // Let's return a python array.
    if ( iReturnType != kReturnScalar )
    {
        switch ( pod )
        {
            CASE_RETURN_ARRAY_VALUE( Abc::BooleanTPTraits, p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::Uint8TPTraits,   p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::Int8TPTraits,    p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::Uint16TPTraits,  p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::Int16TPTraits,   p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::Uint32TPTraits,  p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::Int32TPTraits,   p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::Uint64TPTraits,  p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::Int64TPTraits,   p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::Float16TPTraits, p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::Float32TPTraits, p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::Float64TPTraits, p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::StringTPTraits,  p, iSS, extent );
            CASE_RETURN_ARRAY_VALUE( Abc::WstringTPTraits, p, iSS, extent );
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
void register_iscalarproperty()
{
    // IBaseProperty
    register_IBaseProperty<AbcA::ScalarPropertyReaderPtr>(
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
           bases<Abc::IBasePropertyT<AbcA::ScalarPropertyReaderPtr> > >(
        "IScalarProperty",
        "The IScalarProperty class is a scalar property reader",
        init<>( "Create an empty IScalarProperty" ) )
        .def( init<Abc::ICompoundProperty, const std::string&,
                   optional<Abc::Argument&> >(
                  ( arg( "parent" ), arg( "name" ), arg ( "argument" ) ),
                  "Create a new IScalarProperty with the given parent "
                  "ICompoundProperty, name and optional argument which can be "
                  "used to override the ErrorHandlingPolicy" ) )
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
              ( arg( "iSS" ) = Abc::ISampleSelector() ),
              "Return the sample with the given ISampleSelector" )
        .def( "getScalarValue",
              Overloads::getScalarValue,
              ( arg( "iSS" ) = Abc::ISampleSelector() ),
              "Return the scalar sample with the given ISampleSelector" )
        .def( "getArrayValue",
              Overloads::getArrayValue,
              ( arg( "iSS" ) = Abc::ISampleSelector() ),
              "Return the array sample with the given ISampleSelector" )
        .add_property( "samples", Overloads::getAllSampleList )
        .add_property( "scalarSamples", Overloads::getScalarSampleList )
        .add_property( "arraySamples", Overloads::getArraySampleList )
        ;

    // List and Iterator for scalar samples
    //
    class_<SampleList<Abc::IScalarProperty> >
        ( "ScalarSampleList", no_init )
        .def( "__len__", &SampleList<Abc::IScalarProperty>::len )
        .def( "__getitem__", &SampleList<Abc::IScalarProperty>::getItem )
        .def( "__iter__", &SampleList<Abc::IScalarProperty>::getIterator,
              return_value_policy<manage_new_object>() )
        ;

    class_<SampleIterator<Abc::IScalarProperty> >
        ( "ScalarSampleIterator", no_init )
        .def( "next", &SampleIterator<Abc::IScalarProperty>::next )
        ;
 }
