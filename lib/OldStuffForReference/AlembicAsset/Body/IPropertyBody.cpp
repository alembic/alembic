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

#include <AlembicAsset/Body/IPropertyBody.h>
#include <AlembicAsset/Body/IDataBody.h>
#include <AlembicAsset/Body/ISlabBody.h>

namespace AlembicAsset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// BASE IPROPERTY
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
IPropertyBody::IPropertyBody( SharedIContextBody context,
                              const IObjectBody &parentObject,
                              PropertyInfo pinfo )
{
    AAH5_ASSERT( ( bool )context,
                 "IPropertyBody::IPropertyBody() passed invalid context" );
    AAH5_ASSERT( ( bool )pinfo,
                 "IPropertyBody::IPropertyBody() passed invalid pinfo" );
    m_name = pinfo->name;
    
    // Read this property's group.
    m_group.open( parentObject.propertyGroup(), m_name );
    
    // Read protocol
    std::string prot = pinfo->protocol;
    m_protocol = ReadString( m_group, "protocol" );
    AAH5_ASSERT( prot == "" || prot == m_protocol,
                 "Invalid protocol for property: " << m_name
                 << ", expected: " << prot
                 << ", got: " << m_protocol );

    // Get datatype
    if ( pinfo->dtype.pod == kUnknownPOD )
    {
        DataType dtype = ReadDataTypeFromAttrs( m_group,
                                                "dataTypePOD",
                                                "dataTypeExtent" );
        m_dataTypeTuple = context->dataTypeTupleMap().find( dtype );
    }
    else
    {
        m_dataTypeTuple = context->dataTypeTupleMap().find( pinfo->dtype );
    }

    // Figure out if animated.
    bool animated = m_group.attributeExists( "timeSamplingType" );

    // Read the slabs if needed.
    // Animation always requires a slabs group because
    // the animated values are (at the least) stored in a slab.
    if ( animated )
    {
        m_timeSampling = ReadTimeSampling( context, m_group );
    }
}

//-*****************************************************************************
IPropertyBody::~IPropertyBody() throw()
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
            std::cerr << "ERROR: IPropertyBody::~IPropertyBody() "
                      << "EXCEPTION: " << e.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr << "ERROR: IPropertyBody::~IPropertyBody() "
                      << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
std::string IPropertyBody::fullPathName() const
{
    char buf[1024];
    ssize_t len = H5Iget_name( m_group.id(), buf, 1024 );
    std::vector<char> bufStrBuf( ( size_t )( len + 10 ) );
    len = H5Iget_name( m_group.id(), &bufStrBuf.front(), len+1 );
    std::string bufStr = ( const char * )( &bufStrBuf.front() );
    AAH5_ASSERT( len > 0,
                 "IObject::fullPathName() H5Iget_name failed." );
    return bufStr;
}

