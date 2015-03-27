//-*****************************************************************************
//
// Copyright (c) 2009-2015,
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

#include <ai.h>
#include <iostream>
#include <string>
#include <sstream>

void usage()
{
    std::cerr << "runs an arnold procedural and spits out ass" << std::endl;
    std::cerr << "usage:" << std::endl;
    std::cerr << "testbed pathto.so [args...]" << std::endl;
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

    AiBegin();

    AiMsgSetConsoleFlags(AI_LOG_WARNINGS );//| AI_LOG_BACKTRACE);

    AtNode* procedural = AiNode("procedural");
    AiNodeSetStr(procedural, "name", "testbed");
    AiNodeSetStr(procedural, "dso", argv[1]);
    AiNodeSetStr(procedural, "data", buffer.str().c_str());
    AiNodeSetInt(procedural, "visibility", AI_RAY_CAMERA);

    AiASSWrite("/dev/stdout", AI_NODE_ALL, true);

    AiEnd();
}
