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

#include <Alembic/PdbIO/PdbIO.h>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <vector>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

//-*****************************************************************************
class OstreamError : public Alembic::PdbIO::ErrorHandler
{
public:
    OstreamError() : Alembic::PdbIO::ErrorHandler() {}

    virtual void handle( const std::string &what )
    {
        std::cerr << "PDB Read ERROR: " << what << std::endl;
        exit( -1 );
    }
};

//-*****************************************************************************
class OstreamSink : public Alembic::PdbIO::Sink
{
public:
    OstreamSink( std::ostream &ostr, bool all )
      : Alembic::PdbIO::Sink(),
        m_ostream( ostr ),
        m_all( all ) {}

    void beginObjectRead( const Alembic::PdbIO::ObjectInfo &obj );

    char *beginAttributeRead( const Alembic::PdbIO::ObjectInfo &obj,
                              const Alembic::PdbIO::AttributeInfo &attr );

    void endAttributeRead( const Alembic::PdbIO::ObjectInfo &obj,
                           const Alembic::PdbIO::AttributeInfo &attr );

    void endObjectRead( const Alembic::PdbIO::ObjectInfo &obj );

protected:
    std::ostream &m_ostream;
    bool m_all;
    std::vector<char> m_data;
};

//-*****************************************************************************
void OstreamSink::beginObjectRead( const Alembic::PdbIO::ObjectInfo &obj )
{
    if ( m_all )
    {
        m_data.resize( 12 * obj.numParticles );
    }

    m_ostream <<
        ( boost::format( "<object numParticles=%d numAttributes=%d>" )
          % ( int )obj.numParticles % ( int )obj.numAttributes )
              << std::endl;
}

//-*****************************************************************************
static inline const char *attrTypeName( Alembic::PdbIO::AttributeType atyp )
{
    switch ( atyp )
    {
    case Alembic::PdbIO::k_VectorAttribute: return "vector";
    case Alembic::PdbIO::k_FloatAttribute: return "float";
    case Alembic::PdbIO::k_IntAttribute: return "int";
    };

    return "UNDEFINED";
}

//-*****************************************************************************
char *OstreamSink::beginAttributeRead( const Alembic::PdbIO::ObjectInfo &obj,
                                       const Alembic::PdbIO::AttributeInfo &attr )
{
    m_ostream <<
        ( boost::format( "<attribute name=%s type=%s index=%d>" )
          % attr.name % attrTypeName( attr.type ) % ( int )attr.index )
              << std::endl;

    if ( m_all )
    {
        return &( m_data.front() );
    }
    else
    {
        m_ostream << "</attribute>" << std::endl;
        return NULL;
    }
}

//-*****************************************************************************
void OstreamSink::endAttributeRead( const Alembic::PdbIO::ObjectInfo &obj,
                                    const Alembic::PdbIO::AttributeInfo &attr )
{
    if ( m_all )
    {
        if ( attr.type == Alembic::PdbIO::k_VectorAttribute )
        {
            float *vdata = ( float * )&( m_data.front() );

            for ( size_t p = 0; p < obj.numParticles; ++p )
            {
                m_ostream <<
                    ( boost::format( "[ %f, %f, %f ] " )
                      % vdata[p*3]
                      % vdata[1 + p*3]
                      % vdata[2 + p*3] );

                if ( ( (p+1) % 3 ) == 0 )
                {
                    m_ostream << std::endl;
                }
            }
        }
        else if ( attr.type == Alembic::PdbIO::k_FloatAttribute )
        {
            float *fdata = ( float * )&( m_data.front() );

            for ( size_t p = 0; p < obj.numParticles; ++p )
            {
                m_ostream << fdata[p] << ", ";

                if ( ( (p+1) % 9 ) == 0 )
                {
                    m_ostream << std::endl;
                }
            }
        }
        else if ( attr.type == Alembic::PdbIO::k_IntAttribute )
        {
            int *idata = ( int * )&( m_data.front() );

            for ( size_t p = 0; p < obj.numParticles; ++p )
            {
                m_ostream << idata[p] << ", ";

                if ( ( (p+1) % 9 ) == 0 )
                {
                    m_ostream << std::endl;
                }
            }
        }
    }
    
    m_ostream << "</attribute>" << std::endl;
}

//-*****************************************************************************
void OstreamSink::endObjectRead( const Alembic::PdbIO::ObjectInfo &obj )
{
    m_ostream << "</object>" << std::endl;
}

//-*****************************************************************************
//-*****************************************************************************



namespace po = boost::program_options;
    
int main( int argc, char *argv[] )
{
    std::string inFileName = "UNSPECIFIED";
    
    po::options_description desc( "PDB Echo options" );
    desc.add_options()

        ( "help,h", "prints this help message" )

        ( "input,i",
          po::value<std::string>( &inFileName ),
          "PDB filename to echo" )

        ( "all,a", "prints all data" )

        ;

    po::variables_map vm;
    po::store( po::command_line_parser( argc, argv ).
               options( desc ).run(), vm );
    po::notify( vm );

    //-*************************************************************************
    if ( vm.count( "help" ) || !vm.count( "input" ) )
    {
        std::cout << desc << std::endl;
        return -1;
    }

    bool all = vm.count( "all" ) > 0;

    OstreamSink osnk( std::cout, all );
    OstreamError oerr;
    Alembic::PdbIO::Read( inFileName, osnk, oerr );

    return 0;
}

