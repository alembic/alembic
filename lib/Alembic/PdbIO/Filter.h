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

#ifndef _Alembic_PdbIO_Filter_h_
#define _Alembic_PdbIO_Filter_h_

#include <Alembic/PdbIO/Foundation.h>
#include <Alembic/PdbIO/Sink.h>
#include <Alembic/PdbIO/Source.h>

namespace Alembic {
namespace PdbIO {

//-*****************************************************************************
// This is a lightweight class intended to just go through the exercise
// of playing around with PDB streams.
// Therefore, not implementing any "nice" functionality.
// DON'T USE THIS TO DO REAL WORK!
//-*****************************************************************************
class Filter
{
public:
    Filter();
    virtual ~Filter();

    // Do it....
    void filter( std::istream &in,
                 std::ostream &out );

    void filter( const std::string &inFileName,
                 const std::string &outFileName );

    // Implement this to decide how you want to filter things.
    // A regex could work.
    virtual bool keepAttribute( const std::string &name,
                                AttributeType type ) = 0;

protected:
    //-*************************************************************************
    // Thin wrapper that mediates between Filter and Sink
    class FiltSink : public Sink
    {
    public:
        FiltSink( Filter &flt )
          : m_filter( flt ) {}

        virtual void beginObjectRead( const ObjectInfo &obj );

        virtual char *beginAttributeRead( const ObjectInfo &obj,
                                          const AttributeInfo &attr );

    protected:
        Filter &m_filter;
    };

    //-*************************************************************************
    // Thin wrapper that mediates between Filter and Source
    class FiltSource : public Source
    {
    public:
        FiltSource( Filter &flt )
          : m_filter( flt ) {}

        virtual void beginObjectWrite( ObjectInfo &obj );
        
        virtual const char *beginAttributeWrite( const ObjectInfo &obj,
                                                 AttributeInfo &attr );
                             
    protected:
        Filter &m_filter;
    };

    //-*************************************************************************
    // Error handler for reading.
    class FiltReadError : public ErrorHandler
    {
    public:
        FiltReadError( Filter &flt )
          : ErrorHandler(),
            m_filter( flt ) {}

        virtual void handle( const std::string &err )
        {
            m_filter.handleReadError( err );
        }

    protected:
        Filter &m_filter;
    };

    //-*************************************************************************
    class FiltWriteError : public ErrorHandler
    {
    public:
        FiltWriteError( Filter &flt )
          : ErrorHandler(),
            m_filter( flt ) {}

        virtual void handle( const std::string &err )
        {
            m_filter.handleWriteError( err );
        }

    protected:
        Filter &m_filter;
    };

    friend class FiltSink;
    friend class FiltSource;
    friend class FiltReadError;
    friend class FiltWriteError;

    //-*************************************************************************
    // Local classes and structures
    //-*************************************************************************
    struct Attribute
    {
        Attribute( const std::string &nme,
                   AttributeType typ,
                   size_t numParts );

        char *dataPtr()
        {
            if ( data.size() == 0 ) { return NULL; }
            else { return &( data.front() ); }
        }

        const char *dataPtr() const
        {
            if ( data.size() == 0 ) { return NULL; }
            else { return &( data.front() ); }
        }

        std::string name;
        AttributeType type;
        std::vector<char> data;
    };

    typedef boost::shared_ptr<Attribute> AttrPtr;
    typedef std::vector<AttrPtr> Attributes;
    
    //-*************************************************************************
    // PROTECTED FUNCTIONALITY (for Filt helper classes to use)
    //-*************************************************************************
    void setNumParticles( size_t np );
    size_t numParticles() const { return m_numParticles; }

    size_t numAttributes() const { return m_attributes.size(); }
    Attribute &attribute( size_t index )
    {
        return *(m_attributes[index]);
    }
    const Attribute &attribute( size_t index ) const
    {
        return *(m_attributes[index]);
    }

    Attribute &newAttribute( const std::string &name,
                             AttributeType typ );

    // Error reporting
    // You can override these if you like. 
    virtual void handleReadError( const std::string &what )
    {
        std::cerr << "PDB IO Read ERROR: " << what << std::endl;
        abort();
    }

    virtual void handleWriteError( const std::string &what )
    {
        std::cerr << "PDB IO Write ERROR: " << what << std::endl;
        abort();
    }

    //-*************************************************************************
    // PRIVATE DATA
    //-*************************************************************************
private:
    size_t m_numParticles;
    Attributes m_attributes;
};

} // End namespace PdbIO
} // End namespace Alembic

#endif
