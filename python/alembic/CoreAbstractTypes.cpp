//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <boost/python/detail/wrap_python.hpp>

#include <Alembic/Abc/All.h>

#include <boost/python.hpp>


//#include <Python.h>

using namespace boost::python;

namespace AbcA = ::Alembic::AbcCoreAbstract::v1;

//-*****************************************************************************
void register_coreabstracttypes()
{
    // DataType
    class_<AbcA::DataType>( "DataType" )
        .def( "getExtent", &AbcA::DataType::getExtent )
        .def( "getNumBytes", &AbcA::DataType::getNumBytes )
        // see
        // http://mail.python.org/pipermail/cplusplus-sig/2004-February/006496.html
        // for explanation of "self_ns" use here
        .def( self_ns::str( self_ns::self ) )

        ;

    // MetaData
    class_<AbcA::MetaData>( "MetaData" )
        .def( "set", &AbcA::MetaData::set )
        .def( "setUnique", &AbcA::MetaData::setUnique )
        .def( "get", &AbcA::MetaData::get )
        .def( "getRequired", &AbcA::MetaData::getRequired )
        .def( "append", &AbcA::MetaData::append )
        .def( "appendUnique", &AbcA::MetaData::appendUnique )
        .def( "size", &AbcA::MetaData::size )
        .def( "matches", &AbcA::MetaData::matches )
        .def( "matchesOverlap", &AbcA::MetaData::matchesOverlap )
        .def( "matchesExactly", &AbcA::MetaData::matchesExactly )
        .def( "serialize", &AbcA::MetaData::serialize )
        .def( "__str__", &AbcA::MetaData::serialize )
        ;

    // ObjectHeader
    AbcA::MetaData & ( AbcA::ObjectHeader::*getNonConstMetaData )() = \
        &AbcA::ObjectHeader::getMetaData;

    class_<AbcA::ObjectHeader>( "ObjectHeader" )
        .def( "getName", &AbcA::ObjectHeader::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getFullName", &AbcA::ObjectHeader::getFullName,
              return_value_policy<copy_const_reference>() )
        .def( "getMetaData", getNonConstMetaData,
              return_internal_reference<1>() )
        .def( "__str__", &AbcA::ObjectHeader::getFullName,
              return_value_policy<copy_const_reference>() )
        ;

    // PropertyHeader
    class_<AbcA::PropertyHeader>( "PropertyHeader" )
        .def( "getName", &AbcA::PropertyHeader::getName,
              return_value_policy<copy_const_reference>() )
        .def( "isScalar", &AbcA::PropertyHeader::isScalar )
        .def( "isArray", &AbcA::PropertyHeader::isArray )
        .def( "isCompound", &AbcA::PropertyHeader::isCompound )
        .def( "isSimple", &AbcA::PropertyHeader::isSimple )
        .def( "getMetaData", &AbcA::PropertyHeader::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &AbcA::PropertyHeader::getDataType,
              return_internal_reference<1>() )
        .def( "__str__", &AbcA::PropertyHeader::getName,
              return_value_policy<copy_const_reference>() )
        ;

    // TimeSamplingType
    class_<AbcA::TimeSamplingType>( "TimeSamplingType" )
        .def( "isUniform", &AbcA::TimeSamplingType::isUniform )
        .def( "isCyclic", &AbcA::TimeSamplingType::isCyclic )
        .def( "isAcyclic", &AbcA::TimeSamplingType::isAcyclic )
        .def( "getNumSamplesPerCycle",
              &AbcA::TimeSamplingType::getNumSamplesPerCycle )
        .def( "getTimePerCycle", &AbcA::TimeSamplingType::getTimePerCycle )
        // see
        // http://mail.python.org/pipermail/cplusplus-sig/2004-February/006496.html
        // for explanation of "self_ns" use here
        .def( self_ns::str( self_ns::self ) )
        ;
}
