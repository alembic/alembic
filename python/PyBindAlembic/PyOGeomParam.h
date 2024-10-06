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

#ifndef PyAlembic_PyOGeomParam_h
#define PyAlembic_PyOGeomParam_h

#include <Foundation.h>
#include <PyTypeBindingTraits.h>

template<class TRAITS>
class SampleDerived : public AbcG::OTypedGeomParam<TRAITS>::Sample
{
    public:
        typedef typename AbcG::OTypedGeomParam<TRAITS>::Sample super;
        typedef AbcU::shared_ptr<Abc::TypedArraySample<TRAITS> > samp_type_ptr;

        SampleDerived() : super() {}

        SampleDerived( const samp_type_ptr &iValsPtr,
                       const AbcG::GeometryScope iScope ) :
        super( *iValsPtr, iScope )
        {
            m_valsPtr = iValsPtr;
        }

        SampleDerived( const samp_type_ptr &iValsPtr,
                       const Abc::UInt32ArraySample &iIndices,
                       const AbcG::GeometryScope iScope ) :
        super( *iValsPtr, iIndices, iScope )
        {
            m_valsPtr = iValsPtr;
        }

        void setVals( const samp_type_ptr &iValsPtr )
        {
            m_valsPtr = iValsPtr;
            super::setVals( *m_valsPtr );
        }

    private:
        samp_type_ptr m_valsPtr;
};

typedef typename py::module_ MODULE;
template <class TRAITS>
struct registerMemCopyableSample
{
    void operator()( py::module_& module_handle, const char* iName )
    {
    using namespace py;
    typedef AbcG::OTypedGeomParam<TRAITS>               OGeomParam;

    //template <class TRAITS>
    struct OGeomParamOverloads
    {
      typename OGeomParam::Sample Sample;
      static void setVals(typename OGeomParam::Sample oSamp, py::array vals)
      {
        AbcU::Dimensions dims(vals.shape()[0]);
        std::vector<typename TRAITS::value_type> array_vals(vals.size());
        std::memcpy(array_vals.data(), vals.data(), vals.size() * sizeof(typename TRAITS::value_type));
        Abc::TypedArraySample<TRAITS> array_sample(array_vals.data(), dims);
        oSamp.setVals(array_sample);
        Abc::TypedArraySample<TRAITS> getVals = oSamp.getVals();
      }
    };

    class_<typename OGeomParam::Sample,  std::shared_ptr<typename OGeomParam::Sample>>(
        module_handle,
        iName )
        .def( init<>() )
        .def( init<Abc::TypedArraySample<TRAITS>,
                   AbcG::GeometryScope> (),
              keep_alive<1,2>() )
        .def( init<Abc::TypedArraySample<TRAITS>,
                   Abc::UInt32ArraySample,
                   AbcG::GeometryScope> (),
              keep_alive<1,2>(),
              keep_alive<1,3>() )
        .def( "setVals",
              &OGeomParam::Sample::setVals,
              keep_alive<1,2>() )
        .def( "setVals",
              OGeomParamOverloads::setVals,
              arg( "vals" ),
              keep_alive<1,2>() )
        .def( "getVals",
              &OGeomParam::Sample::getVals, return_value_policy::automatic,
              keep_alive<0,1>() )
        .def( "setIndices",
              &OGeomParam::Sample::setIndices,
              keep_alive<1,2>() )
        .def( "getIndices",
              &OGeomParam::Sample::getIndices,
              return_value_policy::copy )
        .def( "setScope",
              &OGeomParam::Sample::setScope,
              arg( "scope" ) )
        .def( "getScope",
              &OGeomParam::Sample::getScope )
        .def( "reset", &OGeomParam::Sample::reset )
        .def( "valid", &OGeomParam::Sample::valid )
        ;
    }
};

template <class TRAITS>
struct registerNonMemCopyableSample
{
    void operator()(  py::module_& module_handle, const char* iName )
    {
    using namespace py;
    typedef AbcU::shared_ptr<Abc::TypedArraySample<TRAITS> > samp_type_ptr;

    class_<SampleDerived<TRAITS> >(
        module_handle,
        iName )
        .def( init<>() )
        .def( init<samp_type_ptr,
                   AbcG::GeometryScope> (),
              keep_alive<1,2>() )
        .def( init<samp_type_ptr,
                   Abc::UInt32ArraySample,
                   AbcG::GeometryScope> (),
              keep_alive<1,2>(),
               keep_alive<1,2>() )
        .def( "setVals",
              &SampleDerived<TRAITS>::setVals,
              keep_alive<1,2>() )
        .def( "getVals",
              &SampleDerived<TRAITS>::getVals,
              keep_alive<0,1>() )
        .def( "setIndices",
              &SampleDerived<TRAITS>::setIndices,
              keep_alive<1,2>() )
        .def( "getIndices",
              &SampleDerived<TRAITS>::getIndices,
              return_value_policy::copy )
        .def( "setScope",
              &SampleDerived<TRAITS>::setScope,
              arg( "scope" ) )
        .def( "getScope",
              &SampleDerived<TRAITS>::getScope )
        .def( "reset", &SampleDerived<TRAITS>::reset )
        .def( "valid", &SampleDerived<TRAITS>::valid )
        ;
    }
};

