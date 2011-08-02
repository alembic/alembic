//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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
#include "SubDTags.h"

//-*****************************************************************************
void SubDTagBuilder::add( const std::string &tag )
{
    m_tags.push_back( tag );
    m_outputTags.push_back( const_cast<RtToken>( m_tags.back().c_str() ) );

    m_argCounts.push_back( 0 );
    m_argCounts.push_back( 0 );

    m_argCountsHierarchical.push_back( 0 );
    m_argCountsHierarchical.push_back( 0 );
    m_argCountsHierarchical.push_back( 0 );
}

//-*****************************************************************************
void SubDTagBuilder::addIntArg( RtInt value )
{
    if ( m_tags.empty() ) { return; }

    m_intArgs.push_back( value );

    m_argCounts[( m_tags.size() - 1 ) * 2] += 1;

    m_argCountsHierarchical[( m_tags.size() - 1 ) * 3] += 1;
}

//-*****************************************************************************
void SubDTagBuilder::addFloatArg( RtFloat value )
{
    if ( m_tags.empty() ) { return; }

    m_floatArgs.push_back( value );

    m_argCounts[( m_tags.size() - 1 ) * 2 + 1] += 1;

    m_argCountsHierarchical[( m_tags.size() - 1 ) * 3 + 1] += 1;
}

//-*****************************************************************************
void SubDTagBuilder::addStringArg( const std::string &value )
{
    if ( m_tags.empty() ) { return; }

    m_stringArgs.push_back( value );

    //stow the c_str from our stored std::string to make sure it's still alive
    //when we need it
    m_outputStringArgs.push_back(
        const_cast<RtToken>( m_stringArgs.back().c_str() ) );

    m_argCountsHierarchical[( m_tags.size() - 1 ) * 3 + 2] += 1;
}

//-*****************************************************************************
RtInt SubDTagBuilder::nt()
{
    return m_outputTags.size();
}

//-*****************************************************************************
RtToken* SubDTagBuilder::tags()
{
    if ( m_outputTags.empty() ) { return NULL; }

    return (RtToken*) &m_outputTags.front();
}

//-*****************************************************************************
RtInt* SubDTagBuilder::nargs( bool hierarchicalSubD )
{
    if ( hierarchicalSubD )
    {
        if ( m_argCountsHierarchical.empty() ) { return NULL; }

        return (RtInt*) &m_argCountsHierarchical.front();
    }
    else
    {
        if ( m_argCounts.empty() ) { return NULL; }

        return (RtInt*) &m_argCounts.front();
    }
}

//-*****************************************************************************
RtInt* SubDTagBuilder::intargs()
{
    if ( m_intArgs.empty() ) { return NULL; }

    return &m_intArgs.front();
}

//-*****************************************************************************
RtFloat* SubDTagBuilder::floatargs()
{
    if ( m_floatArgs.empty() ) { return NULL; }

    return &m_floatArgs.front();
}

//-*****************************************************************************
RtToken* SubDTagBuilder::stringargs()
{
    if ( m_outputStringArgs.empty() ) { return NULL; }

    return &m_outputStringArgs.front();
}

//-*****************************************************************************
void ProcessInterpolateBoundry( SubDTagBuilder &tags,
                                ISubDSchema::Sample &sample )
{
    if ( sample.getInterpolateBoundary() > 0 )
    {
        tags.add( "interpolateboundary" );

        tags.addIntArg( sample.getInterpolateBoundary() );
    }
}

//-*****************************************************************************
void ProcessFacevaryingInterpolateBoundry( SubDTagBuilder &tags,
                                           ISubDSchema::Sample &sample )
{
    if ( sample.getFaceVaryingInterpolateBoundary() > 0 )
    {
        tags.add( "facevaryinginterpolateboundary" );

        tags.addIntArg( sample.getFaceVaryingInterpolateBoundary() );
    }
}

//-*****************************************************************************
void ProcessFacevaryingPropagateCorners( SubDTagBuilder &tags,
                                         ISubDSchema::Sample &sample )
{
    if ( sample.getFaceVaryingPropagateCorners() > 0 )
    {
        tags.add( "facevaryingpropagatecorners" );

        tags.addIntArg( sample.getFaceVaryingPropagateCorners() );
    }
}

//-*****************************************************************************
void ProcessHoles( SubDTagBuilder &tags, ISubDSchema::Sample &sample )
{
    if ( Int32ArraySamplePtr holes = sample.getHoles() )
    {
        if ( holes->size() > 0 )
        {
            tags.add( "hole" );
            //TODO, unfortunate to add these one-at-a-time
            for ( size_t i = 0, e = holes->size(); i < e; ++i )
            {
                tags.addIntArg( holes->get()[i] );
            }
        }
    }
}

//-*****************************************************************************
void ProcessCreases( SubDTagBuilder &tags, ISubDSchema::Sample &sample )
{
    if ( sample.getCreaseLengths()
         && sample.getCreaseIndices()
         && sample.getCreaseSharpnesses() )
    {
        Int32ArraySamplePtr creaseLengths = sample.getCreaseLengths();
        Int32ArraySamplePtr creaseIndices = sample.getCreaseIndices();
        FloatArraySamplePtr creaseSharpnesses = sample.getCreaseSharpnesses();

        size_t totalIndices = 0;

        for ( size_t i = 0, e = creaseLengths->size(); i < e; ++i )
        {
            totalIndices += creaseLengths->get()[i];
        }

        // All properties should have the same number of samples
        if ( totalIndices == creaseIndices->size() )
        {
            size_t indicesIndex = 0;
            for ( size_t i = 0, e = creaseLengths->size(); i < e; ++i )
            {
                tags.add( "crease" );

                tags.addFloatArg( creaseSharpnesses->get()[i] );

                int lengthValue = creaseLengths->get()[i];

                for ( int j = 0; j < lengthValue; ++j, ++indicesIndex )
                {
                    tags.addIntArg( creaseIndices->get()[indicesIndex] );
                }
            }
        }
        else
        {
            //TODO, error. For now, just exclude creases
        }
    }
}

//-*****************************************************************************
void ProcessCorners( SubDTagBuilder &tags, ISubDSchema::Sample &sample )
{
    if ( sample.getCornerIndices() && sample.getCornerSharpnesses() )
    {
        Int32ArraySamplePtr cornerIndices = sample.getCornerIndices();
        FloatArraySamplePtr cornerSharpnesses = sample.getCornerSharpnesses();

        if ( cornerIndices->size() > 0 )
        {
            size_t count = cornerIndices->size();

            tags.add("corner");
            for (size_t i = 0; i < count; ++i)
            {
                tags.addIntArg(cornerIndices->get()[i]);
                tags.addFloatArg(cornerSharpnesses->get()[i]);
            }
        }
        else
        {
            //TODO, error, For now, just exclude corners
        }
    }
}
