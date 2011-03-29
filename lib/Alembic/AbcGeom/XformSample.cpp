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

#include <Alembic/AbcGeom/XformSample.h>
#include <Alembic/AbcGeom/XformOp.h>

#include <boost/uuid/uuid_generators.hpp>

#include <ImathMatrix.h>
#include <ImathMatrixAlgo.h>
#include <ImathQuat.h>

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
XformSample::XformSample()
  : m_setWithOpStack( 0 )
  , m_inherits( true )
  , m_hasBeenRead( false )
  , m_opIndex( 0 )
{
    m_childBounds.makeEmpty();
    boost::uuids::random_generator gen;
    m_id = gen();
}

//-*****************************************************************************
std::size_t XformSample::addOp( XformOp iOp, const Abc::V3d &iVal )
{
    for ( size_t i = 0 ; i < 3 ; ++i )
    {
        iOp.setChannelValue( i, iVal[i] );
    }

    if ( ! m_hasBeenRead )
    {
        ABCA_ASSERT( m_setWithOpStack == 0 || m_setWithOpStack == 1,
                     "Cannot mix addOp() and set<Foo>() methods." );

        m_setWithOpStack = 1;

        m_ops.push_back( iOp );
        m_opsArray.push_back( iOp.getOpEncoding() );

        return m_ops.size() - 1;
    }
    else
    {
        std::size_t ret = m_opIndex;

        ABCA_ASSERT( m_setWithOpStack == 1,
                     "Cannot mix addOp() and set<Foo>() methods." );

        ABCA_ASSERT( iOp.getType() == m_ops[ret].getType(),
                     "Cannot update mismatched op-type in already-setted "
                     << "XformSample!" );

        m_ops[ret] = iOp;
        m_opIndex = ++m_opIndex % m_ops.size();

        return ret;
    }
}

//-*****************************************************************************
std::size_t XformSample::addOp( XformOp iOp, const Abc::V3d &iAxis,
                                const double iAngleInDegrees )
{
    {
        for ( size_t i = 0 ; i < 3 ; ++i )
        {
            iOp.setChannelValue( i, iAxis[i] );
        }
        iOp.setChannelValue( 3, iAngleInDegrees );

        if ( ! m_hasBeenRead )
        {
            ABCA_ASSERT( m_setWithOpStack == 0 || m_setWithOpStack == 1,
                         "Cannot mix addOp() and set<Foo>() methods." );

            m_setWithOpStack = 1;

            m_ops.push_back( iOp );
            m_opsArray.push_back( iOp.getOpEncoding() );

            return m_ops.size() - 1;
        }
        else
        {
            std::size_t ret = m_opIndex;

            ABCA_ASSERT( iOp.getType() == m_ops[ret].getType(),
                         "Cannot update mismatched op-type in already-setted "
                         << "XformSample!" );

            ABCA_ASSERT( m_setWithOpStack == 1,
                         "Cannot mix addOp() and set<Foo>() methods." );

            m_ops[ret] = iOp;
            m_opIndex = ++m_opIndex % m_ops.size();

            return ret;
        }
    }
}

//-*****************************************************************************
std::size_t XformSample::addOp( XformOp iOp, const Abc::M44d &iVal )
{
    for ( size_t i = 0 ; i < 4 ; ++i )
    {
        for ( size_t j = 0 ; j < 4 ; ++j )
        {
            iOp.setChannelValue( ( i * 4 ) + j, iVal.x[i][j] );
        }
    }

    if ( ! m_hasBeenRead )
    {
        ABCA_ASSERT( m_setWithOpStack == 0 || m_setWithOpStack == 1,
                     "Cannot mix addOp() and set<Foo>() methods." );

        m_setWithOpStack = 1;

        m_ops.push_back( iOp );
        m_opsArray.push_back( iOp.getOpEncoding() );

        return m_ops.size() - 1;
    }
    else
    {
        std::size_t ret = m_opIndex;

        ABCA_ASSERT( iOp.getType() == m_ops[ret].getType(),
                     "Cannot update mismatched op-type in already-setted "
                     << "XformSample!" );

        ABCA_ASSERT( m_setWithOpStack == 1,
                     "Cannot mix addOp() and set<Foo>() methods." );

        m_ops[ret] = iOp;
        m_opIndex = ++m_opIndex % m_ops.size();

        return ret;
    }
}

