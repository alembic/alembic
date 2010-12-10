//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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
#include <set>
#include <ri.h>

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include "ProcArgs.h"
#include "PathUtil.h"
#include "WriteGeo.h"

using namespace Alembic::AbcGeom;

//-*****************************************************************************
void WalkObject( IObject parent, const ObjectHeader &ohead, ProcArgs &args,
                 PathList::const_iterator I, PathList::const_iterator E )
{
    RiAttributeBegin();
    WriteIdentifier( ohead );

    //set this if we should continue traversing
    IObject nextParentObject;

    if ( ISimpleXform::matches( ohead ) )
    {
        ISimpleXform xform( parent, ohead.getName() );
        ProcessSimpleTransform( xform, args );

        nextParentObject = xform; //we do want to keep walking
    }
    else if ( IXform::matches( ohead ) )
    {
        IXform xform( parent, ohead.getName() );
        ProcessXform( xform, args );

        nextParentObject = xform;
    }
    else if ( ISubD::matches( ohead ) )
    {
        ISubD subd( parent, ohead.getName() );
        ProcessSubD( subd, args );

        nextParentObject = subd;
    }
    else if ( IPolyMesh::matches( ohead ) )
    {
        IPolyMesh polymesh( parent, ohead.getName() );
        ProcessPolyMesh( polymesh, args );

        nextParentObject = polymesh;
    }
    else
    {
        std::cerr << "could not determine type of " << ohead.getName()
                  << std::endl;

        std::cerr << ohead.getName() << " has MetaData: "
                  << ohead.getMetaData().serialize() << std::endl;
    }

    if ( nextParentObject.valid() )
    {
        if ( I == E )
        {
            for ( size_t i = 0; i < nextParentObject.getNumChildren() ; ++i )
            {
                WalkObject( nextParentObject,
                            nextParentObject.getChildHeader( i ),
                            args, I, E );
            }
        }
        else
        {
            const ObjectHeader *nextChildHeader =
                nextParentObject.getChildHeader( *I );

            if ( nextChildHeader != NULL )
            {
                WalkObject( nextParentObject, *nextChildHeader, args, I+1, E );
            }
        }
    }

    RiAttributeEnd();
}

//-*****************************************************************************
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

//-*****************************************************************************
extern "C" RtVoid
Free( RtPointer data )
{
    delete reinterpret_cast<ProcArgs*>( data );
}

//-*****************************************************************************
extern "C" RtVoid
Subdivide( RtPointer data, RtFloat detail )
{
    ProcArgs *args = reinterpret_cast<ProcArgs*>( data );
    if ( !args )
    {
        return;
    }

    try
    {
        IArchive archive( ::Alembic::AbcCoreHDF5::ReadArchive(),
                          args->filename );

        IObject root = archive.getTop();

        PathList path;
        TokenizePath( args->objectpath, path );

        if ( path.empty() ) //walk the entire scene
        {
            for ( size_t i = 0; i < root.getNumChildren(); ++i )
            {
                WalkObject( root, root.getChildHeader(i), *args,
                            path.end(), path.end() );
            }
        }
        else //walk to a location + its children
        {
            PathList::const_iterator I = path.begin();

            const ObjectHeader *nextChildHeader =
                    root.getChildHeader( *I );
            if ( nextChildHeader != NULL )
            {
                WalkObject( root, *nextChildHeader, *args, I+1, path.end() );
            }
        }
    }
    catch ( const std::exception &e )
    {
        std::cerr << "exception thrown during ProcMain Subdivide: "
                  << e.what() << std::endl;
    }
}
