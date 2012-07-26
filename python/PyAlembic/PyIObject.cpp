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

using namespace boost::python;

//-*****************************************************************************
class ChildIterator
{
public:
    ChildIterator( Abc::IObject &p )
        : _p( p ) , _iter( 0 ), _end( p.getNumChildren() ) {}

    Abc::IObject next()
    {
        if ( _iter >= _end )
            boost::python::objects::stop_iteration_error();

        return _p.getChild( _iter++ );
    }
private:
    Abc::IObject& _p;
    AbcA::index_t _iter;
    AbcA::index_t _end;
};

//-*****************************************************************************
class ChildList
{
public:
    ChildList( Abc::IObject &p )
        : _p( p ) {}

    Py_ssize_t len()
    {
        return (Py_ssize_t)_p.getNumChildren();
    }

    Abc::IObject getItem( Py_ssize_t index )
    {
        return _p.getChild( ( AbcA::index_t )index );
    }

    ChildIterator* getIterator()
    {
        return new ChildIterator( _p );
    }
private:
    Abc::IObject& _p;
};

//-*****************************************************************************
static ChildList getChildList( Abc::IObject& p)
{
    return ChildList(p);
}

//-*****************************************************************************
void register_iobject()
{
    // overloads
    //
    Abc::IObject ( Abc::IObject::*getChildByIndex )( size_t ) = \
        &Abc::IObject::getChild;
    Abc::IObject ( Abc::IObject::*getChildByName )( const std::string& ) = \
        &Abc::IObject::getChild;

    const AbcA::ObjectHeader&
        ( Abc::IObject::*getChildHeaderByIndex )( size_t ) = \
        &Abc::IObject::getChildHeader;

    const AbcA::ObjectHeader*
        ( Abc::IObject::*getChildHeaderByName )( const std::string & ) = \
        &Abc::IObject::getChildHeader;

    // IObject
    //
    class_<Abc::IObject>(
         "IObject",
         "The IObject class as an object reader",
         init<Abc::IObject, const std::string&>(
             ( arg( "parent" ), arg( "name" ) ),
             "Create an IObject with the given parent IObject and name" ) )
        .def( init<>( "Create an empty IObject" ) )
        .def( "getHeader",
              &Abc::IObject::getHeader,
              "Return the header which contains all the MetaData that was "
              "specified upon their creation",
              return_internal_reference<1>() )
        .def( "getName",
              &Abc::IObject::getName,
              "Return the name of this object. The name is unique amongst "
              "their siblings",
              return_value_policy<copy_const_reference>() )
        .def( "getFullName",
              &Abc::IObject::getFullName,
              "Return the full name of this object. The full name is unique "
              "within the entire archive",
              return_value_policy<copy_const_reference>() )
        .def( "getNumChildren",
              &Abc::IObject::getNumChildren,
              "Return the number of child IObjects that this object has" )
        .def( "getChildHeader",
              getChildHeaderByIndex,
              ( arg( "index" ) ),
              "Return the header of a child IObject with the given index",
              return_internal_reference<1>() )
        .def( "getChildHeader",
              getChildHeaderByName,
              ( arg( "name" ) ),
              "Return the header of a child IObject with the given name",
              return_value_policy<reference_existing_object>() )
        .def( "getProperties",
              &Abc::IObject::getProperties,
              "Return the single top-level ICompoundProperty",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getChild",
              getChildByIndex,
              ( arg( "index" ) ),
              "Return a child IObject with the given index",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getChild",
              getChildByName,
              ( arg( "name" ) ),
              "Return a child IObject with the given name",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getArchive",
              &Abc::IObject::getArchive,
              "Return this object's archive",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getParent",
              &Abc::IObject::getParent,
              "Return this object's parent\n"
              "If the object is the top level object, the IObject returned "
              "will be an empty Object",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getMetaData",
              &Abc::IObject::getMetaData,
              "Return the MetaData of this object",
              return_internal_reference<1>() )
        .def( "valid", &Abc::IObject::valid )
        .def( "reset", &Abc::IObject::reset )
        .def( "__str__", &Abc::IObject::getFullName,
              return_value_policy<copy_const_reference>() )
        .def( "__nonzero__", &Abc::IObject::valid )
        .add_property( "children", &getChildList )
        ;

    // List and Iterator for child IObjects
    //
    class_<ChildList>
        ( "ChildList", no_init )
        .def( "__len__", &ChildList::len )
        .def( "__getitem__", &ChildList::getItem )
        .def( "__iter__", &ChildList::getIterator,
              return_value_policy<manage_new_object>() )
        ;

    class_<ChildIterator>
        ( "ChildIterator", no_init )
        .def( "next", &ChildIterator::next )
        ;
}
