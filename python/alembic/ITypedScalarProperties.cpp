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
void register_itypedscalarproperties()
{
    // actually, let's just stick with untyped properties for now
    #if 0
    //IBoolProperty
    //
    class_<Abc::IBoolProperty>( "IBoolProperty",
                                init<Abc::ICompoundProperty,
                                const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IBoolProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IBoolProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IBoolProperty::isScalar )
        .def( "isArray", &Abc::IBoolProperty::isArray )
        .def( "isCompound", &Abc::IBoolProperty::isCompound )
        .def( "isSimple", &Abc::IBoolProperty::isSimple )
        .def( "getMetaData", &Abc::IBoolProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IBoolProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IBoolProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IBoolProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IBoolProperty::getNumSamples )
        .def( "getObject", &Abc::IBoolProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IBoolProperty::reset )
        .def( "valid", &Abc::IBoolProperty::valid )
        .def( "__str__", &Abc::IBoolProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IBoolProperty::valid )
        ;

    //IUcharProperty
    //
    class_<Abc::IUcharProperty>( "IUcharProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IUcharProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IUcharProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IUcharProperty::isScalar )
        .def( "isArray", &Abc::IUcharProperty::isArray )
        .def( "isCompound", &Abc::IUcharProperty::isCompound )
        .def( "isSimple", &Abc::IUcharProperty::isSimple )
        .def( "getMetaData", &Abc::IUcharProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IUcharProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IUcharProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IUcharProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IUcharProperty::getNumSamples )
        .def( "getObject", &Abc::IUcharProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IUcharProperty::reset )
        .def( "valid", &Abc::IUcharProperty::valid )
        .def( "__str__", &Abc::IUcharProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IUcharProperty::valid )
        ;

    //ICharProperty
    //
    class_<Abc::ICharProperty>( "ICharProperty",
                                init<Abc::ICompoundProperty,
                                const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::ICharProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::ICharProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::ICharProperty::isScalar )
        .def( "isArray", &Abc::ICharProperty::isArray )
        .def( "isCompound", &Abc::ICharProperty::isCompound )
        .def( "isSimple", &Abc::ICharProperty::isSimple )
        .def( "getMetaData", &Abc::ICharProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::ICharProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::ICharProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::ICharProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::ICharProperty::getNumSamples )
        .def( "getObject", &Abc::ICharProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::ICharProperty::reset )
        .def( "valid", &Abc::ICharProperty::valid )
        .def( "__str__", &Abc::ICharProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::ICharProperty::valid )
        ;

    //IUInt16Property
    //
    class_<Abc::IUInt16Property>( "IUInt16Property",
                                  init<Abc::ICompoundProperty,
                                  const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IUInt16Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IUInt16Property::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IUInt16Property::isScalar )
        .def( "isArray", &Abc::IUInt16Property::isArray )
        .def( "isCompound", &Abc::IUInt16Property::isCompound )
        .def( "isSimple", &Abc::IUInt16Property::isSimple )
        .def( "getMetaData", &Abc::IUInt16Property::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IUInt16Property::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IUInt16Property::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IUInt16Property::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IUInt16Property::getNumSamples )
        .def( "getObject", &Abc::IUInt16Property::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IUInt16Property::reset )
        .def( "valid", &Abc::IUInt16Property::valid )
        .def( "__str__", &Abc::IUInt16Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IUInt16Property::valid )
        ;

    //IInt16Property
    //
    class_<Abc::IInt16Property>( "IInt16Property",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IInt16Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IInt16Property::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IInt16Property::isScalar )
        .def( "isArray", &Abc::IInt16Property::isArray )
        .def( "isCompound", &Abc::IInt16Property::isCompound )
        .def( "isSimple", &Abc::IInt16Property::isSimple )
        .def( "getMetaData", &Abc::IInt16Property::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IInt16Property::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IInt16Property::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IInt16Property::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IInt16Property::getNumSamples )
        .def( "getObject", &Abc::IInt16Property::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IInt16Property::reset )
        .def( "valid", &Abc::IInt16Property::valid )
        .def( "__str__", &Abc::IInt16Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IInt16Property::valid )
        ;

    //IUInt32Property
    //
    class_<Abc::IUInt32Property>( "IUInt32Property",
                                  init<Abc::ICompoundProperty,
                                  const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IUInt32Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IUInt32Property::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IUInt32Property::isScalar )
        .def( "isArray", &Abc::IUInt32Property::isArray )
        .def( "isCompound", &Abc::IUInt32Property::isCompound )
        .def( "isSimple", &Abc::IUInt32Property::isSimple )
        .def( "getMetaData", &Abc::IUInt32Property::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IUInt32Property::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IUInt32Property::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IUInt32Property::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IUInt32Property::getNumSamples )
        .def( "getObject", &Abc::IUInt32Property::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IUInt32Property::reset )
        .def( "valid", &Abc::IUInt32Property::valid )
        .def( "__str__", &Abc::IUInt32Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IUInt32Property::valid )
        ;

    //IInt32Property
    //
    class_<Abc::IInt32Property>( "IInt32Property",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IInt32Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IInt32Property::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IInt32Property::isScalar )
        .def( "isArray", &Abc::IInt32Property::isArray )
        .def( "isCompound", &Abc::IInt32Property::isCompound )
        .def( "isSimple", &Abc::IInt32Property::isSimple )
        .def( "getMetaData", &Abc::IInt32Property::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IInt32Property::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IInt32Property::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IInt32Property::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IInt32Property::getNumSamples )
        .def( "getObject", &Abc::IInt32Property::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IInt32Property::reset )
        .def( "valid", &Abc::IInt32Property::valid )
        .def( "__str__", &Abc::IInt32Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IInt32Property::valid )
        ;

    //IUInt64Property
    //
    class_<Abc::IUInt64Property>( "IUInt64Property",
                                  init<Abc::ICompoundProperty,
                                  const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IUInt64Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IUInt64Property::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IUInt64Property::isScalar )
        .def( "isArray", &Abc::IUInt64Property::isArray )
        .def( "isCompound", &Abc::IUInt64Property::isCompound )
        .def( "isSimple", &Abc::IUInt64Property::isSimple )
        .def( "getMetaData", &Abc::IUInt64Property::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IUInt64Property::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IUInt64Property::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IUInt64Property::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IUInt64Property::getNumSamples )
        .def( "getObject", &Abc::IUInt64Property::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IUInt64Property::reset )
        .def( "valid", &Abc::IUInt64Property::valid )
        .def( "__str__", &Abc::IUInt64Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IUInt64Property::valid )
        ;

    //IInt64Property
    //
    class_<Abc::IInt64Property>( "IInt64Property",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IInt64Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IInt64Property::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IInt64Property::isScalar )
        .def( "isArray", &Abc::IInt64Property::isArray )
        .def( "isCompound", &Abc::IInt64Property::isCompound )
        .def( "isSimple", &Abc::IInt64Property::isSimple )
        .def( "getMetaData", &Abc::IInt64Property::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IInt64Property::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IInt64Property::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IInt64Property::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IInt64Property::getNumSamples )
        .def( "getObject", &Abc::IInt64Property::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IInt64Property::reset )
        .def( "valid", &Abc::IInt64Property::valid )
        .def( "__str__", &Abc::IInt64Property::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IInt64Property::valid )
        ;

    //IHalfProperty
    //
    class_<Abc::IHalfProperty>( "IHalfProperty",
                                init<Abc::ICompoundProperty,
                                const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IHalfProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IHalfProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IHalfProperty::isScalar )
        .def( "isArray", &Abc::IHalfProperty::isArray )
        .def( "isCompound", &Abc::IHalfProperty::isCompound )
        .def( "isSimple", &Abc::IHalfProperty::isSimple )
        .def( "getMetaData", &Abc::IHalfProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IHalfProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IHalfProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IHalfProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IHalfProperty::getNumSamples )
        .def( "getObject", &Abc::IHalfProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IHalfProperty::reset )
        .def( "valid", &Abc::IHalfProperty::valid )
        .def( "__str__", &Abc::IHalfProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IHalfProperty::valid )
        ;

    //IFloatProperty
    //
    class_<Abc::IFloatProperty>( "IFloatProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IFloatProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IFloatProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IFloatProperty::isScalar )
        .def( "isArray", &Abc::IFloatProperty::isArray )
        .def( "isCompound", &Abc::IFloatProperty::isCompound )
        .def( "isSimple", &Abc::IFloatProperty::isSimple )
        .def( "getMetaData", &Abc::IFloatProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IFloatProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IFloatProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IFloatProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IFloatProperty::getNumSamples )
        .def( "getObject", &Abc::IFloatProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IFloatProperty::reset )
        .def( "valid", &Abc::IFloatProperty::valid )
        .def( "__str__", &Abc::IFloatProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IFloatProperty::valid )
        ;

    //IDoubleProperty
    //
    class_<Abc::IDoubleProperty>( "IDoubleProperty",
                                  init<Abc::ICompoundProperty,
                                  const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IDoubleProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IDoubleProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IDoubleProperty::isScalar )
        .def( "isArray", &Abc::IDoubleProperty::isArray )
        .def( "isCompound", &Abc::IDoubleProperty::isCompound )
        .def( "isSimple", &Abc::IDoubleProperty::isSimple )
        .def( "getMetaData", &Abc::IDoubleProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IDoubleProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IDoubleProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IDoubleProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IDoubleProperty::getNumSamples )
        .def( "getObject", &Abc::IDoubleProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IDoubleProperty::reset )
        .def( "valid", &Abc::IDoubleProperty::valid )
        .def( "__str__", &Abc::IDoubleProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IDoubleProperty::valid )
        ;

    //IStringProperty
    //
    class_<Abc::IStringProperty>( "IStringProperty",
                                  init<Abc::ICompoundProperty,
                                  const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IStringProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IStringProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IStringProperty::isScalar )
        .def( "isArray", &Abc::IStringProperty::isArray )
        .def( "isCompound", &Abc::IStringProperty::isCompound )
        .def( "isSimple", &Abc::IStringProperty::isSimple )
        .def( "getMetaData", &Abc::IStringProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IStringProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IStringProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IStringProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IStringProperty::getNumSamples )
        .def( "getObject", &Abc::IStringProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IStringProperty::reset )
        .def( "valid", &Abc::IStringProperty::valid )
        .def( "__str__", &Abc::IStringProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IStringProperty::valid )
        ;

    //IWstringProperty
    //
    class_<Abc::IWstringProperty>( "IWstringProperty",
                                   init<Abc::ICompoundProperty,
                                   const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IWstringProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IWstringProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IWstringProperty::isScalar )
        .def( "isArray", &Abc::IWstringProperty::isArray )
        .def( "isCompound", &Abc::IWstringProperty::isCompound )
        .def( "isSimple", &Abc::IWstringProperty::isSimple )
        .def( "getMetaData", &Abc::IWstringProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IWstringProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IWstringProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IWstringProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IWstringProperty::getNumSamples )
        .def( "getObject", &Abc::IWstringProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IWstringProperty::reset )
        .def( "valid", &Abc::IWstringProperty::valid )
        .def( "__str__", &Abc::IWstringProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IWstringProperty::valid )
        ;

    //IV2sProperty
    //
    class_<Abc::IV2sProperty>( "IV2sProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IV2sProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IV2sProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IV2sProperty::isScalar )
        .def( "isArray", &Abc::IV2sProperty::isArray )
        .def( "isCompound", &Abc::IV2sProperty::isCompound )
        .def( "isSimple", &Abc::IV2sProperty::isSimple )
        .def( "getMetaData", &Abc::IV2sProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IV2sProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IV2sProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IV2sProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IV2sProperty::getNumSamples )
        .def( "getObject", &Abc::IV2sProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IV2sProperty::reset )
        .def( "valid", &Abc::IV2sProperty::valid )
        .def( "__str__", &Abc::IV2sProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IV2sProperty::valid )
        ;

    //IV2iProperty
    //
    class_<Abc::IV2iProperty>( "IV2iProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IV2iProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IV2iProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IV2iProperty::isScalar )
        .def( "isArray", &Abc::IV2iProperty::isArray )
        .def( "isCompound", &Abc::IV2iProperty::isCompound )
        .def( "isSimple", &Abc::IV2iProperty::isSimple )
        .def( "getMetaData", &Abc::IV2iProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IV2iProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IV2iProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IV2iProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IV2iProperty::getNumSamples )
        .def( "getObject", &Abc::IV2iProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IV2iProperty::reset )
        .def( "valid", &Abc::IV2iProperty::valid )
        .def( "__str__", &Abc::IV2iProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IV2iProperty::valid )
        ;

    //IV2fProperty
    //
    class_<Abc::IV2fProperty>( "IV2fProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IV2fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IV2fProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IV2fProperty::isScalar )
        .def( "isArray", &Abc::IV2fProperty::isArray )
        .def( "isCompound", &Abc::IV2fProperty::isCompound )
        .def( "isSimple", &Abc::IV2fProperty::isSimple )
        .def( "getMetaData", &Abc::IV2fProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IV2fProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IV2fProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IV2fProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IV2fProperty::getNumSamples )
        .def( "getObject", &Abc::IV2fProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IV2fProperty::reset )
        .def( "valid", &Abc::IV2fProperty::valid )
        .def( "__str__", &Abc::IV2fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IV2fProperty::valid )
        ;

    //IV2dProperty
    //
    class_<Abc::IV2dProperty>( "IV2dProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IV2dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IV2dProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IV2dProperty::isScalar )
        .def( "isArray", &Abc::IV2dProperty::isArray )
        .def( "isCompound", &Abc::IV2dProperty::isCompound )
        .def( "isSimple", &Abc::IV2dProperty::isSimple )
        .def( "getMetaData", &Abc::IV2dProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IV2dProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IV2dProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IV2dProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IV2dProperty::getNumSamples )
        .def( "getObject", &Abc::IV2dProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IV2dProperty::reset )
        .def( "valid", &Abc::IV2dProperty::valid )
        .def( "__str__", &Abc::IV2dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IV2dProperty::valid )
        ;

    //IV3sProperty
    //
    class_<Abc::IV3sProperty>( "IV3sProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IV3sProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IV3sProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IV3sProperty::isScalar )
        .def( "isArray", &Abc::IV3sProperty::isArray )
        .def( "isCompound", &Abc::IV3sProperty::isCompound )
        .def( "isSimple", &Abc::IV3sProperty::isSimple )
        .def( "getMetaData", &Abc::IV3sProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IV3sProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IV3sProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IV3sProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IV3sProperty::getNumSamples )
        .def( "getObject", &Abc::IV3sProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IV3sProperty::reset )
        .def( "valid", &Abc::IV3sProperty::valid )
        .def( "__str__", &Abc::IV3sProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IV3sProperty::valid )
        ;

    //IV3iProperty
    //
    class_<Abc::IV3iProperty>( "IV3iProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IV3iProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IV3iProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IV3iProperty::isScalar )
        .def( "isArray", &Abc::IV3iProperty::isArray )
        .def( "isCompound", &Abc::IV3iProperty::isCompound )
        .def( "isSimple", &Abc::IV3iProperty::isSimple )
        .def( "getMetaData", &Abc::IV3iProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IV3iProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IV3iProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IV3iProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IV3iProperty::getNumSamples )
        .def( "getObject", &Abc::IV3iProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IV3iProperty::reset )
        .def( "valid", &Abc::IV3iProperty::valid )
        .def( "__str__", &Abc::IV3iProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IV3iProperty::valid )
        ;

    //IV3fProperty
    //
    class_<Abc::IV3fProperty>( "IV3fProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IV3fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IV3fProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IV3fProperty::isScalar )
        .def( "isArray", &Abc::IV3fProperty::isArray )
        .def( "isCompound", &Abc::IV3fProperty::isCompound )
        .def( "isSimple", &Abc::IV3fProperty::isSimple )
        .def( "getMetaData", &Abc::IV3fProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IV3fProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IV3fProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IV3fProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IV3fProperty::getNumSamples )
        .def( "getObject", &Abc::IV3fProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IV3fProperty::reset )
        .def( "valid", &Abc::IV3fProperty::valid )
        .def( "__str__", &Abc::IV3fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IV3fProperty::valid )
        ;

    //IV3dProperty
    //
    class_<Abc::IV3dProperty>( "IV3dProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IV3dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IV3dProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IV3dProperty::isScalar )
        .def( "isArray", &Abc::IV3dProperty::isArray )
        .def( "isCompound", &Abc::IV3dProperty::isCompound )
        .def( "isSimple", &Abc::IV3dProperty::isSimple )
        .def( "getMetaData", &Abc::IV3dProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IV3dProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IV3dProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IV3dProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IV3dProperty::getNumSamples )
        .def( "getObject", &Abc::IV3dProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IV3dProperty::reset )
        .def( "valid", &Abc::IV3dProperty::valid )
        .def( "__str__", &Abc::IV3dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IV3dProperty::valid )
        ;

    //IBox2sProperty
    //
    class_<Abc::IBox2sProperty>( "IBox2sProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IBox2sProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IBox2sProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IBox2sProperty::isScalar )
        .def( "isArray", &Abc::IBox2sProperty::isArray )
        .def( "isCompound", &Abc::IBox2sProperty::isCompound )
        .def( "isSimple", &Abc::IBox2sProperty::isSimple )
        .def( "getMetaData", &Abc::IBox2sProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IBox2sProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IBox2sProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IBox2sProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IBox2sProperty::getNumSamples )
        .def( "getObject", &Abc::IBox2sProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IBox2sProperty::reset )
        .def( "valid", &Abc::IBox2sProperty::valid )
        .def( "__str__", &Abc::IBox2sProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IBox2sProperty::valid )
        ;

    //IBox2iProperty
    //
    class_<Abc::IBox2iProperty>( "IBox2iProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IBox2iProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IBox2iProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IBox2iProperty::isScalar )
        .def( "isArray", &Abc::IBox2iProperty::isArray )
        .def( "isCompound", &Abc::IBox2iProperty::isCompound )
        .def( "isSimple", &Abc::IBox2iProperty::isSimple )
        .def( "getMetaData", &Abc::IBox2iProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IBox2iProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IBox2iProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IBox2iProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IBox2iProperty::getNumSamples )
        .def( "getObject", &Abc::IBox2iProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IBox2iProperty::reset )
        .def( "valid", &Abc::IBox2iProperty::valid )
        .def( "__str__", &Abc::IBox2iProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IBox2iProperty::valid )
        ;

    //IBox2fProperty
    //
    class_<Abc::IBox2fProperty>( "IBox2fProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IBox2fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IBox2fProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IBox2fProperty::isScalar )
        .def( "isArray", &Abc::IBox2fProperty::isArray )
        .def( "isCompound", &Abc::IBox2fProperty::isCompound )
        .def( "isSimple", &Abc::IBox2fProperty::isSimple )
        .def( "getMetaData", &Abc::IBox2fProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IBox2fProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IBox2fProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IBox2fProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IBox2fProperty::getNumSamples )
        .def( "getObject", &Abc::IBox2fProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IBox2fProperty::reset )
        .def( "valid", &Abc::IBox2fProperty::valid )
        .def( "__str__", &Abc::IBox2fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IBox2fProperty::valid )
        ;

    //IBox2dProperty
    //
    class_<Abc::IBox2dProperty>( "IBox2dProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IBox2dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IBox2dProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IBox2dProperty::isScalar )
        .def( "isArray", &Abc::IBox2dProperty::isArray )
        .def( "isCompound", &Abc::IBox2dProperty::isCompound )
        .def( "isSimple", &Abc::IBox2dProperty::isSimple )
        .def( "getMetaData", &Abc::IBox2dProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IBox2dProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IBox2dProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IBox2dProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IBox2dProperty::getNumSamples )
        .def( "getObject", &Abc::IBox2dProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IBox2dProperty::reset )
        .def( "valid", &Abc::IBox2dProperty::valid )
        .def( "__str__", &Abc::IBox2dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IBox2dProperty::valid )
        ;

    //IBox3sProperty
    //
    class_<Abc::IBox3sProperty>( "IBox3sProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IBox3sProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IBox3sProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IBox3sProperty::isScalar )
        .def( "isArray", &Abc::IBox3sProperty::isArray )
        .def( "isCompound", &Abc::IBox3sProperty::isCompound )
        .def( "isSimple", &Abc::IBox3sProperty::isSimple )
        .def( "getMetaData", &Abc::IBox3sProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IBox3sProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IBox3sProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IBox3sProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IBox3sProperty::getNumSamples )
        .def( "getObject", &Abc::IBox3sProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IBox3sProperty::reset )
        .def( "valid", &Abc::IBox3sProperty::valid )
        .def( "__str__", &Abc::IBox3sProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IBox3sProperty::valid )
        ;

    //IBox3iProperty
    //
    class_<Abc::IBox3iProperty>( "IBox3iProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IBox3iProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IBox3iProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IBox3iProperty::isScalar )
        .def( "isArray", &Abc::IBox3iProperty::isArray )
        .def( "isCompound", &Abc::IBox3iProperty::isCompound )
        .def( "isSimple", &Abc::IBox3iProperty::isSimple )
        .def( "getMetaData", &Abc::IBox3iProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IBox3iProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IBox3iProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IBox3iProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IBox3iProperty::getNumSamples )
        .def( "getObject", &Abc::IBox3iProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IBox3iProperty::reset )
        .def( "valid", &Abc::IBox3iProperty::valid )
        .def( "__str__", &Abc::IBox3iProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IBox3iProperty::valid )
        ;

    //IBox3fProperty
    //
    class_<Abc::IBox3fProperty>( "IBox3fProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IBox3fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IBox3fProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IBox3fProperty::isScalar )
        .def( "isArray", &Abc::IBox3fProperty::isArray )
        .def( "isCompound", &Abc::IBox3fProperty::isCompound )
        .def( "isSimple", &Abc::IBox3fProperty::isSimple )
        .def( "getMetaData", &Abc::IBox3fProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IBox3fProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IBox3fProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IBox3fProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IBox3fProperty::getNumSamples )
        .def( "getObject", &Abc::IBox3fProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IBox3fProperty::reset )
        .def( "valid", &Abc::IBox3fProperty::valid )
        .def( "__str__", &Abc::IBox3fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IBox3fProperty::valid )
        ;

    //IBox3dProperty
    //
    class_<Abc::IBox3dProperty>( "IBox3dProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IBox3dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IBox3dProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IBox3dProperty::isScalar )
        .def( "isArray", &Abc::IBox3dProperty::isArray )
        .def( "isCompound", &Abc::IBox3dProperty::isCompound )
        .def( "isSimple", &Abc::IBox3dProperty::isSimple )
        .def( "getMetaData", &Abc::IBox3dProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IBox3dProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IBox3dProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IBox3dProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IBox3dProperty::getNumSamples )
        .def( "getObject", &Abc::IBox3dProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IBox3dProperty::reset )
        .def( "valid", &Abc::IBox3dProperty::valid )
        .def( "__str__", &Abc::IBox3dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IBox3dProperty::valid )
        ;

    //IM33fProperty
    //
    class_<Abc::IM33fProperty>( "IM33fProperty",
                                init<Abc::ICompoundProperty,
                                const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IM33fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IM33fProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IM33fProperty::isScalar )
        .def( "isArray", &Abc::IM33fProperty::isArray )
        .def( "isCompound", &Abc::IM33fProperty::isCompound )
        .def( "isSimple", &Abc::IM33fProperty::isSimple )
        .def( "getMetaData", &Abc::IM33fProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IM33fProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IM33fProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IM33fProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IM33fProperty::getNumSamples )
        .def( "getObject", &Abc::IM33fProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IM33fProperty::reset )
        .def( "valid", &Abc::IM33fProperty::valid )
        .def( "__str__", &Abc::IM33fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IM33fProperty::valid )
        ;

    //IM33dProperty
    //
    class_<Abc::IM33dProperty>( "IM33dProperty",
                                init<Abc::ICompoundProperty,
                                const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IM33dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IM33dProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IM33dProperty::isScalar )
        .def( "isArray", &Abc::IM33dProperty::isArray )
        .def( "isCompound", &Abc::IM33dProperty::isCompound )
        .def( "isSimple", &Abc::IM33dProperty::isSimple )
        .def( "getMetaData", &Abc::IM33dProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IM33dProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IM33dProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IM33dProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IM33dProperty::getNumSamples )
        .def( "getObject", &Abc::IM33dProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IM33dProperty::reset )
        .def( "valid", &Abc::IM33dProperty::valid )
        .def( "__str__", &Abc::IM33dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IM33dProperty::valid )
        ;

    //IM44fProperty
    //
    class_<Abc::IM44fProperty>( "IM44fProperty",
                                init<Abc::ICompoundProperty,
                                const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IM44fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IM44fProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IM44fProperty::isScalar )
        .def( "isArray", &Abc::IM44fProperty::isArray )
        .def( "isCompound", &Abc::IM44fProperty::isCompound )
        .def( "isSimple", &Abc::IM44fProperty::isSimple )
        .def( "getMetaData", &Abc::IM44fProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IM44fProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IM44fProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IM44fProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IM44fProperty::getNumSamples )
        .def( "getObject", &Abc::IM44fProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IM44fProperty::reset )
        .def( "valid", &Abc::IM44fProperty::valid )
        .def( "__str__", &Abc::IM44fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IM44fProperty::valid )
        ;

    //IM44dProperty
    //
    class_<Abc::IM44dProperty>( "IM44dProperty",
                                init<Abc::ICompoundProperty,
                                const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IM44dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IM44dProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IM44dProperty::isScalar )
        .def( "isArray", &Abc::IM44dProperty::isArray )
        .def( "isCompound", &Abc::IM44dProperty::isCompound )
        .def( "isSimple", &Abc::IM44dProperty::isSimple )
        .def( "getMetaData", &Abc::IM44dProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IM44dProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IM44dProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IM44dProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IM44dProperty::getNumSamples )
        .def( "getObject", &Abc::IM44dProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IM44dProperty::reset )
        .def( "valid", &Abc::IM44dProperty::valid )
        .def( "__str__", &Abc::IM44dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IM44dProperty::valid )
        ;

    //IQuatfProperty
    //
    class_<Abc::IQuatfProperty>( "IQuatfProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IQuatfProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IQuatfProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IQuatfProperty::isScalar )
        .def( "isArray", &Abc::IQuatfProperty::isArray )
        .def( "isCompound", &Abc::IQuatfProperty::isCompound )
        .def( "isSimple", &Abc::IQuatfProperty::isSimple )
        .def( "getMetaData", &Abc::IQuatfProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IQuatfProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IQuatfProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IQuatfProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IQuatfProperty::getNumSamples )
        .def( "getObject", &Abc::IQuatfProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IQuatfProperty::reset )
        .def( "valid", &Abc::IQuatfProperty::valid )
        .def( "__str__", &Abc::IQuatfProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IQuatfProperty::valid )
        ;

    //IQuatdProperty
    //
    class_<Abc::IQuatdProperty>( "IQuatdProperty",
                                 init<Abc::ICompoundProperty,
                                 const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IQuatdProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IQuatdProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IQuatdProperty::isScalar )
        .def( "isArray", &Abc::IQuatdProperty::isArray )
        .def( "isCompound", &Abc::IQuatdProperty::isCompound )
        .def( "isSimple", &Abc::IQuatdProperty::isSimple )
        .def( "getMetaData", &Abc::IQuatdProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IQuatdProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IQuatdProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IQuatdProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IQuatdProperty::getNumSamples )
        .def( "getObject", &Abc::IQuatdProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IQuatdProperty::reset )
        .def( "valid", &Abc::IQuatdProperty::valid )
        .def( "__str__", &Abc::IQuatdProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IQuatdProperty::valid )
        ;

    //IC3hProperty
    //
    class_<Abc::IC3hProperty>( "IC3hProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IC3hProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IC3hProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IC3hProperty::isScalar )
        .def( "isArray", &Abc::IC3hProperty::isArray )
        .def( "isCompound", &Abc::IC3hProperty::isCompound )
        .def( "isSimple", &Abc::IC3hProperty::isSimple )
        .def( "getMetaData", &Abc::IC3hProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IC3hProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IC3hProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IC3hProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IC3hProperty::getNumSamples )
        .def( "getObject", &Abc::IC3hProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IC3hProperty::reset )
        .def( "valid", &Abc::IC3hProperty::valid )
        .def( "__str__", &Abc::IC3hProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IC3hProperty::valid )
        ;

    //IC3fProperty
    //
    class_<Abc::IC3fProperty>( "IC3fProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IC3fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IC3fProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IC3fProperty::isScalar )
        .def( "isArray", &Abc::IC3fProperty::isArray )
        .def( "isCompound", &Abc::IC3fProperty::isCompound )
        .def( "isSimple", &Abc::IC3fProperty::isSimple )
        .def( "getMetaData", &Abc::IC3fProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IC3fProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IC3fProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IC3fProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IC3fProperty::getNumSamples )
        .def( "getObject", &Abc::IC3fProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IC3fProperty::reset )
        .def( "valid", &Abc::IC3fProperty::valid )
        .def( "__str__", &Abc::IC3fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IC3fProperty::valid )
        ;

    //IC3cProperty
    //
    class_<Abc::IC3cProperty>( "IC3cProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IC3cProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IC3cProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IC3cProperty::isScalar )
        .def( "isArray", &Abc::IC3cProperty::isArray )
        .def( "isCompound", &Abc::IC3cProperty::isCompound )
        .def( "isSimple", &Abc::IC3cProperty::isSimple )
        .def( "getMetaData", &Abc::IC3cProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IC3cProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IC3cProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IC3cProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IC3cProperty::getNumSamples )
        .def( "getObject", &Abc::IC3cProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IC3cProperty::reset )
        .def( "valid", &Abc::IC3cProperty::valid )
        .def( "__str__", &Abc::IC3cProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IC3cProperty::valid )
        ;

    //IC4hProperty
    //
    class_<Abc::IC4hProperty>( "IC4hProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IC4hProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IC4hProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IC4hProperty::isScalar )
        .def( "isArray", &Abc::IC4hProperty::isArray )
        .def( "isCompound", &Abc::IC4hProperty::isCompound )
        .def( "isSimple", &Abc::IC4hProperty::isSimple )
        .def( "getMetaData", &Abc::IC4hProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IC4hProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IC4hProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IC4hProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IC4hProperty::getNumSamples )
        .def( "getObject", &Abc::IC4hProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IC4hProperty::reset )
        .def( "valid", &Abc::IC4hProperty::valid )
        .def( "__str__", &Abc::IC4hProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IC4hProperty::valid )
        ;

    //IC4fProperty
    //
    class_<Abc::IC4fProperty>( "IC4fProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IC4fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IC4fProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IC4fProperty::isScalar )
        .def( "isArray", &Abc::IC4fProperty::isArray )
        .def( "isCompound", &Abc::IC4fProperty::isCompound )
        .def( "isSimple", &Abc::IC4fProperty::isSimple )
        .def( "getMetaData", &Abc::IC4fProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IC4fProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IC4fProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IC4fProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IC4fProperty::getNumSamples )
        .def( "getObject", &Abc::IC4fProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IC4fProperty::reset )
        .def( "valid", &Abc::IC4fProperty::valid )
        .def( "__str__", &Abc::IC4fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IC4fProperty::valid )
        ;

    //IC4cProperty
    //
    class_<Abc::IC4cProperty>( "IC4cProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IC4cProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IC4cProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IC4cProperty::isScalar )
        .def( "isArray", &Abc::IC4cProperty::isArray )
        .def( "isCompound", &Abc::IC4cProperty::isCompound )
        .def( "isSimple", &Abc::IC4cProperty::isSimple )
        .def( "getMetaData", &Abc::IC4cProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IC4cProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IC4cProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IC4cProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IC4cProperty::getNumSamples )
        .def( "getObject", &Abc::IC4cProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IC4cProperty::reset )
        .def( "valid", &Abc::IC4cProperty::valid )
        .def( "__str__", &Abc::IC4cProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IC4cProperty::valid )
        ;

    //IN3fProperty
    //
    class_<Abc::IN3fProperty>( "IN3fProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IN3fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IN3fProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IN3fProperty::isScalar )
        .def( "isArray", &Abc::IN3fProperty::isArray )
        .def( "isCompound", &Abc::IN3fProperty::isCompound )
        .def( "isSimple", &Abc::IN3fProperty::isSimple )
        .def( "getMetaData", &Abc::IN3fProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IN3fProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IN3fProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IN3fProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IN3fProperty::getNumSamples )
        .def( "getObject", &Abc::IN3fProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IN3fProperty::reset )
        .def( "valid", &Abc::IN3fProperty::valid )
        .def( "__str__", &Abc::IN3fProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IN3fProperty::valid )
        ;

    //IN3dProperty
    //
    class_<Abc::IN3dProperty>( "IN3dProperty",
                               init<Abc::ICompoundProperty,
                               const std::string&>() )
        .def( init<>() )
        .def( "getName", &Abc::IN3dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "getHeader", &Abc::IN3dProperty::getHeader,
              return_internal_reference<1>() )
        .def( "isScalar", &Abc::IN3dProperty::isScalar )
        .def( "isArray", &Abc::IN3dProperty::isArray )
        .def( "isCompound", &Abc::IN3dProperty::isCompound )
        .def( "isSimple", &Abc::IN3dProperty::isSimple )
        .def( "getMetaData", &Abc::IN3dProperty::getMetaData,
              return_internal_reference<1>() )
        .def( "getDataType", &Abc::IN3dProperty::getDataType,
              return_internal_reference<1>() )
        .def( "getTimeSamplingType", &Abc::IN3dProperty::getTimeSamplingType )
        .def( "getInterpretation", &Abc::IN3dProperty::getInterpretation,
              return_value_policy<copy_const_reference>() )
        .def( "getNumSamples", &Abc::IN3dProperty::getNumSamples )
        .def( "getObject", &Abc::IN3dProperty::getObject,
              with_custodian_and_ward_postcall<0,1>() )
        .def( "reset", &Abc::IN3dProperty::reset )
        .def( "valid", &Abc::IN3dProperty::valid )
        .def( "__str__", &Abc::IN3dProperty::getName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IN3dProperty::valid )
        ;
    #endif
}
