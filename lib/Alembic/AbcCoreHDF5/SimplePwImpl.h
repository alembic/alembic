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

#ifndef _Alembic_AbcCoreHDF5_SimplePwImpl_h_
#define _Alembic_AbcCoreHDF5_SimplePwImpl_h_

#include <Alembic/AbcCoreHDF5/Foundation.h>
#include <Alembic/AbcCoreHDF5/WriteUtil.h>
#include <Alembic/AbcCoreHDF5/DataTypeRegistry.h>
#include <Alembic/AbcCoreHDF5/HDF5Util.h>

namespace Alembic {
namespace AbcCoreHDF5 {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
// This templated base class implements the common logic behind both the
// scalar and array property writers. The only way these two writers differ
// is the type of sample they write and the way those samples are stored
// and copied.  Scalar samples are simply copied by value, compared by value,
// and stored by value. Array samples are assumed to be "big", and are
// first condensed into a "key" (using a checksum), which is compared
// to other "keys" of previously written samples.  However, the primary logic
// of when to compare, when to copy, when to write, and how to manage time
// values is common across both classes, and represents the primary complexity.
// To minimize redundancy, errors, and code size, we create a common base class.
//
// It is assumed that the IMPL class will provide a mechanism for constructing
// a KEY from a SAMPLE.
// 
// The IMPL class is assumed to have the following functions:
// KEY  computeSampleKey( SAMPLE iSamp ) const;
// bool sameAsPreviousSample( SAMPLE iSamp, const KEY &iKey ) const;
// void copyPreviousSample( index_t iSampleIndex );
// void writeSample( index_t iSampleIndex, SAMPLE iSamp, const KEY &iKey );
//
//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE, class KEY>
class SimplePwImpl : public ABSTRACT
{
protected:
    SimplePwImpl( AbcA::CompoundPropertyWriterPtr iParent,
                  hid_t iParentGroup,
                  PropertyHeaderPtr iHeader );

public:
    virtual ~SimplePwImpl();
    
    //-*************************************************************************
    // FROM ABSTRACT
    //-*************************************************************************
    
    virtual const AbcA::PropertyHeader & getHeader() const;

    virtual AbcA::ObjectWriterPtr getObject();

    virtual AbcA::CompoundPropertyWriterPtr getParent();

private:
    hid_t getSampleIGroup();

public:
    // Scalar/Array API
    virtual void setSample( index_t iSampleIndex,
                            chrono_t iSampleTime,
                            SAMPLE iSamp );

    virtual void setFromPreviousSample( index_t iSampleIndex,
                                        chrono_t iSampleTime );
    
    virtual size_t getNumSamples();

protected:
    // The parent compound property writer.
    AbcA::CompoundPropertyWriterPtr m_parent;
    
    // The parent group. We need to keep this around because we
    // don't create our group until we need to. This is guaranteed to
    // exist because our parent (or object) is guaranteed to exist.
    hid_t m_parentGroup;

    // The header which defines this property.
    PropertyHeaderPtr m_header;

    // The DataTypes for this property.
    hid_t m_fileDataType;
    bool m_cleanFileDataType;
    hid_t m_nativeDataType;
    bool m_cleanNativeDataType;

    // The group corresponding to this property.
    // It may never be created or written.
    hid_t m_sampleIGroup;
    
    // The time samples. The number of these will be determined by
    // the TimeSamplingType
    std::vector<chrono_t> m_timeSamples;

    // Index of the next sample to write
    uint32_t m_nextSampleIndex;

