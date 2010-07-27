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

#ifndef _AlembicAsset_OProperty_h_
#define _AlembicAsset_OProperty_h_

#include <AlembicAsset/Foundation.h>
#include <AlembicAsset/OObject.h>
#include <AlembicAsset/Base/TypedPropertyTraits.h>

namespace AlembicAsset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// BASE PROPERTY
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
class OProperty : public OBase
{
protected:
    // Create an empty property.
    OProperty() throw()
      : OBase()/*, m_baseBody()*/ {}

    // Copy a property
    OProperty( const OProperty &copy ) throw()
      : OBase( copy )/*, m_baseBody( copy.m_baseBody )*/ {}
    
    // Assign a property.
    void operator=( const OProperty &copy ) throw()
    { OBase::operator=( copy );
        /*m_baseBody = copy.m_baseBody;*/ }
    
    // Init!
    void init( SharedOPropertyBody baseBody )
    {
        /*m_baseBody = baseBody;*/
    }
    
public:
    typedef OProperty this_type;

    // Releases resources. Does NOT close, as this would violate
    // handle/body idiom.
    virtual ~OProperty() throw() {}

    virtual bool valid() const throw();

    std::string name() const throw();
    std::string protocol() const throw();
    DataType dataType() const throw();
    PropertyType propertyType() const throw();

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
    bool isAnimated() const throw();

    // Return the time sampling info.
    TimeSamplingInfo timeSamplingInfo() const throw();

    // Convert a sample into a time. This only works for
    // non-variable time sampling types.
    seconds_t sampleToSeconds( size_t samp ) const throw();

    // Same disclaimer: non-variable only.
    size_t secondsToSample( seconds_t secs ) const throw();

