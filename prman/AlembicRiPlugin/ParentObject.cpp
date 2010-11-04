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

#include "ParentObject.h"
#include "Factory.h"
#include "Request.h"
#include "RiUtil.h"
#include "AbcUtil.h"

namespace AlembicRiPlugin {

//-*****************************************************************************
void ParentObject::init( Factory &factory, const Abc::IParentObject &obj )
{
    if ( !obj.valid() )
    {
        m_name = ( boost::format( "ERROR: %s" ) % obj.name() ).str();
        m_fullPathName = ( boost::format( "ERROR: %s" )
                           % obj.fullPathName() ).str();
        m_protocol = ( boost::format( "ERROR: %s" ) % obj.protocol() ).str();
        return;
    }
    
    m_name = obj.name();
    m_fullPathName = obj.fullPathName();
    m_protocol = obj.protocol();
    
    // Assume that subclass constructor will take care of locally storing
    // whatever is needed for local instantiation.
    m_children.erase( m_children.begin(), m_children.end() );
    size_t numChildren = obj.numChildren();
    for ( size_t c = 0; c < numChildren; ++c )
    {
        Abc::ObjectInfo oinfo = obj.childInfo( c );
        m_children.push_back( factory.make( *this,
                                          oinfo->name,
                                          oinfo->protocol ) );
    }
}

//-*****************************************************************************
void ParentObject::instantiate( Request &rq )
{
    // This line of code makes me giggle, and always will.
    Request::Keep keep = rq.keep( m_name,
                                  m_fullPathName,
                                  m_protocol );


    if ( keep.local || keep.recurse )
    {
        RiAttributeBegin();
        
        this->instantiateAttributeState( rq );

        if ( keep.local )
        {
            this->instantiateLocal( rq );
        }

        if ( keep.recurse )
        {
            for ( ParentObjects::iterator iter = m_children.begin();
                  iter != m_children.end(); ++iter )
            {
                (*iter)->instantiate( rq );
            }
        }

        // Unscope!
        RiAttributeEnd();
    }
}

} // End namespace AlembicRiPlugin
