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

#include <AlembicTraitsGeom/AlembicTraitsGeom.h>
#include <ImathRandom.h>

namespace Abc = AlembicAsset;

namespace AlembicTraitsGeom {

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
        Abc::V3d gravity;
        float elasticity;
        float lifespan;
        float emitRate;
        float emitRadius;
        
        float emitVelocitySpread;
        Abc::V3f emitVelocity;
        
        float emitColorSpread;
        Abc::V3f emitColor;
    };
    
    ParticleSystem( const Parameters &params );

    void advectExisting( Abc::seconds_t dt,
                         Abc::seconds_t targetTime );

    void destroyOld( Abc::seconds_t dt,
                     Abc::seconds_t targetTime );

    void emitNew( Abc::seconds_t dt,
                  Abc::seconds_t targetTime );

    void timeStep( Abc::seconds_t dt,
                   Abc::seconds_t targetTime )
    {
        advectExisting( dt, targetTime );
        destroyOld( dt, targetTime );
        emitNew( dt, targetTime );
        m_currentTime = targetTime;
    }

    size_t numParticles() const { return m_id.size(); }
    const Abc::int64_t *idData() const { return &m_id.front(); }
    const Abc::V3d *positionData() const { return &m_position.front(); }
    const Abc::V3f *colorData() const { return &m_color.front(); }
    const Abc::V3f *velocityData() const { return &m_velocity.front(); }
    const float *ageData() const { return &m_age.front(); }

protected:
    Parameters m_params;
    
    std::vector<Abc::int64_t> m_id;
    std::vector<Abc::V3d> m_position;
    std::vector<Abc::V3f> m_color;
    std::vector<Abc::V3f> m_velocity;
    std::vector<float> m_age;
    
    Abc::int64_t m_nextId;
    double m_emitDither;
    Abc::seconds_t m_currentTime;
};

//-*****************************************************************************
ParticleSystem::ParticleSystem( const Parameters &params )
  : m_params( params ),
    m_nextId( 1 ),
    m_emitDither( 0.0 ),
    m_currentTime( 0.0 )
{
    // Nothing
}

