//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks Inc. and
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
// Industrial Light & Magic, nor the names of their contributors may be used
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




//-*****************************************************************************
struct PropertyInfoVisitor
{
    PropertyInfoVisitor( const H5G &grp,
                         std::vector<PropertyInfo> &ivec )
      : group( grp ),
        infoVec( ivec ) {}
    
    const H5G &group;
    std::vector<PropertyInfo> &infoVec;
};

//-*****************************************************************************
// This is an HDF5 Link Visitor
// Visits (without recursing) all the links inside a group. Returns 0
// no matter what.
static herr_t VisitAllSubPropsCB( hid_t group,
                                  const char *name,
                                  const H5L_info_t *linfo,
                                  void *op_data )
{
    PropertyInfoVisitor *visitor = ( PropertyInfoVisitor * )op_data;
    ABCH5_ASSERT( visitor != NULL,
                  "Null PropertyInfoVisitor passed to iteration op." );
    
    // Verify that it's a hard link.
    ABCH5_ASSERT( linfo->type == H5L_TYPE_HARD,
                  "Only Hard Links allowed in groups of Alembic::Objects" );
    
    // Stat the object
    H5O_info_t  oinfo;
    herr_t status = H5Oget_info_by_name( visitor->group.id(), name,
                                         &oinfo, H5P_DEFAULT );
    ABCH5_ASSERT( status >= 0,
                  "H50get_info_by_name failed for object: " << name );
    
    // Verify that it's a group
    ABCH5_ASSERT( oinfo.type == H5O_TYPE_GROUP,
                  "Only groups allowed in groups of Alembic::Objects" );

    //-*************************************************************************
    // AbcCoreAbstract stuff
    
    // Get the metadata of the object being read.
    std::string metaDataStr = ReadString( visitor->group, name, "metaData" );

    // This could throw. Eeek.
    ABCA::MetaData metaData( metaDataStr );

    // Get the property type.
    ABCA::PropertyType propertyType =
        ( ABCA::PropertyType )ReadEnumChar( visitor->group,
                                            name,
                                            "propertyType" );
    
    ABCH5_ASSERT( propertyType == ABCA::kScalarProperty ||
                  propertyType == ABCA::kArrayProperty ||
                  propertyType == ABCA::kCompoundProperty,
                  
                  "Unrecognized property type: "
                  << ( int )propertyType
                  << " for sub-property: " << name );
    
    //-*************************************************************************
    // Okay, we have a name, a metadata, and a property type.
    // Make one of the three kinds of properties we know how to make.
    if ( ptype == ABCA::kCompoundProperty )
    {
        // We have everything we need for a compound property.
        MakeCompoundProperty( SOMETHING, name, metaData );
    }
    else
    {
        // Simple properties require a DataType and a TimeSamplingType.

        // Get the Data Type of the object being read.
        DataType dataType = ReadDataType( visitor->group, name );

        // Get the Time Sampling Type
        TimeSamplingType timeSamplingType =
            ReadTimeSamplingType( visitor>group, name );

        if ( propertyType == ABCA::kScalarProperty )
        {
            MakeScalarProperty( SOMETHING, name, metaData,
                                dataType, timeSamplingType );
        }
        else
        {
            assert( ptype == ABCA::kArrayProperty );
            MakeArrayProperty( SOMETHING, name, metaData,
                               dataType, timeSamplingType );
        }
    }
    
    // Create object info
    visitor->infoVec.push_back( MakePropertyInfo( name,
                                                  protocol,
                                                  ptype,
                                                  dtype ) );

    // Keep iterating!
    return 0;
}
