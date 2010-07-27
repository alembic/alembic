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

#include <AlembicHDF5/Init.h>
#include <AlembicHDF5/Exception.h>
#include <AlembicHDF5/Assert.h>
#include <boost/utility.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/make_shared.hpp>

namespace AlembicHDF5 {

//-*****************************************************************************
static bool g_init = false;

//-*****************************************************************************
// For forcing H5close().
struct HDF5Closer : public boost::noncopyable
{
    HDF5Closer() {}
    ~HDF5Closer()
    {
        herr_t status = H5close();
        ABCH5_WARN( status >= 0,
                    "AlembicHDF5::shutdown. H5close() failed." );
        //std::cout << "H5close() completed successfully." << std::endl;
    }
};

static boost::shared_ptr<HDF5Closer> g_closer;

//-*****************************************************************************
void Init( bool disableAutoDelete, bool disableErrors )
{
    if ( g_init )
    {
        return;
    }

    herr_t status;
    
    if ( disableAutoDelete )
    {
        status = H5dont_atexit();
        ABCH5_ASSERT( status >= 0,
                      "AlembicHDF5::Init() H5dont_atexit() failed." );
    }

    // Call "H5open", just in case.
    status = H5open();
    ABCH5_ASSERT( status >= 0,
                  "AlembicHDF5::Init() H5open() failed." );

    // Make a closer. Some things need H5close().
    ABCH5_ASSERT( !g_closer,
                  "AlembicHDF5::Init() somebody already made a closer?" );
    g_closer = boost::make_shared<HDF5Closer>();


    if ( disableErrors )
    {
        status = H5Eset_auto2( H5E_DEFAULT, NULL, NULL );
        ABCH5_ASSERT( status >= 0,
                      "AlembicHDF5::Init() H5Eset_auto2() failed." );
    }

    g_init = true;
}

//-*****************************************************************************
bool IsInitialized()
{
    return g_init;
}

} // End namespace AlembicHDF5