//-*****************************************************************************
void ParticleSystem::advectExisting( Abc::seconds_t dt,
                                     Abc::seconds_t targetTime )
{
    size_t numParticles = m_id.size();
    for ( size_t part = 0; part < numParticles; ++part )
    {
        Abc::V3f &velF = m_velocity[part];
        Abc::V3d velD( velF.x, velF.y, velF.z );
        velD += m_params.gravity * dt;

        Abc::V3d &pos = m_position[part];
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
void ParticleSystem::destroyOld( Abc::seconds_t dt,
                                 Abc::seconds_t targetTime )
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
void ParticleSystem::emitNew( Abc::seconds_t dt,
                              Abc::seconds_t targetTime )
{
    Imath::Rand48 rand48;

    double numNewPartsD = m_emitDither + ( dt * m_params.emitRate );
    size_t numNewParts = ( size_t )floor( numNewPartsD );
    m_emitDither = numNewPartsD - ( double )numNewParts;

    for ( size_t newPart = 0; newPart < numNewParts; ++newPart )
    {
        Abc::int64_t newId = m_nextId;
        ++m_nextId;

        rand48.init( ( unsigned long int )newId );

        // Position
        Abc::V3d emitPos = Imath::gaussSphereRand<Abc::V3d>( rand48 )
            * ( double ) m_params.emitRadius;

        // Velocity
        Abc::V3f emitVel = Imath::gaussSphereRand<Abc::V3f>( rand48 )
            * m_params.emitVelocitySpread;
        emitVel += m_params.emitVelocity;

        // Age.
        float emitAge = dt * rand48.nextf();

        // Color.
        Abc::V3f emitColor = Imath::gaussSphereRand<Abc::V3f>( rand48 )
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
    const Abc::OParentObject &parent,
    const ParticleSystem::Parameters &params,
    int startFrame,
    int endFrame
)
{
    // Make the particle system.
    ParticleSystem parts( params );

    // Create our object.
    OSimpleParticles partsOut( parent, "simpleParticles" );
    std::cout << "Created Simple Particles" << std::endl;

    // Add attributes
    Abc::OV3fArrayProperty velOut( partsOut, "perVertex|velocity" );
    Abc::OV3fArrayProperty rgbOut( partsOut, "perVertex|Cs" );
    Abc::OFloatArrayProperty ageOut( partsOut, "perVertex|age" );

    // Animate all the attributes.
    Abc::TimeSamplingInfo tinfo( Abc::kUniformTimeSampling,
                                 (( Abc::seconds_t )startFrame)/24.0,
                                 1.0/24.0 );
    partsOut->bounds().makeAnimated( tinfo );
    partsOut->ids().makeAnimated( tinfo );
    partsOut->positions().makeAnimated( tinfo );
    velOut.makeAnimated( tinfo );
    rgbOut.makeAnimated( tinfo );
    ageOut.makeAnimated( tinfo );

    // Set the rest pose for xform & bounds.
    // We'll set the rest pose for the rest inside the time loop.
    Abc::M44d xform;
    xform.makeIdentity();
    partsOut->xform().set( xform );
    Abc::Box3d bounds;
    bounds.makeEmpty();
    partsOut->bounds().set( bounds );

    for ( int frame = startFrame; frame <= endFrame; ++frame )
    {
        size_t samp = ( size_t )( frame - startFrame );

        if ( samp == 0 )
        {
            // Write rest pose.
            // This tests that we can write NULL multi props, which is important.
            partsOut->ids().set( NULL, 0 );
            partsOut->positions().set( NULL, 0 );
            velOut.set( NULL, 0 );
            rgbOut.set( NULL, 0 );
            ageOut.set( NULL, 0 );
            std::cout << "Wrote rest pose." << std::endl;
        }

        Abc::seconds_t dt = 1.0 / 24.0;
        Abc::seconds_t targetTime = dt * ( Abc::seconds_t )frame;

        parts.timeStep( dt, targetTime );

        size_t numParticles = parts.numParticles();
        if ( numParticles <= 0 )
        {
            partsOut->ids().setAnim( samp, targetTime, NULL, 0 );
            partsOut->positions().setAnim( samp, targetTime, NULL, 0 );
            velOut.setAnim( samp, targetTime, NULL, 0 );
            rgbOut.setAnim( samp, targetTime, NULL, 0 );
            ageOut.setAnim( samp, targetTime, NULL, 0 );
        }
        else
        {
            partsOut->ids().setAnim( samp, targetTime,
                                     parts.idData(), numParticles );
            partsOut->positions().setAnim( samp, targetTime,
                                           parts.positionData(), numParticles );
            velOut.setAnim( samp, targetTime,
                            parts.velocityData(), numParticles );
            rgbOut.setAnim( samp, targetTime,
                            parts.colorData(), numParticles );
            ageOut.setAnim( samp, targetTime,
                            parts.ageData(), numParticles );
        }

        std::cout << "Wrote " << numParticles
                  << " particles to frame: " << frame << std::endl;
    }

    // Close the properties.
    ageOut.close();
    rgbOut.close();
    velOut.close();

    // Close the parts.
    partsOut.close();
    std::cout << "Closed SimpleParticles" << std::endl;
}

} // End namespace AlembicTraitsGeom

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// Particles Test
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
int main( int argc, char *argv[] )
{
    AlembicTraitsGeom::ParticleSystem::Parameters params;
    params.gravity = Abc::V3d( 0.0, -9.81, 0.0 );
    params.elasticity = 0.75f;
    params.lifespan = 4.5f;
    params.emitRate = 6000.0f;
    params.emitRadius = 0.5f;
    params.emitVelocitySpread = 1.5f;
    params.emitVelocity = Abc::V3f( 1.0, 10.0, 2.35 );
    params.emitColorSpread = 0.25f;
    params.emitColor = Abc::V3f( 0.85f, 0.9f, 0.1f );
    int startFrame = 1;
    int endFrame = 250;

#ifdef DEBUG
    Abc::Init( true, false );
#else
    Abc::Init();
#endif
    
    Abc::OAsset asset( "particlesOut1.abc", Abc::kThrowException );
    RunAndWriteParticles( asset, params, startFrame, endFrame );
    asset.close();
    std::cout << "Closed asset particlesOut1.abc" << std::endl;
    return 0;
}
