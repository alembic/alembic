#include <iostream>
#include <set>
#include <ri.h>

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
using namespace Alembic;

///////////////////////////////////////////////////////////////////////////////

#include "ProcArgs.h"
#include "PathUtil.h"
#include "WriteGeo.h"

///////////////////////////////////////////////////////////////////////////////

void WalkObject(
        Abc::IObject parent,
        const Abc::ObjectHeader & ohead,
        ProcArgs & args,
        PathList::const_iterator I,
        PathList::const_iterator E
)
{
    RiAttributeBegin();
    WriteIdentifier(ohead);
    
    //set this if we should continue traversing
    Abc::IObject nextParentObject;
    
    if (AbcGeom::ISimpleXform::matches(ohead))
    {
        AbcGeom::ISimpleXform xform(parent, ohead.getName());
        ProcessSimpleTransform(xform, args);
        
        nextParentObject = xform; //we do want to keep walking
    }
    else if (AbcGeom::ISubD::matches(ohead))
    {
        AbcGeom::ISubD subd(parent, ohead.getName());
        ProcessSubD(subd, args);
    }
    else if (AbcGeom::IPolyMesh::matches(ohead))
    {
        AbcGeom::IPolyMesh polymesh(parent, ohead.getName());
        ProcessPolyMesh(polymesh, args);
    }
    else
    {
        //unrecognized, continue walking?
    }
    
    if (nextParentObject.valid())
    {
        if (I == E)
        {
            for (size_t i = 0; i < nextParentObject.getNumChildren(); ++i)
            {
                WalkObject(nextParentObject,
                        nextParentObject.getChildHeader(i), args, I, E);
            }
        }
        else
        {
            const Abc::ObjectHeader * nextChildHeader =
                    nextParentObject.getChildHeader(*I);
            if (nextChildHeader != NULL)
            {
                WalkObject(nextParentObject, *nextChildHeader, args, I+1, E);
            }
        }
    }
    
    RiAttributeEnd();
}

///////////////////////////////////////////////////////////////////////////////

extern "C" RtPointer
ConvertParameters( RtString paramstr )
{
    try
    {
        return (RtPointer) new ProcArgs(paramstr);
    }
    catch (const std::exception & e)
    {
        std::cerr << "Exception thrown during ProcMain ConvertParameters: ";
        std::cerr << "\"" << paramstr << "\"";
        std::cerr << " " << e.what() << std::endl;
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////

extern "C" RtVoid
Free( RtPointer data )
{
    delete reinterpret_cast<ProcArgs*>(data);
}

///////////////////////////////////////////////////////////////////////////////

extern "C" RtVoid
Subdivide(RtPointer data, RtFloat detail)
{
    ProcArgs * args = reinterpret_cast<ProcArgs*>(data);
    if (!args)
    {
        return;
    }
    
    try
    {
        Abc::IArchive archive(AbcCoreHDF5::ReadArchive(), args->filename);
        
        Abc::IObject root = archive.getTop();
        
        PathList path;
        TokenizePath(args->objectpath, path);
        
        if (path.empty()) //walk the entire scene
        {
            for (size_t i = 0; i < root.getNumChildren(); ++i)
            {
                WalkObject(root, root.getChildHeader(i), *args,
                        path.end(), path.end());
            }
        }
        else //walk to a location + its children
        {
            PathList::const_iterator I = path.begin();
            
            const Abc::ObjectHeader * nextChildHeader =
                    root.getChildHeader(*I);
            if (nextChildHeader != NULL)
            {
                WalkObject(root, *nextChildHeader, *args, I+1, path.end());
            }
        }
    }
    catch (const std::exception & e)
    {
        std::cerr << "exception thrown during ProcMain Subdivide: " <<
                e.what() << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////