//-*****************************************************************************
void IPropertyBody::close()
{
    AAH5_ASSERT( m_group.valid(),
                 "IPropertyBody::close() called on invalid IPropertyBody" );

    m_group.close();
    m_timeSampling.reset();
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// SINGULAR
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
ISingularPropertyBody::~ISingularPropertyBody() throw()
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
            std::cerr
                << "ERROR: ISingularPropertyBody::~ISingularPropertyBody() "
                << "EXCEPTION: " << e.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr
                << "ERROR: ISingularPropertyBody::~ISingularPropertyBody() "
                << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
void ISingularPropertyBody::init( SharedIContextBody context,
                                  const IObjectBody &parentObject )
{
    AAH5_ASSERT( ( bool )context,
                 "ISingularPropertyBody::init() passed invalid context" );
    
    // Read the rest value.
    // Make sure we have enough space to hold the rest value.
    m_restSampleBuffer.resize( m_dataTypeTuple.dataType().bytes() );

    // Read singular rest sample, using dataTypeTuple to validate types.
    ReadScalar( m_group,
                "singularRestSample",
                m_dataTypeTuple.nativeH5T(),
                ( void * )&m_restSampleBuffer.front(),

                &m_dataTypeTuple.fileH5T() );

    // If we are animated, read the animated values and keep them.
    // CJH: WHAT ABOUT LOAD ON DEMAND??? Seriously, by doing this here,
    // we're basically saying that every time you load an animated
    // property, you... yeah, just do it.
    if ( m_timeSampling )
    {
        // Okay, we're animated. The animated curve is stored here
        // Load the slab.
        size_t CheckRank = 1;
        m_animSamplesSlab = FindSlab( context, m_group,
                                      "singularAnimSamples",
                                      
                                      // Error checking
                                      &m_dataTypeTuple.dataType(),
                                      &CheckRank,
                                      NULL );
    }
}

//-*****************************************************************************
PropertyType ISingularPropertyBody::propertyType() const
{
    return kSingularProperty;
}

//-*****************************************************************************
// Get the rest value, passed by pointer.
const void *ISingularPropertyBody::restSample() const
{
    return ( const void * )&m_restSampleBuffer.front();
}

//-*****************************************************************************
// Get the anim values
const void *ISingularPropertyBody::animSample( size_t samp ) const
{
    // If not animated, return the rest value.
    if ( !m_timeSampling ) { return restSample(); }

    // Check existence of anim values
    AAH5_ASSERT( ( bool )m_animSamplesSlab,
                 "Corrupt property - timeSampling exists, "
                 "but no anim values" );
    
    AAH5_ASSERT( samp < (*m_timeSampling).numSamples(),
                 "Cannot get out-of-range anim sample: " << samp );

    const char *rd = ( const char * )( (*m_animSamplesSlab).slab().rawData() );
    return ( const void * )( rd +
                             ( m_dataTypeTuple.dataType().bytes() * samp ) );
}

//-*****************************************************************************
void ISingularPropertyBody::close()
{
    IPropertyBody::close();
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MULTI PROPERTY
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
IMultiPropertyBody::~IMultiPropertyBody() throw()
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
            std::cerr
                << "ERROR: IMultiPropertyBody::~IMultiPropertyBody() "
                << "EXCEPTION: " << e.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr
                << "ERROR: IMultiPropertyBody::~IMultiPropertyBody() "
                << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}


//-*****************************************************************************
void IMultiPropertyBody::init( SharedIContextBody context,
                               const IObjectBody &parent )
{
    AAH5_ASSERT( ( bool )context,
                 "IMultiPropertyBody::init() passed invalid context" );

    // Get anim samples context, but only if animated.
    if ( m_timeSampling )
    {
        m_animSampsGroup.open( m_group, "animSamples" );
        size_t CheckRank = 1;
        DataType dtypeSlabID( kUint8POD, 16 );
        m_animSlabIDs = FindSlab( context, m_group,
                                  "multiAnimSlabIDs",

                                  // Error checking
                                  &dtypeSlabID,
                                  &CheckRank,
                                  NULL );
    }
}

//-*****************************************************************************
PropertyType IMultiPropertyBody::propertyType() const
{
    return kMultiProperty;
}

//-*****************************************************************************
ISlabCache::Handle
IMultiPropertyBody::restSample( SharedIContextBody context ) const
{
    AAH5_ASSERT( ( bool )context,
                 "IMultiPropertyBody::restSample() passed invalid context" );

    return FindSlab( context, m_group, "multiRestSample",

                     // Data checks
                     &m_dataTypeTuple.dataType(), NULL, NULL );
}

//-*****************************************************************************
ISlabCache::Handle
IMultiPropertyBody::animSample( SharedIContextBody context,
                                size_t samp ) const
{
    AAH5_ASSERT( ( bool )context,
                 "IMultiPropertyBody::animSample() passed invalid context" );
    
    // If not animated, return the rest value.
    if ( !m_timeSampling ) { return restSample( context ); }

    // Check existence of anim values
    AAH5_ASSERT( ( bool )m_animSlabIDs,
                 "Corrupt property - timeSampling exists, "
                 "but no anim slab ids" );
    
    AAH5_ASSERT( m_animSampsGroup.valid(),
                 "Corrupt property - timeSampling exists, "
                 "but no anim samps group" );

    AAH5_ASSERT( m_animSlabIDs->slab().size() ==
                 (*m_timeSampling).numSamples(),
                 "Corrupt property - timeSampling has different samples "
                 "than animSlabIDs" );
    
    AAH5_ASSERT( samp < (*m_timeSampling).numSamples(),
                 "Cannot get out-of-range anim sample: " << samp );

    Slab::HashID slabID =
        (( const Slab::HashID * )m_animSlabIDs->slab().rawData())[samp];
    
    return FindSlabFromHashID( context, m_animSampsGroup,
                               ( boost::format( "slab_samp_%012d" )
                                 % ( int )samp ).str(),
                               slabID,
                               
                               // Error checking.
                               &m_dataTypeTuple.dataType(),
                               NULL, NULL );
}

//-*****************************************************************************
void IMultiPropertyBody::close()
{
    if ( m_animSampsGroup.valid() )
    {
        m_animSampsGroup.close();
    }
    IPropertyBody::close();
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// GLOBAL READ FUNCTION
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
SharedIPropertyBody
ReadProperty( SharedIContextBody context,
              const IObjectBody &parentObject,
              const std::string &propertyName,
              const std::string &checkProt )
{
    AAH5_ASSERT( ( bool )context,
                 "ReadProperty() passed invalid context" );
    
    
    // Here's the group for the property.
    const H5G &propGroup = parentObject.propertyGroup();

    // Read the datatype.
    DataType dtype;
    ReadDataTypeFromAttrs( propGroup, propertyName,
                           "dataTypePOD", "dataTypeExtent" );

    // Figure out whether or not the property is singular or multiple
    char propertyType = ReadEnumChar( propGroup, propertyName,
                                      "propertyType" );

    AAH5_ASSERT( propertyType == ( char )kSingularProperty ||
                 propertyType == ( char )kMultiProperty,
                 "Unrecognized property type: "
                 << ( int )propertyType
                 << " for property: " << propertyName );

    PropertyInfo pinfo = MakePropertyInfo( propertyName,
                                           checkProt,
                                           ( PropertyType )propertyType,
                                           dtype );

    if ( propertyType == kSingularProperty )
    {
        SharedISingularPropertyBody ppp(
            new ISingularPropertyBody( context,
                                       parentObject,
                                       pinfo ) );
        return ppp;
    }
    else
    {
        SharedIMultiPropertyBody ppp(
            new IMultiPropertyBody( context,
                                    parentObject,
                                    pinfo ) );
        return ppp;
    }
}

} // End namespace AlembicAsset

