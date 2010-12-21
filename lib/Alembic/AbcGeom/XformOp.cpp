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

#include <Alembic/AbcGeom/XformOp.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
XformOp::XformOp() : m_type(kTranslateOperation), m_anim(0), m_hint(0) 
{
};

//-*****************************************************************************
XformOp::XformOp(XformOperationType iType, Alembic::Util::uint8_t iHint)
    : m_type(iType)
{
    setHint(iHint);
    m_anim = 0;
}

//-*****************************************************************************
XformOperationType XformOp::getType() const
{
    return m_type;
}

//-*****************************************************************************
void XformOp::setType(XformOperationType iType)
{
    m_type = iType;
    m_hint = 0;
    m_anim = 0;
}

//-*****************************************************************************
uint8_t XformOp::getHint() const
{
    return m_hint;
}

//-*****************************************************************************
void XformOp::setHint(Alembic::Util::uint8_t iHint)
{
    // if a non-existant hint value is set, default it to 0
    if ( m_type == kScaleOperation && iHint > kScaleHint )
    {
        m_hint = 0;
    }
    else if ( m_type == kTranslateOperation && iHint > 
        kRotatePivotTranslationHint )
    {
        m_hint = 0;
    }
    else if ( m_type == kRotateOperation && iHint > kRotateOrientationHint )
    {
        m_hint = 0;
    }
    else if ( m_type == kMatrixOperation && iHint > kMayaShearHint )
    {
        m_hint = 0;
    }
    else
    {
        m_hint = iHint;
    }
}

//-*****************************************************************************
bool XformOp::isXAnimated() const
{
    return isIndexAnimated(0);
}

//-*****************************************************************************
void XformOp::setXAnimated(bool iAnim)
{
    setIndexAnimated(0, iAnim);
}

//-*****************************************************************************
bool XformOp::isYAnimated() const
{
    return isIndexAnimated(1);
}

//-*****************************************************************************
void XformOp::setYAnimated(bool iAnim)
{
    setIndexAnimated(1, iAnim);
}

//-*****************************************************************************
bool XformOp::isZAnimated() const
{
    return isIndexAnimated(2);
}

//-*****************************************************************************
void XformOp::setZAnimated(bool iAnim)
{
    setIndexAnimated(2, iAnim);
}

//-*****************************************************************************
bool XformOp::isAngleAnimated() const
{
    return isIndexAnimated(3);
}

//-*****************************************************************************
void XformOp::setAngleAnimated(bool iAnim)
{
    setIndexAnimated(3, iAnim);
}

//-*****************************************************************************
bool XformOp::isIndexAnimated(uint8_t iIndex) const
{
    return ( m_anim >> iIndex ) & 0x01;
}

//-*****************************************************************************
void XformOp::setIndexAnimated(uint8_t iIndex, bool iAnim)
{
    // if the index is not correct for the operation, then just return
    if ( iIndex > 15 || (m_type == kRotateOperation && iIndex > 3) ||
        ((m_type == kTranslateOperation || m_type == kScaleOperation) &&
        iIndex > 2) )
    {
        return;
    }

    // set the bit
    if (iAnim)
        m_anim = m_anim | (0x1 << iIndex);
    // unset the bit
    else
        m_anim = m_anim & (0xffff ^ (0x1 << iIndex));
}

//-*****************************************************************************
Alembic::Util::uint8_t XformOp::getNumIndices() const
{
    switch (m_type)
    {
        case kScaleOperation:
        case kTranslateOperation:
            return 3;
        break;

        case kRotateOperation:
            return 4;
        break;

        case kMatrixOperation:
            return 16;
        break;

        default:
            return 0;
        break;
    }
    return 0;
}

//-*****************************************************************************
Alembic::Util::uint32_t XformOp::getEncodedValue() const
{
    return (m_anim << 16) | (m_hint << 8) | m_type;
}

//-*****************************************************************************
void XformOp::setEncodedValue(Alembic::Util::uint32_t iVal)
{
    // do it this way to make sure every byte is sane, or end up being
    // broken down into a sane value

    Alembic::Util::uint32_t rawType = iVal & 0xff;

    // beyond matrix reset to type scale
    if (rawType > 3)
        rawType = 0;

    XformOperationType type = (XformOperationType)(rawType);
    this->setType(type);
    this->setHint((iVal >> 8) & 0xff);
    uint16_t anim = (iVal >> 16) & 0xffff;
    for (size_t i = 0; i < 16; ++i)
    {
        this->setIndexAnimated(i, (anim >> i) & 0x1);
    }
}

} // End namespace AbcGeom
} // End namespace Alembic
