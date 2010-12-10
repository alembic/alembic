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

#ifndef _AlembicAsset_IProperty_h_
#define _AlembicAsset_IProperty_h_

#include <AlembicAsset/Foundation.h>
#include <AlembicAsset/ISample.h>
#include <AlembicAsset/IObject.h>
#include <AlembicAsset/Base/TypedPropertyTraits.h>
#include <AlembicAsset/Base/ITimeSampling.h>

namespace AlembicAsset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// BASE PROPERTY
// You can't really do much with these directly, but it's conceivable
// that you might wish to store a list of OProperties and then upcast
// them as needed, or something like that.
// What do all properties know?
// name
// protocol
// dataType
// type
// isAnimated
// timeSampling
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
class IProperty : public IBase
{
protected:
    // Create an empty property.
    IProperty() throw() : IBase() {}

    // Copy a property
    IProperty( const IProperty &copy ) throw() : IBase( copy ) {}

    // Assign a property
    void operator=( const IProperty &copy ) throw()
    { IBase::operator=( copy ); }

public:
    typedef IProperty this_type;
    
    virtual ~IProperty() throw() {}

    virtual DataType dataType() const throw() = 0;
    virtual PropertyType propertyType() const throw() = 0;
    virtual bool isAnimated() const throw() = 0;
    virtual const ITimeSampling *timeSampling() const throw() = 0;
    
    bool isSingular() const throw()
    { return ( propertyType() == kSingularProperty ); }
    bool isMulti() const throw()
    { return ( propertyType() == kMultiProperty ); }

    // Base classes can return this
    virtual SharedIPropertyBody asPropertyBody() const throw() = 0;
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// SINGULAR PROPERTIES
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
class ISingularProperty : public IProperty
{
private:
    void init( const IObject &parent,
               const std::string &nme,
               const std::string &prot,
               const DataType &dtype,
               const IContext &ctx );
    
public:
    typedef ISingularProperty this_type;
    
    //-*************************************************************************
    // Default, copy & assignment
    ISingularProperty()
      : IProperty() {}

    ISingularProperty( const ISingularProperty &copy )
      : IProperty( copy ),
        m_body( copy.m_body ) {}

    ISingularProperty &operator=( const ISingularProperty &copy )
    {
        IProperty::operator=( copy );
        m_body = copy.m_body;
        return *this;
    }
    
    //-*************************************************************************
    // Construction from name and optional protocol spec and optional
    // datatype spec.
    // NO EXCEPTIONS
    ISingularProperty( const IObject &parent,
                       const std::string &nme,
                       const std::string &prot = "",
                       const DataType &dtype = DataType(),
                       const IContext &ctx = IContext() )
        throw();

    // EXCEPTIONS
    ISingularProperty( const IObject &parent,
                       const std::string &nme,
                       ThrowExceptionFlag,
                       const std::string &prot = "",
                       const DataType &dtype = DataType(),
                       const IContext &ctx = IContext() );

    //-*************************************************************************
    // Construction from PropertyInfo
    // NO EXCEPTIONS
    ISingularProperty( const IObject &parent,
                       PropertyInfo pinfo,
                       const IContext &ctx = IContext() )
        throw();
    
    // EXCEPTIONS
    ISingularProperty( const IObject &parent,
                       PropertyInfo pinfo,
                       ThrowExceptionFlag,
                       const IContext &ctx = IContext() );

    //-*************************************************************************
    // Construction from index
    // NO EXCEPTIONS
    ISingularProperty( const IObject &parent,
                       size_t propIdx,
                       const IContext &ctx = IContext() )
        throw();
    
    // EXCEPTIONS
    ISingularProperty( const IObject &parent,
                       size_t propIdx,
                       ThrowExceptionFlag,
                       const IContext &ctx = IContext() );

    //-*************************************************************************
    // Destructor releases, but does not close.
    virtual ~ISingularProperty() throw() {}

