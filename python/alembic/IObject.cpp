//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#include <AlembicAsset/AlembicAsset.h>

#include <boost/python.hpp>

#include <Python.h>

using namespace boost::python;

using namespace AlembicAsset;

//-*****************************************************************************
void register_iobject()
{
    PropertyInfo ( IObject::*propinfo1 )( size_t ) const = \
        &IObject::propertyInfo;
    PropertyInfo ( IObject::*propinfo2 )( const std::string&,
                                          const std::string& ) const =  \
        &IObject::propertyInfo;

    ObjectInfo ( IObject::*childInfo1 )( size_t ) const = &IObject::childInfo;
    ObjectInfo ( IObject::*childInfo2 )( const std::string&,
                                         const std::string& ) const =   \
        &IObject::childInfo;

    void ( IObject::*klose )() = &IObject::close;

    class_<PropertyInfoBody>( "PropertyInfoBody" )
        .def_readonly( "name", &PropertyInfoBody::name )
        .def_readonly( "protocol", &PropertyInfoBody::protocol )
        .def_readonly( "ptype", &PropertyInfoBody::ptype )
        .def_readonly( "dtype", &PropertyInfoBody::dtype )
        ;

    register_ptr_to_python<PropertyInfo>();

    class_<IObject, bases<IParentObject> >( "IObject",
                                            init<const IParentObject&,
                                            const std::string&,
                                            const std::string&,
                                            optional<const IContext&> >() )
        .def( init<const IParentObject&, ObjectInfo,
              optional<const IContext&> >() )
        .def( init<const IParentObject&, size_t, optional<const IContext&> >() )
        .def( "name", &IObject::name )
        .def( "fullPathName", &IObject::fullPathName )
        .def( "protocol", &IObject::protocol )
        .def( "comments", &IObject::comments )
        .def( "propertyInfo", propinfo1 )
        .def( "propertyInfo", propinfo2 )
        .def( "numProperties", &IObject::numProperties )
        .def( "numChildren", &IObject::numChildren )
        .def( "childInfo", childInfo1 )
        .def( "childInfo", childInfo2 )
        .def( "valid", &IObject::valid )
        .def( "close", klose )
        .def( "release", &IObject::release )
        .def( "__str__", &IObject::fullPathName )
        ;
}
