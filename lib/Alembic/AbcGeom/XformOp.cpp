//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <boost/format.hpp>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
XformOp::XformOp()
  : m_type( kTranslateOperation )
  , m_hint( 0 )
  , m_opName( ".t" )
{
    m_channels.clear();
    m_channels.resize( 3 );
}

//-*****************************************************************************
XformOp::XformOp( const XformOperationType iType,
                  const Alembic::Util::uint8_t iHint )
    : m_type( iType )
    , m_hint( iHint )
{
    m_channels.clear();

    switch ( m_type )
    {
    case kScaleOperation:
        m_opName = ".s";
        m_channels.resize( 3 );
        break;
    case kTranslateOperation:
        m_opName = ".t";
        m_channels.resize( 3 );
        break;
    case kRotateOperation:
        m_opName = ".r";
        m_channels.resize( 4 );
        break;
    case kMatrixOperation:
        m_opName = ".m";
        m_channels.resize( 16 );
        break;
    }
}

//-*****************************************************************************
XformOp::XformOp( const Alembic::Util::uint8_t iEncodedOp )
{

    m_type = (XformOperationType)(iEncodedOp >> 4);
    m_hint = iEncodedOp & 0xF;

    switch ( m_type )
    {
    case kScaleOperation:
        m_opName = ".s";
        m_channels.resize( 3 );
        break;
    case kTranslateOperation:
        m_opName = ".t";
        m_channels.resize( 3 );
        break;
    case kRotateOperation:
        m_opName = ".r";
        m_channels.resize( 4 );
        break;
    case kMatrixOperation:
        m_opName = ".m";
        m_channels.resize( 16 );
        break;
    }
}

//-*****************************************************************************
XformOperationType XformOp::getType() const
{
    return m_type;
}

//-*****************************************************************************
void XformOp::setType( const XformOperationType iType )
{
    m_type = iType;

    switch ( m_type )
    {
    case kScaleOperation:
        m_opName = ".s";
        break;
    case kTranslateOperation:
        m_opName = ".t";
        break;
    case kRotateOperation:
        m_opName = ".r";
        break;
    case kMatrixOperation:
        m_opName = ".m";
        break;
    }
}

//-*****************************************************************************
uint8_t XformOp::getHint() const
{
    return m_hint;
}

//-*****************************************************************************
void XformOp::setHint( const Alembic::Util::uint8_t iHint )
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
    return m_animChannels.count( iIndex ) > 0;
}

//-*****************************************************************************
std::size_t XformOp::getNumChannels() const
{
    return m_channels.size();
}

//-*****************************************************************************
std::string XformOp::getChannelName( std::size_t iIndex ) const
{
    std::string c;

    switch ( iIndex )
    {
    case 0:
        if ( m_type != kMatrixOperation ) { c = "x_"; }
        else { c = "00_"; }
        break;
    case 1:
        if ( m_type != kMatrixOperation ) { c = "y_"; }
        else { c = "01_"; }
        break;
    case 2:
        if ( m_type != kMatrixOperation ) { c = "z_"; }
        else { c = "02_"; }
        break;
    case 3:
        if ( m_type == kRotateOperation ) { c = "r_"; }
        else if ( m_type == kMatrixOperation ) { c = "03_"; }
        else
        {
            ABCA_ASSERT( false,
                         "Bad index '" << iIndex << "' for non-matrix or "
                         << "non-rotation xform op." );
        }
        break;
    case 4:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "10_";
        break;
    case 5:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "11_";
        break;
    case 6:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "12_";
        break;
    case 7:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "13_";
        break;
    case 8:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "20_";
        break;
    case 9:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "21_";
        break;
    case 10:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "22_";
        break;
    case 11:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "23_";
        break;
    case 12:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "30_";
        break;
    case 13:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "31_";
        break;
    case 14:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "32_";
        break;
    case 15:
        ABCA_ASSERT( m_type == kMatrixOperation,
                     "Bad index '" << iIndex << "' for non-matrix xform op." );
        c = "33_";
        break;
    }

    return ( boost::format( "%s%s" ) % m_opName % c ).str();
}

//-*****************************************************************************
double XformOp::getDefaultChannelValue( std::size_t iIndex ) const
{
    switch ( m_type )
    {
    case kTranslateOperation:
    case kRotateOperation:
        return 0.0;
    case kScaleOperation:
        return 1.0;
    case kMatrixOperation:
        switch ( iIndex )
        {
        case 0:
        case 5:
        case 10:
        case 15:
            return 1.0;
        default:
            return 0.0;
        }
    default:
        return 0.0;
    }
}