    //-*************************************************************************
    // INHERITED STUFF
    //-*************************************************************************
    virtual bool valid() const throw();
    virtual std::string name() const throw();
    virtual std::string fullPathName() const throw();
    virtual std::string protocol() const throw();
    virtual DataType dataType() const throw();
    virtual PropertyType propertyType() const throw();
    virtual bool isAnimated() const throw();
    virtual const ITimeSampling *timeSampling() const throw();

    // Return values. These are stored inside the body
    // so it is okay.
    // These can throw for now.
    const void *restSample() const;
    const void *animSample( size_t samp ) const;
    const void *animSampleLowerBound( seconds_t secs ) const;
    const void *animSampleUpperBound( seconds_t secs ) const;
    const void *animSampleLowerBound( const Time &time ) const;
    const void *animSampleUpperBound( const Time &time ) const;
    
    // No exceptions
    virtual void close() throw();

    // Exceptions
    virtual void close( ThrowExceptionFlag );

    virtual void release() throw();

    SharedISingularPropertyBody body() const throw() { return m_body; }
    virtual SharedIPropertyBody asPropertyBody() const throw();

protected:
    SharedISingularPropertyBody m_body;
};

//-*****************************************************************************
//-*****************************************************************************
template <class TRAITS>
class ITypedSingularProperty : public ISingularProperty
{
private:
    // Handy helper
    static PropertyInfo overridePinfo( PropertyInfo pinfo, bool pedantic )
    {
        if ( !pinfo ) { return pinfo; }
        else
        {
            return MakePropertyInfo(
                pinfo->name,
                pedantic ? TRAITS::protocol() : "",
                kSingularProperty,
                TRAITS::dataType() );
        }
    }
public:
    typedef TRAITS traits_type;
    typedef ITypedSingularProperty<TRAITS> this_type;
    typedef typename TRAITS::value_type value_type;
    
    //-*************************************************************************
    // Default, copy & assignment
    ITypedSingularProperty()
      : ISingularProperty() {}

    ITypedSingularProperty( const this_type &copy )
      : ISingularProperty( copy ) {}

    this_type &operator=( const this_type &copy )
    {
        ISingularProperty::operator=( copy );
        return *this;
    }

    //-*************************************************************************
    // Construction from name and optional protocol spec and optional
    // datatype spec.
    // NO EXCEPTIONS
    ITypedSingularProperty( const IObject &parent,
                            const std::string &nme,
                            bool pedanticProtocol = false,
                            const IContext &ctx = IContext() ) throw()
      : ISingularProperty( parent, nme,
                           pedanticProtocol ? TRAITS::protocol() : "",
                           TRAITS::dataType(), ctx ) {}

    // EXCEPTIONS
    ITypedSingularProperty( const IObject &parent,
                            const std::string &nme,
                            ThrowExceptionFlag tf,
                            bool pedanticProtocol = false,
                            const IContext &ctx = IContext() )
      : ISingularProperty( parent, nme, tf,
                           pedanticProtocol ? TRAITS::protocol() : "",
                           TRAITS::dataType(), ctx ) {}

    //-*************************************************************************
    // Construction from PropertyInfo
    // NO EXCEPTIONS
    ITypedSingularProperty( const IObject &parent,
                            PropertyInfo pinfo,
                            bool pedanticProtocol = false,
                            const IContext &ctx = IContext() )
        throw()
      : ISingularProperty( parent,
                           overridePinfo( pinfo, pedanticProtocol ), ctx ) {}
    
    // EXCEPTIONS
    ITypedSingularProperty( const IObject &parent,
                            PropertyInfo pinfo,
                            ThrowExceptionFlag tf,
                            bool pedanticProtocol = false,
                            const IContext &ctx = IContext() )
      : ISingularProperty( parent,
                           overridePinfo( pinfo, pedanticProtocol ),
                           tf, ctx ) {}

