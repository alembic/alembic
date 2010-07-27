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

#ifndef _AlembicTako_PointPrimitiveReader_h_
#define _AlembicTako_PointPrimitiveReader_h_

#include <AlembicTako/Foundation.h>
#include <AlembicTako/HDFReaderNode.h>
#include <AlembicTako/PointPrimitive.h>
#include <AlembicTako/TransformReader.h>

namespace AlembicTako {

//-*****************************************************************************
class PointPrimitiveReader : public HDFReaderNode
{
public:
    // Make as child of reader.
    PointPrimitiveReader( const std::string &nodeName,
                          TransformReader &parent )
      : HDFReaderNode(),
        m_pointPrimitive( parent.alembicObject(), nodeName,
                          AlembicAsset::kThrowException )
    {
        HDFReaderNode::init( m_pointPrimitive );
    }

    // Return type for read function.
    enum PointType
    {
        POINT_STATIC,
        POINT_ANIMATED,
        READ_ERROR
    };

    PointType read( float iFrame );

    const AlembicAsset::IV3fSample &position() const
    { return m_position; }
    const AlembicAsset::IV3fSample &velocity() const
    { return m_velocity; }
    const AlembicAsset::IIntSample &particleIds() const
    { return m_particleIds; }
    
    bool hasWidth() const { return m_pointPrimitive->width().valid(); }
    const AlembicAsset::IFloatSample &width() const
    { return m_width; }

    bool hasConstantWidth() const
    { return m_pointPrimitive->constantWidth().valid(); }
    float constantWidth() const { return m_constantWidth; }

protected:
    IPointPrimitiveObj m_pointPrimitive;

    AlembicAsset::IV3fSample m_position;
    AlembicAsset::IV3fSample m_velocity;
    AlembicAsset::IIntSample m_particleIds;
    AlembicAsset::IFloatSample m_width;
    float m_constantWidth;
};

} // End namespace AlembicTako

#endif