//-*****************************************************************************
double XformOp::getChannelValue( std::size_t iIndex ) const
{
    return m_channels[iIndex];
}

//-*****************************************************************************
void XformOp::setChannelValue( std::size_t iIndex, double iVal )
{
    m_channels[iIndex] = iVal;
}

//-*****************************************************************************
Alembic::Util::uint8_t XformOp::getOpEncoding() const
{
    return ( m_type << 4 ) | ( m_hint & 0xF );
}

//-*****************************************************************************
bool XformOp::isTranslateOp() const
{
    return m_type == kTranslateOperation;
}

//-*****************************************************************************
bool XformOp::isScaleOp() const
{
    return m_type == kScaleOperation;
}

//-*****************************************************************************
bool XformOp::isRotateOp() const
{
    return m_type == kRotateOperation;
}

//-*****************************************************************************
bool XformOp::isMatrixOp() const
{
    return m_type == kMatrixOperation;
}

//-*****************************************************************************
void XformOp::setVector( const Abc::V3d &iVec )
{
    ABCA_ASSERT( m_type != kMatrixOperation,
                 "Meaningless to set Abc::V3d on matrix op" );

    m_channels[0] = iVec.x;
    m_channels[1] = iVec.y;
    m_channels[2] = iVec.z;
}

//-*****************************************************************************
void XformOp::setTranslate( const Abc::V3d &iTrans )
{
    ABCA_ASSERT( m_type == kTranslateOperation,
                 "Meaningless to set translate on non-translate op." );

    this->setVector( iTrans );
}

//-*****************************************************************************
void XformOp::setScale( const Abc::V3d &iScale )
{
    ABCA_ASSERT( m_type == kScaleOperation,
                 "Meaningless to set scale on non-scale op." );

    this->setVector( iScale );
}

//-*****************************************************************************
void XformOp::setAxis( const Abc::V3d &iAxis )
{
    ABCA_ASSERT( m_type == kRotateOperation,
                 "Meaningless to set rotation axis on non-rotation op." );

    this->setVector( iAxis );
}

//-*****************************************************************************
void XformOp::setAngle( const double iAngle )
{
    ABCA_ASSERT( m_type == kRotateOperation,
                 "Meaningless to set rotation angle on non-rotation op." );

    m_channels[3] = iAngle;
}

//-*****************************************************************************
void XformOp::setMatrix( const Abc::M44d &iMatrix )
{
    ABCA_ASSERT( m_type == kMatrixOperation,
                 "Cannot set non-matrix op from Abc::M44d" );

    for ( size_t i = 0 ; i < 4 ; ++i )
    {
        for ( size_t j = 0 ; j < 4 ; ++j )
        {
            m_channels[( i * 4 ) + j] = iMatrix.x[i][j];
        }
    }
}

//-*****************************************************************************
Abc::V3d XformOp::getVector() const
{
    ABCA_ASSERT( m_type != kMatrixOperation,
                 "Meaningless to get Abc::V3d from matrix op" );

    return Abc::V3d( m_channels[0], m_channels[1], m_channels[2] );
}

//-*****************************************************************************
Abc::V3d XformOp::getTranslate() const
{
    ABCA_ASSERT( m_type == kTranslateOperation,
                 "Meaningless to get translate vector from non-translate op." );

    return this->getVector();
}

//-*****************************************************************************
Abc::V3d XformOp::getScale() const
{
    ABCA_ASSERT( m_type == kScaleOperation,
                 "Meaningless to get scaling vector from non-scale op." );

    return this->getVector();
}

//-*****************************************************************************
Abc::V3d XformOp::getAxis() const
{
    ABCA_ASSERT( m_type == kRotateOperation,
                 "Meaningless to get rotation axis from non-rotation op." );

    return this->getVector();
}

//-*****************************************************************************
double XformOp::getAngle() const
{
    ABCA_ASSERT( m_type == kRotateOperation,
                 "Meaningless to get rotation angle from non-rotation op." );

    return m_channels[3];
}

//-*****************************************************************************
Abc::M44d XformOp::getMatrix() const
{
    ABCA_ASSERT( m_type == kMatrixOperation,
                 "Can't get matrix from non-matrix op." );

    Abc::M44d ret;

    for ( size_t i = 0 ; i < 4 ; ++i )
    {
        for ( size_t j = 0 ; j < 4 ; ++j )
        {
            ret.x[i][j] = m_channels[( i * 4 ) + j];
        }
    }

    return ret;
}

} // End namespace AbcGeom
} // End namespace Alembic