    //-*************************************************************************
    // Construction from index
    // NO EXCEPTIONS
    ITypedSingularProperty( const IObject &parent,
                            size_t propIdx,
                            bool pedanticProtocol = false,
                            const IContext &ctx = IContext() )
        throw()
      : ISingularProperty( parent,
                           overridePinfo( parent.propertyInfo( propIdx ),
                                          pedanticProtocol ), ctx ) {}
    
    // EXCEPTIONS
    ITypedSingularProperty( const IObject &parent,
                            size_t propIdx,
                            ThrowExceptionFlag tf,
                            bool pedanticProtocol = false,
                            const IContext &ctx = IContext() )
      : ISingularProperty( parent,
                           overridePinfo( parent.propertyInfo( propIdx ),
                                          pedanticProtocol ), tf, ctx ) {}

    //-*************************************************************************
    // DATA ACCESS - CAN THROW FOR NOW
    //-*************************************************************************
private:
    static value_type convertVoid( const void *ptr ) throw()
    {
        return ptr ? *(( const value_type * )ptr)
            : TRAITS::defaultValue();
    }

public:
    value_type get() const
    {
        return convertVoid( ISingularProperty::restSample() );
    }

    value_type getAnim( size_t samp ) const
    {
        return convertVoid( ISingularProperty::animSample( samp ) );
    }

    value_type getAnimLowerBound( seconds_t secs ) const
    {
        return convertVoid( 
                 ISingularProperty::animSampleLowerBound( secs ) );
    }
    value_type getAnimUpperBound( seconds_t secs ) const
    {
        return convertVoid( 
                 ISingularProperty::animSampleUpperBound( secs ) );
    }
    
    value_type getAnimLowerBound( const Time &time ) const
    {
        return convertVoid( 
            ISingularProperty::animSampleLowerBound( time ) );
    }
    value_type getAnimUpperBound( const Time &time ) const
    {
        return convertVoid( 
            ISingularProperty::animSampleUpperBound( time ) );
    }
    
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MULTI PROPERTIES
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
class IMultiProperty : public IProperty
{
private:
    void init( const IObject &parent,
               const std::string &nme,
               const std::string &prot,
               const DataType &dtype,
               const IContext &ctx );
    
public:
    typedef IMultiProperty this_type;
    
    //-*************************************************************************
    // Default, copy & assignment
    IMultiProperty() throw()
      : IProperty() {}

    IMultiProperty( const IMultiProperty &copy ) throw()
      : IProperty( copy ),
        m_body( copy.m_body ) {}

    IMultiProperty &operator=( const IMultiProperty &copy ) throw()
    {
        IProperty::operator=( copy );
        m_body = copy.m_body;
        return *this;
    }
    
    //-*************************************************************************
    // Construction from name and optional protocol spec and optional
    // datatype spec.
    // NO EXCEPTIONS
    IMultiProperty( const IObject &parent,
                    const std::string &nme,
                    const std::string &prot = "",
                    const DataType &dtype = DataType(),
                    const IContext &ctx = IContext() )
        throw();

    // EXCEPTIONS
    IMultiProperty( const IObject &parent,
                    const std::string &nme,
                    ThrowExceptionFlag,
                    const std::string &prot = "",
                    const DataType &dtype = DataType(),
                    const IContext &ctx = IContext() );

    //-*************************************************************************
    // Construction from PropertyInfo
    // NO EXCEPTIONS
    IMultiProperty( const IObject &parent,
                    PropertyInfo pinfo,
                    const IContext &ctx = IContext() )
        throw();
    
    // EXCEPTIONS
    IMultiProperty( const IObject &parent,
                    PropertyInfo pinfo,
                    ThrowExceptionFlag,
                    const IContext &ctx = IContext() );

    //-*************************************************************************
    // Construction from index
    // NO EXCEPTIONS
    IMultiProperty( const IObject &parent,
                    size_t propIdx,
                    const IContext &ctx = IContext() )
        throw();
    
