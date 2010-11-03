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

#include "ExportCmd.h"
#include "Top.h"
#include <maya/MFnPlugin.h>

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
AbcExport::AbcExport()
{
}

//-*****************************************************************************
AbcExport::~AbcExport()
{
}

//-*****************************************************************************
MSyntax AbcExport::createSyntax()
{
    MSyntax syntax;

    syntax.addFlag("-v",  "-verbose", MSyntax::kNoArg);
    syntax.addFlag("-h",  "-help", MSyntax::kNoArg);
    syntax.addFlag("-fs", "-frameStart", MSyntax::kLong);
    syntax.addFlag("-fe", "-frameEnd", MSyntax::kLong);
    syntax.addArg(MSyntax::kString);

    syntax.enableQuery(true);
    syntax.enableEdit(false);

    return syntax;
}

//-*****************************************************************************
void* AbcExport::creator()
{
    return new AbcExport();
}

//-*****************************************************************************
MStatus AbcExport::doIt( const MArgList & args )
{
    MStatus status;

    MTime oldCurTime = MAnimControl::currentTime();

    MArgParser argData( syntax(), args, &status );
    if ( status != MS::kSuccess )
    {
        return status;
    }

    unsigned int numberOfArguments = args.length();

    MString msg;
    msg += "AlembicSimpleAbcExport  [options] OutputFileName.abc\n\n";
    msg += "Options:\n";
    
    msg += "-h  / help  Print this message.\n";
    msg += "\n";
    
    msg += "-fs / frameStart int (default: 0)\n";
    msg += "The export start frame\n";
    msg += "\n";
    
    msg += "-fe / frameEnd int (default: 0)\n";
    msg += "The export end frame\n";
    msg += "\n";
    
    msg += "-v  / verbose  Verbose output\n";
    msg += "\n";
    
    if ( argData.isFlagSet( "help" ) )
    {
        MGlobal::displayInfo( msg );
        return MS::kSuccess;
    }

    bool verbose = argData.isFlagSet( "verbose" );

    int frameStart = 0;
    if ( argData.isFlagSet( "frameStart" ) )
    {
        argData.getFlagArgument( "frameStart", 0, frameStart );
    }

    int frameEnd = 0;
    if ( argData.isFlagSet( "frameEnd" ) )
    {
        argData.getFlagArgument( "frameEnd", 0, frameEnd );
    }

    // status = argData.getCommandArgument(0, argStr);
    // Get filenameArgument
    MString fileNameStr = args.asString( numberOfArguments-1, &status );

    // Okay, do it.
    Parameters params;
    params.fileName = fileNameStr.asChar();
    params.startFrame = frameStart;
    params.endFrame = frameEnd;
    params.verbose = verbose;
    params.polysAsSubds = false;
    params.deforming = true;
    params.allUserAttributes = true;
    params.allMayaAttributes = false;

    try
    {
        status = AbcExportSelected( params );
    }
    catch ( std::exception &exc )
    {
        MGlobal::displayError( exc.what() );
        status = MS::kFailure;
    }
    catch ( ... )
    {
        MGlobal::displayError( "AlembicSimpleAbcExport: UNKNOWN EXCEPTION" );
        status = MS::kFailure;
    }
   
    return status;
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// EXPORT SELECTED FUNCTION
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
MStatus AbcExportSelected( const Parameters &iConfig )
{
    // Abc::Init();
    
    //-*************************************************************************
    // CREATE SELECTION LIST
    //-*************************************************************************
    MSelectionList slist;
    MGlobal::getActiveSelectionList( slist );
    if ( slist.length() == 0 )
    {
        MGlobal::displayError( "Nothing selected." );
        return MS::kFailure;
    }

    //-*************************************************************************
    // CREATE THE ARCHIVE
    //-*************************************************************************
    if ( iConfig.fileName == "UNSPECIFIED_FILE_NAME.abc" ||
         iConfig.fileName == "" )
    {
        MGlobal::displayError( "No filename specified." );
        return MStatus::kFailure;
    }
        
    // Create the time sampling.
    Abc::TimeSamplingType tSmpType;
    if ( iConfig.endFrame > iConfig.startFrame )
    {
        tSmpType = Abc::TimeSamplingType(
                
            // Increment, in seconds, between samples.
            ( Abc::chrono_t )
            MTime( 1.0, MTime::uiUnit() ).as( MTime::kSeconds ) );
    }

    // Get FPS
    Abc::chrono_t fps =
        MTime( 1.0, MTime::kSeconds ).as( MTime::uiUnit() );
        
    Top top( iConfig.fileName, tSmpType, fps );
        
    std::cout << "AlembicSimpleAbcExport: Opened Alembic Archive: "
              << top.getName()
              << " for writing." << std::endl;
        
    // Build comments
    std::string comments = "AlembicSimpleAbcExport v0.1.1";
    comments += "\n";
    MString exportedFromStr =
        "(Exported from " + MFileIO::currentFile() + ")";
    comments += exportedFromStr.asChar();
    comments += "\n";
    // top.setComments( comments );


    //-*********************************************************************
    // BUILD TREE OF NODE:OBJECT PAIRS TO EXPORT
    //-*********************************************************************
    
    // Create the factory
    Factory factory( iConfig );
    
    for ( MItSelectionList liter( slist ); !liter.isDone(); liter.next() )
    {
        MDagPath dagPath;
        MObject component;
        liter.getDagPath( dagPath, component );
            
        // This will skip nodes we've already visited. HAVE NO
        // FEAR.
        factory.makeTree( top, dagPath, 1000000, tSmpType );
    }
    std::cout << "AlembicSimpleAbcExport: Created DAG Tree to export."
              << std::endl;

    //-*********************************************************************
    // EXPORT SAMPLES PER FRAME
    //-*********************************************************************
    
    // Loop over time
    MComputation computation;
    computation.beginComputation();
    for ( int frame = iConfig.startFrame;
          frame <= iConfig.endFrame; ++frame )
    {   
        // Get a time.
        MTime thisTime( ( double )frame, MTime::uiUnit() );
            
        // Set the time.
        MAnimControl::setCurrentTime( thisTime );
            
        // Get the chrono and the index
        Abc::index_t frameIndex =
            ( Abc::index_t )( frame - iConfig.startFrame );
        Abc::chrono_t frameTime =
            ( Abc::chrono_t )thisTime.as( MTime::kSeconds );
        
        // Is this necessary to force an eval?  Sometimes?
        // MGlobal::viewFrame( t );
        // M3dView currentView = M3dView::active3dView();
        // currentView.refresh( true, true, true );
            
        // Write the frame.
        top.writeSample( Abc::OSampleSelector( frameIndex, frameTime ) );
        std::cout << "AlembicSimpleAbcExport: Wrote frame: "
                  << frame << std::endl;
            
        if ( computation.isInterruptRequested() )
        {
            break;
        }
    }
        
    top.close();
    computation.endComputation();
    // H5close();

    std::cout << "AlembicSimpleAbcExport: Closed Archive." << std::endl;
    return MS::kSuccess;
}

} // End namespace AlembicSimpleAbcExport

//-*****************************************************************************
MStatus initializePlugin( MObject obj )
{
    MStatus status;
    MFnPlugin plugin( obj, "Alembic", "1.0", "Any" );

    // H5dont_atexit();
    // H5open();

    status = plugin.registerCommand( "AlembicSimpleAbcExport",
                                     AlembicSimpleAbcExport::AbcExport::creator,
                                     AlembicSimpleAbcExport::AbcExport::createSyntax );

    if ( !status )
    {
        status.perror( "registerCommand" );
    }

    return status;
}

//-*****************************************************************************
MStatus uninitializePlugin( MObject obj )
{
    
    MStatus status;
    MFnPlugin plugin( obj );

    status = plugin.deregisterCommand( "AlembicSimpleAbcExport" );
    if ( !status )
    {
        status.perror( "deregisterCommand" );
    }

    
    // H5close();

    return status;
}
