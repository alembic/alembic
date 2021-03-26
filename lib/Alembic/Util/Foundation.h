//-*****************************************************************************
//
// Copyright (c) 2009-2015,
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

#ifndef Alembic_Util_Foundation_h
#define Alembic_Util_Foundation_h

#include <Alembic/Util/Config.h>

#include <unordered_map>

#include <memory>

#include <half.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <exception>
#include <limits>

#include <list>
#include <map>
#include <string>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <Alembic/Util/Export.h>

#ifdef _MSC_VER

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// avoid windows min/max predefined macro conflicts
#ifndef NOMINMAX
#define NOMINMAX
#endif

// needed for mutex stuff
#include <Windows.h>
#endif

// needed for std min/max
#include <algorithm>

// When we change this version (because of an ABI change) Make sure to at
// LEAST bump the minor version in CMakeLists.txt i.e. PROJECT_VERSION_MINOR
// This way we will hopefully not break any distros that kindly include us.
// See Issue243
#ifndef ALEMBIC_VERSION_NS
#define ALEMBIC_VERSION_NS v12
#endif

namespace Alembic {
namespace Util {
namespace ALEMBIC_VERSION_NS {

// similiar to boost::noncopyable
// explicitly hides copy construction and copy assignment
class ALEMBIC_EXPORT noncopyable
{
protected:
    noncopyable() {}
    ~noncopyable() {}

private:
    noncopyable( const noncopyable& );
    const noncopyable& operator=( const noncopyable& );
};

using std::dynamic_pointer_cast;
using std::enable_shared_from_this;
using std::shared_ptr;
using std::static_pointer_cast;
using std::weak_ptr;
using std::unordered_map;
using std::unique_ptr;

// similiar to boost::totally_ordered
// only need < and == operators and this fills in the rest
template < class T >
class totally_ordered
{
    friend bool operator > ( const T& x, const T& y )
    {
        return y < x;
    }

    friend bool operator <= ( const T& x, const T& y )
    {
        return !( y < x );
    }

    friend bool operator >= ( const T& x, const T& y )
    {
        return !( x < y );
    }

    friend bool operator != ( const T& x, const T& y )
    {
        return !( x == y );
    }
};

// inspired by boost::mutex
#ifdef _MSC_VER

class mutex : noncopyable
{
public:
    mutex()
    {
         InitializeCriticalSection(&cs);
    }

    ~mutex()
    {
        DeleteCriticalSection(&cs);
    }

    void lock()
    {
        EnterCriticalSection(&cs);
    }

    void unlock()
    {
        LeaveCriticalSection(&cs);
    }

private:
    CRITICAL_SECTION cs;
};

#else


class mutex : noncopyable
{
public:
    mutex()
    {
        pthread_mutex_init( &m, NULL );
    }

    ~mutex()
    {
        pthread_mutex_destroy( &m );
    }

    void lock()
    {
        pthread_mutex_lock( &m );
    }

    void unlock()
    {
        pthread_mutex_unlock( &m );
    }

private:
    pthread_mutex_t m;
};

#endif

class scoped_lock : noncopyable
{
public:
    scoped_lock( mutex & l ) : m( l )
    {
        m.lock();
    }

    ~scoped_lock()
    {
        m.unlock();
    }

private:
    mutex & m;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Util
} // End namespace Alembic

#endif
