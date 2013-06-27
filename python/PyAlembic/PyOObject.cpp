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

using namespace boost::python;

//-*****************************************************************************
static Abc::OObject getChildByIndex( Abc::OObject &o, size_t i )
{
    if ( i >= o.getNumChildren() )
    {
        std::stringstream stream;
        stream << i;
        throwPythonIndexException( stream.str().c_str() );
    }
    const Abc::OObject& c = o.getChild( i );
    if ( !c.valid() )
    {
        Abc::OObject();
    }
    return c;
}

//-*****************************************************************************
static Abc::OObject getChildByName( Abc::OObject &o, const std::string& name )
{
    const Abc::OObject& c = o.getChild( name );
    if ( c.valid() )
    {
        return c;
    }

    std::stringstream stream;
    stream << name;
    throwPythonKeyException( stream.str().c_str() );
    return Abc::OObject();
}

//-*****************************************************************************
void register_oobject()
{
    // overloads
    //
    const AbcA::ObjectHeader&
        ( Abc::OObject::*getChildHeaderByIndex )( size_t ) = \
        &Abc::OObject::getChildHeader;

    const AbcA::ObjectHeader*
        ( Abc::OObject::*getChildHeaderByName )( const std::string & ) = \
        &Abc::OObject::getChildHeader;

    // OObject
    //
    class_<Abc::OObject>(
          "OObject",
          "The OObject class is an object writer",
          init<Abc::OObject, const std::string&,
                     optional<
                     const Abc::Argument&,
                     const Abc::Argument&,
                     const Abc::Argument&> >(
              ( arg( "parent"), arg( "name" ), arg( "argument" ),
                arg( "argument" ), arg( "argument" ) ),
              "Create a new OObject with the given parent OObject, name and "
              "optional arguments which can be use to override the "
              "ErrorHandlerPolicy, to specify MetaData" ) )
        .def( init<>( "Create an empty OObject" ) )
        .def( "addChildInstance",
              &Abc::OObject::addChildInstance,
              "Add a child to this OObject as an instance of another already created OObject" )
        .def( "getHeader",
              &Abc::OObject::getHeader,
              "Return the header of this object", 
              return_internal_reference<>() )
        .def( "getName",
              &Abc::OObject::getName,
              "Return the name of this object. The name is unique amongst "
              "their siblings",
              return_value_policy<copy_const_reference>() )
        .def( "getFullName",
              &Abc::OObject::getFullName,
              "Return the full name of this object. The full name is unique "
              "within the entire archive",
              return_value_policy<copy_const_reference>() )
        .def( "getNumChildren",
              &Abc::OObject::getNumChildren,
              "Return the number of child OObjects that this object has\n"
              "This may change as new  children are created for writing" )
        .def( "getChildHeader",
              getChildHeaderByIndex,
              ( arg( "index" ) ),
              "Return the header of a child OObject with the given index",
              return_internal_reference<>() )
        .def( "getChildHeader",
              getChildHeaderByName,
              ( arg("name" ) ),
              "Return the header of a child OObject with the given name of "
              "the OObject - even if the OObject no longer exists",
              return_value_policy<reference_existing_object>() )
        .def( "getProperties",
              &Abc::OObject::getProperties,
              "Return the single top-level OCompoundProperty",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getChild",
              &getChildByIndex,
              ( arg( "index" ) ),
              "Return an already created child OObject with the given index\n"
              "This does not create a new OObject as a child",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getChild",
              &getChildByName,
              ( arg( "name" ) ),
              "Return an already created OObject with the given name\n"
              "This does not create a new OObject as a child",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getArchive",
              &Abc::OObject::getArchive,
              "Return this object's archive",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getParent",
              &Abc::OObject::getParent,
              "Return this object's parent\n"
              "If the object is the top level object, the OObject returned "
              "will be an empty Object",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getMetaData",
              &Abc::OObject::getMetaData,
              "Return the MetaData of this object",
              return_internal_reference<>() )
        .def( "valid", &Abc::OObject::valid )
        .def( "reset", &Abc::OObject::reset )
        .def( "__str__", &Abc::OObject::getFullName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::OObject::valid )
        ;
}
