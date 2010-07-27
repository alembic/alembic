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

#include <FluidSimDemo/TransformDrw.h>
#include <FluidSimDemo/CollisionObject.h>
#include <FluidSimDemo/MeshDrw.h>

namespace FluidSimDemo {

//-*****************************************************************************
TransformDrw::TransformDrw( IParentPtr iXform,
                            CollisionObjects &oColliders )
  : Drawable(),
    m_parent( iXform ),
    m_object(
        boost::dynamic_pointer_cast<Abc::IObject, Abc::IParentObject>( iXform ) )
{
    // Basically reset everything.
    m_children.clear();
    m_localToWorld.makeIdentity();
    m_localToParent.makeIdentity();
    m_shapeBounds.makeEmpty();
    m_childrenBounds.makeEmpty();
    m_bounds.makeEmpty();

    // If the pointer is valid, make children.
    if ( m_parent )
    {
        if ( m_object )
        {
            m_xformTrait = Atg::IXformLocalTrait( *m_object );
            m_boundsTrait = Atg::IBoundsLocalTrait( *m_object );
        }
        
        size_t numChildren = m_parent->numChildren();
        for ( size_t cIdx = 0; cIdx < numChildren; ++cIdx )
        {
            Abc::ObjectInfo oinfo = m_parent->childInfo( cIdx );
            if ( !oinfo ) { continue; }

            if ( oinfo->protocol == "AlembicSimpleNull_v0001" )
            {
                IObjectPtr newObject(
                    new Abc::IObject( *m_parent,
                                      oinfo ) );
                
                Abc::ICharProperty shapeProp( *newObject,
                                              "ALEMBIC_FLUID_SIM_DEMO_TYPE" );
                if ( shapeProp )
                {
                    ShapeType stype = ( ShapeType )( shapeProp.get() );
                    CollisionObjectPtr cptr;
                    switch ( stype )
                    {
                    case kSphere:
                    case kCube:
                    case kCylinder:
                    case kCone:
                        cptr.reset( new CollisionObject( newObject,
                                                         oColliders,
                                                         stype ) );
                        oColliders.push_back( cptr );
                        m_children.push_back( cptr );
                        break; 
                    }
                }
                else
                {
                    DrawablePtr sdrw( new TransformDrw( newObject,
                                                        oColliders ) );
                    m_children.push_back( sdrw );
                }
            }
            else if ( oinfo->protocol == "AlembicSimplePolyMesh_v0001" ||
                      oinfo->protocol == "AlembicSimpleSubd_v0001" )
            {
                IObjectPtr newObject(
                    new Abc::IObject( *m_parent,
                                      oinfo ) );

                DrawablePtr sdrw( new MeshDrw( newObject,
                                               oColliders ) );
                m_children.push_back( sdrw );
            }
            else
            {
                IObjectPtr newObject(
                    new Abc::IObject( *m_parent,
                                      oinfo ) );
                
                DrawablePtr sdrw( new TransformDrw( newObject,
                                                    oColliders ) );
                m_children.push_back( sdrw );
            }
        }
    }
}

//-*****************************************************************************
static M44d InterpolateMatrices( const M44d &iLoVal,
                                 const M44d &iHiVal,
                                 double iAlpha )
{   
    // Safety checks and early exits
    if ( iAlpha >= 1.0 )
    {
        return iHiVal;
    }
    else if ( iAlpha <= 0.0 )
    {
        return iLoVal;
    }

    // Linearly interpolate the translation, scale and shear, but slerp
    // the rotation

    Imath::V3d ls, lh, lr, lt;
    Imath::Eulerd le;
    Imath::Quatd  lq;
    try
    {
        Imath::extractSHRT( iLoVal, ls, lh, lr, lt );
        le = Imath::Eulerd( lr );
        lq = le.toQuat();
    }
    catch ( Iex::BaseExc &e )
    {
        FSD_THROW( "Exception caught extracting SHRT from matrix: "
                   << e.what() );
    }

    Imath::V3d hs, hh, hr, ht;
    Imath::Eulerd he;
    Imath::Quatd hq;
    try
    {
        Imath::extractSHRT( iHiVal, hs, hh, hr, ht );
        he = Imath::Eulerd( hr );
        he.makeNear( le );
        hq = he.toQuat();
    }
    catch ( Iex::BaseExc &e )
    {
        FSD_THROW( "Exception caught extracting SHRT from matrix: "
                   << e.what() );
    }
    
    Imath::V3d rs = Imath::lerp( ls, hs, iAlpha );
    Imath::V3d rt = Imath::lerp( lt, ht, iAlpha );
    Imath::Quatd rq = Imath::slerp( lq, hq, iAlpha );
    
    Imath::M44d mS, mH, mR, mT;
    mS.setScale( rs );
    mH.setShear( lh );
    
    Imath::Eulerd eul;
    eul.extract( rq );
    mR.setEulerAngles( eul );

    mT.setTranslation( rt );

    return ( mS * mH * mR * mT );
}

//-*****************************************************************************
void TransformDrw::readInterpolatedFrame( float iFrame,
                                          const M44d &iParentXform )
{
    //std::cout << "BEGIN UPDATE FOR: " << getName() << std::endl;
    // Reset...

    // Get interpolated transform
    Abc::seconds_t timeInSeconds = iFrame / 24.0f;
    if ( m_xformTrait )
    {
        if ( m_xformTrait.isAnimated() )
        {
            m_localToParent = m_xformTrait.getAnimLowerBound( timeInSeconds );
        }
        else
        {
            m_localToParent = m_xformTrait.get();
        }
    }
    else
    {
        m_localToParent.makeIdentity();
    }

    // Our world local to world is localToParent * parentToWorld
    m_localToWorld = m_localToParent * iParentXform;

    // Read shape.
    readShape( iFrame, m_localToWorld );
    
    // Get children
    m_childrenBounds.makeEmpty();
    for ( Children::iterator iter = m_children.begin();
          iter != m_children.end(); ++iter )
    {
        (*iter)->readInterpolatedFrame( iFrame, m_localToWorld );
        
        m_childrenBounds.extendBy( (*iter)->getBounds() );

    }

    Abc::Box3d comboBnds = m_childrenBounds;
    comboBnds.extendBy( m_shapeBounds );

    if ( !comboBnds.isEmpty() )
    {
        m_bounds = Imath::transform( comboBnds, m_localToParent );
    }
}

//-*****************************************************************************
Box3d TransformDrw::getBounds() const
{
    return m_bounds;
}

//-*****************************************************************************
void TransformDrw::draw( const DrawContext & iCtx ) const
{
#if 1
    // Get the matrix
    GLdouble currentMatrix[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, currentMatrix );
    
    // Basically, we want to load our matrix into the thingy.
    // We don't use the OpenGL transform stack because we have deep
    // deep hierarchy that exhausts the stack depth quickly.
    M44d concatXform = m_localToWorld * iCtx.cameraTransform();
    // M44d concatXform = iCtx.cameraTransform() * m_localToWorld();

    // Load this xform.
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd( ( const GLdouble * )&( concatXform[0][0] ) );

    // Draw shape.
    drawShape( iCtx );

    // Draw children.
    DrawContext childCtx( iCtx );
    childCtx.setTransform( concatXform );
    for ( Children::const_iterator citer = m_children.begin();
          citer != m_children.end(); ++citer )
    {
        (*citer)->draw( childCtx );
    }
    
    // Reset matrix?
    // Probably not necessary
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd( currentMatrix );
#else
    glPushMatrix();

    glMatrixMode( GL_MODELVIEW );
    glMultMatrixd( ( const GLdouble * )&( m_localToParent[0][0] ) );

    // Draw children.
    DrawContext childCtx( iCtx );
    childCtx.setTransform( m_localToParent );
    for ( Children::const_iterator citer = m_children.begin();
          citer != m_children.end(); ++citer )
    {
        (*citer)->draw( childCtx );
    }

    glPopMatrix();

#endif
}

} // End namespace FluidSimDemo
