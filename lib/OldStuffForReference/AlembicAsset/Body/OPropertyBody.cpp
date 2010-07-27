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

#include <AlembicAsset/Body/OPropertyBody.h>
#include <AlembicAsset/Body/ODataBody.h>
#include <AlembicAsset/Body/OSlabBody.h>
#include <AlembicAsset/Body/OTimeSamplingBody.h>
#include <string.h>

namespace AlembicAsset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// BASE OPROPERTY
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
OPropertyBody::OPropertyBody( OObjectBody &parentObject,
                              const std::string &nme,
                              const std::string &prot,
                              const DataType &dtype )
  : m_context( parentObject.context() ),
    m_name( nme ),
    m_protocol( prot ),
    m_dataType( dtype ),
    m_restSampleWritten( false ),
    m_timeSamplingInfo( kUnknownTimeSamplingType )
{
    // std::cout << "\n\nOPropertyBody::ctor, name = "
    //          << m_name  << ", restSampleWritten = "
    //          << m_restSampleWritten << std::endl;
    
    // Create the property group.
    AlembicHDF5::CreationOrderPlist copl;
    m_group.create( parentObject.propertyGroup(), m_name,
                    H5P_DEFAULT,
                    copl );
    AAH5_ASSERT( m_group.valid(),
                 "ERROR: Trying to create OPropertyBody. Name = "
                 << m_name
                 << " protocol = " << m_protocol
                 << " dataType = " << m_dataType );

    
    // Write the protocol
    WriteString( m_group, "protocol", m_protocol );
    
    // Write the data type as an attribute.
    WriteDataTypeAsAttrs( m_group,
                          "dataTypePOD", "dataTypeExtent",
                          m_dataType );

    // Get the data type tuple
    m_dataTypeTuple = m_context->dataTypeTupleMap().find( m_dataType );

    // The rest of the data is written along the way, or when we finish.
}

