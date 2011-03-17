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

#ifndef _Alembic_AbcCoreHDF5_SimplePrImpl_h_
#define _Alembic_AbcCoreHDF5_SimplePrImpl_h_

#include <Alembic/AbcCoreHDF5/Foundation.h>

#include <Alembic/AbcCoreHDF5/OrImpl.h>
#include <Alembic/AbcCoreHDF5/ArImpl.h>
#include <Alembic/AbcCoreHDF5/ReadUtil.h>
#include <Alembic/AbcCoreHDF5/DataTypeRegistry.h>
#include <Alembic/AbcCoreHDF5/HDF5Util.h>

namespace Alembic {
namespace AbcCoreHDF5 {

//-*****************************************************************************
// This templated base class implements the common logic behind both the
// scalar and array property readers. The only way these two readers differ
// is the type of sample they read and the way those samples are returned
// and managed. Scalar samples are simply copied by value, compared by value,
// and stored by value. Array samples are returned from an optional sample
// cache.
//
// There is a bit of template hoosafudgery going on here, which is always
// troublesome in foundation libraries, because it is hard to read and
// hard to decipher errors. I have kept it to a reasonable minimum, but the
// value of a single code instance is high enough that it's worth a bit of
// obfuscation.
//
// The IMPL class is assumed to have the following functions:
// void readSample( H5G &iGroup,
//                  const std::string &iSampleName,
//                  index_t iSampleIndex,
//                  SAMPLE oSample );
//
//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE>
class SimplePrImpl : public ABSTRACT
{
protected:
    SimplePrImpl( AbcA::CompoundPropertyReaderPtr iParent,
                  hid_t iParentGroup,
                  PropertyHeaderPtr iHeader );

public:
    //-*************************************************************************
    // ABSTRACT API
    //-*************************************************************************
    virtual ~SimplePrImpl();

    virtual const AbcA::PropertyHeader &getHeader() const;

    virtual AbcA::ObjectReaderPtr getObject();

    virtual AbcA::CompoundPropertyReaderPtr getParent();

    virtual size_t getNumSamples();

    virtual bool isConstant();

    virtual AbcA::TimeSampling getTimeSampling();

    virtual void getSample( index_t iSampleIndex,
                            SAMPLE oSample );

    virtual bool getKey( index_t iSampleIndex, AbcA::ArraySampleKey & oKey );

protected:
    // Parent compound property writer. It must exist.
    AbcA::CompoundPropertyReaderPtr m_parent;

    // The HDF5 Group associated with the parent property reader.
    hid_t m_parentGroup;

    // We don't hold a pointer to the object, but instead
    // get it from the compound property reader.

    // The Header
    PropertyHeaderPtr m_header;

    // Data Types.
    hid_t m_fileDataType;
    bool m_cleanFileDataType;
    hid_t m_nativeDataType;
    bool m_cleanNativeDataType;

    // The number of samples that were written. This may be greater
    // than the number of samples that were stored, because on the tail
    // of the property we don't write the same sample out over and over
    // until it changes.
    uint32_t m_numSamples;

    // The number of unique samples
    // This may be the same as the number of samples,
    // or it may be less. This corresponds to the number of samples
    // that are actually stored. In the case of a "constant" property,
    // this will be 1.
    uint32_t m_numUniqueSamples;

    // Value of the single time sample, if there's only one unique
    // sample.
    chrono_t m_timeSample0;

    // The time sampling ptr.
    // Contains Array Sample corresponding to the time samples
    AbcA::TimeSamplingPtr m_timeSamplingPtr;

