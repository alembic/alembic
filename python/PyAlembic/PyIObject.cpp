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
    Abc::IObject _p;
    size_t _iter;
    size_t _end;
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
    Abc::IObject _p;
};

//-*****************************************************************************
static ChildList getChildList( Abc::IObject& p)
{
    return ChildList(p);
}

//-*****************************************************************************
static Abc::IObject getChildByIndex( Abc::IObject &o, size_t i )
{
    if ( i >= o.getNumChildren() )
    {
        std::stringstream stream;
        stream << i;
        throwPythonIndexException( stream.str().c_str() );
    }
    const Abc::IObject& c = o.getChild( i );
    if ( !c.valid() )
    {
        return Abc::IObject();
    }
    return c;
}

//-*****************************************************************************
static Abc::IObject getChildByName( Abc::IObject &o, const std::string& name )
{
    const Abc::IObject& c = o.getChild( name );
    if ( c.valid() )
    {
        return c;
    }

    std::stringstream stream;
    stream << name;
    throwPythonKeyException( stream.str().c_str() );
    return Abc::IObject();
}

//-*****************************************************************************
static bool isChildInstanceByIndex( Abc::IObject &o, size_t i )
{
    if ( i >= o.getNumChildren() )
    {
        std::stringstream stream;
        stream << i;
        throwPythonIndexException( stream.str().c_str() );
    }
    const Abc::IObject& c = o.getChild( i );
    if ( !c.valid() )
    {
        return false;
    }

    return c.isInstanceDescendant();
}

//-*****************************************************************************
static bool isChildInstanceByName( Abc::IObject &o, const std::string& name )
{
    const Abc::IObject& c = o.getChild( name );
    if ( c.valid() )
    {
        return c.isInstanceDescendant();
    }

    std::stringstream stream;
    stream << name;
    throwPythonKeyException( stream.str().c_str() );
    return false;
}

//-*****************************************************************************
static std::string getPropertiesHash( Abc::IObject &o )
{
    Alembic::Util::Digest pHash;
    if ( !o.getPropertiesHash( pHash ) )
        return "";

    return pHash.str();
}

//-*****************************************************************************
static std::string getChildrenHash( Abc::IObject &o )
{
    Alembic::Util::Digest cHash;
    if ( !o.getChildrenHash( cHash ) )
        return "";

    return cHash.str();
}

//-*****************************************************************************
void register_iobject()
{
    // overloads
    //
    const AbcA::ObjectHeader&
        ( Abc::IObject::*getChildHeaderByIndex )( size_t ) const = \
        &Abc::IObject::getChildHeader;

    const AbcA::ObjectHeader*
        ( Abc::IObject::*getChildHeaderByName )( const std::string & ) const = \
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
              &getChildByIndex,
              ( arg( "index" ) ),
              "Return a child IObject with the given index",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getChild",
              &getChildByName,
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
        .def( "isInstanceRoot",
              &Abc::IObject::isInstanceRoot,
              "Return true if this IObject is an instance root" )
        .def( "isInstanceDescendant",
              &Abc::IObject::isInstanceDescendant,
              "Return true if this IObject is an instanceRoot, or an ancestor of one" )
        .def( "instanceSourcePath",
              &Abc::IObject::instanceSourcePath,
              "Return the instance source path that the instance points at. Empty string if not found." )
        .def( "isChildInstance",
              &isChildInstanceByIndex,
              ( arg( "index" ) ),
              "Return true if the child by index is an instance root object" )
        .def( "isChildInstance",
              &isChildInstanceByName,
              ( arg( "name" ) ),
              "Return true if the named child is an instance root object" )
        .def( "getPropertiesHash",
              &getPropertiesHash,
              "Return a string representation of the properties hash for this object. Empty string if not found." )
        .def( "getChildrenHash",
              &getChildrenHash,
              "Return a string representation of the children hash for this object. Empty string if not found." )
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
