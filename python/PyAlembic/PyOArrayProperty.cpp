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
#include <PyOBaseProperty.h>
#include <PyTypeBindingTraits.h>

using namespace boost::python;

//-*****************************************************************************
#define CASE_SET_ARRAY_VALUE( TPTraits, iProp, iFixedArray )    \
case TPTraits::pod_enum:                                        \
{                                                               \
    typedef Abc::TypedArraySample<TPTraits> samp_type;          \
    typedef AbcU::shared_ptr<samp_type>     samp_ptr_type;      \
    if ( TypeBindingTraits<TPTraits>::memCopyable )             \
    {                                                           \
        iProp.set( extract<samp_type>( iFixedArray ) );         \
    }                                                           \
    else                                                        \
    {                                                           \
       samp_ptr_type sampPtr = extract<samp_ptr_type> ( iFixedArray ); \
       iProp.set( *sampPtr );                                   \
    }                                                           \
    return;                                                     \
}

//-*****************************************************************************
static void setArrayValue( Abc::OArrayProperty &p, PyObject *val )
{
    assert( val != 0 );

    const AbcA::DataType &dt = p.getDataType();
    const AbcU::PlainOldDataType pod = dt.getPod();
    const uint8_t extent = dt.getExtent();

    if( pod < 0 || pod >= AbcU::kNumPlainOldDataTypes )
    {
        std::stringstream stream;
        stream << "ERROR: Unknown type " << AbcU::PODName( pod )
               << " with extent " << (int)extent;
        throwPythonException( stream.str().c_str() );
    }

    if ( extent == 1 )
    {
        switch ( pod )
        {
            CASE_SET_ARRAY_VALUE( Abc::BooleanTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::Uint8TPTraits,   p, val );
            CASE_SET_ARRAY_VALUE( Abc::Int8TPTraits,    p, val );
            CASE_SET_ARRAY_VALUE( Abc::Uint16TPTraits,  p, val );
            CASE_SET_ARRAY_VALUE( Abc::Int16TPTraits,   p, val );
            CASE_SET_ARRAY_VALUE( Abc::Uint32TPTraits,  p, val );
            CASE_SET_ARRAY_VALUE( Abc::Int32TPTraits,   p, val );
            CASE_SET_ARRAY_VALUE( Abc::Uint64TPTraits,  p, val );
            CASE_SET_ARRAY_VALUE( Abc::Int64TPTraits,   p, val );
            CASE_SET_ARRAY_VALUE( Abc::Float16TPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::Float32TPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::Float64TPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::StringTPTraits,  p, val );
            CASE_SET_ARRAY_VALUE( Abc::WstringTPTraits, p, val );
            default:
            break;
        }
    }
    else if ( extent  == 2 )
    {
        switch ( pod )
        {
            CASE_SET_ARRAY_VALUE( Abc::V2sTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::V2iTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::V2fTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::V2dTPTraits, p, val );
            default:
            break;
        }
    }
    else if ( extent == 3 )
    {
        switch ( pod )
        {
            CASE_SET_ARRAY_VALUE( Abc::C3cTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::C3hTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::V3sTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::V3iTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::V3dTPTraits, p, val );
            case AbcU::kFloat32POD:
            {
                std::string interp (p.getMetaData().get ("interpretation"));
                if (!interp.compare (Abc::C3fTPTraits::interpretation()))
                {
                    p.set( extract<Abc::TypedArraySample<Abc::C3fTPTraits> >( val ) );
                    return;
                }
                else
                {
                    p.set( extract<Abc::TypedArraySample<Abc::V3fTPTraits> >( val ) );
                    return;
                }
            }
            default:
            break;
        }
    }
    else if ( extent == 4 )
    {
        switch ( pod )
        {
            CASE_SET_ARRAY_VALUE( Abc::C4cTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::C4hTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::Box2sTPTraits, p, val )
            CASE_SET_ARRAY_VALUE( Abc::Box2iTPTraits, p, val )
            case AbcU::kFloat32POD:
            {
                std::string interp (p.getMetaData().get ("interpretation"));
                if (!interp.compare (Abc::C4fTPTraits::interpretation()))
                {
                    p.set( extract<Abc::TypedArraySample<Abc::C4fTPTraits> >( val ) );
                    return;
                }
                else if (!interp.compare (Abc::QuatfTPTraits::interpretation()))
                {
                    p.set( extract<Abc::TypedArraySample<Abc::QuatfTPTraits> >( val ) );
                    return;
                }
                else if (!interp.compare (Abc::Box2fTPTraits::interpretation()))
                {
                    p.set( extract<Abc::TypedArraySample<Abc::Box2fTPTraits> >( val ) );
                    return;
                }
            }
            case AbcU::kFloat64POD:
            {
                std::string interp (p.getMetaData().get ("interpretation"));
                if (!interp.compare (Abc::QuatdTPTraits::interpretation()))
                {
                    p.set( extract<Abc::TypedArraySample<Abc::QuatdTPTraits> >( val ) );
                    return;
                }
                else if (!interp.compare (Abc::Box2dTPTraits::interpretation()))
                {
                    p.set( extract<Abc::TypedArraySample<Abc::Box2dTPTraits> >( val ) );
                    return;
                }
            }
            default:
            break;
        }
    }
    else if ( extent == 6 )
    {
        switch ( pod )
        {
            CASE_SET_ARRAY_VALUE( Abc::Box3sTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::Box3iTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::Box3fTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::Box3dTPTraits, p, val );
            default:
            break;
        }
    }
    else if ( extent == 9 )
    {
        switch ( pod )
        {
            CASE_SET_ARRAY_VALUE( Abc::M33fTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::M33dTPTraits, p, val );
            default:
            break;
        }
    }
    else if ( extent == 16 )
    {
        switch ( pod )
        {
            CASE_SET_ARRAY_VALUE( Abc::M44fTPTraits, p, val );
            CASE_SET_ARRAY_VALUE( Abc::M44dTPTraits, p, val );
            default:
            break;
        }
    }

    std::stringstream stream;
    stream << "ERROR: Unhandled type " << AbcU::PODName (pod)
           << " with extent " << (int)extent;
    throwPythonException( stream.str().c_str() );
}

