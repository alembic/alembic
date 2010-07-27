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

#ifndef _Alembic_Core_WrittenSlabRef_h_
#define _Alembic_Core_WrittenSlabRef_h_

#include <Alembic/Core/Foundation.h>

namespace Alembic {
namespace Core {

//-*****************************************************************************
class WrittenSlabRef
{
public:
    WrittenSlabRef()
      : m_objLocId( 0 ),
        m_objName( "" ) {}
    WrittenSlabRef( const WrittenSlabRef &copy )
      : m_objLocId( copy.m_objLocId ),
        m_objName( copy.m_objName ) {}
    WrittenSlabRef& operator=( const WrittenSlabRef &copy )
    {
        m_objLocId = copy.m_objLocId;
        m_objName = copy.m_objName;
        return *this;
    }
    
    bool valid() const
    {
        return ( m_objLocId > 0 ) && ( m_objName != "" );
    }
                    
protected:                
    friend class SlabWriter;
    WrittenSlabRef( hid_t obj_loc_id,
                    const std::string &obj_name )
      : m_objLocId( obj_loc_id ),
        m_objName( obj_name ) {}

    hid_t objLocId() const { return m_objLocId; }
    const std::string &objName() const { return m_objName; }

private:

    hid_t m_objLocId;
    std::string m_objName;
};

} // End namespace Core
} // End namespace Alembic

#endif
