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

#ifndef _Alembic_Core_Foundation_h_
#define _Alembic_Core_Foundation_h_

#include <Alembic/HDF5/HDF5.h>
#include <Alembic/Util/Util.h>

#include <MD5Hash/MD5.h>
#include <MD5Hash/MapUtil.h>
#include <MD5Hash/Process.h>

#include <boost/utility.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/format.hpp>
#include <boost/cstdint.hpp>

#include <typeinfo>

#include <utility>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <half.h>
#include <assert.h>

namespace Alembic {
namespace Core {

//-*****************************************************************************
// Basic promotion of typedefs and renaming to avoid confusion.
typedef HDF5::AttributedObject  H5AttrObj;
typedef HDF5::File              H5F;
typedef HDF5::Group             H5G;
typedef HDF5::Dataset           H5D;
typedef HDF5::Datatype          H5T;
typedef HDF5::Dataspace         H5S;
typedef HDF5::Attribute         H5A;

typedef boost::shared_ptr<H5F> SharedH5F;
typedef boost::shared_ptr<H5G> SharedH5G;
typedef boost::shared_ptr<H5D> SharedH5D;
typedef boost::shared_ptr<H5T> SharedH5T;
typedef boost::shared_ptr<H5S> SharedH5S;
typedef boost::shared_ptr<H5A> SharedH5A;

//-*****************************************************************************
// H5T
inline SharedH5T MakeSharedH5T( hid_t id )
{ return boost::make_shared<H5T>( id ); }
inline SharedH5T MakeSharedH5T( const H5T &base, hsize_t rank0dims )
{ return boost::make_shared<H5T>( base, rank0dims ); }


} // End namespace Core
} // End namespace Alembic

#endif
