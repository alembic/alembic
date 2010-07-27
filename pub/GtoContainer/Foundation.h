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

#ifndef _GtoContainer_Foundation_h_
#define _GtoContainer_Foundation_h_

#include <Gto/Header.h>
#include <sstream>
#include <string>

#ifdef GTO_SUPPORT_HALF
#include <half.h>
#endif

namespace GtoContainer {

//-*****************************************************************************
#ifndef GTO_INTERPRET_DEFAULT
#define GTO_INTERPRET_DEFAULT "default"
#endif

//-*************************************************************************
//  Types
//-*************************************************************************
    
// Used for generic memory manipulation on POD types.
enum Layout
{
    CompoundLayout,
    FloatLayout,
    IntLayout,
    BoolLayout,
    DoubleLayout,
    HalfLayout,
    ByteLayout,
    ShortLayout,
    StringLayout
};

//-*************************************************************************
// Static functions for converting between layouts and Gto types.
inline Gto::DataType layoutToGtoType( Layout l )
{
    switch ( l )
    {
    case FloatLayout:   return Gto::Float;
    case HalfLayout:    return Gto::Half;
    case DoubleLayout:  return Gto::Double;
    case ShortLayout:   return Gto::Short;
    case IntLayout:     return Gto::Int;
    case ByteLayout:    return Gto::Byte;
    case StringLayout:  return Gto::String;
    }

    return Gto::Byte;
}

//-*****************************************************************************
inline Layout gtoTypeToLayout( Gto::DataType d )
{
    switch ( d )
    {
    case Gto::Float:  return FloatLayout;
    case Gto::Half:   return HalfLayout;
    case Gto::Double: return DoubleLayout;
    case Gto::Short:  return ShortLayout;
    case Gto::Int:    return IntLayout;
    case Gto::Byte:   return ByteLayout;
    case Gto::String: return StringLayout;
    }

    return ByteLayout;
}

//-*****************************************************************************
inline std::string layoutName( Layout lyt, size_t width = 1 )
{
    std::stringstream sstr;

    switch ( lyt )
    {
    case FloatLayout:   sstr << "float"; break;
    case HalfLayout:    sstr << "half"; break;
    case DoubleLayout:  sstr << "double"; break;
    case ShortLayout:   sstr << "short"; break;
    case IntLayout:     sstr << "int"; break;
    case StringLayout:  sstr << "string"; break;
    default:
    case ByteLayout:    sstr << "byte"; break;
    }

    if ( width > 1 )
    {
        sstr << "[" << ( int )width << "]";
    }

    return sstr.str();
}

//-*****************************************************************************
// Handy assignable, passable tuple class.
template <class T, size_t N>
struct TxN
{
    TxN() {}
    explicit TxN( const T &t )
    {
        for ( size_t i = 0; i < N; ++i ) { data[i] = t; }
    }
    TxN( const TxN &tn )
    {
        for ( size_t i = 0; i < N; ++i ) { data[i] = tn.data[i]; }
    }
    TxN &operator=( const TxN &tn )
    {
        for ( size_t i = 0; i < N; ++i ) { data[i] = tn.data[i]; }
        return *this;
    }
    TxN &operator=( const T &t )
    {
        for ( size_t i = 0; i < N; ++i ) { data[i] = t; }
        return *this;
    }
    
    T &operator[]( size_t i ) { return data[i]; }
    const T &operator[]( size_t i ) const { return data[i]; }
    
    T data[N];
};

} // End namespace GtoContainer

//-*****************************************************************************
//-*****************************************************************************
// THE TYPEDEFS
// You can use this to declare the typedefs in your namespace, which is handy
//-*****************************************************************************
//-*****************************************************************************
#ifdef GTO_SUPPORT_HALF

#define GTOC_DECLARE_TYPES                      \
                                                \
typedef Gto::uint32 uint32;                     \
typedef Gto::int32 int32;                       \
typedef Gto::uint16 uint16;                     \
typedef Gto::uint8 uint8;                       \
typedef uint8 byte;                             \
                                                \
typedef TxN<double,2> double2;                  \
typedef TxN<double,3> double3;                  \
typedef TxN<double,4> double4;                  \
typedef TxN<double,6> double6;                  \
typedef TxN<double,9> double9;                  \
typedef TxN<double,16> double16;                \
                                                \
typedef TxN<float,2> float2;                    \
typedef TxN<float,3> float3;                    \
typedef TxN<float,4> float4;                    \
typedef TxN<float,6> float6;                    \
typedef TxN<float,9> float9;                    \
typedef TxN<float,16> float16;                  \
                                                \
typedef TxN<half,2> half2;                      \
typedef TxN<half,3> half3;                      \
typedef TxN<half,4> half4;                      \
typedef TxN<half,6> half6;                      \
typedef TxN<half,9> half9;                      \
typedef TxN<half,16> half16;                    \
                                                \
typedef TxN<int32,2> int2;                      \
typedef TxN<int32,3> int3;                      \
typedef TxN<int32,4> int4;                      \
typedef TxN<int32,6> int6;                      \
                                                \
typedef TxN<uint16,2> ushort2;                  \
typedef TxN<uint16,3> ushort3;                  \
typedef TxN<uint16,4> ushort4;                  \
typedef TxN<uint16,6> ushort6;                  \
                                                \
typedef TxN<byte,2> byte2;                      \
typedef TxN<byte,3> byte3;                      \
typedef TxN<byte,4> byte4;                      \
typedef TxN<byte,6> byte6;

#else

#define GTOC_DECLARE_TYPES                      \
                                                \
typedef Gto::uint32 uint32;                     \
typedef Gto::int32 int32;                       \
typedef Gto::uint16 uint16;                     \
typedef Gto::uint8 uint8;                       \
typedef uint8 byte;                             \
                                                \
typedef TxN<double,2> double2;                  \
typedef TxN<double,3> double3;                  \
typedef TxN<double,4> double4;                  \
typedef TxN<double,6> double6;                  \
typedef TxN<double,9> double9;                  \
typedef TxN<double,16> double16;                \
                                                \
typedef TxN<float,2> float2;                    \
typedef TxN<float,3> float3;                    \
typedef TxN<float,4> float4;                    \
typedef TxN<float,6> float6;                    \
typedef TxN<float,9> float9;                    \
typedef TxN<float,16> float16;                  \
                                                \
typedef TxN<int32,2> int2;                      \
typedef TxN<int32,3> int3;                      \
typedef TxN<int32,4> int4;                      \
typedef TxN<int32,6> int6;                      \
                                                \
typedef TxN<uint16,2> ushort2;                  \
typedef TxN<uint16,3> ushort3;                  \
typedef TxN<uint16,4> ushort4;                  \
typedef TxN<uint16,6> ushort6;                  \
                                                \
typedef TxN<byte,2> byte2;                      \
typedef TxN<byte,3> byte3;                      \
typedef TxN<byte,4> byte4;                      \
typedef TxN<byte,6> byte6;

#endif

namespace GtoContainer {

//-*****************************************************************************
GTOC_DECLARE_TYPES;

} // End namespace GtoContainer

#endif
