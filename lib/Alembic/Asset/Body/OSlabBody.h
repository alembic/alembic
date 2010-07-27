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

#ifndef _Alembic_Asset_Body_OSlabBody_h_
#define _Alembic_Asset_Body_OSlabBody_h_

#include <Alembic/Asset/Body/FoundationBody.h>
#include <Alembic/Asset/Body/OContextBody.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
// Writing a slab always returns a refID.
// You need to provide a full path to the slabs group, which would
// presumably look like: "/Bob/Children/Jane/Children/Alistair/Children/

OSlabMapBody::SlabReference
WriteSlab( OContextBody &context,
           const H5G &parentGrp,
           const std::string &slabName,
           const void *data,
           const DataType &dtype,
           const Dimensions &dims,
           const Slab::HashID &hashID );

inline OSlabMapBody::SlabReference
WriteSlab( OContextBody &context,
           const H5G &parentGrp,
           const std::string &slabName,
           const void *data,
           const DataType &dtype,
           const Dimensions &dims )
{
    return WriteSlab( context, parentGrp, slabName,
                      data, dtype, dims,
                      CalculateHashID( dtype, dims, data ) );
}

inline OSlabMapBody::SlabReference
WriteSlab( OContextBody &context,
           const H5G &parentGrp,
           const std::string &slabName,
           const Slab &slab )
{
    return WriteSlab( context, parentGrp, slabName,
                      slab.rawData(), slab.dataType(), slab.dimensions() );
}

OSlabMapBody::SlabReference
WriteSlabFromRef( OContextBody &context,
                  const H5G &parentGrp,
                  const std::string &slabName,
                  const OSlabMapBody::SlabReference &sref );

} // End namespace Asset
} // End namespace Alembic

#endif


