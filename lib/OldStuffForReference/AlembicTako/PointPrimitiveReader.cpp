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

#include <AlembicTako/PointPrimitiveReader.h>

namespace AlembicTako {

//-*****************************************************************************
PointPrimitiveReader::PointType PointPrimitiveReader::read( float iFrame )
{
    if ( !m_pointPrimitive ) { return READ_ERROR; }

    AlembicAsset::seconds_t seconds = iFrame == FLT_MAX ?
        AlembicAsset::Time::kRestTimeValue() :
        ( AlembicAsset::seconds_t )iFrame;
    
    m_position = m_pointPrimitive->position().getAnimLowerBound( seconds );
    m_velocity = m_pointPrimitive->velocity().getAnimLowerBound( seconds );
    m_particleIds =
        m_pointPrimitive->particleIds().getAnimLowerBound( seconds );
    m_width =
        m_pointPrimitive->width().getAnimLowerBound( seconds );
    m_constantWidth =
        m_pointPrimitive->constantWidth().getAnimLowerBound( seconds );

    if ( m_pointPrimitive->position().isAnimated() ||
         m_pointPrimitive->velocity().isAnimated() ||
         m_pointPrimitive->particleIds().isAnimated() ||
         m_pointPrimitive->width().isAnimated() ||
         m_pointPrimitive->constantWidth().isAnimated() )
    {
        return POINT_ANIMATED;
    }
    else
    {
        return POINT_STATIC;
    }
}

} // End namespace AlembicTako
