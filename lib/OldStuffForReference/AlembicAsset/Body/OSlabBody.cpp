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

#include <AlembicAsset/Body/OSlabBody.h>
#include <AlembicAsset/Body/ODataBody.h>

namespace AlembicAsset {

//-*****************************************************************************
// Writing a slab from a reference.
OSlabMapBody::SlabReference
WriteSlabFromRef( OContextBody &context,
                  const H5G &parentGrp,
                  const std::string &slabName,
                  const OSlabMapBody::SlabReference &sref )
{
    AAH5_ASSERT( ( bool )sref,
                 "WriteSlabFromRef() passed a bogus ref" );

    // We have a reference. Create a link to it.
    herr_t status = H5Lcreate_hard( sref.obj_loc_id,
                                    sref.obj_name.c_str(),
                                    parentGrp.id(),
                                    slabName.c_str(),
                                    H5P_DEFAULT,
                                    H5P_DEFAULT );
    AAH5_ASSERT( status >= 0,
                 "WriteSlab() H5Lcreate_hard failed!" << std::endl
                 << "Slab obj id: " << sref.obj_loc_id << std::endl
                 << "Slab obj name: " << sref.obj_name << std::endl
                 << "Link loc id: " << parentGrp.id() << std::endl
                 << "Link name: " << slabName );
    
    return sref;
}

//-*****************************************************************************
// This is for writing the slab once everything has been verified.
OSlabMapBody::SlabReference
WriteSlab( OContextBody &context,
           const H5G &parentGrp,
           const std::string &slabName,
           const void *data,
           const DataType &dtype,
           const Dimensions &dims,
           const Slab::HashID &slabID )
{
    // See whether or not we've already stored this.
#ifdef DEBUG
    Slab::HashID testSlabID = CalculateHashID( dtype, dims, data );
    AAH5_ASSERT( testSlabID == slabID,
                 "Passed a bogus slabID into WriteSlab()" );
#endif
    OSlabMapBody::SlabReference slabRef = context.slabMap().find( slabID );
    if ( slabRef )
    {
        return WriteSlabFromRef( context, parentGrp, slabName, slabRef );
    }

    // Okay, need to actually store it.
    // It will be a dataset with an internal attribute for storing
    // the hash id.
    // CJH std::string slabName = "slab_";
    // slabName += slabID.str();
    const DataTypeTuple &slabDTT =
        context.dataTypeTupleMap().find( dtype );

    // Make a dataspace from the dimensions
    H5D dset;
    if ( dims.numPoints() > 0 )
    {
        H5S dspace( dims );
        AAH5_ASSERT( dspace.id() >= 0,
                     "WriteSlab() Failed in dataspace constructor" );
        
        if ( context.compressionLevel() >= 0 )
        {
            // Make a compression plist
            AlembicHDF5::DsetGzipCreatePlist
                compressor( dims, context.compressionLevel() );
            AAH5_ASSERT( compressor.id() >= 0,
                         "WriteSlab() Failed in plist constructor" );
            
            // Make the dataset.
            dset.create( parentGrp, slabName, slabDTT.fileH5T(), dspace,
                         H5P_DEFAULT, // link creation
                         compressor,  // Creation
                         H5P_DEFAULT  // Access
                       );
            AAH5_ASSERT( dset.id() >= 0,
                         "WriteSlab() Failed in dataset constructor" );
        }
        else
        {
            dset.create( parentGrp, slabName, slabDTT.fileH5T(), dspace );
            AAH5_ASSERT( dset.id() >= 0,
                         "WriteSlab() Failed in dataset constructor" );
        }

        // Write the data.
        dset.writeAll( slabDTT.nativeH5T(), data );
    }
    else
    {
        // Need to create a "null" dataspace.
        H5S dspace( H5S::kScalar );
        dset.create( parentGrp, slabName, slabDTT.fileH5T(), dspace );
        AAH5_ASSERT( dset.id() >= 0,
                     "WriteSlab() Failed in null dataset constructor" );

        // Write fake data, just to keep HDF5 happy.
        Bytes bytes( slabDTT.dataType().bytes() );
        slabDTT.dataType().setDefaultBytes( ( void * )&bytes.front() );
        dset.writeAll( slabDTT.nativeH5T(), ( void * )&bytes.front() );
    }
    
    // Create hashIDdtype for writing hash id.
    // Okay - slab contains an attribute called "hashID"
    // that provides the hashID
    WriteHashID( context, dset, slabID, "hashID" );

    // Store the full information necessary to recreate
    // this as fileId and full path.
    {
        char buf[1024];
        ssize_t len = H5Iget_name( dset.id(), buf, 1024 );
        std::vector<char> bufStrBuf( ( size_t )( len + 10 ) );
        len = H5Iget_name( dset.id(), &bufStrBuf.front(), len+1 );
        std::string bufStr = ( const char * )( &bufStrBuf.front() );
        AAH5_ASSERT( len > 0,
                     "WriteSlab() H5Iget_name failed." );
        //std::cout << "Writing slab with full name: "
        //          << bufStr << std::endl;

        hid_t fileId = H5Iget_file_id( dset.id() );
        AAH5_ASSERT( fileId >= 0,
                     "WriteSlab() H5Iget_file_id failed." );
        
        slabRef.obj_loc_id = fileId;
        slabRef.obj_name = bufStr;
        context.slabMap().store( slabID, slabRef );
    }

    // Return the reference.
    return slabRef;
}

} // End namespace AlembicAsset
