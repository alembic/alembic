//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <ImathRandom.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

namespace AbcG = Alembic::AbcGeom;
using namespace AbcG;

using Alembic::AbcCoreAbstract::chrono_t;
using Alembic::AbcCoreAbstract::index_t;

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// PARTICLE SYSTEM
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
class ParticleSystem
{
public:
    struct Parameters
    {
        V3f gravity;
        float elasticity;
        float lifespan;
        float emitRate;
        float emitRadius;

        float emitVelocitySpread;
        V3f emitVelocity;

        float emitColorSpread;
        C3f emitColor;
    };

    ParticleSystem( const Parameters &params );

    void advectExisting( chrono_t dt );

    void destroyOld( chrono_t dt );

    void emitNew( chrono_t dt );

    void timeStep( chrono_t dt )
    {
        advectExisting( dt );
        destroyOld( dt );
        emitNew( dt );
        m_currentTime += dt;
    }

    size_t numParticles() const { return m_id.size(); }
    const std::vector<Alembic::Util::uint64_t> &idVec() const { return m_id; }
    const std::vector<V3f> &positionVec() const { return m_position; }
    const std::vector<C3f> &colorVec() const { return m_color; }
    const std::vector<V3f> &velocityVec() const { return m_velocity; }
    const std::vector<Alembic::Util::float32_t> &ageVec() const
    { return m_age; }

protected:
    Parameters m_params;

    std::vector<Alembic::Util::uint64_t> m_id;
    std::vector<V3f> m_position;
    std::vector<C3f> m_color;
    std::vector<V3f> m_velocity;
    std::vector<Alembic::Util::float32_t> m_age;

    Alembic::Util::uint64_t m_nextId;
    double m_emitDither;
    chrono_t m_currentTime;
};

//-*****************************************************************************
ParticleSystem::ParticleSystem( const Parameters &params )
  : m_params( params )
  , m_nextId( 1 )
  , m_emitDither( 0.0 )
  , m_currentTime( 0.0 )
{
    // Nothing
}

//-*****************************************************************************
void ParticleSystem::advectExisting( chrono_t dt )
{
    size_t numParticles = m_id.size();
    for ( size_t part = 0; part < numParticles; ++part )
    {
        V3f &velF = m_velocity[part];
        V3f velD( velF.x, velF.y, velF.z );
        velD += m_params.gravity * dt;

        V3f &pos = m_position[part];
        pos += velD * dt;

        if ( pos.y < 0.0 )
        {
            pos.y = -pos.y;
            if ( velD.y < 0.0 )
            {
                velD.y = -velD.y;
                velD *= ( double )m_params.elasticity;
            }
        }

        velF.setValue( velD );

        m_age[part] += ( float )dt;
    }
}

//-*****************************************************************************
void ParticleSystem::destroyOld( chrono_t dt )
{
    // Delete everybody whose age is greater than lifespan.
    // We delete by simply swapping out with the last position.
    size_t numParticles = m_id.size();
    size_t lastParticle = numParticles - 1;
    for ( size_t part = 0; part < numParticles; ++part )
    {
        if ( m_age[part] >= m_params.lifespan )
        {
            std::swap( m_id[part],              m_id[lastParticle] );
            std::swap( m_position[part],        m_position[lastParticle] );
            std::swap( m_color[part],           m_color[lastParticle] );
            std::swap( m_velocity[part],        m_velocity[lastParticle] );
            std::swap( m_age[part],             m_age[lastParticle] );
            --numParticles;
            --lastParticle;
        }
    }

    m_id.resize( numParticles );
    m_position.resize( numParticles );
    m_color.resize( numParticles );
    m_velocity.resize( numParticles );
    m_age.resize( numParticles );
}

