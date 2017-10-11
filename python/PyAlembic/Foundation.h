//-*****************************************************************************
//
// Copyright (c) 2009-2016,
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

#ifndef PyAlembic_Foundation_h_
#define PyAlembic_Foundation_h_

#include <Python.h>
#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreAbstract/All.h>
#include <Alembic/Util/Config.h>
#ifdef ALEMBIC_WITH_HDF5
#include <Alembic/AbcCoreHDF5/All.h>
#endif
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcCollection/All.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcMaterial/All.h>
#include <string>

#include <boost/version.hpp>

// if Alembic uses shared_ptr from boost, then we don't need this function because boost provides it
// if Alembic uses shared_ptr from std, then we don't need this function because boost provides it since 1.53
// if Alembic uses shared_ptr from tr1, then we do need this function
#if defined(ALEMBIC_LIB_USES_TR1) || (BOOST_VERSION < 105300 && !defined(ALEMBIC_LIB_USES_BOOST))
namespace boost
{
template<class T>
inline T * get_pointer( Alembic::Util::shared_ptr<T> const & p )
{
        return p.get();
}

} // namespace boost
#endif

#include <boost/python/detail/wrap_python.hpp>
#include <boost/python.hpp>

namespace Abc  = ::Alembic::Abc;
namespace AbcA = ::Alembic::AbcCoreAbstract;
namespace AbcF = ::Alembic::AbcCoreFactory;
#ifdef ALEMBIC_WITH_HDF5
namespace AbcH = ::Alembic::AbcCoreHDF5;
#endif
namespace AbcO = ::Alembic::AbcCoreOgawa;
namespace AbcC = ::Alembic::AbcCollection;
namespace AbcG = ::Alembic::AbcGeom;
namespace AbcU = ::Alembic::Util;
namespace AbcM = ::Alembic::AbcMaterial;

//-*****************************************************************************
inline void throwPythonException( const char* iMessage )
{
    PyErr_SetString( PyExc_RuntimeError, iMessage );
    throw boost::python::error_already_set();
}

inline void throwPythonKeyException( const char* iMessage )
{
    PyErr_SetString( PyExc_KeyError, iMessage );
    throw boost::python::error_already_set();
}

inline void throwPythonIndexException( const char* iMessage )
{
    PyErr_SetString( PyExc_IndexError, iMessage );
    throw boost::python::error_already_set();
}

#endif
