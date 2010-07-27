//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic, nor the names of their contributors may be used
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

#include <FluidSimDemo/SimWriter.h>

namespace FluidSimDemo {

//-*****************************************************************************
SimWriter::SimWriter( const std::string &iAlembicOutFileName )
  : m_fileName( iAlembicOutFileName ),
    m_lastFrameWritten( NON_FRAME ),
    m_rootNode( MakeTakoSceneWriter( iAlembicOutFileName ) )
{
    if ( !m_rootNode )
    {
        FSD_THROW( "Could not open file: " << iAlembicOutFileName );
    }

    m_partsXform = MakeTransformWriterPtr( "waterParticles",
                                           m_rootNode );
    
    m_partsShape = MakePointPrimitiveWriterPtr( "waterParticlesShape",
                                                m_partsXform );

    
    m_partsXform->push( Translate( Translate::cTranslate,
                                   Value( 0.0 ),
                                   Value( 0.0 ),
                                   Value( 0.0 ) ) );
    m_partsXform->writeTransformStack( FLT_MAX, true );
}

//-*****************************************************************************
SimWriter::~SimWriter()
{
    if ( !m_partsShape )
    {
        FSD_THROW( "Trying to close: "
                   << m_fileName << " after has been closed." );
    }
    
    if ( m_lastFrameWritten == NON_FRAME )
    {
        std::vector<float> dummy;
        writeFrame( 1, dummy, dummy, 0.0001f );
    }

    m_partsShape.reset();
    m_partsXform.reset();
    m_rootNode.reset();
}

//-*****************************************************************************
void SimWriter::writeFrame( int iFrame,
                            const std::vector<float> &iPositions,
                            const std::vector<float> &iVelocities,
                            float iParticleRadius )
{
    if ( !m_partsShape )
    {
        FSD_THROW( "Trying to write frame to: "
                   << m_fileName << " after has been closed." );
    }
    
    if ( m_lastFrameWritten != NON_FRAME )
    {
        if ( iFrame != ( m_lastFrameWritten + 1 ) )
        {
            FSD_THROW( "Error, out-of-sequence writes: "
                       << m_lastFrameWritten
                       << " followed by "
                       << iFrame );
        }
    }

    size_t numParticles = iPositions.size() / 3;
    if ( numParticles < 1 )
    {
        // Fake it.
        std::vector<float> pos;
        pos.push_back( 0.0f );
        pos.push_back( 0.0f );
        pos.push_back( 0.0f );
        
        std::vector<float> vel;
        vel.push_back( 0.0f );
        vel.push_back( 0.0f );
        vel.push_back( 0.0f );
        
        m_particleIds.resize( 1 );
        m_particleIds[0] = 0;
        
        m_partsShape->write( ( float )iFrame,
                             pos, vel, m_particleIds,
                             0.0001f );
    }
    else
    {
        size_t oldSize = m_particleIds.size();
        m_particleIds.resize( numParticles );
        for ( size_t i = oldSize; i < numParticles; ++i )
        {
            m_particleIds[i] = ( int )i;
        }
        
        m_partsShape->write( ( float )iFrame,
                             iPositions,
                             iVelocities,
                             m_particleIds,
                             iParticleRadius );
    }

    m_lastFrameWritten = iFrame;
}

} // End namespace FluidSimDemo
