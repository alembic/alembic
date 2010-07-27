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

#include <Alembic/TakoSPI/TopNode.h>

namespace Alembic {
namespace TakoSPI {
namespace TAKO_SPI_LIB_VERSION_NS {

//-*****************************************************************************
TopNode::~TopNode()
{
    hid_t rootId = iPtr->getHid();
    if ( rootId < 0 )
    {
        throw( std::runtime_error( "Node had invalid hid_t" ) );
    }
            
    hid_t fileId = H5Iget_file_id( rootId );
    if ( fileId < 0 )
    {
        throw( std::runtime_error( "Could not get fileId to close" ) );
    }
            
    // Delete the guy.
    delete iPtr;

#if DEBUG
    int dsetCount = H5Fget_obj_count( fileId, H5F_OBJ_DATASET );
    int grpCount = H5Fget_obj_count( fileId, H5F_OBJ_GROUP );
    int dtypCount = H5Fget_obj_count( fileId, H5F_OBJ_DATATYPE );
    int attrCount = H5Fget_obj_count( fileId, H5F_OBJ_ATTR );

    int objCount = dsetCount + grpCount + dtypCount + attrCount;
            
    if ( objCount != 0 )
    {
        std::string excStr =
            ( boost::format( "Aw dang, man - I was all trying to close "
                             "this HDF5 file, but there are still open objects.\n"
                             "That sucks, man.\n"
                             "Dsets: %d, Grps: %d, "
                             "Dtyps: %d, Attrs: %d" )
              % dsetCount
              % grpCount
              % dtypCount
              % attrCount ).str();
        throw( std::runtime_error( excStr.c_str() ) );
    }
#endif
            
    herr_t status = H5Fflush( fileId, H5F_SCOPE_LOCAL );
    if ( status < 0 )
    {
        throw( std::runtime_error( "Could not flush HDF5 file" ) );
    }
    status = H5Fclose( fileId );
    if ( status < 0 )
    {
        throw( std::runtime_error( "Could not close HDF5 file" ) );
    }
            
    std::cout << "CLOSED FILE ID: " << fileId << std::endl;

}



} // End namespace TAKO_SPI_LIB_VERSION_NS
} // End namespace TakoSPI
} // End namespace Alembic

