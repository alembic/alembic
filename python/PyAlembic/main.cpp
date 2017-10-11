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

#include <boost/python.hpp>

using namespace boost::python;

// forwards
void register_typedarraysampleconverters();
void register_abctypes();
void register_abcgeomtypes();
void register_coreabstracttypes();
void register_utiltypes();

void register_archiveinfo();

void register_abccorelayer();

void register_iarchive();
void register_oarchive();
void register_iobject();
void register_oobject();
void register_icompoundproperty();
void register_ocompoundproperty();
void register_isampleselector();
void register_iscalarproperty();
void register_iarrayproperty();
void register_oscalarproperty();
void register_oarrayproperty();
void register_itypedscalarproperty();
void register_itypedarrayproperty();
void register_otypedscalarproperty();
void register_otypedarrayproperty();
void register_typedpropertytraits();

void register_visibility();
void register_archivebounds();

void register_igeomparam();
void register_ogeomparam();

void register_igeombase();

void register_ofaceset();
void register_ifaceset();

void register_oxform();
void register_ixform();
void register_xformop();
void register_filmbackxformop();
void register_xformsample();

void register_ipoints();
void register_opoints();

void register_opolymesh();
void register_ipolymesh();

void register_icurves();
void register_ocurves();

void register_isubd();
void register_osubd();

void register_inupatch();
void register_onupatch();

void register_icamera();
void register_ocamera();
void register_camerasample();

void register_ilight();
void register_olight();

void register_imaterial();
void register_omaterial();
void register_materialflatten();
void register_materialassignment();

void register_icollections();
void register_ocollections();

BOOST_PYTHON_MODULE( alembic )
{
    docstring_options doc_options( true, true, false );

    handle<> imath( PyImport_ImportModule( "imath" ) );
    if( PyErr_Occurred() ) throw_error_already_set();

    register_typedarraysampleconverters();

    object package = scope();
    package.attr( "__path__" ) = "alembic";

    {
        const char* scopeName = "AbcCoreAbstract";
        const char* moduleName = "alembic.AbcCoreAbstract";
        object module( handle<>( borrowed( PyImport_AddModule(
                                            moduleName ) ) ) );
        scope().attr( scopeName ) = module;
        scope within( module );

        register_coreabstracttypes();
    }

    {
        const char* scopeName = "Util";
        const char* moduleName = "alembic.Util";

        object module( handle<>( borrowed( PyImport_AddModule(
                                            moduleName ) ) ) );
        scope().attr( scopeName ) = module;
        scope within( module );

        register_utiltypes();
    }

    {
        const char* scopeName = "Abc";
        const char* moduleName = "alembic.Abc";
        object module( handle<>( borrowed( PyImport_AddModule(
                                            moduleName ) ) ) );
        scope().attr( scopeName ) = module;
        scope within( module );

        register_abctypes();

        // registered here because the utils within are only really needed when
        // creating object and properties
        register_abccorelayer();

        register_archiveinfo();
        register_iarchive();
        register_oarchive();
        register_iobject();
        register_oobject();
        register_icompoundproperty();
        register_ocompoundproperty();
        register_isampleselector();
        register_iscalarproperty();
        register_iarrayproperty();
        register_oscalarproperty();
        register_oarrayproperty();
        register_itypedscalarproperty();
        register_itypedarrayproperty();
        register_otypedscalarproperty();
        register_otypedarrayproperty();
        register_typedpropertytraits();
    }

    {
        const char* scopeName = "AbcGeom";
        const char* moduleName = "alembic.AbcGeom";
        object module( handle<>( borrowed( PyImport_AddModule(
                                            moduleName ) ) ) );
        scope().attr( scopeName ) = module;
        scope within( module );

        register_abcgeomtypes();

        register_visibility();
        register_archivebounds();

        register_ofaceset();
        register_ifaceset();

        register_igeomparam();
        register_ogeomparam();

        register_igeombase();

        register_ipolymesh();
        register_opolymesh();

        register_oxform();
        register_ixform();
        register_xformop();
        register_filmbackxformop();
        register_xformsample();

        register_ipoints();
        register_opoints();

        register_icurves();
        register_ocurves();

        register_isubd();
        register_osubd();

        register_inupatch();
        register_onupatch();

        register_icamera();
        register_ocamera();
        register_camerasample();

        register_ilight();
        register_olight();
   }

   {
        const char* scopeName = "AbcCollection";
        const char* moduleName = "alembic.AbcCollection";
        object module( handle<>( borrowed( PyImport_AddModule(
                                            moduleName ) ) ) );
        scope().attr( scopeName ) = module;
        scope within( module );

        register_icollections();
        register_ocollections();
   }

   {
        const char* scopeName = "AbcMaterial";
        const char* moduleName = "alembic.AbcMaterial";
        object module( handle<>( borrowed( PyImport_AddModule(
                                            moduleName ) ) ) );
        scope().attr( scopeName ) = module;
        scope within( module );

        register_imaterial();
        register_omaterial();
        register_materialflatten();
        register_materialassignment();
   }
}
