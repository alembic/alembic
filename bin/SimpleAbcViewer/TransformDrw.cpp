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

#include <SimpleAbcViewer/TransformDrw.h>
#include <SimpleAbcViewer/CollisionObject.h>
#include <SimpleAbcViewer/InitStateObject.h>

#include <boost/tokenizer.hpp>

namespace SimpleAbcViewer {

//-*****************************************************************************
// Constructor is below

//-*****************************************************************************
TransformDrw::~TransformDrw()
{
    // Nothing. Vector elements delete themselves.
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
    M44d result;
    result.makeIdentity();

    Imath::V3d s, h, r, t;
    Imath::Quatd  lq, uq, rq;
    try
    {
        Imath::extractSHRT( iLoVal, s, h, r, t );
        Imath::Eulerd eul( r );
        lq = eul.toQuat();
    }
    catch ( Iex::BaseExc &e )
    {
        ABC_THROW( "Exception caught extracting SHRT from matrix: "
                   << e.what() );
    }

    Imath::V3d rs, rt;
    rs = iAlpha * s;
    // rh = alpha * h;    // ignore shear
    rt = iAlpha * t;

    try
    {
        Imath::extractSHRT( iHiVal, s, h, r, t );
        Imath::Eulerd eul( r );
        uq = eul.toQuat();
    }
    catch ( Iex::BaseExc &e )
    {
        ABC_THROW( "Exception caught extracting SHRT from matrix: "
                   << e.what() );
    }

    rs += ( 1.0 - iAlpha ) * s;
    // rh += (1.0-iAlpha) * h;     // ignore shear
    rt += ( 1.0 - iAlpha ) * t;

    // rq = Imath::slerpShortestArc( lq, uq, iAlpha );
    rq = Imath::slerp( lq, uq, iAlpha );
    Imath::Eulerd eul;
    eul.extract( rq );
    result.setEulerAngles( eul );

    // set translation
    result[3][0] = rt[0];
    result[3][1] = rt[1];
    result[3][2] = rt[2];
    result[3][3] = 1;

    // set scale
    Imath::M44d scaleMat;
    scaleMat.setScale( rs );
    result = scaleMat * result;

    return result;
}

//-*****************************************************************************
static inline M44d ReadMatrixFromFrame( TransformReaderPtr iXform,
                                        float iFrame )
{
    if ( iXform->read( iFrame ) == TransformReader::READ_ERROR )
    {
        ABC_THROW( "Error reading frame: " << iFrame
                   << " from xform: " << iXform->getName() );
    }

    return iXform->getMatrix();
}

//-*****************************************************************************
void TransformDrw::readInterpolatedFrame( float iFrame,
                                          const M44d &iParentXform )
{
    // Reset...
    m_bounds.makeEmpty();
    m_childrenBounds.makeEmpty();

    // Get interpolated transform
    if ( !m_constant )
    {
        if ( iFrame <= m_minFrame )
        {
            m_localToParent = ReadMatrixFromFrame( m_takoTransformReader,
                                                   m_minFrame );
        }
        else if ( iFrame >= m_maxFrame )
        {
            m_localToParent = ReadMatrixFromFrame( m_takoTransformReader,
                                                   m_maxFrame );
        }
        else
        {
            float lowFrame = (*(m_sampleFrames.lower_bound( iFrame )));
            float hiFrame = (*(m_sampleFrames.upper_bound( iFrame )));

#if 0
            if ( fabs( lowFrame - iFrame ) < 1.0e-4f )
            {
                m_localToParent = ReadMatrixFromFrame( m_takoTransformReader,
                                                       lowFrame );
            }
            else if ( fabs( hiFrame - iFrame ) < 1.0e-4f )
            {
                m_localToParent = ReadMatrixFromFrame( m_takoTransformReader,
                                                       hiFrame );
            }
            else
#endif
            {
                M44d lowMatrix = ReadMatrixFromFrame( m_takoTransformReader,
                                                      lowFrame );
                
                float denom = hiFrame - lowFrame;
                if ( denom < 1.0e-4f )
                {
                    m_localToParent = lowMatrix;
                }
                else
                {
                    M44d hiMatrix = ReadMatrixFromFrame( m_takoTransformReader,
                                                         hiFrame );
                    double alpha = ( iFrame - lowFrame ) / denom;
                    m_localToParent = InterpolateMatrices( lowMatrix,
                                                           hiMatrix,
                                                           alpha );
                }
            }
        }
    }

    // Our world local to world is localToParent * parentToWorld
    m_localToWorld = m_localToParent * iParentXform;
    
    // Get children
    for ( Children::iterator iter = m_children.begin();
          iter != m_children.end(); ++iter )
    {
        (*iter)->readInterpolatedFrame( iFrame, m_localToWorld );
        m_childrenBounds.extendBy( (*iter)->getBounds() );
    }

    if ( !m_childrenBounds.isEmpty() )
    {
        m_bounds = Imath::transform( m_childrenBounds, m_localToWorld );
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
    // Get the matrix
    GLdouble currentMatrix[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, currentMatrix );
    
    // Basically, we want to load our matrix into the thingy.
    // We don't use the OpenGL transform stack because we have deep
    // deep hierarchy that exhausts the stack depth quickly.
    M44d concatXform = m_localToWorld * iCtx.cameraTransform();

    // Load this xform.
    glMatrixMode( GL_MODELVIEW );
    glLoadMatrixd( ( const GLdouble * )&( concatXform[0][0] ) );

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
}

//-*****************************************************************************
//-*****************************************************************************
// CREATION OF CHILDREN
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// Create a visitor which will, for each of the different child types in Tako
// create the appropriate drawable, and store that in the parent.
namespace {

//-*****************************************************************************
template <class READER_PTR>
bool GetStringProp( READER_PTR iReader,
                    const std::string & iPropName,
                    std::string &oPropValue )
{
    PropertyPair theValuePair;
    if ( iReader->readProperty( iPropName.c_str(),
                                FLT_MAX,
                                theValuePair ) != PROPERTY_NONE )
    {
        try
        {
            oPropValue = boost::get<std::string>( theValuePair.first );
        }
        catch ( boost::bad_get &e )
        {
            return false;
        }
        return true;
    }
    return false;
}

//-*****************************************************************************
template <class READER_PTR>
SharedDrawable CreatePossibleSimObject( READER_PTR iTakoNode,
                                        InitStateObjects &iInitStates,
                                        CollisionObjects &iColliders )
{
    std::string PRIM_TYPE = "NONE";

    std::string NodeName = iTakoNode->getName();

    bool foundHead = false;
    bool foundSphereCurve = false;
    
    typedef boost::tokenizer<boost::char_separator<char> > TKN;
    boost::char_separator<char> sep( "_" );
    TKN tokens( NodeName, sep );
    std::vector<std::string> StrToks;
    for ( TKN::iterator iter = tokens.begin();
          iter != tokens.end(); ++iter )
    {
        if ( (*iter).find( "head" ) != std::string::npos )
        {
            foundHead = true;
        }

        if ( (*iter) == "sphereCurve" )
        {
            foundSphereCurve = true;
        }
            
        StrToks.push_back( (*iter) );
    }
    int NUM = 100;
    if ( !StrToks.empty() )
    {
        NUM = atoi( StrToks.back().c_str() );
    }
    
    // CJH HACK HACK HACK HACK
    if ( ( foundHead || ( foundSphereCurve && NUM <= 8 ) )
         &&
         GetStringProp( iTakoNode, "ALEMBIC_FLUID_SIM_DEMO_TYPE",
                        PRIM_TYPE ) )
    {
        if ( PRIM_TYPE == "SphereCollision" )
        {
            CollisionObjectPtr sdrw(
                new CollisionObject( iTakoNode, kSphere ) );
            iColliders.push_back( sdrw );
            return sdrw;
        }
        else if ( PRIM_TYPE == "SphereInit" )
        {
            InitStateObjectPtr sdrw(
                new InitStateObject( iTakoNode, kSphere ) );
            iInitStates.push_back( sdrw );
            return sdrw;
        }
        else if ( PRIM_TYPE == "CubeCollision" )
        {
            CollisionObjectPtr sdrw(
                new CollisionObject( iTakoNode, kCube ) );
            iColliders.push_back( sdrw );
            return sdrw;
        }
        else if ( PRIM_TYPE == "CubeInit" )
        {
            InitStateObjectPtr sdrw(
                new InitStateObject( iTakoNode, kCube ) );
            iInitStates.push_back( sdrw );
            return sdrw;
        }
        else if ( PRIM_TYPE == "CylinderCollision" )
        {
            CollisionObjectPtr sdrw(
                new CollisionObject( iTakoNode, kCylinder ) );
            iColliders.push_back( sdrw );
            return sdrw;
        }
        else if ( PRIM_TYPE == "CylinderInit" )
        {
            InitStateObjectPtr sdrw(
                new InitStateObject( iTakoNode, kCylinder ) );
            iInitStates.push_back( sdrw );
            return sdrw;
        }
        else if ( PRIM_TYPE == "ConeCollision" )
        {
            CollisionObjectPtr sdrw(
                new CollisionObject( iTakoNode, kCone ) );
            iColliders.push_back( sdrw );
            return sdrw;
        }
        else if ( PRIM_TYPE == "ConeInit" )
        {
            InitStateObjectPtr sdrw(
                new InitStateObject( iTakoNode, kCone ) );
            iInitStates.push_back( sdrw );
            return sdrw;
        }
    }

    // If we get here, just make a mesh.
    // For now, make a nothing!
    return SharedDrawable();
}

//-*****************************************************************************
class ChildrenVisitor : public boost::static_visitor<SharedDrawable>
{
public:
    ChildrenVisitor( InitStateObjects &oInitStates,
                     CollisionObjects &oColliders )
      : m_initStates( oInitStates ),
        m_colliders( oColliders ) {}
    
    //-*************************************************************************
    // VISITATION POINTS
    //-*************************************************************************
    SharedDrawable operator()( TransformReaderPtr & i_child ) const
    {
        SharedDrawable sdrw( new TransformDrw( i_child,
                                               m_initStates,
                                               m_colliders ) );
        return sdrw;
    }

    SharedDrawable operator()( SubDReaderPtr & i_child ) const
    {
        return CreatePossibleSimObject( i_child,
                                        m_initStates,
                                        m_colliders );
    }

    SharedDrawable operator()( PolyMeshReaderPtr & i_child ) const
    {
        return CreatePossibleSimObject( i_child,
                                        m_initStates,
                                        m_colliders );
    }
    
    SharedDrawable operator()( NurbsSurfaceReaderPtr & i_child ) const
    {
        return CreatePossibleSimObject( i_child,
                                        m_initStates,
                                        m_colliders );
    }

    SharedDrawable operator()( PointPrimitiveReaderPtr & i_child ) const
    { return SharedDrawable(); }
    SharedDrawable operator()( NurbsCurveReaderPtr & i_child ) const
    { return SharedDrawable(); }
    SharedDrawable operator()( CameraReaderPtr & i_child ) const
    { return SharedDrawable(); }
    SharedDrawable operator()( GenericNodeReaderPtr & i_child ) const
    { return SharedDrawable(); }

protected:
    InitStateObjects & m_initStates;
    CollisionObjects & m_colliders;
};

} // End anonymous namespace; 

//-*****************************************************************************
TransformDrw::TransformDrw( TransformReaderPtr iXform,
                            InitStateObjects &oInitStates,
                            CollisionObjects &oColliders )
  : BaseTakoDrw( iXform ),
    m_takoTransformReader( iXform )
{
    // Basically reset everything.
    m_children.clear();
    m_localToWorld.makeIdentity();
    m_localToParent.makeIdentity();
    m_childrenBounds.makeEmpty();
    m_bounds.makeEmpty();

    // If the pointer is valid, make children.
    if ( m_takoTransformReader )
    {
        ChildrenVisitor cvisit( oInitStates, oColliders );
        size_t numChildren = m_takoTransformReader->numChildren();
        for ( size_t cIdx = 0; cIdx < numChildren; ++cIdx )
        {
            ChildNodePtr child = m_takoTransformReader->getChild( cIdx );
            SharedDrawable sdrw = boost::apply_visitor( cvisit, child );
            if ( sdrw )
            {
                m_children.push_back( sdrw );
            }
        }
    }

    // If we're constant, load the matrix.
    if ( m_constant )
    {
        if ( m_takoTransformReader->read( FLT_MAX ) ==
             TransformReader::READ_ERROR )
        {
            ABC_THROW( "Couldn't read static transform: "
                       << m_takoTransformReader->getName() );
        }
    }   
}

} // End namespace SimpleAbcViewer
