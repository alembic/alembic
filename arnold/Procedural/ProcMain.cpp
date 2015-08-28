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

#include <cstring>
#include <memory>
#include "ProcArgs.h"
#include "PathUtil.h"
#include "SampleUtil.h"
#include "WriteGeo.h"

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreFactory/All.h>

namespace
{

using namespace Alembic::AbcGeom;



void WalkObject( IObject parent, const ObjectHeader &ohead, ProcArgs &args,
             PathList::const_iterator I, PathList::const_iterator E,
                    MatrixSampleMap * xformSamples)
{
    //Accumulate transformation samples and pass along as an argument
    //to WalkObject

    IObject nextParentObject;

    std::auto_ptr<MatrixSampleMap> concatenatedXformSamples;

    if ( IXform::matches( ohead ) )
    {
        if ( args.excludeXform )
        {
            nextParentObject = IObject( parent, ohead.getName() );
        }
        else
        {
            IXform xform( parent, ohead.getName() );

            IXformSchema &xs = xform.getSchema();

            if ( xs.getNumOps() > 0 )
            {
                TimeSamplingPtr ts = xs.getTimeSampling();
                size_t numSamples = xs.getNumSamples();

                SampleTimeSet sampleTimes;
                GetRelevantSampleTimes( args, ts, numSamples, sampleTimes,
                        xformSamples);

                MatrixSampleMap localXformSamples;

                MatrixSampleMap * localXformSamplesToFill = 0;

                concatenatedXformSamples.reset(new MatrixSampleMap);

                if ( !xformSamples )
                {
                    // If we don't have parent xform samples, we can fill
                    // in the map directly.
                    localXformSamplesToFill = concatenatedXformSamples.get();
                }
                else
                {
                    //otherwise we need to fill in a temporary map
                    localXformSamplesToFill = &localXformSamples;
                }


                for (SampleTimeSet::iterator I = sampleTimes.begin();
                        I != sampleTimes.end(); ++I)
                {
                    XformSample sample = xform.getSchema().getValue(
                            Abc::ISampleSelector(*I));
                    (*localXformSamplesToFill)[(*I)] = sample.getMatrix();
                }

                if ( xformSamples )
                {
                    ConcatenateXformSamples(args,
                            *xformSamples,
                            localXformSamples,
                            *concatenatedXformSamples.get());
                }


                xformSamples = concatenatedXformSamples.get();

            }

            nextParentObject = xform;
        }
    }
    else if ( ISubD::matches( ohead ) )
    {
        std::string faceSetName;

        ISubD subd( parent, ohead.getName() );

        //if we haven't reached the end of a specified -objectpath,
        //check to see if the next token is a faceset name.
        //If it is, send the name to ProcessSubD for addition of
        //"face_visibility" tags for the non-matching faces
        if ( I != E )
        {
            if ( subd.getSchema().hasFaceSet( *I ) )
            {
                faceSetName = *I;
            }
        }

        ProcessSubD( subd, args, xformSamples, faceSetName );

        //if we found a matching faceset, don't traverse below
        if ( faceSetName.empty() )
        {
            nextParentObject = subd;
        }
    }
    else if ( IPolyMesh::matches( ohead ) )
    {
        std::string faceSetName;

        IPolyMesh polymesh( parent, ohead.getName() );

        //if we haven't reached the end of a specified -objectpath,
        //check to see if the next token is a faceset name.
        //If it is, send the name to ProcessSubD for addition of
        //"face_visibility" tags for the non-matching faces
        if ( I != E )
        {
            if ( polymesh.getSchema().hasFaceSet( *I ) )
            {
                faceSetName = *I;
            }
        }

        ProcessPolyMesh( polymesh, args, xformSamples, faceSetName );

        //if we found a matching faceset, don't traverse below
        if ( faceSetName.empty() )
        {
            nextParentObject = polymesh;
        }
    }
    else if ( INuPatch::matches( ohead ) )
    {
        INuPatch patch( parent, ohead.getName() );
        // TODO ProcessNuPatch( patch, args );

        nextParentObject = patch;
    }
    else if ( IPoints::matches( ohead ) )
    {
        IPoints points( parent, ohead.getName() );
        // TODO ProcessPoints( points, args );

        nextParentObject = points;
    }
    else if ( ICurves::matches( ohead ) )
    {
        ICurves curves( parent, ohead.getName() );
        // TODO ProcessCurves( curves, args );

        nextParentObject = curves;
    }
    else if ( IFaceSet::matches( ohead ) )
    {
        //don't complain about discovering a faceset upon traversal
    }
    else
    {
        std::cerr << "could not determine type of " << ohead.getName()
                  << std::endl;

        std::cerr << ohead.getName() << " has MetaData: "
                  << ohead.getMetaData().serialize() << std::endl;

        nextParentObject = parent.getChild(ohead.getName());
    }

    if ( nextParentObject.valid() )
    {
        //std::cerr << nextParentObject.getFullName() << std::endl;

        if ( I == E )
        {
            for ( size_t i = 0; i < nextParentObject.getNumChildren() ; ++i )
            {
                WalkObject( nextParentObject,
                            nextParentObject.getChildHeader( i ),
                            args, I, E, xformSamples);
            }
        }
        else
        {
            const ObjectHeader *nextChildHeader =
                nextParentObject.getChildHeader( *I );

            if ( nextChildHeader != NULL )
            {
                WalkObject( nextParentObject, *nextChildHeader, args, I+1, E,
                    xformSamples);
            }
        }
    }



}

//-*************************************************************************

int ProcInit( struct AtNode *node, void **user_ptr )
{
    ProcArgs * args = new ProcArgs( AiNodeGetStr( node, "data" ) );
    args->proceduralNode = node;
    *user_ptr = args;

#if (AI_VERSION_ARCH_NUM == 3 && AI_VERSION_MAJOR_NUM < 3) || AI_VERSION_ARCH_NUM < 3
    #error Arnold version 3.3+ required for AlembicArnoldProcedural
#endif

    if (!AiCheckAPIVersion(AI_VERSION_ARCH, AI_VERSION_MAJOR, AI_VERSION_MINOR))
    {
        std::cout << "AlembicArnoldProcedural compiled with arnold-"
                  << AI_VERSION
                  << " but is running with incompatible arnold-"
                  << AiGetVersion(NULL, NULL, NULL, NULL) << std::endl;
        return 1;
    }

    if ( args->filename.empty() )
    {
        args->usage();
        return 1;
    }

    Alembic::AbcCoreFactory::IFactory factory;
    factory.setPolicy(ErrorHandler::kQuietNoopPolicy);
    IArchive archive = factory.getArchive( args->filename );

    IObject root = archive.getTop();

    PathList path;
    TokenizePath( args->objectpath, path );

    try
    {
        if ( path.empty() ) //walk the entire scene
        {
            for ( size_t i = 0; i < root.getNumChildren(); ++i )
            {
                WalkObject( root, root.getChildHeader(i), *args,
                            path.end(), path.end(), 0 );
            }
        }
        else //walk to a location + its children
        {
            PathList::const_iterator I = path.begin();

            const ObjectHeader *nextChildHeader =
                    root.getChildHeader( *I );
            if ( nextChildHeader != NULL )
            {
                WalkObject( root, *nextChildHeader, *args, I+1,
                        path.end(), 0);
            }
        }
    }
    catch ( const std::exception &e )
    {
        std::cerr << "exception thrown during ProcInit: "
              << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "exception thrown\n";
    }


    return 1;
}

//-*************************************************************************

int ProcCleanup( void *user_ptr )
{
    delete reinterpret_cast<ProcArgs*>( user_ptr );
    return 1;
}

//-*************************************************************************

int ProcNumNodes( void *user_ptr )
{
    ProcArgs * args = reinterpret_cast<ProcArgs*>( user_ptr );
    return (int) args->createdNodes.size();
}

//-*************************************************************************

struct AtNode* ProcGetNode(void *user_ptr, int i)
{
    ProcArgs * args = reinterpret_cast<ProcArgs*>( user_ptr );

    if ( i >= 0 && i < (int) args->createdNodes.size() )
    {
        return args->createdNodes[i];
    }

    return NULL;
}

} //end of anonymous namespace



extern "C"
{
    ALEMBIC_EXPORT int ProcLoader(AtProcVtable* api)
    {
        api->Init        = ProcInit;
        api->Cleanup     = ProcCleanup;
        api->NumNodes    = ProcNumNodes;
        api->GetNode     = ProcGetNode;
        strcpy(api->version, AI_VERSION);
        return 1;
    }
}