//-*****************************************************************************
std::size_t XformSample::addOp( const XformOp &iOp )
{
    if ( ! m_hasBeenRead )
    {
        ABCA_ASSERT( m_setWithOpStack == 0 || m_setWithOpStack == 1,
                     "Cannot mix addOp() and set<Foo>() methods." );

        m_setWithOpStack = 1;

        m_ops.push_back( iOp );
        m_opsArray.push_back( iOp.getOpEncoding() );

        return m_ops.size() - 1;
    }
    else
    {
        std::size_t ret = m_opIndex;

        ABCA_ASSERT( iOp.getType() == m_ops[ret].getType(),
                     "Cannot update mismatched op-type in already-setted "
                     << "XformSample!" );

        ABCA_ASSERT( m_setWithOpStack == 1,
                     "Cannot mix addOp() and set<Foo>() methods." );

        m_ops[ret] = iOp;
        m_opIndex = ++m_opIndex % m_ops.size();

        return ret;
    }
}

//-*****************************************************************************
XformOp XformSample::getOp( std::size_t iIndex ) const
{
    return m_ops[iIndex];
}

//-*****************************************************************************
XformOp &XformSample::operator[]( const std::size_t &iIndex )
{
    return m_ops[iIndex];
}

//-*****************************************************************************
const XformOp &XformSample::operator[]( const std::size_t &iIndex ) const
{
    return m_ops[iIndex];
}

//-*****************************************************************************
const std::vector<Alembic::Util::uint8_t> &XformSample::getOpsArray() const
{
    return m_opsArray;
}

//-*****************************************************************************
std::size_t XformSample::getNumOps() const
{
    return m_opsArray.size();
}

//-*****************************************************************************
std::size_t XformSample::getNumOpChannels() const
{
    std::size_t ret = 0;
    for ( size_t i = 0 ; i < m_ops.size() ; ++i )
    {
        ret += m_ops[i].getNumChannels();
    }

    return ret;
}

//-*****************************************************************************
void XformSample::setInheritsXforms( bool iInherits )
{
    m_inherits = iInherits;
}

//-*****************************************************************************
bool XformSample::getInheritsXforms() const
{
    return m_inherits;
}

//-*****************************************************************************
void XformSample::setChildBounds( const Abc::Box3d &iBnds )
{
    m_childBounds = iBnds;
}

//-*****************************************************************************
const Abc::Box3d &XformSample::getChildBounds() const
{
    return m_childBounds;
}

//-*****************************************************************************
void XformSample::setTranslation( const Abc::V3d &iTrans )
{
    XformOp op( kTranslateOperation, kTranslateHint );

    for ( size_t i = 0 ; i < 3 ; ++i )
    {
        op.setChannelValue( i, iTrans[i] );
    }

    if ( ! m_hasBeenRead )
    {
        ABCA_ASSERT( m_setWithOpStack == 0 || m_setWithOpStack == 2,
                     "Cannot mix addOp() and set<Foo>() methods." );

        m_setWithOpStack = 2;

        m_ops.push_back( op );
        m_opsArray.push_back( op.getOpEncoding() );
    }
    else
    {
        std::size_t ret = m_opIndex;

        ABCA_ASSERT( m_setWithOpStack == 2,
                     "Cannot mix addOp() and set<Foo>() methods." );

        ABCA_ASSERT( op.getType() == m_ops[ret].getType(),
                     "Cannot update mismatched op-type in already-setted "
                     << "XformSample!" );

        m_ops[ret] = op;
        m_opIndex = ++m_opIndex % m_ops.size();
    }
}

//-*****************************************************************************
void XformSample::setRotation( const Abc::V3d &iAxis,
                               const double iAngleInDegrees )
{
    XformOp op( kRotateOperation, kRotateHint );

    for ( size_t i = 0 ; i < 3 ; ++i )
    {
        op.setChannelValue( i, iAxis[i] );
    }
    op.setChannelValue( 3, DegreesToRadians( iAngleInDegrees ) );

    if ( ! m_hasBeenRead )
    {
        ABCA_ASSERT( m_setWithOpStack == 0 || m_setWithOpStack == 2,
                     "Cannot mix addOp() and set<Foo>() methods." );

        m_setWithOpStack = 2;

        m_ops.push_back( op );
        m_opsArray.push_back( op.getOpEncoding() );
    }
    else
    {
        std::size_t ret = m_opIndex;

        ABCA_ASSERT( m_setWithOpStack == 2,
                     "Cannot mix addOp() and set<Foo>() methods." );

        ABCA_ASSERT( op.getType() == m_ops[ret].getType(),
                     "Cannot update mismatched op-type in already-setted "
                     << "XformSample!" );

        m_ops[ret] = op;
        m_opIndex = ++m_opIndex % m_ops.size();
    }
}

