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
#ifndef __Gto__Header__h__
#define __Gto__Header__h__

#ifdef __APPLE__
#ifndef PLATFORM_DARWIN
#define PLATFORM_DARWIN
#endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <string>
#include <utility>
#include <algorithm>
#include <iosfwd>

namespace Gto {

//
//  Types and MACROS
//

#define GTO_MAGIC       0x29f
#define GTO_MAGICl      0x9f020000
#define GTO_MAGIC_TEXT  0x47544f61
#define GTO_MAGIC_TEXTl 0x614f5447
#define GTO_VERSION     3

typedef unsigned int        uint32;
typedef int                 int32;
typedef unsigned short      uint16;
typedef unsigned char       uint8;
typedef float               float32;
typedef double              float64;

//
//  File Header
//

struct Header
{
    static const unsigned int MagicText = GTO_MAGIC_TEXT;
    static const unsigned int CigamText = GTO_MAGIC_TEXTl;
    static const unsigned int Magic     = GTO_MAGIC;
    static const unsigned int Cigam     = GTO_MAGICl;

    uint32        magic;
    uint32        numStrings;
    uint32        numObjects;
    uint32        version;
    uint32        flags;                    // undetermined;
};

//
//  Object Header
//

struct ObjectHeader
{
    uint32        name;             // string
    uint32        protocolName;     // string
    uint32        protocolVersion;
    uint32        numComponents;
    uint32        pad;
};

struct ObjectHeader_v2
{
    uint32        name;             // string
    uint32        protocolName;     // string
    uint32        protocolVersion;
    uint32        numComponents;
};

//
//  Componenent Header
//

enum ComponentFlags
{
    Transposed  = 1 << 0,
    Matrix      = 1 << 1,
};

struct ComponentHeader
{
    uint32        name;             // string
    uint32        numProperties;
    uint32        flags;
    uint32        interpretation;
    uint32        pad;
};

struct ComponentHeader_v2
{
    uint32        name;             // string
    uint32        numProperties;
    uint32        flags;
};

//
//  Property Header
//

enum DataType
{
    Int,                // int32
    Float,              // float32
    Double,             // float64
    Half,               // float16
    String,             // string table indices
    Boolean,            // bit
    Short,              // uint16
    Byte,               // uint8

    NumberOfDataTypes,
    ErrorType
};


struct PropertyHeader
{
    uint32        name;             // string
    uint32        size;
    uint32        type;
    uint32        width;
    uint32        interpretation;   // string
    uint32        pad;
};

struct PropertyHeader_v2
{
    uint32        name;
    uint32        size;
    uint32        type;
    uint32        width;
};

} // Gto

#endif // __Gto__Header__h__
