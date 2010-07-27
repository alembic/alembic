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

#include <Alembic/Asset/Body/ITimeSamplingBody.h>
#include <Alembic/Asset/Body/ISlabBody.h>
#include <Alembic/Asset/Body/IDataBody.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
static SharedIUniformTimeSampling
internalReadUniformTimeSampling( SharedIContextBody context,
                                 const H5G &parentGrp )
{
    H5T checkDouble( H5T_IEEE_F64LE );
    H5T checkULong( H5T_STD_U64LE );
    
    seconds_t firstSampleSeconds = 0.0;
    ReadScalar( parentGrp,
                "timeSamplingUniformFirstSampleSeconds",
                H5T_NATIVE_DOUBLE,
                ( void * )&firstSampleSeconds,
                &checkDouble );

    seconds_t firstSampleInterval = 0.0;
    ReadScalar( parentGrp,
                "timeSamplingUniformFirstSampleInterval",
                H5T_NATIVE_DOUBLE,
                ( void * )&firstSampleInterval,
                &checkDouble );
    
    unsigned long numSamples;
    ReadScalar( parentGrp,
                "timeSamplingUniformNumSamples",
                H5T_NATIVE_ULONG,
                ( void * )&numSamples,
                &checkULong );
    
    AAST_ASSERT( numSamples > 0,
                 "ReadTimeSampling() must have more than zero samples" );

    return MakeSharedIUniformTimeSampling( firstSampleSeconds,
                                           firstSampleInterval,
                                           numSamples );
}

//-*****************************************************************************
static SharedIVariableTimeSampling
internalReadVariableTimeSampling( SharedIContextBody context,
                                  const H5G &parentGrp )
{
    // Variable time sampling just has a list of doubles that represent
    // seconds.
    // We need to read this as a slab.
    DataType CheckDataType( kFloat64POD, 1 );
    size_t CheckRank = 1;
    ISlabCache::Handle slabHnd =
        FindSlab( context, parentGrp,
                  "timeSamplingVariableSampleSeconds",
                  
                  // Error checking
                  &CheckDataType, &CheckRank, NULL );
    
    // make it.
    return MakeSharedIVariableTimeSampling( slabHnd );
}

//-*****************************************************************************
SharedITimeSampling
ReadTimeSampling( SharedIContextBody context,
                  const H5G &parentGrp )
{
    // Check it.
    AAST_ASSERT( ( bool )context,
                 "ReadTimeSampling() passed invalid context" );
    
    
    // First read the time sampling type.
    char timeSamplingType = ReadEnumChar( parentGrp, "timeSamplingType" );
    AAST_ASSERT( timeSamplingType == ( char )kUniformTimeSampling ||
                 timeSamplingType == ( char )kVariableTimeSampling,
                 "ReadTimeSampling(): Unrecognized time sampling type: "
                 << ( int )timeSamplingType );

    if ( timeSamplingType == ( char )kUniformTimeSampling )
    {
        return internalReadUniformTimeSampling( context, parentGrp );
    }
    else
    {
        return internalReadVariableTimeSampling( context, parentGrp );
    }
}

} // End namespace Asset
} // End namespace Alembic
