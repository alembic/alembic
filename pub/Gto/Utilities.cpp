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
#include <Gto/Utilities.h>
#include <assert.h>
#include <fstream>
#ifdef GTO_SUPPORT_ZIP
#include <zlib.h>
#endif
#ifdef GTO_SUPPORT_HALF
#include <half.h>
#endif

namespace Gto {
using namespace std;

static unsigned int Csizes[] = 
{
    Int,        sizeof(int32),
    Float,      sizeof(float32),
    Double,     sizeof(float64),
#ifdef GTO_SUPPORT_HALF
    Half,       sizeof(half),
#else
    Half,       sizeof(float32) / 2,
#endif
    String,     sizeof(uint32),
    Boolean,    sizeof(uint8), 
    Short,      sizeof(uint16),
    Byte,       sizeof(uint8)
};

size_t
dataSize(Gto::uint32 type)
{
    return Csizes[type * 2 + 1];
}

const char* 
typeName(Gto::DataType t)
{
    switch (t)
    {
      case Float:   return "float";
      case Boolean: return "bool";
      case Double:  return "double";
      case Int:     return "int";
      case Short:   return "short";
      case Byte:    return "byte";
      case String:  return "string";
      case Half:    return "half";
      default:
          abort();
          return "";
    }
}

bool 
isNumber(Gto::DataType t)
{
    return t != String && t != ErrorType;
}

Number 
asNumber(void* data, Gto::DataType t)
{
    Number n;
    n.type = t;

    switch (t)
    {
      case Float: 
          n._double = *reinterpret_cast<float*>(data); 
          n.type = Float;
          break;
      case Double: 
          n._double = *reinterpret_cast<double*>(data);
          n.type = Float;
          break;
#ifdef GTO_SUPPORT_HALF
      case Half: 
          n._double = double(*reinterpret_cast<half*>(data));
          n.type = Float;
          break;
#endif
      case Int: 
          n._int = int(*reinterpret_cast<int*>(data));
          n.type = Int;
          break;
      case Short: 
          n._int = int(*reinterpret_cast<short*>(data));
          n.type = Int;
          break;
      case Byte: 
          n._int = int(*reinterpret_cast<unsigned char*>(data));
          n.type = Int;
          break;
      default:
          n.type = ErrorType;
    }

    return n;
}

bool isGTOFile(const char* infile)
{
    Header header;

#ifdef GTO_SUPPORT_ZIP
    if (gzFile file = gzopen(infile, "rb"))
    {
        if (gzread(file, &header, sizeof(header)) != sizeof(Header))
        {
            gzclose(file);
            return false;
        }
    }
#else
    ifstream file(infile);
    if (!file) return false;
    if (file.readsome((char*)&header, sizeof(Header)) != sizeof(Header)) return false;
    if (file.fail()) return false;
#endif

    return  header.magic == GTO_MAGIC ||
            header.magic == GTO_MAGICl ||
            header.magic == GTO_MAGIC_TEXT ||
            header.magic == GTO_MAGIC_TEXTl;
}

void
swapWords(void *data, size_t size)
{
    struct bytes { char c[4]; };

    bytes* ip = reinterpret_cast<bytes*>(data);

    for (size_t i=0; i<size; i++)
    {
        bytes temp = ip[i];
        ip[i].c[0] = temp.c[3];
        ip[i].c[1] = temp.c[2];
        ip[i].c[2] = temp.c[1];
        ip[i].c[3] = temp.c[0];
    }
}

void
swapShorts(void *data, size_t size)
{
    struct bytes { char c[2]; };

    bytes* ip = reinterpret_cast<bytes*>(data);

    for (size_t i=0; i<size; i++)
    {
        bytes temp = ip[i];
        ip[i].c[0] = temp.c[1];
        ip[i].c[1] = temp.c[0];
    }
}


} // Gto
