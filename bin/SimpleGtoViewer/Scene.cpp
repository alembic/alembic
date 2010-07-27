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

#include "Scene.h"
#include "DisplayList.h"
#include "Foundation.h"

//-*****************************************************************************
using namespace GtoContainer;

//-*****************************************************************************
namespace SimpleGtoViewer {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
class MyReader : public GtoContainer::Reader
{
public:
    MyReader() : GtoContainer::Reader() {}

    virtual const GtoContainer::MetaProperty *
    findMetaProperty( GtoContainer::Layout lyt,
                      size_t width,
                      const std::string &nt ) const;
};

//-*****************************************************************************
const GtoContainer::MetaProperty *
MyReader::findMetaProperty( GtoContainer::Layout lyt,
                            size_t width,
                            const std::string &nt ) const
{
    //std::cout << "\nFind Meta Property!" << std::endl
    //          << "Layout[width]: " << layoutName( lyt, width ) << std::endl
    //          << "Interp: " << nt << std::endl;

    const GtoContainer::MetaProperty *mp =
        GtoContainer::Reader::findMetaProperty( lyt, width, nt );

    return mp;
}

//-*****************************************************************************
static bool isBadMatrix( const M44d &m )
{
    V3d newX, newY, newZ;
    m.multDirMatrix( V3d( 1, 0, 0 ), newX );
    m.multDirMatrix( V3d( 0, 1, 0 ), newY );
    m.multDirMatrix( V3d( 0, 0, 1 ), newZ );
    V3d calcZ = newX.cross( newY );
    if ( calcZ.dot( newZ ) < 0.0 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-*****************************************************************************
void setMaterials( float o, bool negMatrix = false )
{
    if ( negMatrix )
    {
        GLfloat mat_front_diffuse[] = { 0.1 * o, 0.1 * o, 0.9 * o, o };
        GLfloat mat_back_diffuse[] = { 0.9 * o, 0.1 * o, 0.9 * o, o };

        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = { 100.0 };
        GLfloat light_position[] = { 20.0, 20.0, 20.0, 0.0 };

        glClearColor( 0.0, 0.0, 0.0, 0.0 );
        glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_front_diffuse );
        glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
        glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );

        glMaterialfv( GL_BACK, GL_DIFFUSE, mat_back_diffuse );
        glMaterialfv( GL_BACK, GL_SPECULAR, mat_specular );
        glMaterialfv( GL_BACK, GL_SHININESS, mat_shininess );    
    }
    else
    {

        GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat mat_shininess[] = { 100.0 };
        GLfloat light_position[] = { 20.0, 20.0, 20.0, 0.0 };
        GLfloat mat_front_emission[] = {0.0, 0.0, 0.0, 0.0 };
        GLfloat mat_back_emission[] = {o, 0.0, 0.0, o };

        glClearColor( 0.0, 0.0, 0.0, 0.0 );
        glMaterialfv( GL_FRONT, GL_EMISSION, mat_front_emission );
        glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
        glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );

        glMaterialfv( GL_BACK, GL_EMISSION, mat_back_emission );
        glMaterialfv( GL_BACK, GL_SPECULAR, mat_specular );
        glMaterialfv( GL_BACK, GL_SHININESS, mat_shininess );    

        glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);

    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// Positions
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
class Positions
{
public:
    Positions( Property *p )
    {
        Float3Property *fp =
            dynamic_cast<Float3Property*>( p );
        if ( fp )
        {
            m_data.f = fp;
            m_float = true;
        }
        else
        {
            m_data.d = dynamic_cast<Double3Property*>( p );
            m_float = false;
        }
    }
    
    V3f f( size_t i ) const
    {
        return m_float ? toV3f( (*(m_data.f))[i] ) :
            V3f( toV3d( (*(m_data.d))[i] ) );
    }
    
    V3d d( size_t i ) const
    {
        return m_float ? V3d( toV3f( (*(m_data.f))[i] ) ):
            toV3d( (*(m_data.d))[i] );
    }

    void set( const V3f &f, size_t i )
    {
        if ( m_float )
        {
            (*(m_data.f))[i] = fromV3f( f );
        }
        else
        {
            (*(m_data.d))[i] = fromV3d( V3d( f ) );
        }
    }
    
    void set( const V3d &d, size_t i )
    {
        if ( m_float )
        {
            (*(m_data.f))[i] = fromV3f( V3f( d ) );
        }
        else
        {
            (*(m_data.d))[i] = fromV3d( d );
        }
    }
    
