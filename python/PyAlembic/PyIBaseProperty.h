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

#ifndef PyAlembic_PyIBaseProperty_h
#define PyAlembic_PyIBaseProperty_h

#include <Foundation.h>

//-*****************************************************************************
template <class PROP_PTR>
void register_IBaseProperty( const char* iName )
{
    using namespace boost::python;

    typedef Abc::IBasePropertyT<PROP_PTR>       IBaseProperty;

    //IBaseProperty
    //
    class_<IBaseProperty>(
        iName,
        "The IBaseProperty class is a property reader",
        no_init )
        .def( "getHeader",
              &IBaseProperty::getHeader,
              "Return the header of this property",
              return_internal_reference<>() )
        .def( "getName",
              &IBaseProperty::getName,
              "Return the local name of this property",
              return_value_policy<copy_const_reference>() )
        .def( "getPropertyType",
              &IBaseProperty::getPropertyType )
        .def( "isScalar",
              &IBaseProperty::isScalar,
              "Return True if this property is scalar" )
        .def( "isArray",
              &IBaseProperty::isArray,
              "Return True if this property is array" )
        .def( "isCompound",
              &IBaseProperty::isCompound,
              "Return True if this property is compound" )
        .def( "isSimple",
              &IBaseProperty::isSimple,
              "Return True if this property is simple (non-compound)" )
        .def( "getMetaData",
              &IBaseProperty::getMetaData,
              "Return the MetaData of this property",
              return_internal_reference<>() )
        .def( "getDataType",
              &IBaseProperty::getDataType,
              "Return the DataType of this property",
              return_value_policy<copy_const_reference>() )
        .def( "getTimeSampling",
              &IBaseProperty::getTimeSampling,
              "Return the TimeSampling of this property" )
        .def( "getObject",
              &IBaseProperty::getObject,
              "Return this property's IObject" )
        .def( "reset",
              &IBaseProperty::reset,
              "Reset this property to an empty state" )
        .def( "valid", &IBaseProperty::valid )
        .def( "__str__", &IBaseProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &IBaseProperty::valid )
        ;
 }

#endif
