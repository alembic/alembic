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

#include <Alembic/AbcGeom/IXform.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
void IXformSchema::init( const Abc::IArgument &iArg0,
                         const Abc::IArgument &iArg1 )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformTrait::init()" );

    // .ops, .static, .anim and .inherits do not need to exist

    AbcA::CompoundPropertyReaderPtr _this = this->getPtr();

    if (this->getPropertyHeader(".ops") != NULL)
    {
        Abc::IUInt32ArrayProperty ops( _this, ".ops" );
        Abc::UInt32ArraySamplePtr opSamp;
        ops.get(opSamp);
        if (opSamp)
        {
            size_t numOps = opSamp->size();
            m_ops.resize(numOps);
            for (size_t i = 0; i < numOps; ++i)
            {
                XformOp op;
                op.setEncodedValue( (*opSamp)[i] );
                m_ops[i] =  op;
            }
        }
    }

    if (this->getPropertyHeader(".static") != NULL)
    {
        Abc::IDoubleArrayProperty staticData( _this, ".static");
        staticData.get(m_static);
    }

    if (this->getPropertyHeader(".anim") != NULL)
        m_anim = Abc::IDoubleArrayProperty( _this, ".anim" );

    if (this->getPropertyHeader(".inherits") != NULL)
        m_inherits = Abc::IBoolProperty( _this, ".inherits" );

    if ( this->getPropertyHeader( ".childBnds" ) != NULL )
    {
        m_childBounds = Abc::IBox3dProperty( _this, ".childBnds", iArg0,
                                             iArg1 );
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
Abc::M44d IXformSchema::getMatrix( const Abc::ISampleSelector &iSS )
{
    Abc::M44d ret;
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformTrait::getMatrix()" );

    Abc::DoubleArraySamplePtr anim;
    if ( m_anim.valid() )
        m_anim.get( anim, iSS );

    size_t staticIndex = 0;
    size_t animIndex = 0;

    size_t numOps = m_ops.size();
    for (size_t i = 0; i < numOps; ++i)
    {
        Abc::M44d m;
        XformOperationType type = m_ops[i].getType();
        if (type == kMatrixOperation)
        {
            for (size_t j = 0; j < 4; ++j)
            {
                for (size_t k = 0; k < 4; ++k)
                {
                    if (m_ops[i].isIndexAnimated(j*4 + k))
                    {
                        m.x[j][k] = (*anim)[animIndex++];
                    }
                    else
                    {
                        m.x[j][k] = (*m_static)[staticIndex++];
                    }
                }
            }
        }
        else
        {
            double x, y, z;
            if (m_ops[i].isXAnimated())
            {
                x = (*anim)[animIndex++];
            }
            else
            {
                x = (*m_static)[staticIndex++];
            }

            if (m_ops[i].isYAnimated())
            {
                y = (*anim)[animIndex++];
            }
            else
            {
                y = (*m_static)[staticIndex++];
            }

            if (m_ops[i].isZAnimated())
            {
                z = (*anim)[animIndex++];
            }
            else
            {
                z = (*m_static)[staticIndex++];
            }

            if (type == kScaleOperation)
            {
                m.setScale( V3d(x,y,z) );
            }
            else if (type == kTranslateOperation)
            {
                m.setTranslation( V3d(x, y, z) );
            }
            else if (type == kRotateOperation)
            {
                double angle;
                if (m_ops[i].isAngleAnimated())
                {
                    angle = (*anim)[animIndex++];
                }
                else
                {
                    angle = (*m_static)[staticIndex++];
                }

                m.setAxisAngle( V3d(x,y,z), angle );
            }
        }

        ret = m * ret;
    }
    ALEMBIC_ABC_SAFE_CALL_END();

    return ret;
}

//-*****************************************************************************
bool IXformSchema::isOpStatic( size_t iIndex ) const
{
    if ( iIndex >= m_ops.size() )
        return true;

    const XformOp & op = m_ops[iIndex];
    for ( uint8_t i = 0; i < op.getNumIndices(); ++i )
    {
        if (op.isIndexAnimated(i))
            return false;
    }
    return true;
}

//-*****************************************************************************
void IXformSchema::get( XformSample & oSamp,
                        const Abc::ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformTrait::getSample()" );

    Abc::DoubleArraySamplePtr anim;
    if ( m_anim.valid() )
        m_anim.get( anim, iSS );

    size_t staticIndex = 0;
    size_t animIndex = 0;

    size_t numOps = m_ops.size();
    oSamp.clear();
    for (size_t i = 0; i < numOps; ++i)
    {
        XformOperationType type = m_ops[i].getType();
        if (type == kMatrixOperation)
        {
            Abc::M44d m;
            for (size_t j = 0; j < 4; ++j)
            {
                for (size_t k = 0; k < 4; ++k)
                {
                    if (m_ops[i].isIndexAnimated(j*4 + k))
                    {
                        m.x[j][k] = (*anim)[animIndex];
                        animIndex ++;
                    }
                    else
                    {
                        m.x[j][k] = (*m_static)[staticIndex];
                        staticIndex ++;
                    }
                }
            }
            XformDataPtr p(new MatrixData(m));
            oSamp.push( p );
        }
        else
        {
            double x, y, z;
            if (m_ops[i].isXAnimated())
            {
                x = (*anim)[animIndex];
                animIndex ++;
            }
            else
            {
                x = (*m_static)[staticIndex];
                staticIndex ++;
            }

            if (m_ops[i].isYAnimated())
            {
                y = (*anim)[animIndex];
                animIndex ++;
            }
            else
            {
                y = (*m_static)[staticIndex];
                staticIndex ++;
            }

            if (m_ops[i].isZAnimated())
            {
                z = (*anim)[animIndex];
                animIndex ++;
            }
            else
            {
                z = (*m_static)[staticIndex];
                staticIndex ++;
            }

            if (type == kScaleOperation)
            {
                XformDataPtr p( new ScaleData(V3d(x,y,z)) );
                oSamp.push( p );
            }
            else if (type == kTranslateOperation)
            {
                XformDataPtr p( new TranslateData(V3d(x,y,z)) );
                oSamp.push( p );
            }
            else if (type == kRotateOperation)
            {
                double angle;
                if (m_ops[i].isAngleAnimated())
                {
                    angle = (*anim)[animIndex];
                    animIndex ++;
                }
                else
                {
                    angle = (*m_static)[staticIndex];
                    staticIndex ++;
                }

                XformDataPtr p( new RotateData(V3d(x,y,z), angle) );
                oSamp.push( p );
            }
        }

    }
    ALEMBIC_ABC_SAFE_CALL_END();
}

} // End namespace AbcGeom
} // End namespace Alembic