//-*****************************************************************************
void register_oarrayproperty()
{
    // OBaseProperty
    register_OBaseProperty<AbcA::ArrayPropertyWriterPtr>(
                                                "OBaseProperty_Array" );

    // overloads
    //
    void ( Abc::OArrayProperty::*setTimeSamplingByIndex )( AbcU::uint32_t )
        = &Abc::OArrayProperty::setTimeSampling;
    void ( Abc::OArrayProperty::*setTimeSamplingByTimeSamplingPtr )
        ( AbcA::TimeSamplingPtr ) = &Abc::OArrayProperty::setTimeSampling;

    // OArrayProperty
    //
    class_<Abc::OArrayProperty,
           bases<Abc::OBasePropertyT<AbcA::ArrayPropertyWriterPtr> > >(
        "OArrayProperty",
        "The OArrayProperty class is an array property writer",
        init<>( "Create an empty OArrayProperty" ) )
        .def( init<Abc::OCompoundProperty,
                   const std::string&,
                   const AbcA::DataType&,
                   optional<
                   const Abc::Argument&,
                   const Abc::Argument&,
                   const Abc::Argument&> >(
                  ( arg( "parent" ), arg( "name" ), arg( "DataType" ), 
                    arg( "argument" ), arg( "argument" ), arg( "argument" ) ),
                    "Create a new OArrayProperty with the given parent "
                    "OCompoundProperty, name, DataType and optional arguments "
                    "which can be use to override the ErrorHandlingPolicy, to "
                    "specify MetaData, and to specify time sampling or time "
                    "sampling index" ) )
        .def( "getNumSamples",
              &Abc::OArrayProperty::getNumSamples,
              "Return the number of samples contained in this property" )
        .def( "setValue",
              &setArrayValue,
              ( arg( "array" ) ),
              "Set a sample with the given array" )
        .def( "setFromPrevious",
              &Abc::OArrayProperty::setFromPrevious,
              "Set a Sample from the previous sample" )
        .def( "setTimeSampling",
              setTimeSamplingByIndex,
              ( arg( "index" ) ),
              "Change the TimeSampling used by this property with the given "
              "index" )
        .def( "setTimeSampling",
              setTimeSamplingByTimeSamplingPtr,
              ( arg( "TimeSampling" ) ),
              "Change the TimeSampling used by this property with the given "
              "TimeSampling" )
        .def( "getParent",
              &Abc::OArrayProperty::getParent,
              "Return the parent OCompoundProperty" )
        ;
}