//-*****************************************************************************
void ParticleSystem::emitNew( chrono_t dt )
{
    Imath::Rand48 rand48;

    double numNewPartsD = m_emitDither + ( dt * m_params.emitRate );
    size_t numNewParts = ( size_t )floor( numNewPartsD );
    m_emitDither = numNewPartsD - ( double )numNewParts;

    for ( size_t newPart = 0; newPart < numNewParts; ++newPart )
    {
        Alembic::Util::uint64_t newId = m_nextId;
        ++m_nextId;

        rand48.init( ( Alembic::Util::uint64_t )newId );

        // Position
        V3f emitPos = Imath::gaussSphereRand<V3f>( rand48 )
            * ( double ) m_params.emitRadius;

        // Velocity
        V3f emitVel = Imath::gaussSphereRand<V3f>( rand48 )
            * m_params.emitVelocitySpread;
        emitVel += m_params.emitVelocity;

        // Age.
        float emitAge = dt * rand48.nextf();

        // Color.
        C3f emitColor = Imath::gaussSphereRand<C3f>( rand48 )
            * m_params.emitColorSpread;
        emitColor += m_params.emitColor;
        emitColor.x = std::max( emitColor.x, 0.0f );
        emitColor.y = std::max( emitColor.y, 0.0f );
        emitColor.z = std::max( emitColor.z, 0.0f );

        // Do it.
        m_id.push_back( newId );
        m_position.push_back( emitPos );
        m_velocity.push_back( emitVel );
        m_color.push_back( emitColor );
        m_age.push_back( emitAge );
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// PARTICLES WRITER
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
void RunAndWriteParticles
(
    OObject &iParent,
    const ParticleSystem::Parameters &iParams,
    size_t iNumFrames,
    chrono_t iFps
)
{
    // Make the particle system.
    ParticleSystem parts( iParams );

    // Create the time sampling type.
    TimeSampling ts(iFps, 0.0);
    Alembic::Util::uint32_t tsidx = iParent.getArchive().addTimeSampling(ts);

    // Create our object.
    OPoints partsOut( iParent, "simpleParticles", tsidx );
    std::cout << "Created Simple Particles" << std::endl;

    // Add attributes
    OPointsSchema &pSchema = partsOut.getSchema();
    MetaData mdata;
    SetGeometryScope( mdata, kVaryingScope );
    OV3fArrayProperty velOut( pSchema, "velocity", mdata, tsidx );
    OC3fArrayProperty rgbOut( pSchema, "Cs", tsidx );
    OFloatArrayProperty ageOut( pSchema, "age", tsidx );

    // Get seconds per frame.
    chrono_t iSpf = 1.0 / iFps;

    // CJH: Until we fix zero-array-property bug, loop a few frames.
    for ( int preRoll = 0; preRoll < 100; ++preRoll )
    {
        parts.timeStep( iSpf );
        if ( parts.numParticles() > 0 )
        {
            break;
        }
    }

    ABCA_ASSERT( parts.numParticles() > 0,
                 "Degenerate particle system" );

    // Loop over the frames.
    for ( index_t sampIndex = 0;
          sampIndex < ( index_t )iNumFrames; ++sampIndex )
    {
        // First, write the sample.
        OPointsSchema::Sample psamp(
            V3fArraySample( parts.positionVec() ),
            UInt64ArraySample( parts.idVec() ) );
        pSchema.set( psamp );
        velOut.set( V3fArraySample( parts.velocityVec() ) );
        rgbOut.set( C3fArraySample( parts.colorVec() ) );
        ageOut.set( FloatArraySample( parts.ageVec() ) );

        // Now time step.
        parts.timeStep( iSpf );

        // Print!
        std::cout << "Wrote " << parts.numParticles()
                  << " particles to frame: " << sampIndex << std::endl;
    }

    // End it.
    std::cout << "Finished Sim, About to finish writing" << std::endl;
}

//-*****************************************************************************
void ReadParticles( const std::string &iFileName )
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                      iFileName );
    IObject topObj( archive, kTop );

    IPoints points( topObj, "simpleParticles" );
    IPointsSchema& pointsSchema = points.getSchema();

    index_t numSamps = pointsSchema.getNumSamples();
    std::cout << "\n\nReading points back in. Num frames: "
              << numSamps << std::endl;

    IV3fArrayProperty velProp( pointsSchema, "velocity" );
    IC3fArrayProperty rgbProp( pointsSchema, "Cs" );
    IFloatArrayProperty ageProp( pointsSchema, "age" );

    for ( index_t samp = 0; samp < numSamps; ++samp )
    {
        IPointsSchema::Sample psamp;
        pointsSchema.get( psamp, samp );

        Box3f bounds;
        bounds.makeEmpty();
        size_t numPoints = psamp.getPositions()->size();
        for ( size_t p = 0; p < numPoints; ++p )
        {
            bounds.extendBy( (*(psamp.getPositions()))[p] );
        }
        std::cout << "Sample: " << samp << ", numPoints: " << numPoints
                  << ", bounds: " << bounds.min
                  << " to " << bounds.max << std::endl;
    }
}

