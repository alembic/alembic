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

#include <AlembicAsset/Body/OAssetBody.h>

namespace AlembicAsset {

//-*****************************************************************************
OAssetBody::OAssetBody( const std::string &fname )
  : OParentBody( MakeSharedOContextBody() ),
    m_fileName( fname )
{
    m_file.create( m_fileName.c_str(), H5F_ACC_TRUNC );

    H5G root( m_file, "/", AlembicHDF5::kOpen );
    
    parentInit( root, "Alembic" );
}

//-*****************************************************************************
OAssetBody::~OAssetBody() throw()
{
    // These could throw.
    if ( m_file.valid() )
    {
        try
        {
            OAssetBody::close();
            assert( !m_file.valid() );
        }
        catch ( std::exception &exc )
        {
            std::cerr << "ERROR: OAssetBody::~OAssetBody() "
                      << "Exception: " << exc.what() << std::endl;
        }
        catch ( ... )
        {
            std::cerr << "ERROR: OAssetBody::~OAssetBody() "
                      << "UNKNOWN EXCEPTION" << std::endl;
        }
    }
}

//-*****************************************************************************
int OAssetBody::compressionLevel() const
{
    AAH5_ASSERT( ( bool )m_context,
                 "OAssetBody::compressionLevel() null ocontext" );
    return m_context->compressionLevel();
}

//-*****************************************************************************
void OAssetBody::setCompressionLevel( int level )
{
    AAH5_ASSERT( ( bool )m_context,
                 "OAssetBody::setCompressionLevel() null ocontext" );
    m_context->setCompressionLevel( level );
}

//-*****************************************************************************
void OAssetBody::close()
{
    AAH5_ASSERT( m_file.valid(),
                 "OAssetBody::close() ERROR: Invalid file." << std::endl
                 << "File Name: " << m_fileName << std::endl );
    
    OParentBody::close();

    m_file.flush();
    m_file.close();
}

} // End namespace AlembicAsset

