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

#ifndef _FluidSimDemo_MeshDrw_h_
#define _FluidSimDemo_MeshDrw_h_

#include <FluidSimDemo/Foundation.h>
#include <FluidSimDemo/Drawable.h>
#include <FluidSimDemo/MeshDrwHelper.h>

namespace FluidSimDemo {

//-*****************************************************************************
//! \brief Drawable for PolyMeshes.
//! ...
template <class TAKO_READER_PTR>
class MeshDrw : public BaseTakoDrw
{
public:
    typedef typename TAKO_READER_PTR::element_type tako_reader_type;
    typedef TAKO_READER_PTR tako_reader_ptr_type;
    typedef MeshDrw<TAKO_READER_PTR> this_type;
    
    MeshDrw( TAKO_READER_PTR i_meshReader );

    virtual ~MeshDrw();

    virtual void readInterpolatedFrame( float i_frame,
                                        const M44d &iParentXform );

    virtual Box3d getBounds() const;

    virtual void draw( const DrawContext & i_ctx ) const;

protected:
    TAKO_READER_PTR m_takoReader;
    MeshDrwHelper m_helper;
};

//-*****************************************************************************
// TYPEDEFS
typedef MeshDrw<PolyMeshReaderPtr> PolyMeshDrw;
typedef MeshDrw<SubDReaderPtr> SubDDrw;


//-*****************************************************************************
inline const std::vector<float> *MeshDrwNormals(
    const PolyMeshReaderPtr mptr )
{
    if ( !mptr ) { return NULL; }
    else if ( mptr->mNormals.size() == mptr->mPoints.size() )
    { return &( mptr->mNormals ); }
    else { return NULL; }
}

//-*****************************************************************************
inline const std::vector<float> *MeshDrwNormals(
    const SubDReaderPtr mptr )
{
    return NULL;
}


//-*****************************************************************************
// TEMPLATE IMPLEMENTATIONS
//-*****************************************************************************
template <class TRP>
MeshDrw<TRP>::MeshDrw( TRP i_meshReader )
  : BaseTakoDrw( i_meshReader ),
    m_takoReader( i_meshReader ),
    m_helper()
{
    if ( m_constant )
    {
        unsigned int readType = m_takoReader->read( FLT_MAX );
        if ( readType == ( unsigned int )tako_reader_type::READ_ERROR )
        {
            FSD_THROW( "Couldn't read constant mesh" );
        }

        
        if ( readType == ( unsigned int )PolyMeshReader::TOPOLOGY_STATIC )
        {
            std::cout << "Mesh: " << getName() << " is STATIC" << std::endl;
        }
        
        m_helper.updateMesh( &(m_takoReader->mPoints),
                             MeshDrwNormals( m_takoReader ),
                             &(m_takoReader->mFacePoints),
                             &(m_takoReader->mFaceList ) );
    }
#if 1
    else
    {
        unsigned int readType = m_takoReader->read( m_minFrame );
        if ( readType == ( unsigned int )tako_reader_type::READ_ERROR )
        {
            FSD_THROW( "Couldn't read constant mesh" );
        }

        if ( readType == ( unsigned int )PolyMeshReader::TOPOLOGY_STATIC )
        {
            std::cout << "Mesh: " << getName() << " is STATIC" << std::endl;
        }
        
        m_helper.updateMesh( &(m_takoReader->mPoints),
                             MeshDrwNormals( m_takoReader ),
                             &(m_takoReader->mFacePoints),
                             &(m_takoReader->mFaceList ) );
    }
#endif
}

//-*****************************************************************************
template <class TRP>
MeshDrw<TRP>::~MeshDrw()
{
    // Nothing
}

//-*****************************************************************************
template <class TRP>
void MeshDrw<TRP>::readInterpolatedFrame( float i_frame,
                                          const M44d &iParentXform )
{
    //return;
    
    if ( m_constant ) { return; }
    
    // Check validity
    if ( !m_takoReader )
    {
        FSD_THROW( "Invalid mesh reader" );
    }

    assert( i_frame != FLT_MAX );
    assert( !m_constant );

    float realFrame = FLT_MAX;
    
    if ( i_frame <= m_minFrame ) { realFrame = m_minFrame; }
    else if ( i_frame >= m_maxFrame ) { realFrame = m_maxFrame; }
    else
    {
        realFrame = (*(--(m_sampleFrames.upper_bound( i_frame ))));
    }

    if ( m_sampleFrames.find( realFrame ) == m_sampleFrames.end() )
    {
        return;
        //FSD_THROW( "This is not a good frame for me: " << realFrame );
    }
    //assert( m_sampleFrames.count( realFrame ) > 0 );

    unsigned int readType = m_takoReader->read( realFrame );
    if ( readType == ( unsigned int )tako_reader_type::READ_ERROR )
    {
        return;
        //FSD_THROW( "Couldn't read mesh at frame: " << realFrame );
    }

    // Trim the read type.
    readType = readType & 0x0000000f;
    enum
    {
        MESH_STATIC,
        MESH_HOMOGENOUS,
        MESH_HETEROGENOUS
    };

    // Update mesh - either completely, or partially.
#if 1
    
        m_helper.updateMesh( &(m_takoReader->mPoints),
                             MeshDrwNormals( m_takoReader ),
                             &(m_takoReader->mFacePoints),
                             &(m_takoReader->mFaceList ) );
#else
    if ( !m_helper.valid() ||
         readType == MESH_HETEROGENOUS )
    {
        m_helper.updateMesh( &(m_takoReader->mPoints),
                             MeshDrwNormals( m_takoReader ),
                             &(m_takoReader->mFacePoints),
                             &(m_takoReader->mFaceList ) );
    }
    else
    {
        if ( readType == MESH_STATIC )
        {
            // return m_helper.valid();
            m_helper.updateMesh(  &(m_takoReader->mPoints),
                                  MeshDrwNormals( m_takoReader ),
                                  &(m_takoReader->mFacePoints),
                                  &(m_takoReader->mFaceList) );
        }
        else
        {
            assert( readType == MESH_HOMOGENOUS );
            m_helper.updateMesh( &(m_takoReader->mPoints),
                                 MeshDrwNormals( m_takoReader ) );
        }
    }
#endif
}

//-*****************************************************************************
template <class TRP>
Box3d MeshDrw<TRP>::getBounds() const
{
    return m_helper.getBounds();
}

//-*****************************************************************************
template <class TRP>
void MeshDrw<TRP>::draw( const DrawContext & i_ctx ) const
{
    m_helper.draw( i_ctx );
}

} // End namespace FluidSimDemo

#endif