//-*****************************************************************************
void XformSample::setScale( const Abc::V3d &iScale )
{
    XformOp op( kScaleOperation, kScaleHint );

    for ( size_t i = 0 ; i < 3 ; ++i )
    {
        op.setChannelValue( i, iScale[i] );
    }

    if ( ! m_hasBeenRead )
    {
        ABCA_ASSERT( m_setWithOpStack == 0 || m_setWithOpStack == 2,
                     "Cannot mix addOp() and set<Foo>() methods." );

        m_setWithOpStack = 2;

        m_ops.push_back( op );
        m_opsArray.push_back( op.getOpEncoding() );
    }
    else
    {
        std::size_t ret = m_opIndex;

        ABCA_ASSERT( m_setWithOpStack == 2,
                     "Cannot mix addOp() and set<Foo>() methods." );

        ABCA_ASSERT( op.getType() == m_ops[ret].getType(),
                     "Cannot update mismatched op-type in already-setted "
                     << "XformSample!" );

        m_ops[ret] = op;
        m_opIndex = ++m_opIndex % m_ops.size();
    }
}

//-*****************************************************************************
void XformSample::setMatrix( const Abc::M44d &iMatrix )
{
    XformOp op( kMatrixOperation, kMatrixHint );

    for ( size_t i = 0 ; i < 16 ; ++i )
    {
        op.setChannelValue( i, *(iMatrix[i]) );
    }

    if ( ! m_hasBeenRead )
    {
        ABCA_ASSERT( m_setWithOpStack == 0 || m_setWithOpStack == 2,
                     "Cannot mix addOp() and set<Foo>() methods." );

        m_setWithOpStack = 2;

        m_ops.push_back( op );
        m_opsArray.push_back( op.getOpEncoding() );
    }
    else
    {
        std::size_t ret = m_opIndex;

        ABCA_ASSERT( m_setWithOpStack == 2,
                     "Cannot mix addOp() and set<Foo>() methods." );

        ABCA_ASSERT( op.getType() == m_ops[ret].getType(),
                     "Cannot update mismatched op-type in already-setted "
                     << "XformSample!" );

        m_ops[ret] = op;
        m_opIndex = ++m_opIndex % m_ops.size();
    }
}

//-*****************************************************************************
Abc::M44d XformSample::getMatrix() const
{
    Abc::M44d ret;

    for ( std::size_t i = 0 ; i < m_ops.size() ; ++i )
    {
        Abc::M44d m;
        m.makeIdentity();

        XformOp op = m_ops[i];

        XformOperationType otype = op.getType();

        if ( otype == kMatrixOperation )
        {
            for ( std::size_t j = 0 ; j < 4 ; ++j )
            {
                for ( std::size_t k = 0 ; k < 4 ; ++k )
                {
                    m.x[j][k] = op.getChannelValue( ( 4 * j ) + k );
                }
            }
        }
        else
        {
            Abc::V3d vec( op.getChannelValue( 0 ),
                          op.getChannelValue( 1 ),
                          op.getChannelValue( 2 ) );

            if ( otype == kScaleOperation )
            {
                m.setScale( vec );
            }
            else if ( otype == kTranslateOperation )
            {
                m.setTranslation( vec );
            }
            else // must be rotation
            {
                m.setAxisAngle( vec,
                                DegreesToRadians( op.getChannelValue( 3 ) ) );
            }
        }
        ret = m * ret;
    }

    return ret;
}

//-*****************************************************************************
Abc::V3d XformSample::getTranslation() const
{
    Abc::M44d m = this->getMatrix();
    return m.translation();
}

//-*****************************************************************************
Abc::V3d XformSample::getScale() const
{
    Abc::V3d scl;
    Imath::extractScaling( this->getMatrix(), scl );
    return scl;
}

//-*****************************************************************************
Abc::V3d XformSample::getAxis() const
{
    Imath::Quatd q = Imath::extractQuat( this->getMatrix() );

    return q.axis();
}

//-*****************************************************************************
double XformSample::getAngle() const
{
    Imath::Quatd q = Imath::extractQuat( this->getMatrix() );

    return RadiansToDegrees( q.angle() );
}

//-*****************************************************************************
void XformSample::setHasBeenRead()
{
    m_hasBeenRead = true;
}

//-*****************************************************************************
void XformSample::clear()
{
    m_hasBeenRead = false;
    m_ops.resize( 0 );
    m_setWithOpStack = 0;
}

//-*****************************************************************************
const boost::uuids::uuid &XformSample::getID() const
{
    return m_id;
}



} // End namespace AbcGeom
} // End namespace Alembic
