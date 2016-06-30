//-*****************************************************************************
//
// Copyright (c) 2012-2013,
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

#ifndef _PyAlembic_PyIGeomParam_h_
#define _PyAlembic_PyIGeomParam_h_

#include <Foundation.h>

template <class IGEOMPARAM>
static void register_( const char* iName )
{
    using namespace boost::python;

    // ITypedGeomParam
    //
    class_<IGEOMPARAM>(
           iName,
           "This class is a typed geom param reader.",
           init<>() )
        .def( "getInterpretation",
              &IGEOMPARAM::getInterpretation )
        .staticmethod( "getInterpretation" )
        .def( "matches",
              &IGEOMPARAM::matches,
              ( arg( "header" ),
                arg( "matchingSchema" ) = Abc::kStrictMatching ) )
        .staticmethod( "matches" )
        .def( init<Abc::ICompoundProperty,
                   const std::string&,
                   optional<
                   const Abc::Argument&,
                   const Abc::Argument&> >(
                   ( arg( "parent" ), arg( "name" ),
                     arg( "argument" ), arg( "argument" ) ),
                   "doc") )
        .def( "getIndexedValue",
              &IGEOMPARAM::getIndexedValue,
              ( arg( "iSampleSelector" ) = Abc::ISampleSelector() ) )
        .def( "getExpandedValue",
              &IGEOMPARAM::getExpandedValue,
              ( arg( "iSampleSelector" ) = Abc::ISampleSelector() ) )
        .def( "getNumSamples",
              &IGEOMPARAM::getNumSamples )
        .def( "getDataType",
              &IGEOMPARAM::getDataType )
        .def( "getArrayExtent",
              &IGEOMPARAM::getArrayExtent )
        .def( "isIndexed",
              &IGEOMPARAM::isIndexed )
        .def( "getScope",
              &IGEOMPARAM::getScope )
        .def( "getTimeSampling",
              &IGEOMPARAM::getTimeSampling )
        .def( "getName",
              &IGEOMPARAM::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getParent",
              &IGEOMPARAM::getParent,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getHeader",
              &IGEOMPARAM::getHeader,
              return_internal_reference<>() )
        .def( "getMetaData",
              &IGEOMPARAM::getMetaData,
              return_internal_reference<>() )
        .def( "isConstant",
              &IGEOMPARAM::isConstant )
        .def( "reset",
              &IGEOMPARAM::reset )
        .def( "valid",
              &IGEOMPARAM::valid )
        .def( "getValueProperty",
              &IGEOMPARAM::getValueProperty,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getIndexProperty",
              &IGEOMPARAM::getIndexProperty,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "__nonzero__", &IGEOMPARAM::valid )
       ;

    // IGEOMPARAM::Sample
    //
    std::string sampleName = std::string( iName ) + "Sample";
    class_<typename IGEOMPARAM::Sample>( sampleName.c_str(), init<>() )
        .def( "getIndices",
              &IGEOMPARAM::Sample::getIndices,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getVals",
              &IGEOMPARAM::Sample::getVals,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getScope",
              &IGEOMPARAM::Sample::getScope )
        .def( "isIndexed",
              &IGEOMPARAM::Sample::isIndexed )
        .def( "reset", &IGEOMPARAM::Sample::reset )
        .def( "valid", &IGEOMPARAM::Sample::valid )
        ;
}

#endif