//-*****************************************************************************
OPropertyBody::~OPropertyBody() throw()
{
    // CJH - group will close itself on destruction. This could
    // be happening because of an exception somewhere, so we can't
    // know for sure that anything bad is going on internally.
    // Everything we have will clean itself up.
    // Still, call close if we're valid.
    // This should probably never happen because it will happen
    // in derived classes.
    if ( m_group.valid() )
    {
        try
        {
            this->close();
            assert( !m_group.valid() );
        }
        catch ( std::exception &e )
        {
            std::cerr << "ERROR: OPropertyBody::~OPropertyBody() "
                      << "EXCEPTION: " << e.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr << "ERROR: OPropertyBody::~OPropertyBody() "
                      << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
void OPropertyBody::setRestSample( const void *data,
                                   const Dimensions &dims )
{
    // std::cout << "\n\nOPropertyBody::setRestSample(): "
    //           << "Name = " << m_name << std::endl;
    
    AAH5_ASSERT( !m_restSampleWritten,
                 "OPropertyBody::setRestSample() Illegal to call twice"
                 << std::endl
                 << "Name: " << m_name << std::endl
                 << "Protocol: " << m_protocol << std::endl );
    
    internalSetRestSample( data, dims );

    m_restSampleWritten = true;

    // std::cout << "OPropertyBOdy::setRestSample() restSampleWritten set true"
    //          << std::endl;
}

//-*****************************************************************************
void OPropertyBody::makeAnimated( const TimeSamplingInfo &tinfo )
{
    // It is an error to make something animated more than once.
    AAH5_ASSERT( m_timeSamplingInfo.type == kUnknownTimeSamplingType &&
                 m_sampleTimes.size() == 0,
                 "OPropertyBody::makeAnimated() Illegal to call twice."
                 << std::endl
                 << "Name: " << m_name << std::endl
                 << "Protocol: " << m_protocol << std::endl );

    // Set it.
    m_timeSamplingInfo = tinfo;

    // Lay out the animation stuff.
    m_sampleTimes.resize( 0 );
    if ( m_timeSamplingInfo.numSamplesHint > 0 )
    {
        m_sampleTimes.reserve( m_timeSamplingInfo.numSamplesHint );
    }

    // That's it.
}

//-*****************************************************************************
void OPropertyBody::setAnimSample( size_t samp,
                                   seconds_t seconds,
                                   const void *data,
                                   const Dimensions &dims )
{
    //std::cout << "Setting anim sample: " << samp
    //          << " to seconds: " << seconds
    //          << " and data: " << (( const float * )data)[0]
    //          << std::endl;
    
    // It is an error to set anim samples without making animated.
    AAH5_ASSERT( isAnimated(),
                 "OPropertyBody::setAnimSample() Must call makeAnimated first."
                 << std::endl
                 << "Name: " << m_name << std::endl
                 << "Protocol: " << m_protocol << std::endl );

    // Make sure sample times are sufficient.
    if ( samp >= m_sampleTimes.size() )
    {
        // Resize, set unmarked samples to non value.
        size_t oldSize = m_sampleTimes.size();
        size_t newSize = samp+1;
        m_sampleTimes.resize( newSize );
        for ( size_t s = oldSize; s < newSize; ++s )
        {
            m_sampleTimes[s] = Time::kRestTimeValue();
        }
    }
    assert( m_sampleTimes.size() > samp );
    
    AAH5_ASSERT( m_sampleTimes[samp] == Time::kRestTimeValue(),
                 "OPropertyBody::setAnimSample() Cannot set same sample twice."
                 << std::endl
                 << "Name: " << m_name << std::endl
                 << "Protocol: " << m_protocol << std::endl
                 << "Sample: " << samp << std::endl );
    m_sampleTimes[samp] = seconds;

    internalSetAnimSample( samp, data, dims );
}

//-*****************************************************************************
void OPropertyBody::close()
{   
    // This would most likely occur if close was called multiple times.
    // This is an error. 
    AAH5_ASSERT( m_group.valid(),
                 "OPropertyBody::close() ERROR: Invalid group." << std::endl
                 << "Name: " << m_name << std::endl
                 << "Protocol: " << m_protocol << std::endl
                 << "This: " << ( void * )this << std::endl
                 << "Group id: " << m_group.id() << std::endl
                 << "Group name: " << m_group.name() << std::endl );

    // We do want to be able to recover from this.
    if ( !m_restSampleWritten )
    {
        std::cerr << "OPropertyBody::close() WARNING: No rest sample written."
                  << std::endl
                  << "Name: " << m_name << std::endl
                  << "Protocol: " << m_protocol << std::endl;
        internalSetDefaultRestSample();
        m_restSampleWritten = true;
    }

    // Now check to see whether we're REALLY animated.
    if ( internalCheckAnimVariation() )
    {
        // Okay, the biggie. Are we animated at all?
        if ( isAnimated() && m_sampleTimes.size() > 0 )
        {
            // First, write the time sampling.
            WriteTimeSampling( *m_context, m_group, m_timeSamplingInfo,
                               m_sampleTimes );
        }
    }

    // Call the INTERNAL close.
    internalClose();

    // Actually close the group.
    m_group.close();

    // std::cout << "Successfully closed property: "
    //          << "Name: " << m_name << std::endl
    //          << "Protocol: " << m_protocol << std::endl
    //          << "This: " << ( void * )this << std::endl
    //          << "Group id: " << m_group.id() << std::endl
    //          << "Group name: " << m_group.name() << std::endl
    //          << "About to leave OPropertyBody::close() " << std::endl
    //          << std::endl << std::flush;
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// SINGULAR
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
OSingularPropertyBody::OSingularPropertyBody( OObjectBody &parentObject,
                                              const std::string &nme,
                                              const std::string &prot,
                                              const DataType &dtype )
  : OPropertyBody( parentObject, nme, prot, dtype ),
    m_restSampleBytes( dtype.bytes() ),
    m_animVariation( false )
{
    std::fill( m_restSampleBytes.begin(), m_restSampleBytes.end(),
               ( unsigned char )0 );
    
    // Write the property type.
    WriteEnumChar( m_group, "propertyType", ( char )kSingularProperty );
}

//-*****************************************************************************
OSingularPropertyBody::~OSingularPropertyBody() throw()
{
    if ( m_group.valid() )
    {
        try
        {
            this->close();
            assert( !m_group.valid() );
        }
        catch ( std::exception &e )
        {
            std::cerr << "OSingularPropertyBody::~OSingularPropertyBody() "
                      << "EXCEPTION:  " << e.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr << "OSingularPropertyBody::~OSingularPropertyBody() "
                      << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
PropertyType OSingularPropertyBody::propertyType() const throw()
{
    return kSingularProperty;
}

//-*****************************************************************************
// Set the rest value, passed by pointer.
void OSingularPropertyBody::internalSetRestSample( const void *rv,
                                                   const Dimensions &dims )
{
    AAH5_ASSERT( dims.rank() == 0,
                 "Singular properties must be scalar" );
    
    WriteScalar( m_group,
                 "singularRestSample",
                 m_dataTypeTuple.nativeH5T(),
                 m_dataTypeTuple.fileH5T(),
                 ( const void * )rv );

    std::copy( ( const unsigned char * )rv,
               m_dataType.bytes() + ( const unsigned char * )rv,
               ( unsigned char * )&m_restSampleBytes.front() );
}

//-*****************************************************************************
void OSingularPropertyBody::internalSetDefaultRestSample()
{
    size_t numBytes = m_dataType.bytes();
    Bytes bytes( numBytes );
    m_dataType.setDefaultBytes( ( void * )&bytes.front() );
    internalSetRestSample( ( void * )&bytes.front(),
                           Dimensions() );
}

//-*****************************************************************************
void OSingularPropertyBody::internalSetAnimSample( size_t samp,
                                                   const void *data,
                                                   const Dimensions &dims )
{
    AAH5_ASSERT( dims.rank() == 0,
                 "Singular properties must be scalar" );

    const size_t dtb = m_dataType.bytes();
    assert( dtb > 0 );

    // Make sure anim slab is sufficently sized.
    if ( samp >= m_animValuesSlab.size() )
    {
        // Resize, set unmarked samples to default value.
        size_t oldSize = m_animValuesSlab.size();
        size_t newSize = samp+1;
        m_animValuesSlab.reset( m_dataType, newSize );
        char *thisAnimSample =
            ( ( char * )m_animValuesSlab.rawData()
              + ( oldSize*dtb ) );
        for ( size_t s = oldSize; s < newSize; ++s,
                  thisAnimSample += dtb )
        {
            m_dataType.setDefaultBytes( ( void * )thisAnimSample );
        }
    }
    assert( m_animValuesSlab.size() > samp );
    assert( m_sampleTimes.size() == m_animValuesSlab.size() );

    // Anim slab is filled appropriately. just set the bytes
    std::copy( ( const unsigned char * )data,
               dtb + ( const unsigned char * )data,
               dtb*samp + ( unsigned char * )( m_animValuesSlab.rawData() ) );

    // Do the rest-sample comparison.
    if ( !m_restSampleWritten )
    {
        m_animVariation = true;
    }
    else
    {
        assert( m_restSampleBytes.size() == dtb );
        bool diff = false;
        for ( size_t b = 0; b < dtb; ++b )
        {
            if ( m_restSampleBytes[b] !=
                 (( const unsigned char * )data)[b] )
            {
                diff = true;
                break;
            }
        }
        if ( diff )
        {
            m_animVariation = true;
        }
    }
}

//-*****************************************************************************
bool OSingularPropertyBody::internalCheckAnimVariation()
{
    if ( !isAnimated() || m_animValuesSlab.size() == 0 )
    {
        return false;
    }

    if ( m_animVariation )
    {
        return true;
    }

    // Loop over all the values and see if they compare.
    size_t numBytesPerDatum = m_dataType.bytes();
    const unsigned char *dataBytes =
        ( const unsigned char * )m_animValuesSlab.rawData();
    size_t numElements = m_animValuesSlab.numElements();
    for ( size_t elem = 0; elem < numElements; ++elem )
    {
        const unsigned char *sampBuf = dataBytes + elem*numBytesPerDatum;
        for ( size_t b = 0; b < numBytesPerDatum; ++b )
        {
            if ( m_restSampleBytes[b] != sampBuf[b] )
            {
                return true;
            }
        }
    }
    
    return false;
}

//-*****************************************************************************
void OSingularPropertyBody::internalClose()
{
    // If we're animated, we have a slab of anim values to write.
    if ( isAnimated() && m_sampleTimes.size() > 0 )
    {
        if ( !m_animVariation )
        {
            // Nothing needs doing.
            return;
        }

        AAH5_ASSERT( m_animValuesSlab.size() == m_sampleTimes.size(),
                     "OSingularPropertyBody::internalClose() "
                     << "ERROR: Invalid AnimValuesSlab."
                     << std::endl
                     << "Name: " << m_name << std::endl
                     << "Protocol: " << m_protocol << std::endl );
        
        // Fill in the unwritten values with the rest.
        const size_t dtb = m_dataType.bytes();
        const uint8_t *restSampBytes =
            ( const uint8_t * )&m_restSampleBytes.front();
        uint8_t *sampSlabBytes = ( uint8_t * )m_animValuesSlab.rawData();
        size_t numSamps = m_sampleTimes.size();
        for ( size_t s = 0; s < numSamps; ++s )
        {
            if ( m_sampleTimes[s] == Time::kRestTimeValue() )
            {
                // This means this sample wasn't written.
                std::cerr << "OSingularPropertyBody::internalClose() "
                          << "WARNING: " << std::endl
                          << "Sample #: " << s
                          << " was not written. Using default."
                          << std::endl
                          << "Name: " << m_name << std::endl
                          << "Protocol: " << m_protocol
                          << std::endl;
                
                std::copy( restSampBytes,
                           restSampBytes + dtb,
                           sampSlabBytes + s*dtb );
            }
        }
        
        // Write the slab...
        // std::cout << "I AM WRITING ANIMATION SLAB FOR: "
        //           << m_name << std::endl;
        OSlabMapBody::SlabReference slabID =
            WriteSlab( *m_context, m_group, "singularAnimSamples",
                       m_animValuesSlab );
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MULTI PROPERTY
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
OMultiPropertyBody::OMultiPropertyBody( OObjectBody &parentObject,
                                        const std::string &nme,
                                        const std::string &prot,
                                        const DataType &dtype )
  : OPropertyBody( parentObject, nme, prot, dtype ),
    m_animVariation( false )
{
    // Write the property type.
    WriteEnumChar( m_group, "propertyType", ( char )kMultiProperty );
}

//-*****************************************************************************
OMultiPropertyBody::~OMultiPropertyBody() throw()
{
    if ( m_group.valid() )
    {
        try
        {
            this->close();
            assert( !m_group.valid() );
            assert( !m_animSampsGroup.valid() );
        }
        catch ( std::exception &e )
        {
            std::cerr << "ERROR: OMultiPropertyBody::~OMultiPropertyBody() "
                      << "EXCEPTION: " << e.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr << "ERROR: OMultiPropertyBody::~OMultiPropertyBody() "
                      << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
PropertyType OMultiPropertyBody::propertyType() const throw()
{
    return kMultiProperty;
}

//-*****************************************************************************
// Set the rest value, passed by pointer.
void OMultiPropertyBody::internalSetRestSample( const void *rv,
                                                const Dimensions &dims )
{
    // Write the sample!
    m_restSlabID = CalculateHashID( m_dataType, dims, rv );
    m_restSlabRef =
        WriteSlab( *m_context,
                   m_group,
                   "multiRestSample",
                   rv, m_dataType, dims,
                   m_restSlabID );
}

//-*****************************************************************************
// Set default rest value.
// It is totally valid to have an MultiProperty with dimensions of size 0.
void OMultiPropertyBody::internalSetDefaultRestSample()
{
    Dimensions dims( 1 );
    dims[0] = 0;
    internalSetRestSample( ( void * )NULL, dims );
}

//-*****************************************************************************
void OMultiPropertyBody::internalSetAnimSample( size_t samp,
                                                const void *data,
                                                const Dimensions &dims )
{
    // Make sure anim slab is sufficiently sized.
    if ( samp >= m_animSlabIDs.size() )
    {
        size_t oldSize = m_animSlabIDs.size();
        size_t newSize = samp+1;
        m_animSlabIDs.resize( newSize );
    }
    assert( m_animSlabIDs.size() > samp );
    assert( m_sampleTimes.size() == m_animSlabIDs.size() );
    
    // Now we need to check to see whether or not we have animation
    // variations.
    Slab::HashID animSlabID = CalculateHashID( m_dataType, dims, data );
    m_animSlabIDs[samp] = animSlabID;
    
    bool writeThisSlab = false;
    bool writeOtherSlabs = false;
    if ( !m_animVariation )
    {
        if ( !m_restSampleWritten )
        {
            // If we've written an anim sample before writing a rest sample,
            // we disable the ability to check for anim variation. that's
            // reasonable.
            m_animVariation = true;

            // If this happens, we write the slab.
            // However, no other slabs will have been written
            // because if they had been, animVariation would already be
            // true.
            writeThisSlab = true;
            writeOtherSlabs = false;
        }
        else
        {
            // Okay, rest sample has been written. If our slabID
            // is exactly the same as the rest sample, no animation
            // variation has occurred.
            if ( animSlabID == m_restSlabID )
            {   
                // Nothing needs to be written, for now.
                m_animVariation = false;
                return;
            }
            else
            {
                // Oh dear. We are not the same as the rest slab. This
                // means there IS variation. But, there wasn't anim
                // variation before. 
                m_animVariation = true;
                writeThisSlab = true;
                writeOtherSlabs = true;
            }
        }
    }
    else
    {
        m_animVariation = true;
        writeThisSlab = true;
        writeOtherSlabs = false;
    }

    // If we get this far, we're writing at least something.
    m_animVariation = true;
    
    // Create a group to hold the time samples.
    if ( !m_animSampsGroup.valid() )
    {
        m_animSampsGroup.create( m_group, "animSamples" );
    }

    // Write this slab.
    if ( writeThisSlab )
    {
        // Many checks have gone before this which would have failed
        // so we can just plow ahead with this.
        // Write the slab
        WriteSlab( *m_context, m_animSampsGroup,
                   ( boost::format( "slab_samp_%012d" ) % ( int )samp ).str(),
                   data, m_dataType, dims );
    }

    // The other slabs that need to be written are all equal to the rest slab.
    if ( writeOtherSlabs )
    {
        assert( m_restSampleWritten );
        assert( m_animVariation );
        assert( ( bool )m_restSlabRef );
        size_t numSlabs = m_sampleTimes.size();
        for ( size_t slab = 0; slab < numSlabs; ++slab )
        {
            // The anim slabs we need to write are the ones that
            // have a valid time value and are NOT us.
            if ( slab != samp &&
                 m_sampleTimes[slab] != Time::kRestTimeValue() )
            {
                // This is a time sample that got written, but no
                // slab was written because we were waiting to see whether
                // there would be no animation variation. Turns out there
                // IS animation variation, so we need to fill in all those
                // slabs that didn't get written. We know that they
                // were the same as the restSample, which was written.
                WriteSlabFromRef( *m_context, m_animSampsGroup,
                                  ( boost::format( "slab_samp_%012d" )
                                    % ( int )slab ).str(),
                                  m_restSlabRef );
            }
        }
    }
}

//-*****************************************************************************
bool OMultiPropertyBody::internalCheckAnimVariation()
{
    if ( !isAnimated() )
    {
        return false;
    }

    if ( m_animVariation || !m_restSampleWritten )
    {
        return true;
    }

    // if we get here, there is animation,
    // animVariation is false, and rest sample was written. this means
    return false;
}

//-*****************************************************************************
void OMultiPropertyBody::internalClose()
{
    // If we're animated, our slabs have been written one at a time
    // into the anim samples group. If we missed some samples, though,
    // we need to make sure that something is there for file integrity.
    if ( isAnimated() && m_sampleTimes.size() > 0 )
    {
        // At this point, the rest sample is written.
        assert( m_restSampleWritten && ( bool )m_restSlabRef );
        
        if ( !m_animVariation )
        {
            assert( !m_animSampsGroup.valid() );
            
            // Nothing needs doing!
            return;
        }

        // This should be.
        AAH5_ASSERT( m_animSlabIDs.size() == m_sampleTimes.size(),
                     "OMultiPropertyBody::internalClose() "
                     << "ERROR: Invalid AnimSlabIDs."
                     << std::endl
                     << "Name: " << m_name << std::endl
                     << "Protocol: " << m_protocol << std::endl );
        
        AAH5_ASSERT( m_animSampsGroup.valid(),
                     "OMultiPropertyBody::internalClose() "
                     << "ERROR: Invalid AnimSampsGroup."
                     << std::endl
                     << "Name: " << m_name << std::endl
                     << "Protocol: " << m_protocol << std::endl );
        
        // Loop over seconds, and for every one that is not written,
        // make a hard link to the default property.
        size_t numSamps = m_sampleTimes.size();
        for ( size_t s = 0; s < numSamps; ++s )
        {
            if ( m_sampleTimes[s] == Time::kRestTimeValue() )
            {
                m_animSlabIDs[s] = m_restSlabID;
                
                // This means this sample wasn't written.
                std::cerr << "OMultiPropertyBody::internalClose() "
                          << "WARNING: " << std::endl
                          << "Sample #: " << s
                          << " was not written. Using default."
                          << std::endl
                          << "Name: " << m_name << std::endl
                          << "Protocol: " << m_protocol
                          << std::endl;
                WriteSlabFromRef( *m_context, m_animSampsGroup,
                                  ( boost::format( "slab_samp_%012d" )
                                    % ( int )s ).str(),
                                  m_restSlabRef );
            }
        }

        // Write the slab...
        {
            Dimensions dims( 1 );
            dims[0] = m_animSlabIDs.size();
            WriteSlab( *m_context, m_group, "multiAnimSlabIDs",
                       ( const void * )&m_animSlabIDs.front(),
                       DataType( kUint8POD, 16 ),
                       dims );
        }

        m_animSampsGroup.close();
    }
}

} // End namespace AlembicAsset

