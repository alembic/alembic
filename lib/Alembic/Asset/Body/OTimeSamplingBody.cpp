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

#include <Alembic/Asset/Body/OTimeSamplingBody.h>
#include <Alembic/Asset/Body/ODataBody.h>
#include <Alembic/Asset/Body/OSlabBody.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
static void
internalWriteUniformTimeSampling( OContextBody &context,
                                  H5G &parentGrp,
                                  const TimeSamplingInfo &tinfo,
                                  size_t numSamples )
{
    WriteScalar( parentGrp,
                 "timeSamplingUniformFirstSampleSeconds",
                 H5T_NATIVE_DOUBLE,
                 H5T_IEEE_F64LE,
                 ( const void * )&( tinfo.firstSampleSeconds ) );
    
    WriteScalar( parentGrp,
                 "timeSamplingUniformFirstSampleInterval",
                 H5T_NATIVE_DOUBLE,
                 H5T_IEEE_F64LE,
                 ( const void * )&( tinfo.firstSampleInterval ) );
    
    // Write number of samples.
    unsigned long nsUL = ( unsigned long )numSamples;
    WriteScalar( parentGrp,
                 "timeSamplingUniformNumSamples",
                 H5T_NATIVE_ULONG,
                 H5T_STD_U64LE,
                 ( const void * )&nsUL );
}

//-*****************************************************************************
static void
internalWriteVariableTimeSampling( OContextBody &context,
                                   const H5G &parentGrp,
                                   const SecondsArray &seconds )
{
    // Variable time sampling just has a list of doubles that represent
    // seconds, which needs to be stored as a slab.
    Dimensions dims( 1 );
    dims[0] = seconds.size();
    assert( seconds.size() > 0 );
    WriteSlab( context, parentGrp,
               "timeSamplingVariableSampleSeconds",
               ( const void * )&seconds.front(),
               DataType( kFloat64POD, 1 ),
               dims );
}

//-*****************************************************************************
void 
WriteTimeSampling( OContextBody &context,
                   H5G &parentGrp,
                   const TimeSamplingInfo &tinfo,
                   const SecondsArray &seconds )
{
    // Verify inputs.
    AAST_ASSERT( tinfo.type == kUniformTimeSampling ||
                 tinfo.type == kVariableTimeSampling,
                 "WriteTimeSampling(): Unrecognized time sampling type" );
    AAST_ASSERT( parentGrp.valid(),
                 "WriteTimeSampling(): Invalid parent group." );
    AAST_ASSERT( seconds.size() > 0,
                 "WriteTimeSampling(): Zero time samples." );
    
    // It is an error to write 
    // First write the time sampling type.
    char timeSamplingType = ( char )tinfo.type;
    WriteEnumChar( parentGrp, "timeSamplingType", timeSamplingType );

    if ( timeSamplingType == ( char )kUniformTimeSampling )
    {
        internalWriteUniformTimeSampling( context, parentGrp,
                                          tinfo, seconds.size() );
    }
    else
    {
        internalWriteVariableTimeSampling( context, parentGrp, seconds );
    }
}

} // End namespace Asset
} // End namespace Alembic
