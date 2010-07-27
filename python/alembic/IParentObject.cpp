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
struct IBaseWrap : IBase, wrapper<IBase>
{
    std::string name()
    {
        return this->get_override( "name" )();
    }

    std::string fullPathName()
    {
        return this->get_override( "fullPathName" )();
    }

    std::string protocol()
    {
        return this->get_override( "protocol" )();
    }

    bool valid()
    {
        return this->get_override( "valid" )();
    }

    void close() throw()
    {
        this->get_override( "close" )();
    }

    void release() throw()
    {
        this->get_override( "release" )();
    }

};

//-*****************************************************************************
struct IParentObjectWrap : IParentObject, wrapper<IParentObject>
{
    std::string comments()
    {
        return this->get_override( "comments" )();
    }

    size_t numChildren()
    {
        return this->get_override( "numChildren" )();
    }

    ObjectInfo childInfo( size_t p )
    {
        return this->get_override( "childInfo" )( p );
    }

    ObjectInfo childInfo( const std::string &name, const std::string &prot = "" )
    {
        return this->get_override( "childInfo" )( name, prot );
    }

};

//-*****************************************************************************
void register_iparentobject()
{
    ObjectInfo ( IParentObject::*childInfo1 )( size_t ) const = \
        &IParentObject::childInfo;
    ObjectInfo ( IParentObject::*childInfo2 )( const std::string&,
                                               const std::string& ) const = \
        &IParentObject::childInfo;

    void ( IBase::*klose )() = &IBase::close;


    class_<IBaseWrap, boost::noncopyable>( "IBase", no_init )
        .def( "name", pure_virtual( &IBase::name ) )
        .def( "fullPathName", pure_virtual( &IBase::fullPathName ) )
        .def( "protocol", pure_virtual( &IBase::protocol ) )
        .def( "valid", pure_virtual( &IBase::valid ) )
        .def( "close", pure_virtual( klose ) )
        .def( "release", pure_virtual( &IBase::release ) )
        ;

    class_<IParentObjectWrap, bases<IBase>,
        boost::noncopyable>( "IParentObject",
                             no_init )
        .def( "comments", pure_virtual( &IParentObject::comments ) )
        .def( "numChildren", pure_virtual( &IParentObject::numChildren ) )
        .def( "childInfo", pure_virtual( childInfo1 ) )
        .def( "childInfo", pure_virtual( childInfo2 ) )
        ;
}
