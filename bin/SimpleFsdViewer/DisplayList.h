//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks, Inc. and
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
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
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

#ifndef _SimpleFsdViewer_DisplayList_h_
#define _SimpleFsdViewer_DisplayList_h_

#include "Foundation.h"

namespace SimpleFsdViewer {

//-*****************************************************************************
// CLASS DisplayListBase
// Encapsulation of an OpenGL Display List
// This base class cannot be created normally - it exists
// purely as support for the other classes.
class DisplayListBase
{
public:
    void destroy();
    bool valid() const;

protected:
    DisplayListBase();
    ~DisplayListBase();

    void create();

    GLuint m_displayList;
};

//-*****************************************************************************
// CLASS DisplayList
// A display list who's display commands are provided externally
// through the use of the newList and endList commands
class DisplayList : public DisplayListBase
{
public:
    void newList( GLenum mode );
    void endList();
    void callList();
};

//-*****************************************************************************
// TEMPLATE AND INLINE FUNCTIONS
//-*****************************************************************************
inline bool DisplayListBase::valid() const
{
    if ( m_displayList == 0 )
    {
        return false;
    }
    else
    {
        // return ( bool )glIsList( m_displayList );
        return true;
    }
}

} // End namespace SimpleFsdViewer

#endif
