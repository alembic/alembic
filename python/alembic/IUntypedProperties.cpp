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

namespace Abc = ::Alembic::Abc;
namespace AbcA = ::Alembic::AbcCoreAbstract::v1;

//-*****************************************************************************
void register_iuntypedproperties()
{
    //IScalarProperty
    //
    class_<Abc::IScalarProperty>( "IScalarProperty",
                                init<Abc::ICompoundProperty,
                                const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IScalarProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IScalarProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IScalarProperty::isScalar )
        .def( "isArray", &Abc::IScalarProperty::isArray )
        .def( "isCompound", &Abc::IScalarProperty::isCompound )
        .def( "isSimple", &Abc::IScalarProperty::isSimple )
        .def( "getMetaData", &Abc::IScalarProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IScalarProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSampling", &Abc::IScalarProperty::getTimeSampling )
        .def( "getNumSamples", &Abc::IScalarProperty::getNumSamples )
        .def( "getObject", &Abc::IScalarProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IScalarProperty::reset )
        .def( "valid", &Abc::IScalarProperty::valid )
        .def( "__str__", &Abc::IScalarProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IScalarProperty::valid )
        ;

    //IArrayProperty
    //
    class_<Abc::IArrayProperty>( "IArrayProperty",
                                init<Abc::ICompoundProperty,
                                const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IArrayProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IArrayProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IArrayProperty::isScalar )
        .def( "isArray", &Abc::IArrayProperty::isArray )
        .def( "isCompound", &Abc::IArrayProperty::isCompound )
        .def( "isSimple", &Abc::IArrayProperty::isSimple )
        .def( "getMetaData", &Abc::IArrayProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IArrayProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSampling", &Abc::IArrayProperty::getTimeSampling )
        .def( "getNumSamples", &Abc::IArrayProperty::getNumSamples )
        .def( "getObject", &Abc::IArrayProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IArrayProperty::reset )
        .def( "valid", &Abc::IArrayProperty::valid )
        .def( "__str__", &Abc::IArrayProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IArrayProperty::valid )
        ;
}
