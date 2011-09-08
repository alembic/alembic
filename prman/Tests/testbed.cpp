//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <iostream>
#include <string>
#include <sstream>
#include <dlfcn.h>
#include <ri.h>
#include <cstdlib>


void usage()
{
    std::cerr << "runs a prman procedural and spits out rib" << std::endl;
    std::cerr << "usage:" << std::endl;
    std::cerr << "testbed pathto.so [args...]" << std::endl;
}

typedef RtPointer (*convertProc_t)(RtString);
typedef RtVoid (*subdivideProc_t)(RtPointer, RtFloat);
typedef RtVoid (*freeProc_t)(RtPointer);


void *getsym(void *handle, const char *name)
{
    void *sym = dlsym(handle, name);
    if (sym == NULL)
    {
        std::cerr << "can't find symbol: " << name << " : ";
        std::cerr << dlerror() << std::endl;
        exit(4);
    }
    return sym;
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage();
        return 1;
    }
    
    std::ostringstream buffer;
    
    if (argc > 2)
    {
        for (int i = 2; i < argc; ++i)
        {
            buffer << argv[i] << " ";
        }
    }
    
    void *handle = dlopen(argv[1], RTLD_NOW);
    if (handle == NULL)
    {
        std::cerr << "couldn't open: " << argv[1] << std::endl;
        std::cerr << dlerror() << std::endl;
        usage();
        return 2;
    }
    
    convertProc_t convert_parameters_p =
            (convertProc_t)getsym(handle, "ConvertParameters");
    subdivideProc_t subdivide_p =
            (subdivideProc_t)getsym(handle, "Subdivide");
    freeProc_t free_p =
            (freeProc_t)getsym(handle, "Free");
    
    RiBegin(NULL);
    
    RtPointer clientData = (*convert_parameters_p)(
            const_cast<RtString>(buffer.str().c_str()));
    (*subdivide_p)(clientData, 1);
    (*free_p)(clientData);
    
    RiEnd();
    
    
}

//placeholder functions usually available only within prman
extern "C" {
    
    int RxOption(void)
    {
        /* return failure */
        return -1;
    }
    
    int RxAttribute(void)
    {
        /* return failure */
        return -1;
    }
    
    int RxRendererInfo(void)
    {
        /* return failure */
        return -1;
    }
    
    void RibParseBuffer(void)
    {
    }
}
