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