    // EXCEPTIONS
    IMultiProperty( const IObject &parent,
                    size_t propIdx,
                    ThrowExceptionFlag,
                    const IContext &ctx = IContext() );

    //-*************************************************************************
    // Releases, but does not close.
    virtual ~IMultiProperty() throw() {}

    //-*************************************************************************
    // INHERITED STUFF
    //-*************************************************************************
    virtual bool valid() const throw();
    virtual std::string name() const throw();
    virtual std::string fullPathName() const throw();
    virtual std::string protocol() const throw();
    virtual DataType dataType() const throw();
    virtual PropertyType propertyType() const throw();
    virtual bool isAnimated() const throw();
    virtual const ITimeSampling *timeSampling() const throw();

    // Return values. These are stored inside the body
    // so it is okay.
    // These can throw for now.
    // Return values. These are stored inside the body
    // so it is okay.
    ISample restSample() const;
    ISample animSample( size_t samp ) const;
    ISample animSampleLowerBound( seconds_t secs ) const;
    ISample animSampleUpperBound( seconds_t secs ) const;
    ISample animSampleLowerBound( const Time &time ) const;
    ISample animSampleUpperBound( const Time &time ) const;
    
    // No exceptions
    virtual void close() throw();

    // Exceptions
    virtual void close( ThrowExceptionFlag );

    virtual void release() throw();

    SharedIMultiPropertyBody body() const throw() { return m_body; }
    virtual SharedIPropertyBody asPropertyBody() const throw();
    
protected:
    SharedIMultiPropertyBody m_body;
};

//-*****************************************************************************
//-*****************************************************************************
template <class TRAITS>
class ITypedMultiProperty : public IMultiProperty
{
private:
    // Handy helper
    static PropertyInfo overridePinfo( PropertyInfo pinfo, bool pedantic )
    {
        if ( !pinfo ) { return pinfo; }
        else
        {
            return MakePropertyInfo(
                pinfo->name,
                pedantic ? TRAITS::protocol() : "",
                kMultiProperty,
                TRAITS::dataType() );
        }
    }
public:
    typedef TRAITS traits_type;
    typedef ITypedMultiProperty<TRAITS> this_type;
    typedef typename TRAITS::value_type value_type;
    typedef ITypedSample<value_type> sample_type;

    //-*************************************************************************
    // Default, copy & assignment
    ITypedMultiProperty() throw()
      : IMultiProperty() {}

    ITypedMultiProperty( const this_type &copy ) throw()
      : IMultiProperty( copy ) {}
    
    this_type &operator=( const this_type &copy ) throw()
    {
        IMultiProperty::operator=( copy );
        return *this;
    }

    //-*************************************************************************
    // Construction from name and optional protocol spec and optional
    // datatype spec.
    // NO EXCEPTIONS
    ITypedMultiProperty( const IObject &parent,
                         const std::string &nme,
                         bool pedanticProtocol = false,
                         const IContext &ctx = IContext() ) throw()
      : IMultiProperty( parent, nme,
                        pedanticProtocol ? TRAITS::protocol() : "",
                        TRAITS::dataType(), ctx ) {}

    // EXCEPTIONS
    ITypedMultiProperty( const IObject &parent,
                         const std::string &nme,
                         ThrowExceptionFlag tf,
                         bool pedanticProtocol = false,
                         const IContext &ctx = IContext() )
      : IMultiProperty( parent, nme, tf,
                        pedanticProtocol ? TRAITS::protocol() : "",
                        TRAITS::dataType(), ctx ) {}

    //-*************************************************************************
    // Construction from PropertyInfo
    // NO EXCEPTIONS
    ITypedMultiProperty( const IObject &parent,
                         PropertyInfo pinfo,
                         bool pedanticProtocol = false,
                         const IContext &ctx = IContext() )
        throw()
      : IMultiProperty( parent,
                        overridePinfo( pinfo, pedanticProtocol ), ctx ) {}
    
