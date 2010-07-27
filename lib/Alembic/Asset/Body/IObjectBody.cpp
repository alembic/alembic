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

#include <Alembic/Asset/Body/IObjectBody.h>
#include <Alembic/Asset/Body/IDataBody.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// PropertyInfo Reading
//-*****************************************************************************
//-*****************************************************************************
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
// Visits (without recursing) all the links inside a group. Returns 0
// no matter what.
static herr_t VisitAllLinksCB( hid_t group,
                               const char *name,
                               const H5L_info_t *linfo,
                               void *op_data )
{
    hid_t       obj;
    
    PropertyInfoVisitor *visitor = ( PropertyInfoVisitor * )op_data;
    AAST_ASSERT( visitor != NULL,
                 "Null PropertyInfoVisitor passed to iteration op." );
    
    // Verify that it's a hard link.
    AAST_ASSERT( linfo->type == H5L_TYPE_HARD,
                 "Only Hard Links allowed in groups of Alembic::Objects" );
    
    // Stat the object
    H5O_info_t  oinfo;
    herr_t status = H5Oget_info_by_name( visitor->group.id(), name,
                                         &oinfo, H5P_DEFAULT );
    AAST_ASSERT( status >= 0,
                 "H50get_info_by_name failed for object: " << name );

    // Verify that it's a group
    AAST_ASSERT( oinfo.type == H5O_TYPE_GROUP,
                 "Only groups allowed in groups of Alembic::Objects" );
    
    // Get the protocol of the object being read.
    std::string protocol = ReadString( visitor->group, name, "protocol" );

    // Get the POD type of the object being read.
    DataType dtype = ReadDataTypeFromAttrs( visitor->group, name,
                                            "dataTypePOD",
                                            "dataTypeExtent" );

    // Get the property type.
    PropertyType ptype = ( PropertyType )ReadEnumChar( visitor->group,
                                                       name,
                                                       "propertyType" );
    AAST_ASSERT( ptype == ( char )kSingularProperty ||
                 ptype == ( char )kMultiProperty,
                 "Unrecognized property type: "
                 << ( int )ptype
                 << " for property: " << name );
    
    // Create object info
    visitor->infoVec.push_back( MakePropertyInfo( name,
                                                  protocol,
                                                  ptype,
                                                  dtype ) );

    // Keep iterating!
    return 0;
}

//-*****************************************************************************
void ReadPropertyInfo( const H5G &group,
                       std::vector<PropertyInfo> &infoVec )
{
    PropertyInfoVisitor visitor( group, infoVec );
    
    hsize_t idx = 0;
    herr_t status = H5Literate( group.id(),
                                H5_INDEX_CRT_ORDER,
                                H5_ITER_INC,
                                &idx,
                                VisitAllLinksCB,
                                ( void * )&visitor );
    AAST_ASSERT( status >= 0,
                 "ReadPropertyInfo() H5Literate failed." );
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// IObjectBody CLASS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// For construction given a name and a check protocol
IObjectBody::IObjectBody( const H5G &grp,
                          const std::string &nme,
                          const std::string &checkProt )
{
    init( grp, nme, checkProt );
}

//-*****************************************************************************
// For construction given ObjectInfo
IObjectBody::IObjectBody( const H5G &grp,
                          ObjectInfo oinfo )
{
    AAST_ASSERT( ( bool )oinfo,
                 "IObjectBody::IObjectBody() passed invalid oinfo" );
    init( grp, oinfo->name, oinfo->protocol );
}

//-*****************************************************************************
// For construction given a parent IObjectBody and child index
IObjectBody::IObjectBody( const IParentBody &parent,
                          size_t whichChild )
{
    ObjectInfo cinfo = parent.childInfo( whichChild );
    AAST_ASSERT( ( bool )cinfo,
                 "IObjectBody::IObjectBody() invalid child index" );
    init( parent.childrenGroup(), cinfo->name, cinfo->protocol );
}

//-*****************************************************************************
IObjectBody::~IObjectBody() throw()
{
    if ( m_group.valid() )
    {
        try
        {
            this->close();
            assert( !m_group.valid() );
            assert( !m_propertyGroup.valid() );
        }
        catch ( std::exception &e )
        {
            std::cerr << "ERROR: IObjectBody::~IObjectBody() "
                      << "EXCEPTION: " << e.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr << "ERROR: IObjectBody::~IObjectBody() "
                      << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
void IObjectBody::init( const H5G &parentGroup,
                        const std::string &nme,
                        
                        const std::string &checkProt )
{
    m_name = nme;

    // Open group corresponding to the top of the object.
    m_group.open( parentGroup, nme );

    // Read comments
    m_comments = m_group.comment();

    // Get protocol and check it if requested
    m_protocol = ReadString( m_group, "protocol" );
    AAST_ASSERT( checkProt == "" || checkProt == m_protocol,
                 "Invalid protocol read for object: " << m_name
                 << ", expected: " << checkProt
                 << ", got: " << m_protocol );

    // Open group for properties and read property info into it
    m_propertyGroup.open( m_group, "Properties" );
    ReadPropertyInfo( m_propertyGroup, m_propertyInfo );

    // Init parent group (reads object info for children)
    parentInit( m_group, "Children" );
}

//-*****************************************************************************
void IObjectBody::close()
{
    if ( m_propertyGroup.valid() )
    {
        m_propertyGroup.close();
    }
    if ( m_group.valid() )
    {
        m_group.close();
    }
    
    m_propertyInfo.erase( m_propertyInfo.begin(), m_propertyInfo.end() );
    IParentBody::close();
}

//-*****************************************************************************
std::string IObjectBody::fullPathName() const
{
    char buf[1024];
    ssize_t len = H5Iget_name( m_group.id(), buf, 1024 );
    std::vector<char> bufStrBuf( ( size_t )( len + 10 ) );
    len = H5Iget_name( m_group.id(), &bufStrBuf.front(), len+1 );
    std::string bufStr = ( const char * )( &bufStrBuf.front() );
    AAST_ASSERT( len > 0,
                 "IObject::fullPathName() H5Iget_name failed." );
    return bufStr;
}

} // End namespace Asset
} // End namespace Alembic

