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

#include <AlembicAsset/Body/ISlabBody.h>
#include <AlembicAsset/Body/IDataBody.h>

namespace AlembicAsset {

//-*****************************************************************************
ISlabCache::Handle
FindSlab( SharedIContextBody context,
          const H5G &slabsGrp,
          const std::string &slabName,
          const DataType *CHECK_DTYPE,
          const size_t *CHECK_RANK,
          const Dimensions *CHECK_DIMS )
{
    // Check it!
    AAH5_ASSERT( ( bool )context,
                 "FindSlab() passed invalid context" );

    // Get the slabID attribute WITHOUT OPENING THE DATASET!
    Slab::HashID slabID;
    ReadHashID( context, slabsGrp, slabName, slabID );

    return FindSlabFromHashID( context, slabsGrp, slabName, slabID,
                               CHECK_DTYPE, CHECK_RANK, CHECK_DIMS );
}

//-*****************************************************************************
ISlabCache::Handle
FindSlabFromHashID( SharedIContextBody context,
                    const H5G &slabsGrp,
                    const std::string &slabName,
                    const Slab::HashID &slabID,
                    const DataType *CHECK_DTYPE,
                    const size_t *CHECK_RANK,
                    const Dimensions *CHECK_DIMS )
{
    // Check it!
    AAH5_ASSERT( ( bool )context,
                 "FindSlab() passed invalid context" );

    // See whether or not we already have the thing.
    ISlabCache::Handle sbuf =
        context->slabCache().find( slabID );
    if ( !sbuf )
    {
        // Load the dataset
        H5D slabDset( slabsGrp, slabName );

        // Read the hash id.
        Slab::HashID readSlabID;
        ReadHashID( context, slabDset, readSlabID, "hashID" );
        AAH5_ASSERT( readSlabID == slabID,
                     "Corrupt slab read. Mismatched slab IDs." );

        // Get the datatype and dataspace of this dataset.
        H5T slabDtype( slabDset );
        H5S slabDspace( slabDset );
        
        // Get the slab datatype tuple.
        const DataTypeTuple &slabDtt =
        context->dataTypeTupleMap().find( slabDtype );

        if ( slabDspace.isScalar() )
        {
            // Check rank
            AAH5_ASSERT( CHECK_RANK == NULL ||
                         (*CHECK_RANK) == 0,
                         "Slab read error. Expected rank: "
                         << (*CHECK_RANK)
                         << " but got scalar: 0" );

            // Check dims.
            AAH5_ASSERT( CHECK_DIMS == NULL,
                         "Slab read error. Expected dims: "
                         << (*CHECK_DIMS)
                         << " but got scalar: 0" );

            // Don't actually have to read anything.
            SharedSlab slab = MakeSharedSlab( slabDtt.dataType(), 0 );

            // And, store.
            sbuf = context->slabCache().store( slabID, slab );

            // Check!
            AAH5_ASSERT( ( bool )sbuf,
                         "Something went wrong in slab store in dataset" );
        }
        else
        {
            // Get dimensions from dspace.
            // It is possible for a slab to have zero size!
            // However, it is not possible for a slab to have
            // a scalar rank
            Dimensions dims;
            slabDspace.getSimpleExtentDims( dims );
            AAH5_ASSERT( dims.size() > 0,
                         "Slab has degenerate dataspace" );
            
            // Check rank
            AAH5_ASSERT( CHECK_RANK == NULL ||
                         (*CHECK_RANK) == dims.rank(),
                         "Slab read error. Expected rank: "
                         << (*CHECK_RANK)
                         << " but got: " << dims.rank() );
            
            // Check dims
            AAH5_ASSERT( CHECK_DIMS == NULL ||
                         (*CHECK_DIMS) == dims,
                         "Slab read error. Expected dims: "
                         << (*CHECK_DIMS)
                         << " but got: " << dims );
            
            // Gotta read the slab from the file, which means actually opening
            // the dataset.
            SharedSlab slab = MakeSharedSlab( slabDtt.dataType(), dims );
            
            // Read the data!
            slabDset.readAll( slabDtt.nativeH5T(), (*slab).rawData() );
            
            // And, store.
            sbuf = context->slabCache().store( slabID, slab );
            
            // CHECK
            AAH5_ASSERT( ( bool )sbuf,
                         "Something went wrong in slab read from dataset" );
        }
    }
    else
    {
        // Get stuff out.
        const Slab &slab = sbuf->slab();
        
        // CJH: Should check dims & dtype & rank against sbuf, in case
        // the cache version doesn't match up.
        if ( CHECK_DTYPE )
        {
            AAH5_ASSERT( slab.dataType() == *CHECK_DTYPE,
                         "FindSlab() Inconsistent slab dtypes. "
                         "Expected: " << *CHECK_DTYPE
                         << " but got: " << slab.dataType() );
        }

        if ( CHECK_RANK )
        {
            AAH5_ASSERT( slab.rank() == *CHECK_RANK,
                         "FindSlab() Inconsistent slab ranks. "
                         "Expected: " << *CHECK_RANK
                         << " but got: " << slab.rank() );
        }

        if ( CHECK_DIMS )
        {
            AAH5_ASSERT( slab.dimensions() == *CHECK_DIMS,
                         "FindSlab() Inconsistent slab dimensions. "
                         "Expected: " << *CHECK_DIMS
                         << " but got: " << slab.dimensions() );
        }

    }

    return sbuf;
}

} // End namespace AlembicAsset
