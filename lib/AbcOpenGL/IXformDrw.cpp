//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#include "IXformDrw.h"

namespace AbcOpenGL {
namespace ABCOPENGL_VERSION_NS {

//-*****************************************************************************
IXformDrw::IXformDrw( IXform &iXform )
  : IObjectDrw( iXform, false )
  , m_xform( iXform )
{
    if ( !m_xform.valid() || m_xform.getSchema().isConstantIdentity() )
    {
        return;
    }

    m_localToParent.makeIdentity();
    m_staticMatrix.makeIdentity();

    if ( m_xform.getSchema().isConstant() )
    {
        m_staticMatrix = m_xform.getSchema().getValue().getMatrix();
    }


    // The object has already set up the min time and max time of
    // all the children.
    // if we have a non-constant time sampling, we should get times
    // out of it.
    TimeSamplingPtr iTsmp = m_xform.getSchema().getTimeSampling();
    if ( !m_xform.getSchema().isConstant() )
    {
        size_t numSamps = m_xform.getSchema().getNumSamples();
        if ( numSamps > 0 )
        {
            chrono_t minTime = iTsmp->getSampleTime( 0 );
            m_minTime = std::min( m_minTime, minTime );
            chrono_t maxTime = iTsmp->getSampleTime( numSamps-1 );
            m_maxTime = std::max( m_maxTime, maxTime );
        }
    }
}

//-*****************************************************************************
IXformDrw::~IXformDrw()
{
    // Nothing
}

//-*****************************************************************************
bool IXformDrw::valid()
{
    return IObjectDrw::valid() && m_xform.valid();
}

//-*****************************************************************************
void IXformDrw::setTime( chrono_t iSeconds )
{
    // Let the object set the time of all the children
    IObjectDrw::setTime( iSeconds );
    if ( !valid() )
    {
        m_localToParent.makeIdentity();
        return;
    }

    ISampleSelector ss( iSeconds, ISampleSelector::kNearIndex );

    m_inherits = m_xform.getSchema().getInheritsXforms( ss );

    if ( m_xform.getSchema().isConstant() )
    {
        m_localToParent = m_staticMatrix;
    }
    else
    {
        m_localToParent = m_xform.getSchema().getValue( ss ).getMatrix();
    }

    // Okay, now we need to recalculate the bounds.
    m_bounds.makeEmpty();
    m_nonInheritedBounds.makeEmpty();
    for ( DrawablePtrVec::iterator iter = m_children.begin();
          iter != m_children.end(); ++iter )
    {
        DrawablePtr dptr = (*iter);
        if ( dptr )
        {
            Box3d bnds = dptr->getBounds();
            if ( !bnds.isEmpty() )
            {
                bnds = Imath::transform( bnds, m_localToParent );
                if ( !m_inherits )
                {
                    m_nonInheritedBounds.extendBy( bnds );
                }
                else
                {
                    m_bounds.extendBy( bnds );
                }
            }

            Box3d ibnds = dptr->getNonInheritedBounds();
            if ( !ibnds.isEmpty() )
            {
                m_nonInheritedBounds.extendBy( ibnds );
            }
        }
    }
}

//-*****************************************************************************
void IXformDrw::draw( const DrawContext & iCtx )
{
    if ( !valid() ) { return; }

    M44d idenMatrix; // Defaults to identity.
    idenMatrix.makeIdentity();
    if ( m_localToParent.equalWithAbsError( idenMatrix, 1.0e-9 ) && m_inherits )
    {
        // Don't need to change anything.
        IObjectDrw::draw( iCtx );
        return;
    }

    // Get the current matrix.
    GLdouble currentMatrix[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, currentMatrix );

    // Basically, we want to load our matrix into the thingy.
    // We don't use the OpenGL transform stack because we have
    // deep deep hierarchy that exhausts the max stack depth quickly.
    glMatrixMode( GL_MODELVIEW );

    if ( m_inherits )
    {
        glMultMatrixd( ( const GLdouble * )&m_localToParent[0][0] );
    }
    else
    {
        M44d cameraLocal = iCtx.getWorldToCamera() * m_localToParent;
        glLoadMatrixd( ( const GLdouble * )&cameraLocal[0][0] );
    }

    // Now draw.
    IObjectDrw::draw( iCtx );

    // And back out, restore the matrix.
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd( currentMatrix );
}

} // End namespace ABCOPENGL_VERSION_NS
} // End namespace AbcOpenGL