    size_t size() const
    { return m_float ? ( m_data.f ? m_data.f->size() : 0 )
            : ( m_data.d ? m_data.d->size() : 0 ); }

protected:
    union
    {
        Float3Property *f;
        Double3Property *d;
    } m_data;
    bool m_float;
};

//-*****************************************************************************
class ConstPositions
{
public:
    ConstPositions( const Property *p )
    {
        const Float3Property *fp =
            dynamic_cast<const Float3Property*>( p );
        if ( fp )
        {
            m_data.f = fp;
            m_float = true;
        }
        else
        {
            m_data.d = dynamic_cast<const Double3Property*>( p );
            m_float = false;
        }
    }
    
    V3f f( size_t i ) const
    {
        return m_float ? toV3f( (*(m_data.f))[i] ) :
            V3f( toV3d( (*(m_data.d))[i] ) );
    }
    
    V3d d( size_t i ) const
    {
        return m_float ? V3d( toV3f( (*(m_data.f))[i] ) ):
            toV3d( (*(m_data.d))[i] );
    }

    size_t size() const
    { return m_float ? ( m_data.f ? m_data.f->size() : 0 )
            : ( m_data.d ? m_data.d->size() : 0 ); }

protected:
    union
    {
        const Float3Property *f;
        const Double3Property *d;
    } m_data;
    bool m_float;
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// SCENE CLASS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
static M44d getGlobalMatrix( const PropertyContainer &pc )
{
    const Float16Property *matrix =
        pc.property<Float16Property>( "object",
                                      "globalMatrix" );
    const Double16Property *matrixd =
        pc.property<Double16Property>( "object",
                                       "globalMatrix" );
        
    M44d m;
    m.makeIdentity();
    if ( matrixd != NULL &&
         matrixd->size() > 0 )
    {
        m = toM44d( (*matrixd)[0] );
    }
    else if ( matrix != NULL &&
              matrix->size() > 0 )
    {
        const M44f &tmp = toM44f( (*matrix)[0] );
        for ( int row = 0; row < 4; ++row )
        {
            for ( int col = 0; col < 4; ++col )
            {
                m[row][col] = ( double )tmp[row][col];
            }
        }
    }
    
    m.transpose();

    return m;
}

//-*****************************************************************************
Scene::Scene( const std::string &gtoFileName )
{
    // Make a reader.
    MyReader reader;

    // Read. Let it throw
    reader.read( gtoFileName, m_objectVector );

    // Figure out the bounding box of the geometry
    m_bounds.makeEmpty();
    ObjectVector::iterator iter = m_objectVector.begin();
    for ( ; iter != m_objectVector.end(); ++iter )
    {
        M44d m = getGlobalMatrix( (*(*iter)) );

        const Property *posProp = (*iter)->find( "points",
                                           "position" );
        ConstPositions cpos( posProp );

        size_t np = cpos.size();
        
        for ( int i = 0; i < np; ++i )
        {
            V3d p = cpos.d( i );
            V3d xformed;
            m.multVecMatrix( p, xformed );
            m_bounds.extendBy( V3f( xformed ) );
        }
    }

    // Bounds have been formed!
    std::cout << "Bounds: " << m_bounds.min << " to "
              << m_bounds.max << std::endl;
}

//-*****************************************************************************
Scene::~Scene()
{
    m_objectVector.deleteContents();
}

//-*****************************************************************************
void Scene::draw( SceneState &s_state ) 
{
    glDrawBuffer( GL_BACK );
    s_state.cam.apply();

    static Protocol PolygonProtocol( GTO_PROTOCOL_POLYGON,
                                     2 );
    static Protocol CatmullClarkProtocol( GTO_PROTOCOL_CATMULL_CLARK,
                                          2 );
    
    static Protocol AbcPolyMeshProtocol( "alembic_polymesh", 1 );
    
    if ( !m_displayList.valid() || s_state.rebuild )
    {
        s_state.rebuild = false;
        m_displayList.newList( GL_COMPILE );
        glEnable( GL_AUTO_NORMAL );
        glEnable( GL_NORMALIZE );

        ObjectVector::const_iterator iter = m_objectVector.begin();
        for ( ; iter != m_objectVector.end(); ++iter )
        {
            const PropertyContainer *pc = (*iter);
            
            glPushMatrix();
            
            M44d m = getGlobalMatrix( *pc );

            M44d mForNorms = m;
            mForNorms.invert();
            mForNorms.transpose();
            
            if ( isBadMatrix( m ) )
            {
                setMaterials( s_state.opacity, true );
            }
            else
            {
                setMaterials( s_state.opacity, false );
            }

            //-*****************************************************************
            // DRAW POLY MESHES
            //-*****************************************************************
            if ( 0 )
            {
                const Property *posProp = pc->find( "points", "position" );
                
                if ( posProp == NULL )
                {
                    continue;
                }

                ConstPositions cpos( posProp );

                int numPositions = cpos.size();
                glDisable( GL_LIGHTING );
                glPointSize( 3 );
                glColor3f( 1.0f, 0.75f, 0.5f );
                glBegin( GL_POINTS );
                for ( int i = 0; i < numPositions; ++i )
                {
                    V3d p = cpos.d( i );
                    V3d px;
                    m.multVecMatrix( p, px );
                    
                    // const float3 &f3 = (*positions)[i];
                    // glVertex3d( f3[0], f3[1], f3[2] );
                    glVertex3d( px.x, px.y, px.z );
                    //std::cout <<
                    //    ( boost::format( "p[%d]: (%f,%f,%f)" )
                    //      % i
                    //      % ( float )( (*positionsd)[i][0])
                    //      % ( float )( (*positionsd)[i][1])
                    //      % ( float )((*positionsd)[i][2]) ) << std::endl;
                }
                glEnd();
            }

            if ( pc->protocol() == PolygonProtocol ||
                 pc->protocol() == CatmullClarkProtocol ||
                 pc->protocol() == AbcPolyMeshProtocol )
            {
                const ByteProperty *types =
                    pc->property<ByteProperty>( "elements", "type" );
                const UshortProperty *sizes =
                    pc->property<UshortProperty>( "elements", "size" );
                const IntProperty *indices =
                    pc->property<IntProperty>( "indices", "vertex" );
                const Property *posProp = pc->find( "points", "position" );

                if ( types == NULL ||
                     sizes == NULL ||
                     indices == NULL ||
                     posProp == NULL )
                {
                    ABC_THROW( "Couldn't get necessary properties "
                                "to draw object: " << pc->name() );
                }

                ConstPositions cpos( posProp );

                const Float3Property *normals =
                    pc->property<Float3Property>( "normals", "normal" );
                const IntProperty *nindices =
                    pc->property<IntProperty>( "indices", "normal" );
                bool calcNormals = ( normals == NULL ) || ( nindices == NULL );

                const Float2Property *uvs =
                    pc->property<Float2Property>( "mappings", "uv" );
                const IntProperty *uvindices =
                    pc->property<IntProperty>( "indices", "uv" );
                bool doUVs = ( uvs != NULL ) && ( uvindices != NULL );

                glEnable( GL_LIGHTING );

                int numElements = types->size();
                assert( numElements == sizes->size() );

                //std::cout << "Drawing object \"" << pc->name()
                //          << "\" with protocol \"" << pc->protocol()
                //          << "\"" << std::endl;

                int firstVertexIndex = 0;
                
                for ( int element = 0; element < numElements; ++element )
                {
                    int typ = (*types)[element];
                    int psize = (*sizes)[element];
                    int vi = firstVertexIndex;

                    //std::cout <<
                    //    ( boost::format( "element[%d]: typ:%d, psize:%d, vi:%d" )
                    //      % element
                    //      % typ
                    //      % psize
                    //      % vi ) << std::endl;

                    if ( typ == 0 ||
                         typ == 1 ||
                         typ == 2 )
                    {
                        glBegin( GL_POLYGON );

                        // Compute normal.
                        if ( calcNormals )
                        {   
                            V3f a,b,c;
                            
                            int vertIndex = (*indices)[vi];
                            m.multVecMatrix( cpos.f( vertIndex ), a );
                            
                            vertIndex = (*indices)[vi+1];
                            m.multVecMatrix( cpos.f( vertIndex ), b );
                            
                            vertIndex = (*indices)[vi + psize - 1];
                            m.multVecMatrix( cpos.f( vertIndex ), c );
                            
                            V3f dPdu = b - a;
                            V3f dPdv = c - a;
                            
                            // This represents a right-handed surface
                            V3f n = dPdu.cross( dPdv );
                            n.normalize();
                            
                            glNormal3f( n.x, n.y, n.z );
                        }

                        for ( int q = 0; q < psize; ++q )
                        {
                            if ( !calcNormals )
                            {
                                int normIndex = (*nindices)[vi + q];
                                V3d norm;
                                mForNorms.multDirMatrix(
                                    V3d( toV3f( (*normals)[normIndex] ) ),
                                    norm );
                                norm.normalize();
                                glNormal3f( norm.x, norm.y, norm.z );
                            }

                            if ( doUVs )
                            {
                                int uvIndex = (*uvindices)[vi + q];
                                V2f uv = toV2f( (*uvs)[uvIndex] );

                                glColor3f( uv.x - floorf( uv.x ),
                                           uv.y - floorf( uv.y ),
                                           1.0f );
                            }   
                            
                            int vertIndex = (*indices)[vi + q];
                            V3d vert;
                            m.multVecMatrix( cpos.d( vertIndex ),
                                             vert );
                            glVertex3f( vert.x, vert.y, vert.z );
                        }

                        glEnd();
                    }

                    // Increment the vertex indices.
                    firstVertexIndex += psize;
                }
            }

            glPopMatrix();
        }
    
        m_displayList.endList();
    }

    m_displayList.callList();

    // overlay();
    
    glutSwapBuffers();
}

} // End namespace SimpleGtoViewer
