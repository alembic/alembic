//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#include <AlembicTako/TransformReader.h>
#include <AlembicTako/PolyMeshReader.h>
#include <AlembicTako/PointPrimitiveReader.h>
#include <AlembicTako/NullReader.h>

namespace Abc = AlembicAsset;

namespace AlembicTako {

//-*****************************************************************************
// This opens the Tako file with the given name.
TransformReader::TransformReader( const std::string &fileName )
  : HDFReaderNode(),
    m_asset( fileName, Abc::kThrowException ),
    m_transform( m_asset, "Top", Abc::kThrowException )
{
    m_inheritsTransform =
        ( bool )( m_transform->inheritsTransform().get() );
    
    m_matrix.makeIdentity();

    HDFReaderNode::init( m_transform );
}

//-*****************************************************************************
// This creates a Transform Reader that is a child
// of the given parent transform reader.
TransformReader::TransformReader( const std::string &nodeName,
                                  TransformReader &parent )
  : HDFReaderNode(),
    m_asset(),
    m_transform( parent.alembicObject(), nodeName,
                 Abc::kThrowException )
{
    m_inheritsTransform =
        ( bool )( m_transform->inheritsTransform().get() );
    
    m_matrix.makeIdentity();

    HDFReaderNode::init( m_transform );
}

//-*****************************************************************************
// This reads the current frame.
TransformReader::SampleType TransformReader::read( float iFrame )
{
    if ( !m_transform )
    {
        return READ_ERROR;
    }
    
    Abc::M44f mtx;
    mtx.makeIdentity();
    
    // If transform is not animated, this will simply redirect to rest
    // pose. Tako uses FLT_MAX to represent the rest pose.
    Abc::seconds_t seconds = iFrame == FLT_MAX ?
        Abc::Time::kRestTimeValue() : ( Abc::seconds_t )iFrame;
    Abc::IM44fSample stack =
        m_transform->stack().getAnimLowerBound( seconds );
    
    // If anything went wrong, or if the stack
    // simply had no data, we'll stick with mtx being identity.
    size_t numStackItems = stack.size();
    if ( numStackItems > 0 )
    {
        if ( m_inheritsTransform )
        {
            // Inherit the whole stack.
            for ( size_t i = 0; i < numStackItems; ++i )
            {
                mtx *= stack[i];
            }
        }
        else
        {
            // Just take the back of the stack.
            mtx = stack[numStackItems-1];
        }
    }
    
    m_matrix = mtx;
    return SAMPLES_READ;
}

//-*****************************************************************************
// Actually creates and returns the child.
SharedHDFReaderNode TransformReader::getChild( size_t index )
{
    SharedHDFReaderNode ret;
    if ( index < m_transform.numChildren() )
    {
        Abc::ObjectInfo cinfo = m_transform.childInfo( index );
        if ( cinfo->protocol == "TakoTransform_v0001" )
        {
            ret.reset( new TransformReader( cinfo->name.c_str(),
                                            *this ) );
        }
        else if ( cinfo->protocol == "TakoPolyMesh_v0001" )
        {
            ret.reset( new PolyMeshReader( cinfo->name.c_str(),
                                           *this ) );
        }
        else if ( cinfo->protocol == "TakoPointPrimitive_v0001" )
        {
            ret.reset( new PointPrimitiveReader( cinfo->name.c_str(),
                                                 *this ) );
        }
        else
        {
            ret.reset( new NullReader( cinfo->name.c_str(),
                                       *this ) );
        }
    }
    return ret;
}

} // End namespace AlembicTako
