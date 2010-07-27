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

#ifndef _Alembic_Asset_IPropertyBody_h_
#define _Alembic_Asset_IPropertyBody_h_

#include <Alembic/Asset/Body/FoundationBody.h>
#include <Alembic/Asset/Body/IObjectBody.h>
#include <Alembic/Asset/Body/ITimeSamplingBody.h>
#include <Alembic/Asset/Body/DataTypeTuple.h>
#include <Alembic/Asset/Body/IContextBody.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
//-*****************************************************************************
// IProperty - the virtual base class underlying all properties on objects.
// Properties have two primary varieties - Singular Properties, which are
// those for which there is only one value at any given time, and
// Multi Properties, which are those for which the 'value' at a given time
// is an array. 
//-*****************************************************************************
class IPropertyBody
{
protected:
    IPropertyBody( SharedIContextBody context,
                   const IObjectBody &parentObject,
                   PropertyInfo pinfo );

public:
    virtual ~IPropertyBody() throw();
    
    const std::string &name() const { return m_name; }
    std::string fullPathName() const;
    const std::string &protocol() const { return m_protocol; }
    DataType dataType() const { return m_dataTypeTuple.dataType(); }
    
    virtual PropertyType propertyType() const = 0;
    bool isSingular() const
    { return ( propertyType() == kSingularProperty ); }
    bool isMulti() const
    { return ( propertyType() == kMultiProperty ); }
    
    // Animation is a basic property.
    bool isAnimated() const { return ( bool )m_timeSampling; }

    // Will kaboom if called on a non-animated property.
    const ITimeSampling *timeSampling() const
    {
        //AAST_ASSERT( ( bool )m_timeSampling,
        //              "Cannot request timeSampling for a static property" );
        return m_timeSampling.get();
    }

    // This will finalize (close and finish writing)
    // The destructor calls this unless it has been called.
    virtual void close();

    // Return the group 
    const H5G &group() const { return m_group; }

protected:
    std::string m_name;
    std::string m_protocol;
    DataTypeTuple m_dataTypeTuple;
    H5G m_group;
    
    SharedITimeSampling m_timeSampling;
};

//-*****************************************************************************
typedef boost::shared_ptr<IPropertyBody> SharedIPropertyBody;

//-*****************************************************************************
//-*****************************************************************************
// Singular Property - values returned as void pointers to single values.
//-*****************************************************************************
class ISingularPropertyBody : public IPropertyBody
{
protected:
    void init( SharedIContextBody context, const IObjectBody &parentObject );
    
public:
    ISingularPropertyBody( SharedIContextBody context,
                           const IObjectBody &parentObject,
                           PropertyInfo pinfo )
      : IPropertyBody( context, parentObject, pinfo )
    {
        init( context, parentObject );
    }

    ISingularPropertyBody( SharedIContextBody context,
                           const IObjectBody &parentObject,
                           const std::string &name,
                           const std::string &checkProt = "",
                           const DataType &dtype = DataType() )
      : IPropertyBody( context, parentObject,
                       MakePropertyInfo( name, checkProt,
                                         kSingularProperty,
                                         dtype ) )
    {
        init( context, parentObject );
    }

    virtual ~ISingularPropertyBody() throw();
    
    virtual PropertyType propertyType() const;
    
    // Get the rest sample, passed by pointer.
    const void *restSample() const;

    // Get the anim samples.
    // If not animated, rest sample is returned.
    const void *animSample( size_t samp ) const;

    virtual void close();

protected:
    Bytes m_restSampleBuffer;
    ISlabCache::Handle m_animSamplesSlab;
};

//-*****************************************************************************
typedef boost::shared_ptr<ISingularPropertyBody> SharedISingularPropertyBody;

//-*****************************************************************************
//-*****************************************************************************
// Multi Property - values are slabs.
// Slab Values loaded on demand only.
// Because of this, a context is required for any data access.
//-*****************************************************************************
class IMultiPropertyBody : public IPropertyBody
{
protected:
    void init( SharedIContextBody context,
               const IObjectBody &parentObject );
public:
    IMultiPropertyBody( SharedIContextBody context,
                        const IObjectBody &parentObject,
                        PropertyInfo pinfo )
      : IPropertyBody( context, parentObject, pinfo )
    {
        init( context, parentObject );
    }

    IMultiPropertyBody( SharedIContextBody context,
                        const IObjectBody &parentObject,
                        const std::string &name,
                        const std::string &checkProt = "",
                        const DataType &dtype = DataType() )
      : IPropertyBody( context, parentObject,
                       MakePropertyInfo( name, checkProt,
                                         kMultiProperty,
                                         dtype ) )
    {
        init( context, parentObject );
    }

    virtual ~IMultiPropertyBody() throw();

    virtual PropertyType propertyType() const;

    // Rest sample is a slab. Only loaded on request.
    ISlabCache::Handle restSample( SharedIContextBody context ) const;

    // Animated samples are also slabs.
    // This is not a cheap function! Data is loaded on demand.
    // A context is needed to acquire cached data.
    ISlabCache::Handle animSample( SharedIContextBody context,
                                   size_t samp ) const;
    
    // This will finalize (close and finish writing)
    // The destructor calls this unless it has been called.
    virtual void close();

protected:
    H5G m_animSampsGroup;
    ISlabCache::Handle m_animSlabIDs;
};


//-*****************************************************************************
typedef boost::shared_ptr<IMultiPropertyBody> SharedIMultiPropertyBody;

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// HERE IS HOW YOU READ THEM, IF YOU DON'T KNOW WHAT TO EXPECT.
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
SharedIPropertyBody
ReadProperty( SharedIContextBody context,
              const IObjectBody &parentObject,
              const std::string &propertyName,
              const std::string &checkProt = "" );

inline SharedIPropertyBody
ReadProperty( SharedIContextBody context,
              const IObjectBody &parentObject,
              PropertyInfo pinfo )
{
    AAST_ASSERT( ( bool )pinfo,
                 "IPropertyBody ReadProperty() passed invalid pinfo" );
    return ReadProperty( context, parentObject, pinfo->name,
                         pinfo->protocol );
}

} // End namespace Asset
} // End namespace Alembic

#endif
