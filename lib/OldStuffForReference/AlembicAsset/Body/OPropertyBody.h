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

#ifndef _AlembicAsset_Body_OPropertyBody_h_
#define _AlembicAsset_Body_OPropertyBody_h_

#include <AlembicAsset/Body/FoundationBody.h>
#include <AlembicAsset/Body/DataTypeTuple.h>
#include <AlembicAsset/Body/OContextBody.h>
#include <AlembicAsset/Body/OObjectBody.h>

namespace AlembicAsset {

//-*****************************************************************************
//-*****************************************************************************
// The output properties need a shared OContext because they need to keep
// it until they are destroyed. The finalization of the OPropertyBody writing
// happens on destruction, or a call to the 'finalize' member function
class OPropertyBody
{   
protected:
    OPropertyBody( OObjectBody &parentObject,
                   const std::string &name,
                   const std::string &protocol,
                   const DataType &dtype );

public:
    virtual ~OPropertyBody() throw();

    SharedOContextBody context() { return m_context; }
    
    const std::string &name() const { return m_name; }
    const std::string &protocol() const { return m_protocol; }
    const DataType &dataType() const { return m_dataType; }
    
    virtual PropertyType propertyType() const = 0;

    //-*************************************************************************
    //-*************************************************************************
    // REST SAMPLE
    // The Rest sample is the sample associated with "no time", and every
    // property must have at least this.
    // The base OPropertyBody virtuals require dimensions, even though
    // the singular properties are scalar and don't require any. They
    // are ignored in the singular versions.
    //-*************************************************************************
    //-*************************************************************************

    // Set data.
    void setRestSample( const void *data,
                        const Dimensions &dims );

    //-*************************************************************************
    //-*************************************************************************
    // ANIMATION
    // Animation must be enabled, property-by-property, by passing
    // in "time sampling info".  Animation samples may only be set after
    // this is done. 
    //-*************************************************************************
    //-*************************************************************************
    
    // Is the object animated? It is animated if the time sampling type
    // is meaningful.
    bool isAnimated() const throw()
    {
        return ( m_timeSamplingInfo.type != kUnknownTimeSamplingType );
    }

    // Return the time sampling info.
    const TimeSamplingInfo &timeSamplingInfo() const throw()
    {
        return m_timeSamplingInfo;
    }

    // Convert a sample into a time. This only works for
    // non-variable time sampling types.
    seconds_t sampleToSeconds( size_t samp ) const throw()
    {
        return m_timeSamplingInfo.sampleToSeconds( samp );
    }

    // Same disclaimer: non-variable only.
    size_t secondsToSample( seconds_t secs ) const throw()
    {
        return m_timeSamplingInfo.secondsToSample( secs );
    }

    //-*************************************************************************
    // TURN ANIMATION ON
    // Only do it once! Error to do it more than once!
    // However, it won't complain if the same settings are used.
    // This could throw an exception if it is called more than once with
    // conflicting types.
    //-*************************************************************************
    void makeAnimated( const TimeSamplingInfo &tinfo );

    // Returns the max sample written.
    size_t sampleTimesSize() const throw()
    { return m_sampleTimes.size(); }

    // Set animation sample.
    // This is the general form of the function, so it has all the arguments,
    // which may not always be used.
    // For example, singular properties ignore the dimensions.
    // uniform & open-close time sampling mostly ignore the seconds.
    void setAnimSample( size_t samp,
                        seconds_t seconds,
                        const void *data,
                        const Dimensions &dims );

    //-*************************************************************************
    // CLOSE IT!!!
    //-*************************************************************************
    // This will finalize (close and finish writing)
    // The destructor calls this unless it has been called.
    void close();
        
protected:
    // Called by the set rest sample above. subclasses must override.
    // This class will take care of common stuff, like setting the
    // 'sample written' flag. Derived classes must simply write the actual
    // sample.
    virtual void internalSetRestSample( const void *data,
                                        const Dimensions &dims ) = 0;

    // Called when a default is required.
    virtual void internalSetDefaultRestSample() = 0;
    
