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

#ifndef PyBind11Alembic_Foundation_h_
#define PyBind11Alembic_Foundation_h_

#include <Python.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/operators.h>
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
#include <optional>

namespace py = pybind11;
typedef unsigned char byte_t;

#if PY_MAJOR_VERSION < 3
#define ALEMBIC_PYTHON_BOOL_NAME "__nonzero__"
#define ALEMBIC_PYTHON_NEXT_NAME "next"
#else
#define ALEMBIC_PYTHON_BOOL_NAME "__bool__"
#define ALEMBIC_PYTHON_NEXT_NAME "__next__"
#endif

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
    throw py::error_already_set();
}

inline void throwPythonKeyException( const char* iMessage )
{
    PyErr_SetString( PyExc_KeyError, iMessage );
    throw py::error_already_set();
}

inline void throwPythonIndexException( const char* iMessage )
{
    PyErr_SetString( PyExc_IndexError, iMessage );
    throw py::error_already_set();
}

template<typename T>
bool check_cast( py::handle obj ) {
  try{
    obj.cast<T>();
    return true;
  }
  catch (py::cast_error &e) {
    return false;
  }
  catch (py::error_already_set &e) {
    return false;
  }
}

template <typename T>
struct extract
{
  py::handle obj;
  extract( py::handle handle_obj ) : obj(handle_obj) {}

bool check() { return check_cast<T>(obj); }
T operator()() { return obj.cast<T>(); }
};

template<class T>
py::array_t<T> getNumPyArray(size_t width, size_t height, T* data_ptr  = nullptr)
{
    size_t ndim = 1;
    std::vector<size_t> shape = {width};
    std::vector<size_t> strides = {height *sizeof(T)};
    if (height > 1)
    {
      ndim = 2;
      shape.push_back(height);
      strides.push_back(sizeof(T));
    }

    return py::array_t<T>(
          py::buffer_info(
               data_ptr,
               sizeof(T), //itemsize
               py::format_descriptor<T>::format(),
               ndim,
               shape,
               strides
           )
    );
}

#endif
