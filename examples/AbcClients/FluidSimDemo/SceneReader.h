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

#ifndef _FluidSimDemo_SceneReader_h_
#define _FluidSimDemo_SceneReader_h_

#include <FluidSimDemo/Foundation.h>
#include <FluidSimDemo/CollisionObject.h>
#include <FluidSimDemo/TransformDrw.h>

namespace FluidSimDemo {

//-*****************************************************************************
class SceneReader
{
public:
    SceneReader( const std::string &iFileName,
                 const M44d &worldXform );

    // Return the file name.
    const std::string & fileName() const { return m_fileName; }

    CollisionObjects & collisionObjects() { return m_collisionObjects; }
    const CollisionObjects & collisionObjects() const
    { return m_collisionObjects; }

    //-*************************************************************************
    // FRAME RANGE AND FPS.
    float startFrame() const { return m_startFrame; }
    float endFrame() const { return m_endFrame; }

    //-*************************************************************************
    // SIM READER CONTROLS.
    //-*************************************************************************
    float currentFrame() const { return m_currentFrame; }

    // This reads forward to the new time.
    void updateToFrame( float newFrame );

    // Get bounds
    Box3d getBounds() const { return m_sceneXformDrw->getBounds(); }

    // This draws, assuming a camera matrix has already been set.
    void draw() const;

protected:
    std::string m_fileName;

    IArchive m_archive;
    IObject m_topObject;
    M44d m_worldXform;

    // These attributes I KNOW to read out of the reader.
    float m_fps;
    float m_startFrame;
    float m_endFrame;

    // Also initialized from the reader
    CollisionObjects m_collisionObjects;

    // Sim state.
    float m_currentFrame;

    // The Actual Scene will require traversal of a hierarchy.
    TransformDrwPtr m_sceneXformDrw;
};

} // End namespace FluidSimDemo

#endif

