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

#ifndef _Alembic_TakoSPI_ReaderGlue_h_
#define _Alembic_TakoSPI_ReaderGlue_h_

#include <Alembic/TakoSPI/Foundation.h>
#include <Alembic/TakoSPI/HDFReaderNode.h>

namespace Alembic {
namespace TakoSPI {

//-*****************************************************************************
// This is the celebrated "Curiously Recurring Template Pattern"
// http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
// It allows us to avoid ambiguities with multiple inheritance.
// CJH: I agree this is non-ideal. It can be fixed later, and for now,
// means that the TakoSPI code is not changed very much.
template <class ABSTRACT_READER>
class ReaderGlue : public ABSTRACT_READER
{
public:
    typedef typename ABSTRACT_READER::PropInfo PropInfoMap;
    typedef typename ABSTRACT_READER::PropNameInfoMap PropNameInfoMap;
    
    explicit ReaderGlue(const std::string & iName)
      : m_reader( iName ) {}

    ReaderGlue(const std::string & iName, hid_t iParent)
      : m_reader( iName, iParent ) {}

    
    //-*************************************************************************
    // Property management
    virtual void readProperties( float iFrame )
    { m_reader.readProperties( iFrame ); }
    
    virtual PropertyType readProperty(
        const std::string & iName,
        float iFrame,
        PropertyPair & oProp )
    { return m_reader.readProperty( iName, iFrame, oProp ); }

    virtual void getPropertyNamesAndInfo(
        PropNameInfoMap & oMap )
    { m_reader.getPropertyNamesAndInfo( oMap ); }
    
    virtual PropertyType getProperty(
        const std::string &iName,
        PropertyPair & oProp )
    { m_reader.getProperty( iName, oProp ); }
    
    virtual void clearNonSampledProperties()
    { m_reader.clearNonSampledProperties(); }

    virtual PropertyMap::const_iterator
    beginNonSampledProperties() const
    { return m_reader.beginNonSampledProperties(); }
    
    virtual PropertyMap::const_iterator
    endNonSampledProperties() const
    { return m_reader.endNonSampledProperties(); }

    virtual PropertyMap::const_iterator
    beginSampledProperties() const
    { return m_reader.beginSampledProperties(); }

    virtual PropertyMap::const_iterator
    endSampledProperties() const
    { return m_reader.endSampledProperties(); }

    virtual void getPropertyFrames(
        std::set<float> & ioFrames ) const
    { m_reader.getPropertyFrames( ioFrames ); }

    virtual bool hasPropertyFrames() const
    { return m_reader.hasPropertyFrames(); }

    virtual const std::string & getName() const
    { return m_reader.getName(); }
    
    // Use with care.
    hid_t getGroup() const { return m_reader.mGroup; }
    
private:
    HDFReaderNodeIMPL m_reader;
};

} // End namespace TakoSPI
} // End namespace Alembic

#endif // _Alembic_TakoSPI_ReaderGlue_h_
