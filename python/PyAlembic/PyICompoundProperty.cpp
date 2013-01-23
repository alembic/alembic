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
#include <PyIBaseProperty.h>

using namespace boost::python;

//-*****************************************************************************
class PropertyHeaderIterator
{
public:
    PropertyHeaderIterator( Abc::ICompoundProperty &p )
        : _p( p ), _iter( 0 ), _end( p.getNumProperties() ) {}

    const AbcA::PropertyHeader& next()
    {
        if ( _iter >= _end )
            boost::python::objects::stop_iteration_error();

        return _p.getPropertyHeader( _iter++ );
    }
private:
    Abc::ICompoundProperty _p;
    size_t _iter;
    size_t _end;
};

//-*****************************************************************************
class PropertyHeaderList
{
public:
    PropertyHeaderList( Abc::ICompoundProperty &p )
        : _p(p) {}

    Py_ssize_t len()
    {
        return (Py_ssize_t)_p.getNumProperties();
    }

    const AbcA::PropertyHeader& getItem( Py_ssize_t index )
    {
        return _p.getPropertyHeader( ( AbcA::index_t )index );
    }

    PropertyHeaderIterator* getIterator()
    {
        return new PropertyHeaderIterator( _p );
    }
private:
    Abc::ICompoundProperty _p;
};

//-*****************************************************************************
static PropertyHeaderList getPropertyHeaderList( Abc::ICompoundProperty& p)
{
    return PropertyHeaderList(p);
}

//-*****************************************************************************
static object getPropertyByName( Abc::ICompoundProperty &p,
                                 const std::string& name )
{
    const AbcA::PropertyHeader* h = p.getPropertyHeader( name );
    if ( !h )
    {
        std::stringstream stream;
        stream << name;
        throwPythonKeyException( stream.str().c_str() );
        return object(); // Returns None object
    }
    else if ( h->isScalar() )
    {
        return_by_value::apply<Abc::IScalarProperty>::type converter;
        return object (handle<>(converter (Abc::IScalarProperty (p, name))));
    }
    else if ( h->isArray() )
    {
        return_by_value::apply<Abc::IArrayProperty>::type converter;
        return object (handle<>(converter (Abc::IArrayProperty (p, name))));
    }
    else if ( h->isCompound() )
    {
        return_by_value::apply<Abc::ICompoundProperty>::type converter;
        return object (handle<>(converter (Abc::ICompoundProperty (p, name))));
    }
    throwPythonException( "Conversion error, unsupported property type" );
    return object();
}

//-*****************************************************************************
static object getPropertyByIndex( Abc::ICompoundProperty &p, size_t i )
{
    if ( i >= p.getNumProperties() )
    {
        std::stringstream stream;
        stream << i;
        throwPythonIndexException( stream.str().c_str() );
        return object(); // Returns None object
    }
    const AbcA::PropertyHeader& h = p.getPropertyHeader( i );
    const std::string name( h.getName() );
    if ( h.isScalar() )
    {
        return_by_value::apply<Abc::IScalarProperty>::type converter;
        return object (handle<>(converter (Abc::IScalarProperty (p, name))));
    }
    else if ( h.isArray() )
    {
        return_by_value::apply<Abc::IArrayProperty>::type converter;
        return object (handle<>(converter (Abc::IArrayProperty (p, name))));
    }
    else if ( h.isCompound() )
    {
        return_by_value::apply<Abc::ICompoundProperty>::type converter;
        return object (handle<>(converter (Abc::ICompoundProperty (p, name))));
    }
    throwPythonException( "Conversion error, unsupported property type" );
    return object();
}


//-*****************************************************************************
void register_icompoundproperty()
{
    // IBaseProperty
    //
    register_IBaseProperty<AbcA::CompoundPropertyReaderPtr>(
                                                    "IBaseProperty_Compound" );

    // overloads
    //
    const AbcA::PropertyHeader &
        ( Abc::ICompoundProperty::*getHeaderByIndex )( size_t ) const = \
        &Abc::ICompoundProperty::getPropertyHeader;
    const AbcA::PropertyHeader *
        ( Abc::ICompoundProperty::*getHeaderByName )( const std::string& ) const = \
        &Abc::ICompoundProperty::getPropertyHeader;

    // ICompoundProperty
    //
    class_<Abc::ICompoundProperty,
           bases<Abc::IBasePropertyT<AbcA::CompoundPropertyReaderPtr> > >(
        "ICompoundProperty",
        "The ICompoundProperty class is a compound property reader",
        init<>( "Create an empty ICompoundProperty" ) )
        .def( init<Abc::ICompoundProperty,
                   const std::string&,
                   optional<const Abc::Argument&> >(
                  ( arg( "parent" ), arg( "name" ), arg( "argument" ) ),
                  "Create an ICompoundProperty with the parent IObject, name "
                  "and optional argument to override the ErrorHandlerPolicy" ) )
        .def( "getNumProperties",
              &Abc::ICompoundProperty::getNumProperties,
              "Returnt the number of child properties contained in this "
              "ICompoundProperty" )
        .def( "getPropertyHeader",
              getHeaderByIndex,
              ( arg( "index" ) ),
              "Return the header of a child property with the given index",
              return_internal_reference<>() )
        .def( "getPropertyHeader",
              getHeaderByName,
              ( arg( "name" ) ),
              "Return the header of a child property with the given name",
              return_value_policy<reference_existing_object>() )
        .def( "getProperty",
              &getPropertyByIndex,
              ( arg( "index" ) ),
              "Return a child property with the given index" )
        .def( "getProperty",
              &getPropertyByName,
              ( arg( "name" ) ),
              "Return a child property with the given name" )
        .def( "getParent",
              &Abc::ICompoundProperty::getParent,
              "Return the parent ICompoundProperty" )
        .add_property( "propertyheaders", &getPropertyHeaderList )
        ;

    // List and Iterator for child properties
    //
    class_<PropertyHeaderList>
        ( "PropertyHeaderList", no_init )
        .def( "__len__", &PropertyHeaderList::len )
        .def( "__getitem__", &PropertyHeaderList::getItem,
              return_internal_reference<>() )
        .def( "__iter__", &PropertyHeaderList::getIterator,
              return_value_policy<manage_new_object>() )
        ;

    class_<PropertyHeaderIterator>
        ( "PropertyHeaderIterator", no_init )
        .def( "next", &PropertyHeaderIterator::next,
              return_internal_reference<1>() )
        ;
}