    // Number of unique samples.
    // If this is zero, it means we haven't written a sample yet.
    // Otherwise, it is the number of samples we've actually written.
    // It is common for the tail end of sampling blocks to be repeated
    // values, so we don't bother writing them out if the tail is
    // non-varying.
    uint32_t m_numUniqueSamples;
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// IMPLEMENTATION
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE, class KEY>
SimplePwImpl<ABSTRACT,IMPL,SAMPLE,KEY>::SimplePwImpl
(
    AbcA::CompoundPropertyWriterPtr iParent,
    hid_t iParentGrp,
    PropertyHeaderPtr iHeader
)
  : m_parent( iParent )
  , m_parentGroup( iParentGrp )
  , m_header( iHeader )
  , m_fileDataType( -1 )
  , m_cleanFileDataType( false )
  , m_nativeDataType( -1 )
  , m_cleanNativeDataType( false )
  , m_sampleIGroup( -1 )
  , m_nextSampleIndex( 0 )
  , m_numUniqueSamples( 0 )
{
    // Check the validity of all inputs.
    ABCA_ASSERT( m_parent, "Invalid parent" );
    ABCA_ASSERT( m_header, "Invalid property header" );
    ABCA_ASSERT( m_parentGroup >= 0, "Invalid parent group" );
    ABCA_ASSERT( m_header->getDataType().getExtent() > 0,
        "Invalid DatatType extent");

    // Get data types
    PlainOldDataType POD = m_header->getDataType().getPod();
    if ( POD != kStringPOD && POD != kWstringPOD )
    {
        m_fileDataType = GetFileH5T( m_header->getDataType(),
                                     m_cleanFileDataType );
        m_nativeDataType = GetNativeH5T( m_header->getDataType(),
                                         m_cleanNativeDataType );
        
        ABCA_ASSERT( m_fileDataType >= 0, "Couldn't get file datatype" );
        ABCA_ASSERT( m_nativeDataType >= 0, "Couldn't get native datatype" );
    }

    // Write the property header.
    // We don't write the time info yet, because there's
    // an optimization that the abstract API allows us to make which has a
    // surprisingly significant effect on file size. This optimization
    // is that when a "constant" property is written - that is, when
    // all the samples are identical, we can skip the writing of the
    // time sampling type and time samples and treat the sample as though
    // it were written with identity time sampling.
    // Since we can't know this until the end, we don't write time
    // sampling yet.
    WritePropertyHeaderExceptTime( m_parentGroup,
                                   m_header->getName(), *m_header );
}

//-*****************************************************************************
// Destructor is at the end, so that this file has a logical ordering that
// matches the order of operations (create, set samples, destroy)
//-*****************************************************************************

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE, class KEY>
const AbcA::PropertyHeader &
SimplePwImpl<ABSTRACT,IMPL,SAMPLE,KEY>::getHeader() const
{
    ABCA_ASSERT( m_header, "Invalid header" );
    return *m_header;
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE, class KEY>
AbcA::ObjectWriterPtr
SimplePwImpl<ABSTRACT,IMPL,SAMPLE,KEY>::getObject()
{
    ABCA_ASSERT( m_parent, "Invalid parent" );
    return m_parent->getObject();
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE, class KEY>
AbcA::CompoundPropertyWriterPtr
SimplePwImpl<ABSTRACT,IMPL,SAMPLE,KEY>::getParent()
{
    ABCA_ASSERT( m_parent, "Invalid parent" );
    return m_parent;
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE, class KEY>
hid_t SimplePwImpl<ABSTRACT,IMPL,SAMPLE,KEY>::getSampleIGroup()
{
    if ( m_sampleIGroup >= 0 )
    {
        return m_sampleIGroup;
    }

    ABCA_ASSERT( m_parentGroup >= 0, "invalid parent group" );
    ABCA_ASSERT( m_numUniqueSamples > 0,
                 "can't create sampleI group before numSamples > 1" );

    const std::string groupName = m_header->getName() + ".smpi";
    
    hid_t copl = CreationOrderPlist();
    PlistCloser plistCloser( copl );
    
    m_sampleIGroup = H5Gcreate2( m_parentGroup,
                                 groupName.c_str(),
                                 H5P_DEFAULT,
                                 copl,
                                 H5P_DEFAULT );
    ABCA_ASSERT( m_sampleIGroup >= 0,
                 "Could not create simple samples group named: "
                 << groupName );
    
    return m_sampleIGroup;
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE, class KEY>
void SimplePwImpl<ABSTRACT,IMPL,SAMPLE,KEY>::setSample
(
    index_t iSampleIndex,
    chrono_t iSampleTime,
    SAMPLE iSamp )
{
    // Check errors.
    ABCA_ASSERT( iSampleIndex == m_nextSampleIndex,
                 "Out-of-order sample writing. Expecting: "
                 << m_nextSampleIndex
                 << ", but got: " << iSampleIndex );

    bool pushTimeSamples = false;

    // decide whether we should push back the sample time later on
    // don't do it now because there are a few more checks that need to pass
    if ( m_timeSamples.size() <
         m_header->getTimeSamplingType().getNumSamplesPerCycle() )
    {
        ABCA_ASSERT(m_timeSamples.empty() || iSampleTime > m_timeSamples.back(),
            "Out-of-order time writing. Last time: " << m_timeSamples.back() <<
            " is greater than or equal to: " << iSampleTime);

        // if we are cyclic sampling make sure the difference between the our
        // first sample time and the one we want to push does not exceed the
        // time per cycle.  Doing so would violate the strictly increasing
        // rule we have for our samples
        ABCA_ASSERT(!m_header->getTimeSamplingType().isCyclic() ||
            m_timeSamples.empty() || iSampleTime - m_timeSamples.front() <
            m_header->getTimeSamplingType().getTimePerCycle(),
            "Out-of-order cyclic time writing.  This time: " << iSampleTime <<
            " minus first time: " << m_timeSamples.front() <<
            " is greater than or equal to: " <<
            m_header->getTimeSamplingType().getTimePerCycle());

        pushTimeSamples = true;
    }

    // Get my name
    const std::string &myName = m_header->getName();
    
    // Figure out if we need to write the sample. At first, no.
    bool needToWriteSample = false;

    // The Key helps us analyze the sample.
    KEY key = static_cast<IMPL*>(this)->computeSampleKey( iSamp );
    if ( m_numUniqueSamples == 0 )
    {
        // This means we are now writing the very first sample.
        assert( iSampleIndex == 0 );
        needToWriteSample = true;
    }
    else
    {
        // Check to see if there have been any changes.
        // Only if they're different do we bother.
        // We use the Key to check.
        // We also rely on the IMPL template parameter, which is
        // a variant of the CRTP
        assert( m_numUniqueSamples > 0 );
        needToWriteSample =
            !( static_cast<IMPL*>(this)->sameAsPreviousSample( iSamp, key ) );
    }

    // If we need to write sample, write sample!
    if ( needToWriteSample )
    {   
        // Write all the samples, starting from the last unique sample,
        // up to this sample.

        // This is tricky. If we get in here, and we have to write,
        // we want to write all the previously-thought-to-be-constant
        // samples. If we've already written them, then m_numUniqueSamples
        // will equal iSampleIndex-1, and this loop will skip.
        // If we haven't written them, this loop will do the right thing.
        // If this is the FIRST sample, iSampleIndex is zero, and this
        // will fail.
        // It's just tricky.
        for ( index_t smpI = m_numUniqueSamples;
              smpI < iSampleIndex; ++smpI )
        {
            assert( smpI > 0 );
            static_cast<IMPL*>(this)->copyPreviousSample(
                getSampleIGroup(),
                getSampleName( myName, smpI ),
                smpI );
        }

        // Write this sample, which will update its internal
        // cache of what the previously written sample was.
        static_cast<IMPL*>(this)->writeSample(
            iSampleIndex == 0 ? m_parentGroup : getSampleIGroup(),
            getSampleName( myName, iSampleIndex ),
            iSampleIndex, iSamp, key );
        
        // Time sample written, all is well.
        m_numUniqueSamples = iSampleIndex+1;
    }

    if (pushTimeSamples)
    {
        m_timeSamples.push_back( iSampleTime );
    }

    // Set the previous sample index.
    m_nextSampleIndex = iSampleIndex+1;
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE, class KEY>
void SimplePwImpl<ABSTRACT,IMPL,SAMPLE,KEY>::setFromPreviousSample
(
    index_t iSampleIndex,
    chrono_t iSampleTime
    )
{
    // Various programmer error checks
    // Check errors.
    ABCA_ASSERT( iSampleIndex == m_nextSampleIndex,
                 "Out-of-order sample writing. Expecting: "
                 << m_nextSampleIndex
                 << ", but got: " << iSampleIndex );

    // Verify indices
    if ( m_nextSampleIndex < 1 || m_numUniqueSamples < 1 )
    {
        ABCA_THROW( "Cannot set from previous sample before any "
                     "samples have been written" );
    }

    // Push back the sample time.
    if ( m_timeSamples.size() <
         m_header->getTimeSamplingType().getNumSamplesPerCycle() )
    {
        ABCA_ASSERT(m_timeSamples.empty() || iSampleTime > m_timeSamples.back(),
            "Out-of-order time writing. Last time: " << m_timeSamples.back() <<
            " is greater than or equal to: " << iSampleTime);

        // if we are cyclic sampling make sure the difference between the our
        // first sample time and the one we want to push does not exceed the
        // time per cycle.  Doing so would violate the strictly increasing
        // rule we have for our samples
        ABCA_ASSERT(!m_header->getTimeSamplingType().isCyclic() ||
            m_timeSamples.empty() || iSampleTime - m_timeSamples.front() <
            m_header->getTimeSamplingType().getTimePerCycle(),
            "Out-of-order cyclic time writing.  This time: " << iSampleTime <<
            " minus first time: " << m_timeSamples.front() <<
            " is greater than or equal to: " <<
            m_header->getTimeSamplingType().getTimePerCycle());

        m_timeSamples.push_back( iSampleTime );
    }

    // Just increase the previous sample index without increasing
    // the number of unique samples.
    m_nextSampleIndex = iSampleIndex + 1;
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE, class KEY>
size_t SimplePwImpl<ABSTRACT,IMPL,SAMPLE,KEY>::getNumSamples()
{
    return ( size_t )m_nextSampleIndex;
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE, class KEY>
SimplePwImpl<ABSTRACT,IMPL,SAMPLE,KEY>::~SimplePwImpl()
{
    // Wrap the whole thing in a try block, so as to prevent
    // exceptions from being thrown out of a destructor.
    try
    {
        if ( m_fileDataType >= 0 && m_cleanFileDataType )
        { H5Tclose( m_fileDataType ); }
        if ( m_nativeDataType >= 0 && m_cleanNativeDataType )
        { H5Tclose( m_nativeDataType ); }
        
        const std::string &myName = m_header->getName();
        
        // Check validity of the group.
        ABCA_ASSERT( m_parentGroup >= 0, "Invalid parent group" );
        
        uint32_t numSamples = m_nextSampleIndex;

        // Write all the sampling information.
        // This function, which is non-templated and lives in WriteUtil.cpp,
        // contains all of the logic regarding which information needs
        // to be written.
        WriteSampling( GetWrittenArraySampleMap(
                           this->getObject()->getArchive() ),
                       m_parentGroup, myName,
                       m_header->getTimeSamplingType(),
                       numSamples,
                       m_numUniqueSamples,
                       m_timeSamples.empty() ? NULL :
                       &m_timeSamples.front() );

        // Close the sampleIGroup if it was open
        if ( m_sampleIGroup >= 0 )
        {
            ABCA_ASSERT( m_numUniqueSamples > 1, "Corrupt SimplePwImpl" );
            H5Gclose( m_sampleIGroup );
            m_sampleIGroup = -1;
        }
    }
    catch ( std::exception & exc )
    {
        std::cerr << "AbcCoreHDF5::SimplePwImpl<A,I,K>::"
                  << "~SimplePwImpl(): EXCEPTION: "
                  << exc.what() << std::endl;
    }
    catch ( ... )
    {
        std::cerr << "AbcCoreHDF5::SimplePwImpl<A,I,K>::~SimplePwImpl(): "
                  << "UNKNOWN EXCEPTION: " << std::endl;
    }

    m_parentGroup = -1;
    m_sampleIGroup = -1;
    m_fileDataType = -1;
    m_nativeDataType = -1;
}

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreHDF5
} // End namespace Alembic

#endif