    // Called by the set anim sample above. subclasses must override.
    // This class will take care of common stuff, like setting the
    // 'sample written' flag, resizing the animation arrays,
    // accumulation, etc. Derived classes must simply write the actual
    // sample.
    virtual void internalSetAnimSample( size_t samp,
                                        const void *data,
                                        const Dimensions &dims ) = 0;

    // Whether or not there is REALLY variation in the animation. If there
    // is not, Alembic will write out the property with just a rest sample.
    virtual bool internalCheckAnimVariation() = 0;

    // Internal close.
    // it is expected that the property will not write animation
    // data if "internalCheckAnimVariation" returned false.
    virtual void internalClose() = 0;
    
    // Input Configuration Data
    SharedOContextBody m_context;
    std::string m_name;
    std::string m_protocol;
    DataType m_dataType;

    // Base HDF5 Objects
    // Whether or not the object is "closed" is determined by checking
    // the validing of m_group.
    H5G m_group;
    DataTypeTuple m_dataTypeTuple;

    // Rest Sample stuff
    bool m_restSampleWritten;

    //-*************************************************************************
    // ANIMATION STUFF
    //-*************************************************************************
    
    // Input time sampling info.
    TimeSamplingInfo m_timeSamplingInfo;

    // Sample times - for variable sampling, serves as the actual
    // variable time sample times, but also denotes "sample written"
    // by comparing against kNonTimeValue.
    SecondsArray m_sampleTimes;
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// Singular Property - values returned as void pointers to single values.
//-*****************************************************************************
//-*****************************************************************************
class OSingularPropertyBody : public OPropertyBody
{
public:
    OSingularPropertyBody( OObjectBody &parentObject,
                           const std::string &nme,
                           const std::string &prot,
                           const DataType &dtype );
    
    virtual ~OSingularPropertyBody() throw();
    
    virtual PropertyType propertyType() const throw();

    void setRestSample( const void *data )
    {
        OPropertyBody::setRestSample( data, Dimensions() );
    }

    void setAnimSample( size_t samp,
                        seconds_t seconds,
                        const void *data )
    {
        OPropertyBody::setAnimSample( samp, seconds, data, Dimensions() );
    }

protected:
    virtual void internalSetRestSample( const void *rsmp,
                                        const Dimensions &dims );

    virtual void internalSetDefaultRestSample();
    
    virtual void internalSetAnimSample( size_t samp,
                                        const void *asmp,
                                        const Dimensions &dims );

    virtual bool internalCheckAnimVariation();

    virtual void internalClose();

    // The rest buffer
    Bytes m_restSampleBytes;

    // Is there anim variation
    bool m_animVariation;

    // The animated values!
    Slab m_animValuesSlab;
};

//-*****************************************************************************
//-*****************************************************************************
// Multi Property - values are slabs.
// Slab Values loaded on demand only.
// Because of this, a context is required for any data access.
//-*****************************************************************************
//-*****************************************************************************
class OMultiPropertyBody : public OPropertyBody
{
public:
    OMultiPropertyBody( OObjectBody &parentObject,
                        const std::string &nme,
                        const std::string &prot,
                        const DataType &dtype );
    
    virtual ~OMultiPropertyBody() throw();

    virtual PropertyType propertyType() const throw();
    
protected:
    virtual void internalSetRestSample( const void *rsmp,
                                        const Dimensions &dims );

    virtual void internalSetDefaultRestSample();
    
    virtual void internalSetAnimSample( size_t samp,
                                        const void *asmp,
                                        const Dimensions &dims );

    virtual bool internalCheckAnimVariation();

    virtual void internalClose();
    
    // The group in which animated multi samples live!
    H5G m_animSampsGroup;

    // The slab id of the rest sample.
    Slab::HashID m_restSlabID;

    // The OSlabMapBody::SlabReference for rest slab.
    OSlabMapBody::SlabReference m_restSlabRef;

    // Test of whether or not variation has occurred.
    bool m_animVariation;

    // The animated slab ids.
    std::vector<Slab::HashID> m_animSlabIDs;
};

} // End namespace AlembicAsset

#endif