template<class TRAITS>
struct Overloads
{
    public:
    typedef typename AbcG::OTypedGeomParam<TRAITS>               OGeomParam;
    typedef typename AbcG::OTypedGeomParam<TRAITS>::Sample       Sample;
/*
    static bool matchesMetaData( const AbcA::MetaData& iMetaData,
                                 Abc::SchemaInterpMatching iMatching )
    {
        return OGeomParam::matches( iMetaData, iMatching );
    }
    static bool matchesHeader( const AbcA::PropertyHeader& iHeader,
                               Abc::SchemaInterpMatching iMatching )
    {
        return OGeomParam::matches( iHeader, iMatching );
    }
*/
    static void set( OGeomParam& iParam, SampleDerived<TRAITS>& iSamp )
    {
        Sample samp( iSamp.getVals(), iSamp.getIndices(), iSamp.getScope() );

        iParam.set( samp );
    }
};

template <class TRAITS>
static void register_( py::module_& module_handle, const char* iName )
{
    using namespace py;

    typedef typename AbcG::OTypedGeomParam<TRAITS>               OGeomParam;

    // Overloads
    //
    void ( OGeomParam::*setTimeSamplingByIndex )( AbcU ::uint32_t )
        = &OGeomParam::setTimeSampling;
    void ( OGeomParam::*setTimeSamplingByTimeSamplingPtr )
        ( AbcA::TimeSamplingPtr ) = &OGeomParam::setTimeSampling;

    // OTypedGeomParam
    //
    class_<OGeomParam>(
        module_handle,
        iName,
        "This class is a typed geom param writer." )
        .def( init<>() )
        .def_static( "getInterpretation",
              &OGeomParam::getInterpretation )
        .def_static( "matches",
              &OGeomParam::matches,
              arg( "header" ),
              arg( "matchingSchema" ) = Abc::kStrictMatching )
        .def( init<Abc::OCompoundProperty,
                   const std::string&,
                   bool,
                   AbcG::GeometryScope,
                   size_t >(),
                   arg( "parent" ), arg( "name" ),
                   arg( "isIndexed" ), arg( "scope" ), arg( "extent" ),
                   "doc" )
        .def( init<Abc::OCompoundProperty,
                   const std::string&,
                   bool,
                   AbcG::GeometryScope,
                   size_t,
                   const Abc::Argument&,
                   const Abc::Argument&,
                   const Abc::Argument&>(),
                   arg( "parent" ), arg( "name" ),
                   arg( "isIndexed" ), arg( "scope" ), arg( "extent" ),
                   arg( "argument" ), arg( "argument" ), arg( "argument" ),
                   "doc" )
        .def( "set",
              &Overloads<TRAITS>::set,
              arg( "sample" ) )
        .def( "set",
              &OGeomParam::set,
              arg( "sample" ) )
        .def( "setFromPrevious",
              &OGeomParam::setFromPrevious )
        .def( "setTimeSampling",
              setTimeSamplingByIndex,
              arg( "index" ) )
        .def( "setTimeSampling",
              setTimeSamplingByTimeSamplingPtr,
              arg( "TimeSampling" ) )
        .def( "getNumSamples",
              &OGeomParam::getNumSamples )
        .def( "getDataType",
              &OGeomParam::getDataType )
        .def( "isIndexed",
              &OGeomParam::isIndexed )
        .def( "getScope",
              &OGeomParam::getScope )
        .def( "getTimeSampling",
              &OGeomParam::getTimeSampling )
        .def( "getName",
              &OGeomParam::getName,
              return_value_policy::copy )
        .def( "getValueProperty",
              &OGeomParam::getValueProperty )
        .def( "getIndexProperty",
              &OGeomParam::getIndexProperty )
        .def( "valid",
              &OGeomParam::valid )
        .def( "reset",
              &OGeomParam::reset )
        .def( ALEMBIC_PYTHON_BOOL_NAME, &OGeomParam::valid )
       ;

    // OGeomParam::Sample
    //
    //

    using namespace py;
    std::string sampleName = std::string( iName ) + "Sample";
    if ( TypeBindingTraits<TRAITS>::memCopyable )
    {
        registerMemCopyableSample<TRAITS> type_mem;
        type_mem(module_handle, sampleName.c_str());
    }
    else
    {
        registerNonMemCopyableSample<TRAITS> type_non_mem;
        type_non_mem(module_handle, sampleName.c_str());
    }
}

#endif
