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

#include <AlembicAsset/Body/OObjectBody.h>
#include <AlembicAsset/Body/ODataBody.h>

namespace AlembicAsset {

//-*****************************************************************************
OObjectBody::OObjectBody( const H5G &parentGroup,
                          SharedOContextBody ctx,
                          const std::string &nme,
                          const std::string &prot )
  : OParentBody( ctx )
{
    init( parentGroup, ctx, nme, prot );
}

//-*****************************************************************************
OObjectBody::OObjectBody( const OParentBody &opb,
                          const std::string &nme,
                          const std::string &prot )
  : OParentBody( opb.context() )
{
    init( opb.childrenGroup(), opb.context(), nme, prot );
}

//-*****************************************************************************
OObjectBody::~OObjectBody() throw()
{
    if ( m_propertyGroup.valid() &&
         m_group.valid() )
    {
        try
        {
            OObjectBody::close();
            assert( !m_propertyGroup.valid() );
            assert( !m_group.valid() );
        }
        catch ( std::exception &e )
        {
            std::cerr << "ERROR: OObjectBody::~OObjectBody() "
                      << "EXCEPTION: " << e.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr << "ERROR: OObjectBody::~OObjectBody() "
                      << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
void OObjectBody::init( const H5G &parentGroup,
                        SharedOContextBody ctx,
                        const std::string &nme,
                        const std::string &prot )
{
    m_name = nme;
    m_protocol = prot;

    AlembicHDF5::CreationOrderPlist copl;
    
    m_group.create( parentGroup, m_name, H5P_DEFAULT, copl );
    
    m_propertyGroup.create( m_group, "Properties", H5P_DEFAULT, copl );

    WriteString( m_group, "protocol", m_protocol );

    parentInit( m_group, "Children" );
}

//-*****************************************************************************
void OObjectBody::close()
{
    AAH5_ASSERT( m_propertyGroup.valid() && m_group.valid(),
                 "OObjectBody::close() invalid objects. Multiple closings."
                 << std::endl
                 << "NAME: " << m_name << std::endl
                 << "PROTOCOL: " << m_protocol << std::endl );
    m_propertyGroup.close();
    m_group.close();
    OParentBody::close();
}

} // End namespace AlembicAsset
