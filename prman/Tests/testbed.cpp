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
