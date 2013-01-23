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

using namespace boost::python;

template<class TPTraits> 
static object getPythonArray( AbcA::ArraySamplePtr& iSampPtr )
{
    typedef Abc::TypedArraySample<TPTraits> samp_type;
    typedef AbcU::shared_ptr<samp_type>     samp_type_ptr;

    samp_type_ptr typedSampPtr =
        AbcU::static_pointer_cast<samp_type>( iSampPtr );

    typename return_by_value::apply<samp_type_ptr>::type converter;

    return object( handle<>( converter( typedSampPtr ) ) );
}

//-*****************************************************************************
#define CASE_RETURN_ARRAY_VALUE( TPTraits, iSampPtr )      \
case TPTraits::pod_enum:                                   \
{                                                          \
    return getPythonArray<TPTraits>( iSampPtr );           \
}

//-*****************************************************************************
template<>
object getValue ( Abc::IArrayProperty &p, 
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

    return_by_value::apply<AbcU::Dimensions>::type converter;

    return object( handle<>( converter( oDim ) ) );
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
void register_iarrayproperty()
{
    // IBaseProperty
    register_IBaseProperty<AbcA::ArrayPropertyReaderPtr>(
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
                buff << getValue<Abc::IArrayProperty>( iProp, iSS, kReturnAll );
                valueStrings[i] = buff.str();
            };

            return ConvertStrings( valueStrings );
        }

    };

    // IArrayProperty
    //
    class_<Abc::IArrayProperty,
           bases<Abc::IBasePropertyT<AbcA::ArrayPropertyReaderPtr> > >(
        "IArrayProperty",
        "The IArrayProperty class is a array property reader",
        init<>( "Create an empty IArrayProperty" ) )
        .def( init<Abc::ICompoundProperty,
                   const std::string&,
                   optional<Abc::Argument,
                            Abc::Argument&> >(
                  ( arg( "parent" ), arg( "name" ), arg( "argument" ),
                    arg( "argument" ) ),
                    "Create a new IArrayProperty with the given parent "
                    "ICompoundProperty, namd and optionial arguments which can "
                    "be used to override the ErrorHandlingPolicy, to specify "
                    "protocol matching policy" ) )
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
        .add_property( "samples", Overloads::getAllSampleList )
        ;

    // List and Iterator for array samples
    //
    class_< SampleList<Abc::IArrayProperty> >
        ( "ArraySampleList", no_init )
        .def( "__len__", &SampleList<Abc::IArrayProperty>::len )
        .def( "__getitem__", &SampleList<Abc::IArrayProperty>::getItem )
        .def( "__iter__", &SampleList<Abc::IArrayProperty>::getIterator,
            return_value_policy<manage_new_object>() )
        ;

    class_<SampleIterator<Abc::IArrayProperty> >
        ( "ArraySampleIterator", no_init )
        .def ( "next", &SampleIterator<Abc::IArrayProperty>::next )
        ;
 }
