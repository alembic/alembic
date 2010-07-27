//
//  Copyright (c) 2009, Tweak Software
//  All rights reserved.
// 
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
//     * Neither the name of the Tweak Software nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
// 
//  THIS SOFTWARE IS PROVIDED BY Tweak Software ''AS IS'' AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL Tweak Software BE LIABLE FOR
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
//  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//

#include <GtoContainer/StdProperties.h>

namespace GtoContainer {

//-*****************************************************************************
#define PROPERTY_INSTANCE( PTYPENAME, VALUE_TYPE, LYT, WDTH, NTRP, DFLT ) \
do                                                                        \
{                                                                         \
    Meta ## PTYPENAME * meta = new Meta ## PTYPENAME ;                    \
    metas.push_back( meta );                                              \
}                                                                         \
while( 0 )

//-*****************************************************************************
void AppendStdMetaProperties( MetaProperties &metas )
{
    //-*************************************************************************
    // The regulars
    PROPERTY_INSTANCE( DoubleProperty, double, DoubleLayout, 1,
                       GTO_INTERPRET_DEFAULT, 0.0 );
    PROPERTY_INSTANCE( FloatProperty, float, FloatLayout, 1,
                       GTO_INTERPRET_DEFAULT, 0.0f );
#ifdef GTO_SUPPORT_HALF
    PROPERTY_INSTANCE( HalfProperty, half, HalfLayout, 1,
                       GTO_INTERPRET_DEFAULT, ( half )0.0f );
#endif
    PROPERTY_INSTANCE( IntProperty, int32, IntLayout, 1,
                       GTO_INTERPRET_DEFAULT, 0 );
    PROPERTY_INSTANCE( UshortProperty, uint16, ShortLayout, 1,
                       GTO_INTERPRET_DEFAULT, 0 );
    PROPERTY_INSTANCE( ByteProperty, byte, ByteLayout, 1,
                       GTO_INTERPRET_DEFAULT, 0 );
    PROPERTY_INSTANCE( StringProperty, std::string, StringLayout, 1,
                       GTO_INTERPRET_DEFAULT, std::string( "" ) );

    //-*************************************************************************
    // The doubles
    PROPERTY_INSTANCE( Double2Property, double2, DoubleLayout, 2,
                       GTO_INTERPRET_DEFAULT, double2( 0.0 ) );
    PROPERTY_INSTANCE( Double3Property, double3, DoubleLayout, 3,
                       GTO_INTERPRET_DEFAULT, double3( 0.0 ) );
    PROPERTY_INSTANCE( Double4Property, double4, DoubleLayout, 4,
                       GTO_INTERPRET_DEFAULT, double4( 0.0 ) );
    PROPERTY_INSTANCE( Double6Property, double6, DoubleLayout, 6,
                       GTO_INTERPRET_DEFAULT, double6( 0.0 ) );
    PROPERTY_INSTANCE( Double9Property, double9, DoubleLayout, 9,
                       GTO_INTERPRET_DEFAULT, double9( 0.0 ) );
    PROPERTY_INSTANCE( Double16Property, double16, DoubleLayout, 16,
                       GTO_INTERPRET_DEFAULT, double16( 0.0 ) );

    //-*************************************************************************
    // The floats
    PROPERTY_INSTANCE( Float2Property, float2, FloatLayout, 2,
                       GTO_INTERPRET_DEFAULT, float2( 0.0 ) );
    PROPERTY_INSTANCE( Float3Property, float3, FloatLayout, 3,
                       GTO_INTERPRET_DEFAULT, float3( 0.0 ) );
    PROPERTY_INSTANCE( Float4Property, float4, FloatLayout, 4,
                       GTO_INTERPRET_DEFAULT, float4( 0.0 ) );
    PROPERTY_INSTANCE( Float6Property, float6, FloatLayout, 6,
                       GTO_INTERPRET_DEFAULT, float6( 0.0 ) );
    PROPERTY_INSTANCE( Float9Property, float9, FloatLayout, 9,
                       GTO_INTERPRET_DEFAULT, float9( 0.0 ) );
    PROPERTY_INSTANCE( Float16Property, float16, FloatLayout, 16,
                       GTO_INTERPRET_DEFAULT, float16( 0.0 ) );

    //-*************************************************************************
    // The halfs
#ifdef GTO_SUPPORT_HALF
    PROPERTY_INSTANCE( Half2Property, half2, HalfLayout, 2,
                       GTO_INTERPRET_DEFAULT, half2( 0.0 ) );
    PROPERTY_INSTANCE( Half3Property, half3, HalfLayout, 3,
                       GTO_INTERPRET_DEFAULT, half3( 0.0 ) );
    PROPERTY_INSTANCE( Half4Property, half4, HalfLayout, 4,
                       GTO_INTERPRET_DEFAULT, half4( 0.0 ) );
    PROPERTY_INSTANCE( Half6Property, half6, HalfLayout, 6,
                       GTO_INTERPRET_DEFAULT, half6( 0.0 ) );
    PROPERTY_INSTANCE( Half9Property, half9, HalfLayout, 9,
                       GTO_INTERPRET_DEFAULT, half9( 0.0 ) );
    PROPERTY_INSTANCE( Half16Property, half16, HalfLayout, 16,
                       GTO_INTERPRET_DEFAULT, half16( 0.0 ) );
#endif

    //-*************************************************************************
    // The ints
    PROPERTY_INSTANCE( Int2Property, int2, IntLayout, 2,
                       GTO_INTERPRET_DEFAULT, int2( 0 ) );
    PROPERTY_INSTANCE( Int3Property, int3, IntLayout, 3,
                       GTO_INTERPRET_DEFAULT, int3( 0 ) );
    PROPERTY_INSTANCE( Int4Property, int4, IntLayout, 4,
                       GTO_INTERPRET_DEFAULT, int4( 0 ) );
    PROPERTY_INSTANCE( Int6Property, int6, IntLayout, 6,
                       GTO_INTERPRET_DEFAULT, int6( 0 ) );

    //-*************************************************************************
    // The ushorts
    PROPERTY_INSTANCE( Ushort2Property, ushort2, ShortLayout, 2,
                       GTO_INTERPRET_DEFAULT, ushort2( 0 ) );
    PROPERTY_INSTANCE( Ushort3Property, ushort3, ShortLayout, 3,
                       GTO_INTERPRET_DEFAULT, ushort3( 0 ) );
    PROPERTY_INSTANCE( Ushort4Property, ushort4, ShortLayout, 4,
                       GTO_INTERPRET_DEFAULT, ushort4( 0 ) );
    PROPERTY_INSTANCE( Ushort6Property, ushort6, ShortLayout, 6,
                       GTO_INTERPRET_DEFAULT, ushort6( 0 ) );

    //-*************************************************************************
    // The bytes
    PROPERTY_INSTANCE( Byte2Property, byte2, ByteLayout, 2,
                       GTO_INTERPRET_DEFAULT, byte2( 0 ) );
    PROPERTY_INSTANCE( Byte3Property, byte3, ByteLayout, 3,
                       GTO_INTERPRET_DEFAULT, byte3( 0 ) );
    PROPERTY_INSTANCE( Byte4Property, byte4, ByteLayout, 4,
                       GTO_INTERPRET_DEFAULT, byte4( 0 ) );
    PROPERTY_INSTANCE( Byte6Property, byte6, ByteLayout, 6,
                       GTO_INTERPRET_DEFAULT, byte6( 0 ) );
}

} // End namespace GtoContainer
