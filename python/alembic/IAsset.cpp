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

void register_iasset()
{
    ObjectInfo ( IAsset::*childInfo1 )( size_t ) const = &IAsset::childInfo;
    ObjectInfo ( IAsset::*childInfo2 )( const std::string&,
                                        const std::string& ) const = \
        &IAsset::childInfo;

    void ( IAsset::*klose )() = &IAsset::close;

    class_<ObjectInfoBody>( "ObjectInfoBody",
                            init<std::string,
                            std::string>() )
        .def_readonly( "name", &ObjectInfoBody::name )
        .def_readonly( "protocol", &ObjectInfoBody::protocol )
        ;

    register_ptr_to_python<ObjectInfo>();

    //enum_<ThrowExceptionFlag>( "ThrowExceptionFlag" )
    //    .value( "kThrowException", kThrowException );

    class_<IAsset, bases<IParentObject> >( "IAsset", init<const std::string&,
                                           optional<const IContext&> >() )
        .def( "fileName", &IAsset::fileName )
        .def( "release", &IAsset::release )
        .def( "name", &IAsset::name )
        .def( "fullPathName", &IAsset::fullPathName )
        .def( "protocol", &IAsset::protocol )
        .def( "comments", &IAsset::comments )
        .def( "numChildren", &IAsset::numChildren )
        .def( "childInfo", childInfo1 )
        .def( "childInfo", childInfo2 )
        .def( "valid", &IAsset::valid )
        .def( "close", klose )
        .def( "__str__", &IAsset::fileName )
        ;
}
