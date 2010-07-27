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

#include <AlembicAsset/Body/OParentBody.h>

namespace AlembicAsset {

//-*****************************************************************************
OParentBody::~OParentBody() throw()
{
    // CJH - children group will close itself on destruction. This could
    // be happening because of an exception somewhere, so we can't
    // know for sure that anything bad is going on internally.
    // Everything we have will clean itself up.
    if ( m_childrenGroup.valid() )
    {
        try
        {
            this->close();
            assert( !m_childrenGroup.valid() );
        }
        catch ( std::exception &e )
        {
            std::cerr << "ERROR: OParentBody::~OParentBody() "
                      << "EXCEPTION: " << e.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr << "ERROR: OParentBody::~OParentBody() "
                      << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
void OParentBody::close()
{
    // This would most likely occur if close was called multiple times.
    // This is an error. 
    AAH5_ASSERT( m_childrenGroup.valid(),
                 "OParentBody::close() ERROR: Invalid group." << std::endl );

    // Actually close the group.
    m_childrenGroup.close();
}

//-*****************************************************************************
void OParentBody::parentInit( const H5G &parentGroup,
                              const std::string &childGroupName )
{
    // Create!!!
    AlembicHDF5::CreationOrderPlist copl;
    m_childrenGroup.create( parentGroup, childGroupName,
                            H5P_DEFAULT,
                            copl );
}

} // End namespace AlembicAsset


