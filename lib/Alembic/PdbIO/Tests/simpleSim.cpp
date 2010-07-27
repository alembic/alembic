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
#include <boost/random.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/cstdint.hpp>
#include <vector>
#include <sys/types.h>
#include <string>
#include <iostream>

//-*****************************************************************************
namespace AbcPdb = Alembic::PdbIO;

//-*****************************************************************************
struct V3f
{
    AbcPdb::float32_t x;
    AbcPdb::float32_t y;
    AbcPdb::float32_t z;
};

typedef std::vector<boost::int32_t> IntAttribute;
typedef std::vector<AbcPdb::float32_t> FltAttribute;
typedef std::vector<V3f> VecAttribute;

//-*****************************************************************************
// Boost random number stuff.
typedef boost::mt19937 base_generator_type;
typedef boost::uniform_real<> distribution_type;
typedef boost::variate_generator<base_generator_type &,
                                 distribution_type> gen_type;

//-*****************************************************************************
class SimpleSim
{
public:
    SimpleSim( size_t randSeed, size_t numParts );

    void timeStep( float dt );

    void write( const std::string &fileName );

protected:
    void randSpherePt( V3f &rslt )
    {
        float len = 2.0f;
        do
        {
            rslt.x = m_randGT();
            rslt.y = m_randGT();
            rslt.z = m_randGT();
            len = rslt.x*rslt.x + rslt.y*rslt.y + rslt.z*rslt.z;
        }
        while ( len > 1.0f );
    }

    void newParticle( boost::int32_t &id,
                      V3f &pos,
                      V3f &vel,
                      V3f &col,
                      AbcPdb::float32_t &age );

    class SimSource : public AbcPdb::Source
    {
    public:
        SimSource( SimpleSim &ss )
          : m_sim( ss ) {}

        virtual void beginObjectWrite( AbcPdb::ObjectInfo &obj );

        virtual const char *beginAttributeWrite(
            const AbcPdb::ObjectInfo &obj,
            AbcPdb::AttributeInfo &attr );

    protected:
        SimpleSim &m_sim;
    };

    class SimError : public AbcPdb::ErrorHandler
    {
    public:
        SimError() : AbcPdb::ErrorHandler() {}
        virtual void handle( const std::string &err )
        {
            std::cerr << "PDB Write ERROR: " << err << std::endl;
            abort();
        }
    };

    friend class SimSource;

    base_generator_type m_randBGT;
    distribution_type m_randDT;
    gen_type m_randGT;

    IntAttribute m_id;
    VecAttribute m_position;
    VecAttribute m_velocity;
    VecAttribute m_color;
    FltAttribute m_age;

