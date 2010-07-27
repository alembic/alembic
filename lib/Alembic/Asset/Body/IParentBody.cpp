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

#include <Alembic/Asset/Body/IParentBody.h>
#include <Alembic/Asset/Body/IDataBody.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// ObjectInfo Reading
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
struct ObjectInfoVisitor
{
    ObjectInfoVisitor( const H5G &grp,
                       std::vector<ObjectInfo> &ivec )
      : group( grp ),
        infoVec( ivec ) {}
    
    const H5G &group;
    std::vector<ObjectInfo> &infoVec;
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
    
    ObjectInfoVisitor *visitor = ( ObjectInfoVisitor * )op_data;
    AAST_ASSERT( visitor != NULL,
                  "Null ObjectInfoVisitor passed to iteration op." );
    
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
    
    // Create object info
    visitor->infoVec.push_back( MakeObjectInfo( name,
                                                protocol ) );

    // Keep iterating!
    return 0;
}

//-*****************************************************************************
void ReadObjectInfo( const H5G &group,
                     std::vector<ObjectInfo> &infoVec )
{
    ObjectInfoVisitor visitor( group, infoVec );
    
    hsize_t idx = 0;
    herr_t status = H5Literate( group.id(),
                                H5_INDEX_CRT_ORDER,
                                H5_ITER_INC,
                                &idx,
                                VisitAllLinksCB,
                                ( void * )&visitor );
    AAST_ASSERT( status >= 0,
                  "ReadObjectInfo() H5Literate failed." );
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// IParentBody Class
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
IParentBody::~IParentBody() throw()
{
    if ( m_childrenGroup.valid() )
    {
        try
        {
            this->close();
            assert( !m_childrenGroup.valid() );
        }
        catch ( std::exception &e )
        {
            std::cerr << "ERROR: IParentBody::~IParentBody() "
                      << "EXCEPTION: " << e.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr << "ERROR: IParentBody::~IParentBody() "
                      << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
void IParentBody::parentInit( const H5G &parentGroup,
                              const std::string &childGroupName )
{
    // Open group for children and read object info into it
    m_childrenGroup.open( parentGroup, childGroupName );
    ReadObjectInfo( m_childrenGroup, m_childrenInfo );
}

//-*****************************************************************************
void IParentBody::close()
{
    if ( m_childrenGroup.valid() )
    {
        m_childrenGroup.close();
    }
    m_childrenInfo.erase( m_childrenInfo.begin(),
                          m_childrenInfo.end() );
}

} // End namespace Asset
} // End namespace Alembic

