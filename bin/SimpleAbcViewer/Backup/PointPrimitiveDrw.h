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

#ifndef _SimpleAbcViewer_PointPrimitiveDrw_h_
#define _SimpleAbcViewer_PointPrimitiveDrw_h_

#include "Foundation.h"
#include "Drawable.h"

namespace SimpleAbcViewer {

class PointPrimitiveDrw : public Drawable
{
public:
    PointPrimitiveDrw( Atk::PointPrimitiveReaderPtr ppReader );
    virtual ~PointPrimitiveDrw();

    virtual bool valid() const;

    virtual bool readFrame( float i_frame );

    virtual void getFrames( std::set<float> & i_sampleFrames ) const;

    virtual bool hasFrames() const;

    virtual Box3d getBounds() const;

    virtual void draw( const DrawContext & i_ctx ) const;

    void makeInvalid();

protected:
    Atk::PointPrimitiveReaderPtr m_ppReader;
    bool m_valid;
    size_t m_numPoints;
    const GLfloat *m_points;
    const GLfloat *m_colors;
    Box3d m_bounds;
};

} // End namespace SimpleAbcViewer

#endif