    boost::int32_t m_nextID;
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
void SimpleSim::SimSource::beginObjectWrite( AbcPdb::ObjectInfo &obj )
{
    obj.numParticles = m_sim.m_id.size();
    obj.numAttributes = 5;
}

//-*****************************************************************************
const char *SimpleSim::SimSource::beginAttributeWrite(
    const AbcPdb::ObjectInfo &obj,
    AbcPdb::AttributeInfo &attr )
{
    if ( attr.index == 0 )
    {
        attr.name = "id";
        attr.type = AbcPdb::k_IntAttribute;
        return ( const char * )&( m_sim.m_id.front() );
    }
    else if ( attr.index == 1 )
    {
        attr.name = "position";
        attr.type = AbcPdb::k_VectorAttribute;
        return ( const char * )&( m_sim.m_position.front() );
    }
    else if ( attr.index == 2 )
    {
        attr.name = "velocity";
        attr.type = AbcPdb::k_VectorAttribute;
        return ( const char * )&( m_sim.m_velocity.front() );
    }
    else if ( attr.index == 3 )
    {
        attr.name = "rgbPP";
        attr.type = AbcPdb::k_VectorAttribute;
        return ( const char * )&( m_sim.m_color.front() );
    }
    else if ( attr.index == 4 )
    {
        attr.name = "age";
        attr.type = AbcPdb::k_FloatAttribute;
        return ( const char * )&( m_sim.m_age.front() );
    }
    else
    {
        return NULL;
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
SimpleSim::SimpleSim( size_t randSeed, size_t numParts )
  : m_randBGT( ( unsigned int )randSeed ),
    m_randDT( -1.0, 1.0 ),
    m_randGT( m_randBGT, m_randDT ),
    m_id( numParts ),
    m_position( numParts ),
    m_velocity( numParts ),
    m_color( numParts ),
    m_age( numParts ),
    m_nextID( 0 )
{
    for ( size_t p = 0; p < numParts; ++p )
    {
        newParticle( m_id[p],
                     m_position[p],
                     m_velocity[p],
                     m_color[p],
                     m_age[p] );
    }
}

//-*****************************************************************************
void SimpleSim::timeStep( float dt )
{
    size_t numParts = m_id.size();
    for ( size_t p = 0; p < numParts; ++p )
    {
        V3f &pos = m_position[p];
        V3f &vel = m_velocity[p];
        float &age = m_age[p];

        pos.x += dt * vel.x;
        pos.y += dt * vel.y;
        pos.z += dt * vel.z;

        vel.y += dt * -9.81f;

        age += dt;

        if ( pos.y < 0.0f )
        {
            newParticle( m_id[p],
                         pos,
                         vel,
                         m_color[p],
                         age );
        }
    }
}

//-*****************************************************************************
void SimpleSim::write( const std::string &fileName )
{
    SimSource src( *this );
    SimError err;
    AbcPdb::Write( fileName, src, err );
    std::cout << "Wrote: " << fileName << std::endl;
}

//-*****************************************************************************
void SimpleSim::newParticle( boost::int32_t &id,
                             V3f &pos,
                             V3f &vel,
                             V3f &col,
                             AbcPdb::float32_t &age )
{
    id = m_nextID;
    ++m_nextID;

    randSpherePt( pos );
    pos.x = ( pos.x * 0.25f );
    pos.y = ( pos.y * 0.25f ) + 0.3f;
    pos.z = ( pos.z * 0.25f );

    randSpherePt( vel );
    vel.x = ( vel.x * 1.0f );
    vel.y = ( vel.y * 1.0f ) + 8.0f;
    vel.z = ( vel.z * 1.0f );

    randSpherePt( col );
    col.x = 0.5f + 0.5f * fabsf( col.x );
    col.y = 0.5f + 0.5f * fabsf( col.y );
    col.z = 0.5f + 0.5f * fabsf( col.z );

    age = 0.0f;
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
namespace po = boost::program_options;

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    std::string outFileBase = "particles";
    int numParts = 10000;
    int randSeed = 54321;
    int startFrame = 1;
    int endFrame = 100;

    po::options_description desc( "Simple PDB Sim options" );
    desc.add_options()

        ( "help,h", "prints this help message" )

        ( "output,o",
          po::value<std::string>( &outFileBase ),
          "base of output filenames" )

        ( "numParts,n",
          po::value<int>( &numParts ),
          "number of particles" )

        ( "randSeed,r",
          po::value<int>( &randSeed ),
          "random seed" )

        ( "startFrame,sf",
          po::value<int>( &startFrame ),
          "start frame" )

        ( "endFrame,ef",
          po::value<int>( &endFrame ),
          "end frame" )

        ;

    po::variables_map vm;
    po::store( po::command_line_parser( argc, argv ).
               options( desc ).run(), vm );
    po::notify( vm );

    //-*************************************************************************
    if ( vm.count( "help" ) || startFrame > endFrame || numParts < 0 )
    {
        std::cout << desc << std::endl;
        return -1;
    }

    std::cout << "Creating simulation." << std::endl;
    SimpleSim sim( ( size_t )randSeed, ( size_t )numParts );

    std::cout << "Simulating frames." << std::endl;
    for ( int frame = startFrame; frame <= endFrame; ++frame )
    {
        std::cout << "Time stepping to frame: " << frame << std::endl;

        sim.timeStep( 1.0f/24.0f );

        sim.write( ( boost::format( "%s.%d.pdb" )
                     % outFileBase % frame ).str() );
    }

    return 0;
}
