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
#include <FluidSimDemo/InitStateObject.h>
#include <FluidSimDemo/MeshDrw.h>

#include <boost/tokenizer.hpp>

namespace FluidSimDemo {

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
void printTransform( const M44d &whatThe )
{
    
    Imath::V3d s, h, r, t;
    Imath::Quatd  lq, uq, rq;
    try
    {
        Imath::extractSHRT( whatThe, s, h, r, t );

        std::cout << "\tScale: " << s << std::endl
                  << "\tRot: " << r << std::endl
                  << "\tTrans: " << t << std::endl;
    }
    catch ( Iex::BaseExc &e )
    {
        FSD_THROW( "Exception caught extracting SHRT from matrix: "
                   << e.what() );
    }

}

//-*****************************************************************************
static inline M44d ReadMatrixFromFrame( TransformReaderPtr iXform,
                                        float iFrame )
{
    if ( iXform->hasFrames() )
    {
        if ( iXform->read( iFrame ) == TransformReader::READ_ERROR )
        {
            // CJH: TEMP
            FSD_THROW( "Error reading frame: " << iFrame
                       << " from xform: " << iXform->getName() );
            //return iXform->getMatrix();
        }
    }
    else
    {
        if ( iXform->read( FLT_MAX ) == TransformReader::READ_ERROR )
        {
            FSD_THROW( "Error reading frame: " << iFrame
                       << " from xform: " << iXform->getName() );
        }
    }

    M44d ret = iXform->getMatrix();
#if 0
    std::cout << "TRANSFORM FROM " << iXform->getName() << " at frame: "
              << iFrame << std::endl;
    printTransform( ret );
#endif
    return ret;
}

//-*****************************************************************************
void TransformDrw::readInterpolatedFrame( float iFrame,
                                          const M44d &iParentXform )
{
    std::cout << "BEGIN UPDATE FOR: " << getName() << std::endl;
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
            float lowFrame = (*(--(m_sampleFrames.lower_bound( iFrame ))));
            float hiFrame = (*(m_sampleFrames.upper_bound( iFrame )));

#if 1
#if 1
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
                    double interp = ( iFrame - lowFrame ) / denom;
                    m_localToParent = InterpolateMatrices( lowMatrix,
                                                           hiMatrix,
                                                           interp );
                }
            }
#else
            m_localToParent = ReadMatrixFromFrame( m_takoTransformReader,
                                                   lowFrame );
#endif
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

#if 0
        //if ( m_baseNodePtr->getName() == "ColAGeom" )
        {
            std::cout << "\t"
                      << getName() << " Children bounds extended to: "
                      << m_childrenBounds.min << ", "
                      << m_childrenBounds.max << " from child: "
                      << (*iter)->getName() << std::endl;
        }
#endif
    }

#if 0
    if ( !m_childrenBounds.isEmpty() )
    {
        std::cout << "Transform: " << m_baseNodePtr->getName()
                  << " childrenBounds: " << m_childrenBounds.min
                  << " to " << m_childrenBounds.max
                  << std::endl;
    }
    else
    {
        std::cout << "Transform: " << m_baseNodePtr->getName()
                  << " childrenBounds EMPTY"
                  << std::endl;
    }
#endif

    if ( !m_childrenBounds.isEmpty() )
    {
        m_bounds = Imath::transform( m_childrenBounds, m_localToParent );
    }

    std::cout << "END UPDATE FOR: " << getName() << std::endl;
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
SharedDrawable CreateGeomObject( READER_PTR iTakoNode )
{
    return SharedDrawable();
}

template <>
SharedDrawable CreateGeomObject( PolyMeshReaderPtr iTakoNode )
{
    SharedDrawable ret( new PolyMeshDrw( iTakoNode ) );
    return ret;
}

template <>
SharedDrawable CreateGeomObject( SubDReaderPtr iTakoNode )
{
    SharedDrawable ret( new SubDDrw( iTakoNode ) );
    return ret;
}

//-*****************************************************************************
template <class READER_PTR>
SharedDrawable CreatePossibleSimObject( READER_PTR iTakoNode,
                                        InitStateObjects &iInitStates,
                                        CollisionObjects &iColliders )
{
    std::string PRIM_TYPE = "NONE";

#if 0
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
#endif
    
    // CJH HACK HACK HACK HACK
    if ( /*( foundHead || ( foundSphereCurve && NUM <= 8 ) )
           &&*/
         GetStringProp( iTakoNode, "ALEMBIC_FLUID_SIM_DEMO_TYPE",
                        PRIM_TYPE ) )
    {
        if ( PRIM_TYPE == "CollisionPlane" )
        {
            //CollisionObjectPtr sdrw(
            //    new CollisionObject( iTakoNode, kPlane ) );
            //iColliders.push_back( sdrw );
            //return sdrw;
            return SharedDrawable();
        }
        else if ( PRIM_TYPE == "InitPlane" )
        {
            InitStateObjectPtr sdrw(
                new InitStateObject( iTakoNode, kPlane ) );
            iInitStates.push_back( sdrw );
            return sdrw;
        }
        else if ( PRIM_TYPE == "SphereCollision" )
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
    return CreateGeomObject( iTakoNode );
    //return SharedDrawable();
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
        // return SharedDrawable();
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
        //return SharedDrawable();
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

    // If we're constant, load the matrix.
    if ( m_constant )
    {
        m_localToParent = ReadMatrixFromFrame( m_takoTransformReader,
                                               FLT_MAX );
    }   

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

    
}

} // End namespace FluidSimDemo
