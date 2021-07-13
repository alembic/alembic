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

#ifndef PyAlembic_PyOBaseProperty_h
#define PyAlembic_PyOBaseProperty_h

#include <Foundation.h>

//-*****************************************************************************
template <class PROP_PTR>
void register_OBaseProperty( py::module_& module_handle, const char* iName )
{
    using namespace py;

    typedef Abc::OBasePropertyT<PROP_PTR>       OBaseProperty;

    //OBaseProperty
    //
    class_<OBaseProperty>(
        module_handle,
        iName,
        "The OBaseProperty class is a property writer" )
        .def( "getHeader",
              &OBaseProperty::getHeader,
              "Return the header of this property",
              return_value_policy::reference_internal )
        .def( "getName",
              &OBaseProperty::getName,
              "Return the local name of this property",
              return_value_policy::copy )
        .def( "getPropertyType",
              &OBaseProperty::getPropertyType )
        .def( "isScalar",
              &OBaseProperty::isScalar,
              "Return True if this property is scalar" )
        .def( "isArray",
              &OBaseProperty::isArray,
              "Return True if this property is array" )
        .def( "isCompound",
              &OBaseProperty::isCompound,
              "Return True if this property is compound" )
        .def( "isSimple",
              &OBaseProperty::isSimple,
              "Return True if this property is simple (non-compound)" )
        .def( "getMetaData",
              &OBaseProperty::getMetaData,
              "Return the MetaData of this property",
              return_value_policy::reference_internal )
        .def( "getDataType",
              &OBaseProperty::getDataType,
              "Return the DataType of this property",
              return_value_policy::copy )
        .def( "getTimeSampling",
              &OBaseProperty::getTimeSampling,
              "Return the TimeSampling of this property" )
        .def( "getObject",
              &OBaseProperty::getObject,
              "Return this property's OObject" )
        .def( "reset",
              &OBaseProperty::reset,
              "Reset this property to an empty state" )
        .def( "valid", &OBaseProperty::valid )
        .def( "__str__", &OBaseProperty::getName,
              return_value_policy::copy )
        .def( ALEMBIC_PYTHON_BOOL_NAME, &OBaseProperty::valid )
        ;
 }

#endif