    // EXCEPTIONS
    ITypedMultiProperty( const IObject &parent,
                         PropertyInfo pinfo,
                         ThrowExceptionFlag tf,
                         bool pedanticProtocol = false,
                         const IContext &ctx = IContext() )
      : IMultiProperty( parent,
                        overridePinfo( pinfo, pedanticProtocol ),
                        tf, ctx ) {}

    //-*************************************************************************
    // Construction from index
    // NO EXCEPTIONS
    ITypedMultiProperty( const IObject &parent,
                         size_t propIdx,
                         bool pedanticProtocol = false,
                         const IContext &ctx = IContext() )
        throw()
      : IMultiProperty( parent,
                        overridePinfo( parent.propertyInfo( propIdx ),
                                       pedanticProtocol ), ctx ) {}
    
    // EXCEPTIONS
    ITypedMultiProperty( const IObject &parent,
                         size_t propIdx,
                         ThrowExceptionFlag tf,
                         bool pedanticProtocol = false,
                         const IContext &ctx = IContext() )
      : IMultiProperty( parent,
                        overridePinfo( parent.propertyInfo( propIdx ),
                                       pedanticProtocol ), tf, ctx ) {}

    
    sample_type get() const
    {
        return sample_type( IMultiProperty::restSample().handle() );
    }
    sample_type getAnim( size_t samp ) const
    {
        return sample_type( IMultiProperty::animSample( samp ).handle() );
    }
    sample_type getAnimLowerBound( seconds_t secs ) const
    {
        return sample_type(
            IMultiProperty::animSampleLowerBound( secs ).handle() );
    }
    sample_type getAnimUpperBound( seconds_t secs ) const
    {
        return sample_type(
            IMultiProperty::animSampleUpperBound( secs ).handle() );
    }
    sample_type getAnimLowerBound( const Time &time ) const
    {
        return sample_type(
            IMultiProperty::animSampleLowerBound( time ).handle() );
    }
    sample_type getAnimUpperBound( const Time &time ) const
    {
        return sample_type(
            IMultiProperty::animSampleUpperBound( time ).handle() );
    }
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// TYPEDEFS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

typedef ITypedSingularProperty<Uint8TPTraits>           IUcharProperty;
typedef ITypedSingularProperty<Int8TPTraits>            ICharProperty;
typedef ITypedSingularProperty<Uint16TPTraits>          IUshortProperty;
typedef ITypedSingularProperty<Int16TPTraits>           IShortProperty;
typedef ITypedSingularProperty<Uint32TPTraits>          IUintProperty;
typedef ITypedSingularProperty<Int32TPTraits>           IIntProperty;
typedef ITypedSingularProperty<Uint64TPTraits>          IUlongProperty;
typedef ITypedSingularProperty<Int64TPTraits>           ILongProperty;
typedef ITypedSingularProperty<Float32TPTraits>         IFloatProperty;
typedef ITypedSingularProperty<Float64TPTraits>         IDoubleProperty;

typedef ITypedSingularProperty<V2sTPTraits>             IV2sProperty;
typedef ITypedSingularProperty<V2iTPTraits>             IV2iProperty;
typedef ITypedSingularProperty<V2fTPTraits>             IV2fProperty;
typedef ITypedSingularProperty<V2dTPTraits>             IV2dProperty;

typedef ITypedSingularProperty<V3sTPTraits>             IV3sProperty;
typedef ITypedSingularProperty<V3iTPTraits>             IV3iProperty;
typedef ITypedSingularProperty<V3fTPTraits>             IV3fProperty;
typedef ITypedSingularProperty<V3dTPTraits>             IV3dProperty;

typedef ITypedSingularProperty<Box2sTPTraits>           IBox2sProperty;
typedef ITypedSingularProperty<Box2iTPTraits>           IBox2iProperty;
typedef ITypedSingularProperty<Box2fTPTraits>           IBox2fProperty;
typedef ITypedSingularProperty<Box2dTPTraits>           IBox2dProperty;

typedef ITypedSingularProperty<Box3sTPTraits>           IBox3sProperty;
typedef ITypedSingularProperty<Box3iTPTraits>           IBox3iProperty;
typedef ITypedSingularProperty<Box3fTPTraits>           IBox3fProperty;
typedef ITypedSingularProperty<Box3dTPTraits>           IBox3dProperty;

typedef ITypedSingularProperty<M33fTPTraits>            IM33fProperty;
typedef ITypedSingularProperty<M33dTPTraits>            IM33dProperty;
typedef ITypedSingularProperty<M44fTPTraits>            IM44fProperty;
typedef ITypedSingularProperty<M44dTPTraits>            IM44dProperty;

typedef ITypedSingularProperty<QuatfTPTraits>           IQuatfProperty;
typedef ITypedSingularProperty<QuatdTPTraits>           IQuatdProperty;

//-*****************************************************************************
typedef ITypedMultiProperty<Uint8TPTraits>             IUcharArrayProperty;
typedef ITypedMultiProperty<Int8TPTraits>              ICharArrayProperty;
typedef ITypedMultiProperty<Uint16TPTraits>            IUshortArrayProperty;
typedef ITypedMultiProperty<Int16TPTraits>             IShortArrayProperty;
typedef ITypedMultiProperty<Uint32TPTraits>            IUintArrayProperty;
typedef ITypedMultiProperty<Int32TPTraits>             IIntArrayProperty;
typedef ITypedMultiProperty<Uint64TPTraits>            IUlongArrayProperty;
typedef ITypedMultiProperty<Int64TPTraits>             ILongArrayProperty;
typedef ITypedMultiProperty<Float32TPTraits>           IFloatArrayProperty;
typedef ITypedMultiProperty<Float64TPTraits>           IDoubleArrayProperty;

typedef ITypedMultiProperty<V2sTPTraits>               IV2sArrayProperty;
typedef ITypedMultiProperty<V2iTPTraits>               IV2iArrayProperty;
typedef ITypedMultiProperty<V2fTPTraits>               IV2fArrayProperty;
typedef ITypedMultiProperty<V2dTPTraits>               IV2dArrayProperty;

typedef ITypedMultiProperty<V3sTPTraits>               IV3sArrayProperty;
typedef ITypedMultiProperty<V3iTPTraits>               IV3iArrayProperty;
typedef ITypedMultiProperty<V3fTPTraits>               IV3fArrayProperty;
typedef ITypedMultiProperty<V3dTPTraits>               IV3dArrayProperty;

typedef ITypedMultiProperty<Box2sTPTraits>             IBox2sArrayProperty;
typedef ITypedMultiProperty<Box2iTPTraits>             IBox2iArrayProperty;
typedef ITypedMultiProperty<Box2fTPTraits>             IBox2fArrayProperty;
typedef ITypedMultiProperty<Box2dTPTraits>             IBox2dArrayProperty;

typedef ITypedMultiProperty<Box3sTPTraits>             IBox3sArrayProperty;
typedef ITypedMultiProperty<Box3iTPTraits>             IBox3iArrayProperty;
typedef ITypedMultiProperty<Box3fTPTraits>             IBox3fArrayProperty;
typedef ITypedMultiProperty<Box3dTPTraits>             IBox3dArrayProperty;

typedef ITypedMultiProperty<M33fTPTraits>              IM33fArrayProperty;
typedef ITypedMultiProperty<M33dTPTraits>              IM33dArrayProperty;
typedef ITypedMultiProperty<M44fTPTraits>              IM44fArrayProperty;
typedef ITypedMultiProperty<M44dTPTraits>              IM44dArrayProperty;

typedef ITypedMultiProperty<QuatfTPTraits>             IQuatfArrayProperty;
typedef ITypedMultiProperty<QuatdTPTraits>             IQuatdArrayProperty;

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

namespace Input {

typedef IUcharProperty             UcharProperty;
typedef ICharProperty              CharProperty;
typedef IUshortProperty            UshortProperty;
typedef IShortProperty             ShortProperty;
typedef IUintProperty              UintProperty;
typedef IIntProperty               IntProperty;
typedef IUlongProperty             UlongProperty;
typedef ILongProperty              LongProperty;
typedef IFloatProperty             FloatProperty;
typedef IDoubleProperty            DoubleProperty;

typedef IV2sProperty               V2sProperty;
typedef IV2iProperty               V2iProperty;
typedef IV2fProperty               V2fProperty;
typedef IV2dProperty               V2dProperty;

typedef IV3sProperty               V3sProperty;
typedef IV3iProperty               V3iProperty;
typedef IV3fProperty               V3fProperty;
typedef IV3dProperty               V3dProperty;

typedef IBox2sProperty             Box2sProperty;
typedef IBox2iProperty             Box2iProperty;
typedef IBox2fProperty             Box2fProperty;
typedef IBox2dProperty             Box2dProperty;

typedef IBox3sProperty             Box3sProperty;
typedef IBox3iProperty             Box3iProperty;
typedef IBox3fProperty             Box3fProperty;
typedef IBox3dProperty             Box3dProperty;

typedef IM33fProperty              M33fProperty;
typedef IM33dProperty              M33dProperty;
typedef IM44fProperty              M44fProperty;
typedef IM44dProperty              M44dProperty;

typedef IQuatfProperty             QuatfProperty;
typedef IQuatdProperty             QuatdProperty;

//-*****************************************************************************
typedef IUcharArrayProperty             UcharArrayProperty;
typedef ICharArrayProperty              CharArrayProperty;
typedef IUshortArrayProperty            UshortArrayProperty;
typedef IShortArrayProperty             ShortArrayProperty;
typedef IUintArrayProperty              UintArrayProperty;
typedef IIntArrayProperty               IntArrayProperty;
typedef IUlongArrayProperty             UlongArrayProperty;
typedef ILongArrayProperty              LongArrayProperty;
typedef IFloatArrayProperty             FloatArrayProperty;
typedef IDoubleArrayProperty            DoubleArrayProperty;

typedef IV2sArrayProperty               V2sArrayProperty;
typedef IV2iArrayProperty               V2iArrayProperty;
typedef IV2fArrayProperty               V2fArrayProperty;
typedef IV2dArrayProperty               V2dArrayProperty;

typedef IV3sArrayProperty               V3sArrayProperty;
typedef IV3iArrayProperty               V3iArrayProperty;
typedef IV3fArrayProperty               V3fArrayProperty;
typedef IV3dArrayProperty               V3dArrayProperty;

typedef IBox2sArrayProperty             Box2sArrayProperty;
typedef IBox2iArrayProperty             Box2iArrayProperty;
typedef IBox2fArrayProperty             Box2fArrayProperty;
typedef IBox2dArrayProperty             Box2dArrayProperty;

typedef IBox3sArrayProperty             Box3sArrayProperty;
typedef IBox3iArrayProperty             Box3iArrayProperty;
typedef IBox3fArrayProperty             Box3fArrayProperty;
typedef IBox3dArrayProperty             Box3dArrayProperty;

typedef IM33fArrayProperty              M33fArrayProperty;
typedef IM33dArrayProperty              M33dArrayProperty;
typedef IM44fArrayProperty              M44fArrayProperty;
typedef IM44dArrayProperty              M44dArrayProperty;

typedef IQuatfArrayProperty             QuatfArrayProperty;
typedef IQuatdArrayProperty             QuatdArrayProperty;

} // End namespace Input

} // End namespace AlembicAsset

#endif
