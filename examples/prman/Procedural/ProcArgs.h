#ifndef _Alembic_Prman_ProcArgs_h_
#define _Alembic_Prman_ProcArgs_h_

#include <string>

#include <ri.h>

struct ProcArgs
{
    //constructor parses
    ProcArgs(RtString paramStr);
    
    //copy constructor
    ProcArgs(const ProcArgs & rhs)
    : filename(rhs.filename)
    , objectpath(rhs.objectpath)
    , frame(rhs.frame)
    , fps(rhs.fps)
    , shutterOpen(rhs.shutterOpen)
    , shutterClose(rhs.shutterClose)
    {
    }
    
    //member variables
    std::string filename;
    std::string objectpath;
    double frame;
    double fps;
    double shutterOpen;
    double shutterClose;
};

#endif
