//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include <Alembic/PdbIO/Filter.h>
#include <Alembic/PdbIO/PdbIO.h>

namespace Alembic {
namespace PdbIO {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// FILTSINK CLASS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
void Filter::FiltSink::beginObjectRead( const ObjectInfo &obj )
{
    // Set the number of particles and attributes.
    m_filter.setNumParticles( obj.numParticles );

    // We'll just add the attributes as we need them.
}

//-*****************************************************************************
char *Filter::FiltSink::beginAttributeRead( const ObjectInfo &obj,
                                            const AttributeInfo &attr )
{
    if ( m_filter.keepAttribute( attr.name, attr.type ) )
    {
        return m_filter.newAttribute( attr.name, attr.type ).dataPtr();
    }
    else
    {
        return NULL;
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// FILTSOURCE CLASS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
void Filter::FiltSource::beginObjectWrite( ObjectInfo &obj )
{
    obj.numParticles = m_filter.numParticles();
    obj.numAttributes = m_filter.numAttributes();
}

//-*****************************************************************************
const char *Filter::FiltSource::beginAttributeWrite( const ObjectInfo &obj,
                                                     AttributeInfo &attr )
{
    const Filter::Attribute &fattr = m_filter.attribute( attr.index );
    attr.name = fattr.name;
    attr.type = fattr.type;
    return fattr.dataPtr();
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// ATTRIBUTE CLASS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
Filter::Attribute::Attribute( const std::string &nme,
                              AttributeType typ,
                              size_t numParts )
  : name( nme ),
    type( typ ),
    data( numParts * bytesPerElement( typ ) )
{
    // Nothing.
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// FILTER CLASS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
Filter::Filter()
  : m_numParticles( 0 )
{
    // Nothing
}

//-*****************************************************************************
Filter::~Filter()
{
    // Nothing - everything is self-deleting.
}

//-*****************************************************************************
void Filter::filter( std::istream &istr,
                     std::ostream &ostr )
{
    // First clean up.
    setNumParticles( 0 );
    
    // Okay, Create a FiltSink to read into, and an error reporter.
    FiltSink fsnk( *this );
    FiltReadError fre( *this );
    Read( istr, fsnk, fre );

    // We have data now, which is already filtered. Write it back out.
    FiltSource fsrc( *this );
    FiltWriteError fwe( *this );
    Write( ostr, fsrc, fwe );
}

//-*****************************************************************************
void Filter::filter( const std::string &inFileName,
                     const std::string &outFileName )
{
    // First clean up.
    setNumParticles( 0 );

    // Okay, Create a FiltSink to read into, and an error reporter.
    FiltSink fsnk( *this );
    FiltReadError fre( *this );
    Read( inFileName, fsnk, fre );

    // We have data now, which is already filtered. Write it back out.
    FiltSource fsrc( *this );
    FiltWriteError fwe( *this );
    Write( outFileName, fsrc, fwe );
}

//-*****************************************************************************
void Filter::setNumParticles( size_t np )
{
    m_numParticles = np;
    m_attributes.clear();
}

//-*****************************************************************************
Filter::Attribute &Filter::newAttribute( const std::string &nme,
                                         AttributeType typ )
{
    AttrPtr attr( new Attribute( nme, typ, m_numParticles ) );
    m_attributes.push_back( attr );
    return *attr;
}

} // End namespace PdbIO
} // End namespace Alembic