void pointTestReadWrite()
{
    {
        OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
            "particlesOut2.abc" );
        OObject topObj( archive, kTop );
        OPoints ptsObj(topObj, "somePoints");

        std::vector< V3f > positions;
        std::vector< V3f > velocities;
        std::vector< Alembic::Util::uint64_t > ids;
        std::vector< Alembic::Util::float32_t > widths;
        for (int i = 0; i < 100; ++i)
        {
            OFloatGeomParam::Sample widthSamp;
            widthSamp.setScope(kVertexScope);
            widthSamp.setVals(FloatArraySample(widths));

            OPointsSchema::Sample psamp(V3fArraySample( positions ),
                UInt64ArraySample( ids ), V3fArraySample( velocities ),
                widthSamp );

            ptsObj.getSchema().set(psamp);

            positions.push_back(V3f(i, i, i));
            velocities.push_back(V3f(100.0-i, 0, 0));
            ids.push_back(i*10);
            widths.push_back(0.1 + i * 0.05);
        }
    }

    {
        IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                          "particlesOut2.abc" );

        IObject topObj = archive.getTop();
        IPoints points( topObj, "somePoints" );
        IPointsSchema& pointsSchema = points.getSchema();
        IFloatGeomParam widthProp = pointsSchema.getWidthsParam();
        TESTING_ASSERT( widthProp.getScope() == kVertexScope );
        for ( size_t i = 0; i < 100; ++i )
        {
            IPointsSchema::Sample pointSamp;
            pointsSchema.get(pointSamp, i);

            IFloatGeomParam::Sample widthSamp;
            widthProp.getExpanded(widthSamp, i);
            TESTING_ASSERT( pointSamp.getPositions()->size() == i );
            TESTING_ASSERT( pointSamp.getVelocities()->size() == i );
            TESTING_ASSERT( pointSamp.getIds()->size() == i );
            TESTING_ASSERT( widthSamp.getVals()->size() == i );
            for ( size_t j = 0; j < i; ++j )
            {
                TESTING_ASSERT( (*pointSamp.getPositions())[j] ==
                                V3f(j, j, j) );

                TESTING_ASSERT( (*pointSamp.getVelocities())[j] ==
                                V3f(100-j, 0, 0) );

                TESTING_ASSERT( (*pointSamp.getIds())[j] == j * 10 );

                TESTING_ASSERT( almostEqual( (*widthSamp.getVals())[j],
                                0.1 + j * 0.05, 7 ) );
            }
        }
    }
}

//-*****************************************************************************
void optPropTest()
{
    std::string name = "pointsOptPropTest.abc";
    {
        OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(), name );
        OPoints ptObj( OObject( archive, kTop ), "pts" );
        OPointsSchema &pt= ptObj.getSchema();

        size_t numVerts = 10;

        std::vector<float> widths( numVerts );
        std::vector<V3f> veloc( numVerts );
        std::vector<V3f> verts( numVerts );
        std::vector< Alembic::Util::uint64_t > ids( numVerts );
        OFloatGeomParam::Sample widthSamp;
        widthSamp.setScope(kVertexScope);
        widthSamp.setVals(FloatArraySample(widths));

        for ( size_t i = 0; i < numVerts; ++i )
        {
            ids[i] = i;
            verts[i] = V3f( i, i * 2.0, i * 3.0 );
            veloc[i] = V3f( 0.0, i, 0.0);
            widths[i] = i + 0.1;
        }

        OPointsSchema::Sample samp;
        samp.setPositions( P3fArraySample( verts ) );
        samp.setIds( UInt64ArraySample( ids ) );

        for ( size_t i = 0; i < 2; ++i )
        {
            pt.set( samp );
            for ( size_t j = 0; j < numVerts; ++j )
            {
                verts[j] *= 2;
            }
        }

        samp.setWidths( widthSamp );
        samp.setVelocities( V3fArraySample( veloc ) );
        pt.set( samp );

        samp.setWidths( OFloatGeomParam::Sample() );
        samp.setVelocities( V3fArraySample() );
        pt.set( samp );


        samp.setWidths( widthSamp );
        samp.setVelocities( V3fArraySample( veloc ) );

        for ( size_t i = 0; i < 2; ++i )
        {
            pt.set( samp );
            for ( size_t j = 0; j < numVerts; ++j )
            {
                verts[j] *= 2;
            }
        }

        samp.setVelocities( V3fArraySample() );
        samp.setWidths( OFloatGeomParam::Sample() );
        pt.set( samp );
    }

    {
        IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), name );

        IPoints ptsObj( IObject( archive, kTop ), "pts" );
        IPointsSchema &pts = ptsObj.getSchema();
        TESTING_ASSERT( 7 == pts.getNumSamples() );
        TESTING_ASSERT( 7 == pts.getVelocitiesProperty().getNumSamples() );
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// Particles Test
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
int main( int argc, char *argv[] )
{
    ParticleSystem::Parameters params;
    params.gravity = V3f( 0.0, -9.81, 0.0 );
    params.elasticity = 0.75f;
    params.lifespan = 4.5f;
    params.emitRate = 600.0f;
    params.emitRadius = 0.5f;
    params.emitVelocitySpread = 1.5f;
    params.emitVelocity = V3f( 1.0, 10.0, 2.35 );
    params.emitColorSpread = 0.25f;
    params.emitColor = C3f( 0.85f, 0.9f, 0.1f );

    {
        OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                          "particlesOut1.abc" );
        OObject topObj( archive, kTop );

        RunAndWriteParticles( topObj, params, 20, 1.0/24.0 );
    }

    std::cout << "Wrote particlesOut1.abc" << std::endl;

    ReadParticles( "particlesOut1.abc" );

    pointTestReadWrite();

    optPropTest();

    return 0;
}
