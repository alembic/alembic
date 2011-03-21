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

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
XformSample::XformSample()
  : m_setWithOpStack( 0 )
  , m_isToWorld( false )
  , m_hasBeenRead( false )
  , m_opIndex( 0 )
{
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

        m_ops[ret] = iOp;
        m_opIndex = ++m_opIndex % m_ops.size();

        return ret;
    }
}

//-*****************************************************************************
std::size_t XformSample::addOp( XformOp iOp, const Abc::V3d &iAxis,
                                const double iAngle )
{
    {
        for ( size_t i = 0 ; i < 3 ; ++i )
        {
            iOp.setChannelValue( i, iAxis[i] );
        }
        iOp.setChannelValue( 3, iAngle );

        if ( ! m_hasBeenRead )
        {
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

            m_ops[ret] = iOp;
            m_opIndex = ++m_opIndex % m_ops.size();

            return ret;
        }
    }
}

} // End namespace AbcGeom
} // End namespace Alembic
