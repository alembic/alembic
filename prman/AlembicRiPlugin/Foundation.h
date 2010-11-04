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

#ifndef _AlembicRiPlugin_Foundation_h_
#define _AlembicRiPlugin_Foundation_h_

#include <ri.h>

#include <AlembicTraitsGeom/AlembicTraitsGeom.h>
#include <AlembicAsset/AlembicAsset.h>

#include <AlembicUtil/OperatorBool.h>

//#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/format.hpp>
#include <boost/utility.hpp>

#include <vector>
#include <assert.h>
#include <map>

namespace AlembicRiPlugin {

//-*****************************************************************************
// Promote the namespace.
namespace Abc = AlembicAsset;
namespace Atg = AlembicTraitsGeom;

//-*****************************************************************************
// test
template<typename T>
class _Lock : public boost::lock_guard<T>
{
public:
    _Lock( T &m_ )
      : boost::lock_guard<T>( m_ )
    {
        std::cout << "LOCK" << std::endl;
    }

    ~_Lock() { std::cout << "UNLOCK" << std::endl; }
};


//-*****************************************************************************
// Promote the thread concepts.
// RAII-style use of the mutex ensures it will be unlocked every time it's
// locked.
typedef boost::mutex Mutex;
typedef boost::lock_guard<Mutex> MutexLock;
//typedef _Lock<Mutex> MutexLock;


//-*****************************************************************************
// Time Samples
typedef std::vector<Abc::chrono_t> ChronoSamples;
typedef std::vector<Abc::Time> TimeSamples;

//-*****************************************************************************
// Create some easy error reporting functions
#define ABCRI_WARN( TEXT )                                              \
do                                                                      \
{                                                                       \
    std::cerr << "AlembicRiPlugin WARNING: " << TEXT << std::endl;      \
}                                                                       \
while ( 0 )

//-*****************************************************************************
#define ABCRI_ERROR( TEXT )                                             \
do                                                                      \
{                                                                       \
    std::cerr << "AlembicRiPlugin ERROR: " << TEXT << std::endl;        \
}                                                                       \
while ( 0 )

} // End namespace AlembicRiPlugin

#endif
