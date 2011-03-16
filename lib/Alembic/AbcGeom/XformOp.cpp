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
XformOp::XformOp()
  : m_type( kTranslateOperation )
  , m_hint( 0 )
{}

//-*****************************************************************************
XformOp::XformOp( XformOperationType iType, Alembic::Util::uint8_t iHint )
    : m_type( iType )
    , m_hint( iHint )
{}

//-*****************************************************************************
XformOperationType XformOp::getType() const
{
    return m_type;
}

//-*****************************************************************************
void XformOp::setType( XformOperationType iType )
{
    m_type = iType;
    m_hint = 0;
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
    return m_animChannels.count( 0 ) > 0;
}

//-*****************************************************************************
bool XformOp::isYAnimated() const
{
    return m_animChannels.count( 1 ) > 0;
}

//-*****************************************************************************
bool XformOp::isZAnimated() const
{
    return m_animChannels.count( 2 ) > 0;
}

//-*****************************************************************************
bool XformOp::isAngleAnimated() const
{
    return m_animChannels.count( 3 ) > 0;
}

//-*****************************************************************************
bool XformOp::isChannelAnimated( std::size_t iIndex ) const
{
    // if the index is not correct for the operation, then just return
    if ( iIndex > 15 || (m_type == kRotateOperation && iIndex > 3) ||
         ((m_type == kTranslateOperation || m_type == kScaleOperation) &&
          iIndex > 2) )
    {
        return false;
    }

    return m_animChannels.count( iIndex ) > 0;
}

//-*****************************************************************************
std::size_t XformOp::getNumChannels() const
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
Alembic::Util::uint8_t XformOp::getOpEncoding() const
{
    return ( m_hint << 4 ) | ( m_type & 0xF )
}

} // End namespace AbcGeom
} // End namespace Alembic