    size_t timeToSample( const Time &t ) const throw()
    {
        return secondsToSample( t.seconds() );
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
    size_t sampleTimesSize() const throw();

    // Set animation sample.
    // This is the general form of the function, so it has all the arguments,
    // which may not always be used.
    // For example, singular properties ignore the dimensions.
    // uniform & open-close time sampling mostly ignore the seconds.
    void setAnimSample( size_t samp,
                        seconds_t seconds,
                        const void *data,
                        const Dimensions &dims );

    void setAnimSample( size_t samp,
                        const Time &t,
                        const void *data,
                        const Dimensions &dims )
    {
        if ( t.isRestTime() )
        {
            OProperty::setRestSample( data, dims );
        }
        else
        {
            OProperty::setAnimSample( samp, t.seconds(), data, dims );
        }
    }

    void setAnimSample( size_t samp,
                        const void *data,
                        const Dimensions &dims )
    {
        OProperty::setAnimSample( samp,
                                  OProperty::sampleToSeconds( samp ),
                                  data,
                                  dims );
    }

    void setAnimSample( seconds_t secs,
                        const void *data,
                        const Dimensions &dims )
    {
        if ( secs == Time::kRestTimeValue() )
        {
            OProperty::setRestSample( data, dims );
        }
        else
        {
            OProperty::setAnimSample( OProperty::secondsToSample( secs ),
                                      secs,
                                      data,
                                      dims );
        }
    }
    
    void setAnimSample( const Time &t,
                        const void *data,
                        const Dimensions &dims )
    {
        if ( t.isRestTime() )
        {
            OProperty::setRestSample( data, dims );
        }
        else
        {
            OProperty::setAnimSample( OProperty::timeToSample( t ),
                                      t.seconds(),
                                      data,
                                      dims );
        }
    }

    //-*************************************************************************
    // CLOSE IT!!!
    //-*************************************************************************
    // This will finalize (close and finish writing)
    // The destructor calls this unless it has been called.
#if 0
    virtual void close() throw() = 0;
    virtual void close( ThrowExceptionFlag ) = 0;
    virtual void release() throw() = 0;
#endif

    //-*************************************************************************
    // OTHER
    bool isSingular() const throw()
    { return ( propertyType() == kSingularProperty ); }
    bool isMulti() const throw()
    { return ( propertyType() == kMultiProperty ); }

    // Base classes can return this.
    virtual SharedOPropertyBody asPropertyBody() const throw() = 0;

private:
    
    // Totally okay to keep shared base around.
    // SharedOPropertyBody m_baseBody;
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// SINGULAR!!!
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
class OSingularProperty : public OProperty
{
public:
    typedef OSingularProperty this_type;
    
    //-*************************************************************************
    // Default, copy & assignment
    OSingularProperty() throw()
      : OProperty(),
        m_body() {}
    
    OSingularProperty( const OSingularProperty &copy ) throw()
      : OProperty( copy ),
        m_body( copy.m_body ) {}
    
    OSingularProperty &operator=( const OSingularProperty &copy ) throw()
    {
        OProperty::operator=( copy );
        m_body = copy.m_body;
        return *this;
    }

    //-*************************************************************************
    // No exceptions
    OSingularProperty( const OObject &parentObj,
                       const std::string &nme,
                       const std::string &prot,
                       const DataType &dtype ) throw();

    // Exceptions
    OSingularProperty( const OObject &parentObj,
                       const std::string &nme,
                       const std::string &prot,
                       const DataType &dtype,
                       ThrowExceptionFlag );

    //-*************************************************************************
    // Destructor releases, but does not close. It would violate handle/body
    // idiom, otherwise.
    virtual ~OSingularProperty() throw() {}

    virtual void close() throw();
    virtual void close( ThrowExceptionFlag );

    virtual void release() throw();

    // For now these could throw.
    void setRestSample( const void *data )
    {
        OProperty::setRestSample( data, Dimensions() );
    }

    // For different time samplings.
    void setAnimSample( size_t samp, seconds_t secs, const void *data )
    {
        OProperty::setAnimSample( samp, secs, data, Dimensions() );
    }
    
    void setAnimSample( size_t samp, const Time &t, const void *data )
    {
        OProperty::setAnimSample( samp, t, data, Dimensions() );
    }
    
    void setAnimSample( size_t samp, const void *data )
    {
        OProperty::setAnimSample( samp, data, Dimensions() );
    }
    
    void setAnimSample( seconds_t secs, const void *data )
    {
        OProperty::setAnimSample( secs, data, Dimensions() );
    }
    
    void setAnimSample( const Time &t, const void *data )
    {
        OProperty::setAnimSample( t, data, Dimensions() );
    }
    
    SharedOSingularPropertyBody body() const throw() { return m_body; }
    virtual SharedOPropertyBody asPropertyBody() const throw();
    
public:
    SharedOSingularPropertyBody m_body;
};

//-*****************************************************************************
template <class TRAITS>
class OTypedSingularProperty : public OSingularProperty
{
public:
    typedef TRAITS traits_type;
    typedef OTypedSingularProperty<TRAITS> this_type;
    typedef typename TRAITS::value_type value_type;

    //-*************************************************************************
    // Default, copy & assignment
    OTypedSingularProperty() throw()
      : OSingularProperty() {}

    OTypedSingularProperty( const this_type &copy ) throw()
      : OSingularProperty( copy ) {}

    this_type &operator=( const this_type &copy ) throw()
    {
        OSingularProperty::operator=( copy );
        return *this;
    }

    //-*************************************************************************
    // No exceptions
    OTypedSingularProperty( const OObject &obj,
                            const std::string &nme )
        throw()
      : OSingularProperty( obj, nme,
                           TRAITS::protocol(),
                           TRAITS::dataType() ) {}

    // Exceptions
    OTypedSingularProperty( const OObject &obj,
                            const std::string &nme,
                            ThrowExceptionFlag tf )
      : OSingularProperty( obj, nme,
                           TRAITS::protocol(),
                           TRAITS::dataType(),
                           tf ) {}

    // Setting rest.
    // For now, at least, these can throw
    void set( const value_type &v )
    {
        OSingularProperty::setRestSample( ( const void * )&v );
    }

    // Setting anim.
    void setAnim( size_t samp, seconds_t seconds, const value_type &v )
    {
        OSingularProperty::setAnimSample( samp, seconds, ( const void * )&v );
    }
    
    void setAnim( size_t samp, const Time &time, const value_type &v )
    {
        OSingularProperty::setAnimSample( samp, time, ( const void * )&v );
    }
    
    void setAnim( size_t samp, const value_type &v )
    {
        OSingularProperty::setAnimSample( samp, ( const void * )&v );
    }
    
    void setAnim( seconds_t secs, const value_type &v )
    {
        OSingularProperty::setAnimSample( secs, ( const void * )&v );
    }

    void setAnim( const Time &t, const value_type &v )
    {
        OSingularProperty::setAnimSample( t, ( const void * )&v );
    }
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MULTI
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

class OMultiProperty : public OProperty
{
public:
    typedef OMultiProperty this_type;
    
    //-*************************************************************************
    // Default, copy & assignment
    OMultiProperty() throw() : OProperty() {}
    
    OMultiProperty( const OMultiProperty &copy ) throw()
      : OProperty( copy ),
        m_body( copy.m_body ) {}
    
    OMultiProperty &operator=( const OMultiProperty &copy ) throw()
    {
        OProperty::operator=( copy );
        m_body = copy.m_body;
        return *this;
    }

    //-*************************************************************************
    // No exceptions
    OMultiProperty( const OObject &parentObj,
                    const std::string &nme,
                    const std::string &prot,
                    const DataType &dtype ) throw();
    
    // Exceptions
    OMultiProperty( const OObject &parentObj,
                    const std::string &nme,
                    const std::string &prot,
                    const DataType &dtype,
                    ThrowExceptionFlag );

    //-*************************************************************************
    // Destructor releases, but does not close. Otherwise it would violate
    // handle/body idiom.
    virtual ~OMultiProperty() throw() {}

    // No exceptions
    virtual void close() throw();

    // Exceptions
    virtual void close( ThrowExceptionFlag );

    virtual void release() throw();

    // These can throw for now.
    void setRestSample( const void *data, size_t numElements )
    {
        Dimensions dims( 1 );
        dims[0] = numElements;
        OProperty::setRestSample( data, dims );
    }

    //-*************************************************************************
    // Anim samples
    void setAnimSample( size_t samp, seconds_t secs,
                        const void *data, size_t numElements )
    {
        Dimensions dims( 1 );
        dims[0] = numElements;
        OProperty::setAnimSample( samp, secs, data, dims );
    }
    void setAnimSample( size_t samp, const Time &t,
                        const void *data, size_t numElements )
    {
        Dimensions dims( 1 );
        dims[0] = numElements;
        OProperty::setAnimSample( samp, t, data, dims );
    }
    void setAnimSample( size_t samp,
                        const void *data, size_t numElements )
    {
        Dimensions dims( 1 );
        dims[0] = numElements;
        OProperty::setAnimSample( samp, data, dims );
    }
    void setAnimSample( seconds_t secs,
                        const void *data, size_t numElements )
    {
        Dimensions dims( 1 );
        dims[0] = numElements;
        OProperty::setAnimSample( secs, data, dims );
    }
    void setAnimSample( const Time &t,
                        const void *data, size_t numElements )
    {
        Dimensions dims( 1 );
        dims[0] = numElements;
        OProperty::setAnimSample( t, data, dims );
    }
    
    SharedOMultiPropertyBody body() const throw() { return m_body; }
    virtual SharedOPropertyBody asPropertyBody() const throw();
    
public:
    SharedOMultiPropertyBody m_body;
};

//-*****************************************************************************
template <class TRAITS>
class OTypedMultiProperty : public OMultiProperty
{
public:
    typedef TRAITS traits_type;
    typedef OTypedMultiProperty<TRAITS> this_type;
    typedef typename TRAITS::value_type value_type;

    //-*************************************************************************
    // Default, copy & assignment
    OTypedMultiProperty() throw()
      : OMultiProperty() {}

    OTypedMultiProperty( const this_type &copy ) throw()
      : OMultiProperty( copy ) {}

    this_type &operator=( const this_type &copy ) throw()
    {
        OMultiProperty::operator=( copy );
        return *this;
    }

    //-*************************************************************************
    // No exceptions
    OTypedMultiProperty( const OObject &obj,
                         const std::string &nme )
        throw()
      : OMultiProperty( obj, nme,
                        TRAITS::protocol(),
                        TRAITS::dataType() ) {}

    // Exceptions
    OTypedMultiProperty( const OObject &obj,
                         const std::string &nme,
                         ThrowExceptionFlag tf )
      : OMultiProperty( obj, nme,
                        TRAITS::protocol(),
                        TRAITS::dataType(),
                        tf ) {}

    //-*************************************************************************
    // SET FUNCTIONS : these can, for now, throw
    //-*************************************************************************
    // Setting rest.
    void set( const value_type *v, const Dimensions &dims )
    {
        OMultiProperty::setRestSample( ( const void * )v, dims );
    }
    void set( const value_type *v, size_t numE )
    {
        OMultiProperty::setRestSample( ( const void * )v, numE );
    }

    //-*************************************************************************
    // Setting anim. There are 10 of these.
    void setAnim( size_t samp, seconds_t secs,
                  const value_type *v, const Dimensions &dims )
    {
        OMultiProperty::setAnimSample( samp, secs, ( const void * )v, dims );
    }
    
    void setAnim( size_t samp, const Time &t,
                  const value_type *v, const Dimensions &dims )
    {
        OMultiProperty::setAnimSample( samp, t, ( const void * )v, dims );
    }
    
    void setAnim( size_t samp, const value_type *v, const Dimensions &dims )
    {
        OMultiProperty::setAnimSample( samp, ( const void * )v, dims );
    }
    
    void setAnim( seconds_t secs, const value_type *v, const Dimensions &dims )
    {
        OMultiProperty::setAnimSample( secs, ( const void * )v, dims );
    }
    
    void setAnim( const Time &t, const value_type *v, const Dimensions &dims )
    {
        OMultiProperty::setAnimSample( t, ( const void * )v, dims );
    }
    
    //-*************************************************************************
    void setAnim( size_t samp, seconds_t secs,
                  const value_type *v, size_t numE )
    {
        OMultiProperty::setAnimSample( samp, secs, ( const void * )v, numE );
    }
    
    void setAnim( size_t samp, const Time &t,
                  const value_type *v, size_t numE )
    {
        OMultiProperty::setAnimSample( samp, t, ( const void * )v, numE );
    }
    
    void setAnim( size_t samp, const value_type *v, size_t numE )
    {
        OMultiProperty::setAnimSample( samp, ( const void * )v, numE );
    }
    
    void setAnim( seconds_t secs, const value_type *v, size_t numE )
    {
        OMultiProperty::setAnimSample( secs, ( const void * )v, numE );
    }
    
    void setAnim( const Time &t, const value_type *v, size_t numE )
    {
        OMultiProperty::setAnimSample( t, ( const void * )v, numE );
    }
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// TYPEDEFS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

typedef OTypedSingularProperty<Uint8TPTraits>           OUcharProperty;
typedef OTypedSingularProperty<Int8TPTraits>            OCharProperty;
typedef OTypedSingularProperty<Uint16TPTraits>          OUshortProperty;
typedef OTypedSingularProperty<Int16TPTraits>           OShortProperty;
typedef OTypedSingularProperty<Uint32TPTraits>          OUintProperty;
typedef OTypedSingularProperty<Int32TPTraits>           OIntProperty;
typedef OTypedSingularProperty<Uint64TPTraits>          OUlongProperty;
typedef OTypedSingularProperty<Int64TPTraits>           OLongProperty;
typedef OTypedSingularProperty<Float32TPTraits>         OFloatProperty;
typedef OTypedSingularProperty<Float64TPTraits>         ODoubleProperty;

typedef OTypedSingularProperty<V2sTPTraits>             OV2sProperty;
typedef OTypedSingularProperty<V2iTPTraits>             OV2iProperty;
typedef OTypedSingularProperty<V2fTPTraits>             OV2fProperty;
typedef OTypedSingularProperty<V2dTPTraits>             OV2dProperty;

typedef OTypedSingularProperty<V3sTPTraits>             OV3sProperty;
typedef OTypedSingularProperty<V3iTPTraits>             OV3iProperty;
typedef OTypedSingularProperty<V3fTPTraits>             OV3fProperty;
typedef OTypedSingularProperty<V3dTPTraits>             OV3dProperty;

typedef OTypedSingularProperty<Box2sTPTraits>           OBox2sProperty;
typedef OTypedSingularProperty<Box2iTPTraits>           OBox2iProperty;
typedef OTypedSingularProperty<Box2fTPTraits>           OBox2fProperty;
typedef OTypedSingularProperty<Box2dTPTraits>           OBox2dProperty;

typedef OTypedSingularProperty<Box3sTPTraits>           OBox3sProperty;
typedef OTypedSingularProperty<Box3iTPTraits>           OBox3iProperty;
typedef OTypedSingularProperty<Box3fTPTraits>           OBox3fProperty;
typedef OTypedSingularProperty<Box3dTPTraits>           OBox3dProperty;

typedef OTypedSingularProperty<M33fTPTraits>            OM33fProperty;
typedef OTypedSingularProperty<M33dTPTraits>            OM33dProperty;
typedef OTypedSingularProperty<M44fTPTraits>            OM44fProperty;
typedef OTypedSingularProperty<M44dTPTraits>            OM44dProperty;

typedef OTypedSingularProperty<QuatfTPTraits>           OQuatfProperty;
typedef OTypedSingularProperty<QuatdTPTraits>           OQuatdProperty;

//-*****************************************************************************
typedef OTypedMultiProperty<Uint8TPTraits>             OUcharArrayProperty;
typedef OTypedMultiProperty<Int8TPTraits>              OCharArrayProperty;
typedef OTypedMultiProperty<Uint16TPTraits>            OUshortArrayProperty;
typedef OTypedMultiProperty<Int16TPTraits>             OShortArrayProperty;
typedef OTypedMultiProperty<Uint32TPTraits>            OUintArrayProperty;
typedef OTypedMultiProperty<Int32TPTraits>             OIntArrayProperty;
typedef OTypedMultiProperty<Uint64TPTraits>            OUlongArrayProperty;
typedef OTypedMultiProperty<Int64TPTraits>             OLongArrayProperty;
typedef OTypedMultiProperty<Float32TPTraits>           OFloatArrayProperty;
typedef OTypedMultiProperty<Float64TPTraits>           ODoubleArrayProperty;

typedef OTypedMultiProperty<V2sTPTraits>               OV2sArrayProperty;
typedef OTypedMultiProperty<V2iTPTraits>               OV2iArrayProperty;
typedef OTypedMultiProperty<V2fTPTraits>               OV2fArrayProperty;
typedef OTypedMultiProperty<V2dTPTraits>               OV2dArrayProperty;

typedef OTypedMultiProperty<V3sTPTraits>               OV3sArrayProperty;
typedef OTypedMultiProperty<V3iTPTraits>               OV3iArrayProperty;
typedef OTypedMultiProperty<V3fTPTraits>               OV3fArrayProperty;
typedef OTypedMultiProperty<V3dTPTraits>               OV3dArrayProperty;

typedef OTypedMultiProperty<Box2sTPTraits>             OBox2sArrayProperty;
typedef OTypedMultiProperty<Box2iTPTraits>             OBox2iArrayProperty;
typedef OTypedMultiProperty<Box2fTPTraits>             OBox2fArrayProperty;
typedef OTypedMultiProperty<Box2dTPTraits>             OBox2dArrayProperty;

typedef OTypedMultiProperty<Box3sTPTraits>             OBox3sArrayProperty;
typedef OTypedMultiProperty<Box3iTPTraits>             OBox3iArrayProperty;
typedef OTypedMultiProperty<Box3fTPTraits>             OBox3fArrayProperty;
typedef OTypedMultiProperty<Box3dTPTraits>             OBox3dArrayProperty;

typedef OTypedMultiProperty<M33fTPTraits>              OM33fArrayProperty;
typedef OTypedMultiProperty<M33dTPTraits>              OM33dArrayProperty;
typedef OTypedMultiProperty<M44fTPTraits>              OM44fArrayProperty;
typedef OTypedMultiProperty<M44dTPTraits>              OM44dArrayProperty;

typedef OTypedMultiProperty<QuatfTPTraits>             OQuatfArrayProperty;
typedef OTypedMultiProperty<QuatdTPTraits>             OQuatdArrayProperty;

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

namespace Output {

typedef OUcharProperty             UcharProperty;
typedef OCharProperty              CharProperty;
typedef OUshortProperty            UshortProperty;
typedef OShortProperty             ShortProperty;
typedef OUintProperty              UintProperty;
typedef OIntProperty               IntProperty;
typedef OUlongProperty             UlongProperty;
typedef OLongProperty              LongProperty;
typedef OFloatProperty             FloatProperty;
typedef ODoubleProperty            DoubleProperty;

typedef OV2sProperty               V2sProperty;
typedef OV2iProperty               V2iProperty;
typedef OV2fProperty               V2fProperty;
typedef OV2dProperty               V2dProperty;

typedef OV3sProperty               V3sProperty;
typedef OV3iProperty               V3iProperty;
typedef OV3fProperty               V3fProperty;
typedef OV3dProperty               V3dProperty;

typedef OBox2sProperty             Box2sProperty;
typedef OBox2iProperty             Box2iProperty;
typedef OBox2fProperty             Box2fProperty;
typedef OBox2dProperty             Box2dProperty;

typedef OBox3sProperty             Box3sProperty;
typedef OBox3iProperty             Box3iProperty;
typedef OBox3fProperty             Box3fProperty;
typedef OBox3dProperty             Box3dProperty;

typedef OM33fProperty              M33fProperty;
typedef OM33dProperty              M33dProperty;
typedef OM44fProperty              M44fProperty;
typedef OM44dProperty              M44dProperty;

typedef OQuatfProperty             QuatfProperty;
typedef OQuatdProperty             QuatdProperty;

//-*****************************************************************************
typedef OUcharArrayProperty             UcharArrayProperty;
typedef OCharArrayProperty              CharArrayProperty;
typedef OUshortArrayProperty            UshortArrayProperty;
typedef OShortArrayProperty             ShortArrayProperty;
typedef OUintArrayProperty              UintArrayProperty;
typedef OIntArrayProperty               IntArrayProperty;
typedef OUlongArrayProperty             UlongArrayProperty;
typedef OLongArrayProperty              LongArrayProperty;
typedef OFloatArrayProperty             FloatArrayProperty;
typedef ODoubleArrayProperty            DoubleArrayProperty;

typedef OV2sArrayProperty               V2sArrayProperty;
typedef OV2iArrayProperty               V2iArrayProperty;
typedef OV2fArrayProperty               V2fArrayProperty;
typedef OV2dArrayProperty               V2dArrayProperty;

typedef OV3sArrayProperty               V3sArrayProperty;
typedef OV3iArrayProperty               V3iArrayProperty;
typedef OV3fArrayProperty               V3fArrayProperty;
typedef OV3dArrayProperty               V3dArrayProperty;

typedef OBox2sArrayProperty             Box2sArrayProperty;
typedef OBox2iArrayProperty             Box2iArrayProperty;
typedef OBox2fArrayProperty             Box2fArrayProperty;
typedef OBox2dArrayProperty             Box2dArrayProperty;

typedef OBox3sArrayProperty             Box3sArrayProperty;
typedef OBox3iArrayProperty             Box3iArrayProperty;
typedef OBox3fArrayProperty             Box3fArrayProperty;
typedef OBox3dArrayProperty             Box3dArrayProperty;

typedef OM33fArrayProperty              M33fArrayProperty;
typedef OM33dArrayProperty              M33dArrayProperty;
typedef OM44fArrayProperty              M44fArrayProperty;
typedef OM44dArrayProperty              M44dArrayProperty;

typedef OQuatfArrayProperty             QuatfArrayProperty;
typedef OQuatdArrayProperty             QuatdArrayProperty;

} // End namespace Output

} // AlembicAsset

#endif
