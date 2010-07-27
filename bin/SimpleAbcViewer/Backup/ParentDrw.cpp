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

#include "ParentDrw.h"
#include "TransformDrw.h"
#include "MeshDrw.h"
#include "PointPrimitiveDrw.h"

namespace SimpleAbcViewer {

//-*****************************************************************************
ParentDrw::ParentDrw()
  : Drawable(),
    m_readStatus( false )
{
    // Nothing
}

//-*****************************************************************************
ParentDrw::~ParentDrw()
{
    // Nothing. Vector elements delete themselves.
}

//-*****************************************************************************
bool ParentDrw::valid() const
{
    return m_readStatus;
}

//-*****************************************************************************
bool ParentDrw::readFrame( float i_frame )
{
    // Reset...
    m_readStatus = false;
    m_currentFrameBounds.makeEmpty();
    
    // The transform does, itself, need certain things to be read.
    // We don't necessarily need a transformation matrix because this
    // may be an asset, which doesn't have one.
    // But, we do need to make sure we are read in ourselves and valid.
    if ( !m_takoTransformReader )
    {
        return m_readStatus;
    }

    if ( m_takoTransformReader->read( i_frame ) ==
         Atk::TransformReader::READ_ERROR )
    {
        return m_readStatus;
    }

    if ( m_children.size() == 0 )
    {
        m_readStatus = true;
        return m_readStatus;
    }
    
    for ( Children::iterator iter = m_children.begin();
          iter != m_children.end(); ++iter )
    {
        if ( (*iter)->readFrame( i_frame ) )
        {
            m_readStatus = true;
            m_currentFrameBounds.extendBy( (*iter)->getBounds() );
        }
    }
    return m_readStatus;
}

//-*****************************************************************************
void ParentDrw::getFrames( std::set<float> & i_sampleFrames ) const
{
    for ( Children::const_iterator iter = m_children.begin();
          iter != m_children.end(); ++iter )
    {
        (*iter)->getFrames( i_sampleFrames );
    }
}

//-*****************************************************************************
bool ParentDrw::hasFrames() const
{
    for ( Children::const_iterator iter = m_children.begin();
          iter != m_children.end(); ++iter )
    {
        if ( (*iter)->hasFrames() )
        {
            return true;
        }
    }
    return false;
}

//-*****************************************************************************
Box3d ParentDrw::getBounds() const
{
    return m_currentFrameBounds;
}

//-*****************************************************************************
void ParentDrw::draw( const DrawContext &ctx ) const
{
    for ( Children::const_iterator iter = m_children.begin();
          iter != m_children.end(); ++iter )
    {
        if ( (*iter)->valid() )
        {
            (*iter)->draw( ctx );
        }
    }
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

class ChildrenVisitor : public boost::static_visitor<SharedDrawable>
{
public:
    ChildrenVisitor() {}

    SharedDrawable operator()( Atk::TransformReaderPtr & i_child ) const
    {
        SharedDrawable sdrw( new TransformDrw( i_child ) );
        return sdrw;
    }

    SharedDrawable operator()( Atk::SubDReaderPtr & i_child ) const
    {
        SharedDrawable sdrw( new SubDDrw( i_child ) );
        return sdrw;
    }

    SharedDrawable operator()( Atk::PolyMeshReaderPtr & i_child ) const
    {
        SharedDrawable sdrw( new PolyMeshDrw( i_child ) );
        return sdrw;
    }

    SharedDrawable operator()( Atk::PointPrimitiveReaderPtr & i_child ) const
    {
        SharedDrawable sdrw( new PointPrimitiveDrw( i_child ) );
        return sdrw;
    }

    SharedDrawable operator()( Atk::NurbsSurfaceReaderPtr & i_child ) const
    { return SharedDrawable(); }

    SharedDrawable operator()( Atk::NurbsCurveReaderPtr & i_child ) const
    { return SharedDrawable(); }

    SharedDrawable operator()( Atk::CameraReaderPtr & i_child ) const
    { return SharedDrawable(); }

    SharedDrawable operator()( Atk::GenericNodeReaderPtr & i_child ) const
    { return SharedDrawable(); }
};

} // End anonymous namespace; 

//-*****************************************************************************
void ParentDrw::setTakoTransformReader( Atk::TransformReaderPtr i_xform )
{
    // Basically reset everything.
    m_children.clear();
    m_readStatus = false;
    m_currentFrameBounds.makeEmpty();
    
    // Set the pointer.
    m_takoTransformReader = i_xform;

    // If the pointer is valid, make children.
    if ( m_takoTransformReader )
    {
        ChildrenVisitor cvisit;
        size_t numChildren = m_takoTransformReader->numChildren();
        for ( size_t cIdx = 0; cIdx < numChildren; ++cIdx )
        {
            Atk::ChildNodePtr child = m_takoTransformReader->getChild( cIdx );
            SharedDrawable sdrw = boost::apply_visitor( cvisit, child );
            if ( sdrw )
            {
                m_children.push_back( sdrw );
            }
        }
    }
}

} // End namespace SimpleAbcViewer
