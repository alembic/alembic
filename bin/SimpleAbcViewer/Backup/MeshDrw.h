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

#ifndef _SimpleAbcViewer_MeshDrw_h_
#define _SimpleAbcViewer_MeshDrw_h_

#include "Foundation.h"
#include "Drawable.h"
#include "MeshDrwHelper.h"

namespace SimpleAbcViewer {

//-*****************************************************************************
//! \brief Drawable for PolyMeshes.
//! ...
template <class TAKO_READER_PTR>
class MeshDrw : public Drawable
{
public:
    typedef typename TAKO_READER_PTR::element_type tako_reader_type;
    typedef TAKO_READER_PTR tako_reader_ptr_type;
    typedef MeshDrw<TAKO_READER_PTR> this_type;
    
    MeshDrw( TAKO_READER_PTR i_meshReader );

    virtual ~MeshDrw();

    virtual bool valid() const;

    virtual bool readFrame( float i_frame );

    virtual void getFrames( std::set<float> & i_sampleFrames ) const;

    virtual bool hasFrames() const;

    virtual Box3d getBounds() const;

    virtual void draw( const DrawContext & i_ctx ) const;

protected:
    TAKO_READER_PTR m_takoReader;
    MeshDrwHelper m_helper;
};

//-*****************************************************************************
// TYPEDEFS
typedef MeshDrw<Atk::PolyMeshReaderPtr> PolyMeshDrw;
typedef MeshDrw<Atk::SubDReaderPtr> SubDDrw;

//-*****************************************************************************
// TEMPLATE IMPLEMENTATIONS
//-*****************************************************************************
template <class TRP>
MeshDrw<TRP>::MeshDrw( TRP i_meshReader )
  : Drawable(),
    m_takoReader( i_meshReader ),
    m_helper()
{
    // Nothing
}

//-*****************************************************************************
template <class TRP>
MeshDrw<TRP>::~MeshDrw()
{
    // Nothing
}

//-*****************************************************************************
template <class TRP>
bool MeshDrw<TRP>::valid() const
{
    return ( m_takoReader && m_helper.valid() );
}

//-*****************************************************************************
inline const std::vector<float> *MeshDrwNormals(
    const Atk::PolyMeshReaderPtr mptr )
{
    if ( !mptr ) { return NULL; }
    else if ( mptr->mNormals.size() == mptr->mPoints.size() )
    { return &( mptr->mNormals ); }
    else { return NULL; }
}

//-*****************************************************************************
inline const std::vector<float> *MeshDrwNormals(
    const Atk::SubDReaderPtr mptr )
{
    return NULL;
}

//-*****************************************************************************
template <class TRP>
bool MeshDrw<TRP>::readFrame( float i_frame )
{
    // Check validity
    if ( !m_takoReader )
    {
        m_helper.makeInvalid();
        return false;
    }

    unsigned int readType = m_takoReader->read( i_frame );
    if ( readType == ( unsigned int )tako_reader_type::READ_ERROR )
    {
        m_helper.makeInvalid();
        return false;
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
    if ( !m_helper.valid() ||
         readType == MESH_HETEROGENOUS )
    {
        m_helper.updateMesh( &(m_takoReader->mPoints),
                             MeshDrwNormals( m_takoReader ),
                             &(m_takoReader->mFacePoints),
                             &(m_takoReader->mFaceList) );
    }
    else
    {
        if ( readType == MESH_STATIC )
        {
            return m_helper.valid();
        }
        else
        {
            assert( readType == MESH_HOMOGENOUS );
            m_helper.updateMesh( &(m_takoReader->mPoints),
                                 MeshDrwNormals( m_takoReader ) );
        }
    }

    // All done.
    return m_helper.valid();
}

//-*****************************************************************************
template <class TRP>
void MeshDrw<TRP>::getFrames( std::set<float> & i_sampleFrames ) const
{
    if ( m_takoReader )
    {
        m_takoReader->getFrames( i_sampleFrames );
    }
}

//-*****************************************************************************
template <class TRP>
bool MeshDrw<TRP>::hasFrames() const
{
    return ( m_takoReader ) && m_takoReader->hasFrames();
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

} // End namespace SimpleAbcViewer

#endif