    // The simple properties only store samples after the first
    // sample in a sub group. Therefore, there may not actually be
    // a group associated with this property.
    hid_t m_samplesIGroup;
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// IMPLEMENTATION
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE>
SimplePrImpl<ABSTRACT,IMPL,SAMPLE>::SimplePrImpl
(
    AbcA::CompoundPropertyReaderPtr iParent,
    hid_t iParentGroup,
    PropertyHeaderPtr iHeader
)
  : m_parent( iParent )
  , m_parentGroup( iParentGroup )
  , m_header( iHeader )
  , m_fileDataType( -1 )
  , m_cleanFileDataType( false )
  , m_nativeDataType( -1 )
  , m_cleanNativeDataType( false )
  , m_timeSample0( 0.0 )
  , m_samplesIGroup( -1 )
{
    // Validate all inputs.
    ABCA_ASSERT( m_parent, "Invalid parent" );
    ABCA_ASSERT( m_parentGroup >= 0, "Invalid parent group" );
    ABCA_ASSERT( m_header, "Invalid header" );
    ABCA_ASSERT( m_header->getPropertyType() != AbcA::kCompoundProperty,
                 "Tried to create a simple property with a compound header" );

    // Get data types
    PlainOldDataType POD = m_header->getDataType().getPod();
    if ( POD != kStringPOD && POD != kWstringPOD )
    {
        m_fileDataType = GetFileH5T( m_header->getDataType(),
                                     m_cleanFileDataType );
        m_nativeDataType = GetNativeH5T( m_header->getDataType(),
                                         m_cleanNativeDataType );
    }

    // Get our name.
    const std::string &myName = m_header->getName();

    // Read the num samples.
    m_numSamples = 0;
    m_numUniqueSamples = 0;
    uint32_t numSamples32 = 0;
    uint32_t numUniqueSamples32 = 0;
    bool isScalar = m_header->getPropertyType() == AbcA::kScalarProperty;
    ReadNumSamples( m_parentGroup,
                    myName,
                    isScalar,
                    numSamples32,
                    numUniqueSamples32 );
    m_numSamples = numSamples32;
    m_numUniqueSamples = numUniqueSamples32;

    // Validate num unique samples.
    ABCA_ASSERT( m_numUniqueSamples <= m_numSamples,
                 "Corrupt numUniqueSamples: " << m_numUniqueSamples
                 << "in property: " << myName
                 << " which has numSamples: " << m_numSamples );

    // Leave timeSamples and timeSamplingPtr until
    // somebody asks for them.
}

//-*****************************************************************************
// Destructor is at the end, so that this file has a logical ordering that
// matches the order of operations (create, get samples, destroy)
//-*****************************************************************************

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE>
const AbcA::PropertyHeader &
SimplePrImpl<ABSTRACT,IMPL,SAMPLE>::getHeader() const
{
    ABCA_ASSERT( m_header, "Invalid header" );
    return *m_header;
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE>
AbcA::ObjectReaderPtr
SimplePrImpl<ABSTRACT,IMPL,SAMPLE>::getObject()
{
    ABCA_ASSERT( m_parent, "Invalid parent" );
    return m_parent->getObject();
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE>
AbcA::CompoundPropertyReaderPtr
SimplePrImpl<ABSTRACT,IMPL,SAMPLE>::getParent()
{
    ABCA_ASSERT( m_parent, "Invalid parent" );
    return m_parent;
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE>
size_t SimplePrImpl<ABSTRACT,IMPL,SAMPLE>::getNumSamples()
{
    return ( size_t )m_numSamples;
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE>
bool SimplePrImpl<ABSTRACT,IMPL,SAMPLE>::isConstant()
{
    return ( m_numUniqueSamples < 2 );
}

//-*****************************************************************************
// This class reads the time sampling on demand.
template <class ABSTRACT, class IMPL, class SAMPLE>
AbcA::TimeSampling
SimplePrImpl<ABSTRACT,IMPL,SAMPLE>::getTimeSampling()
{
    //-*************************************************************************
    // Read the time samples as an array ptr
    // check their sizes, convert to times, create time sampling ptr.
    // whew.
    const std::string &myName = m_header->getName();

    // Read the array, possibly from the cache.
    // We are either a brand new shared_ptr <ArraySample>
    // that owns the memory (and is now in the cache)
    // OR we are a ref_count ++ of the shared_ptr <ArraySample>
    // found from the cache.
    // We'll create a TimeSamplingPtr and it will keep this
    // reference for us.
    AbcA::ArraySamplePtr timeSamples =
        AbcCoreHDF5::ReadTimeSamples( this->getObject()->getArchive()->
                         getReadArraySampleCachePtr(),
                         m_parentGroup, myName + ".time" );
    ABCA_ASSERT( timeSamples,
                 "Couldn't read time samples for attr: " << myName );

    // Check the byte sizes.
    const AbcA::TimeSamplingType &tst = m_header->getTimeSamplingType();
    uint32_t numExpectedTimeSamples =
        std::min( tst.getNumSamplesPerCycle(), m_numSamples );

    size_t gotNumTimes =
        timeSamples->getDimensions().numPoints();

    ABCA_ASSERT( numExpectedTimeSamples == 0 ||
                 numExpectedTimeSamples == gotNumTimes,
                 "Expected: " << numExpectedTimeSamples
                 << " time samples, but got: "
                 << gotNumTimes << " instead." );

    // Build a time sampling ptr.
    // m_timeSamplingPtr is shared_ptr to TimeSampling.
    // TimeSampling contains numSamples and handy accessors AND
    // the ArraySamplePtr of sampleTimes
    m_timeSamplingPtr.reset( new AbcA::TimeSampling( tst, m_numSamples,
                                                     timeSamples ) );

    AbcA::TimeSampling ret = *m_timeSamplingPtr;
    // And return it.
    return ret;
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE>
void
SimplePrImpl<ABSTRACT,IMPL,SAMPLE>::getSample( index_t iSampleIndex,
                                               SAMPLE oSample )
{
    // Verify sample index
    ABCA_ASSERT( iSampleIndex >= 0 &&
                 iSampleIndex < m_numSamples,
                 "Invalid sample index: " << iSampleIndex
                 << ", should be between 0 and " << m_numSamples-1 );

    if ( iSampleIndex >= m_numUniqueSamples )
    {
        iSampleIndex = m_numUniqueSamples-1;
    }

    // Get our name.
    const std::string &myName = m_header->getName();

    if ( iSampleIndex == 0 )
    {
        // Read the sample from the parent group.
        // Sample 0 is always on the parent group, with
        // our name + ".smp0" as the name of it.
        std::string sample0Name = getSampleName( myName, 0 );
        if ( m_header->getPropertyType() == AbcA::kScalarProperty )
        {
            ABCA_ASSERT( H5Aexists( m_parentGroup, sample0Name.c_str() ),
                         "Invalid property: " << myName
                         << ", missing smp0" );
        }
        else
        {
            ABCA_ASSERT( DatasetExists( m_parentGroup, sample0Name ),
                         "Invalid property: " << myName
                         << ", missing smp1" );
        }

        static_cast<IMPL *>( this )->readSample( m_parentGroup,
                                                 sample0Name,
                                                 iSampleIndex,
                                                 oSample );
    }
    else
    {
        // Create the subsequent samples group.
        if ( m_samplesIGroup < 0 )
        {
            std::string samplesIName = myName + ".smpi";
            ABCA_ASSERT( GroupExists( m_parentGroup,
                                      samplesIName ),
                         "Invalid property: " << myName
                         << ", missing smpi" );

            m_samplesIGroup = H5Gopen2( m_parentGroup,
                                        samplesIName.c_str(),
                                        H5P_DEFAULT );
            ABCA_ASSERT( m_samplesIGroup >= 0,
                         "Invalid property: " << myName
                         << ", invalid smpi group" );
        }

        // Read the sample.
        std::string sampleName = getSampleName( myName, iSampleIndex );
        static_cast<IMPL *>( this )->readSample( m_samplesIGroup,
                                                 sampleName,
                                                 iSampleIndex,
                                                 oSample );
    }
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE>
bool
SimplePrImpl<ABSTRACT,IMPL,SAMPLE>::getKey( index_t iSampleIndex,
                                               AbcA::ArraySampleKey & oKey )
{
    // Verify sample index
    ABCA_ASSERT( iSampleIndex >= 0 &&
                 iSampleIndex < m_numSamples,
                 "Invalid sample index: " << iSampleIndex
                 << ", should be between 0 and " << m_numSamples-1 );

    if ( iSampleIndex >= m_numUniqueSamples )
    {
        iSampleIndex = m_numUniqueSamples-1;
    }

    // Get our name.
    const std::string &myName = m_header->getName();

    if ( iSampleIndex == 0 )
    {
        // Read the sample from the parent group.
        // Sample 0 is always on the parent group, with
        // our name + ".sample_0" as the name of it.
        std::string sample0Name = getSampleName( myName, 0 );
        if ( m_header->getPropertyType() == AbcA::kScalarProperty )
        {
            ABCA_ASSERT( H5Aexists( m_parentGroup, sample0Name.c_str() ),
                         "Invalid property: " << myName
                         << ", missing smp0" );
        }
        else
        {
            ABCA_ASSERT( DatasetExists( m_parentGroup, sample0Name ),
                         "Invalid property: " << myName
                         << ", missing smp1" );
        }

        return static_cast<IMPL *>( this )->readKey( m_parentGroup,
                                                    sample0Name,
                                                    oKey );
    }
    else
    {
        // Create the subsequent samples group.
        if ( m_samplesIGroup < 0 )
        {
            std::string samplesIName = myName + ".smpi";
            ABCA_ASSERT( GroupExists( m_parentGroup,
                                      samplesIName ),
                         "Invalid property: " << myName
                         << ", missing smpi" );

            m_samplesIGroup = H5Gopen2( m_parentGroup,
                                        samplesIName.c_str(),
                                        H5P_DEFAULT );
            ABCA_ASSERT( m_samplesIGroup >= 0,
                         "Invalid property: " << myName
                         << ", invalid smpi group" );
        }

        // Read the sample.
        std::string sampleName = getSampleName( myName, iSampleIndex );
        return static_cast<IMPL *>( this )->readKey( m_samplesIGroup,
                                                    sampleName,
                                                    oKey );
    }
}

//-*****************************************************************************
template <class ABSTRACT, class IMPL, class SAMPLE>
SimplePrImpl<ABSTRACT,IMPL,SAMPLE>::~SimplePrImpl()
{
    // Clean up our samples group, if necessary.
    if ( m_samplesIGroup >= 0 )
    {
        H5Gclose( m_samplesIGroup );
        m_samplesIGroup = -1;
    }

    if ( m_fileDataType >= 0 && m_cleanFileDataType )
    {
        H5Tclose( m_fileDataType );
        m_fileDataType = -1;
    }

    if ( m_nativeDataType >= 0 && m_cleanNativeDataType )
    {
        H5Tclose( m_nativeDataType );
        m_nativeDataType = -1;
    }
}

} // End namespace AbcCoreHDF5
} // End namespace Alembic

#endif
