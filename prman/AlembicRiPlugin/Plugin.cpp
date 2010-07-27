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

#include "Plugin.h"

namespace AlembicRiPlugin {

//-*****************************************************************************
Plugin::Plugin()
{
    // Default constructors are okay for now
}

//-*****************************************************************************
AssetPtr Plugin::asset( const std::string &fileName )
{
    AssetMap::iterator iter = m_map.find( fileName );
    if ( iter != m_map.end() )
    {
        return (*iter).second;
    }
    else
    {
        MutexLock lock( m_mutex );

        // This version of the constructor does not throw an exception.
        AssetPtr aptr( new Asset( m_factory, m_context, fileName ) );
        if ( !aptr || !aptr->valid() )
        {
            std::string errStr = ( ( bool )aptr ) ?
                aptr->errorString() : "UNKNOWN ERRORS";
            ABCRI_ERROR( "Could not load Alembic Asset: " << fileName
                         << std::endl
                         << "Alembic Errors: " << errStr );
        }
        m_map[fileName] = aptr;
        return aptr;
    }
}

} // End namespace AlembicRiPlugin
