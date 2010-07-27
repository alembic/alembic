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

namespace AlembicAbcExport {

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
    msg += "AlembicAbcExport  [options] OutputFileName.abc\n\n";
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
        MGlobal::displayError( "AlembicAbcExport: UNKNOWN EXCEPTION" );
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
MStatus AbcExportSelected( const Parameters &config )
{
    Abc::Init();
    
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

    // Create the asset
    Abc::OAsset asset;
    if ( config.fileName != "UNSPECIFIED_FILE_NAME.abc" &&
         config.fileName != "" )
    {
        asset = Abc::OAsset( config.fileName );
        if ( !asset )
        {
            MGlobal::displayError( "AbcOut: OAsset ctor FAILED" );
            return MS::kFailure;
        }
    }
    else
    {
        MGlobal::displayError( "No filename specified." );
        return MStatus::kFailure;
    }

    std::cout << "AlembicAbcExport: Opened Alembic Asset: " << config.fileName
              << " for writing." << std::endl;

    // Build comments
    std::string comments = "AlembicAbcExport v0.1.1";
    comments += "\n";
    MString exportedFromStr =
        "(Exported from " + MFileIO::currentFile() + ")";
    comments += exportedFromStr.asChar();
    comments += "\n";
    asset.setComments( comments );

    // Create the top-exportable.
    Top top( asset, config.fileName );

    // Create the time sampling.
    Abc::TimeSamplingInfo tinfo;
    if ( config.endFrame > config.startFrame )
    {
        tinfo = Abc::TimeSamplingInfo(
            Abc::kUniformTimeSampling,
            
            // Time, in seconds, of first sample.
            MTime( ( double )config.startFrame,
                   MTime::uiUnit() ).as( MTime::kSeconds ),
            
            // Increment, in seconds, between samples.
            MTime( 1.0, MTime::uiUnit() ).as( MTime::kSeconds ) );
    }

    // Create the factory
    Factory factory( config );

    // Use the factory to make a tree from each of the
    // nodes we iterate through.
    for ( MItSelectionList liter( slist ); !liter.isDone(); liter.next() )
    {
        MDagPath dagPath;
        MObject component;
        liter.getDagPath( dagPath, component );

        // This will skip nodes we've already visited. HAVE NO
        // FEAR.
        factory.makeTree( top, dagPath, 1000000, tinfo );
    }
    std::cout << "AlembicAbcExport: Created DAG Tree to export."
              << std::endl;
    
    // Loop over time
    // For now, just setting first frame as "rest" pose.
    MComputation computation;
    computation.beginComputation();
    for ( int frame = config.startFrame;
          frame <= config.endFrame; ++frame )
    {
        // Get a time.
        MTime thisTime( ( double )frame,
                        MTime::uiUnit() );

        // Set the time.
        MAnimControl::setCurrentTime( thisTime );
        
        // Is this necessary to force an eval?  Sometimes?
        // MGlobal::viewFrame( t );
        // M3dView currentView = M3dView::active3dView();
        // currentView.refresh( true, true, true );

        if ( frame == config.startFrame )
        {
            // Set rest pose.
            Abc::Time restTime( Abc::kRestTime );
            top.writeSample( restTime );
            std::cout << "AlembicAbcExport: Wrote Rest Pose" << std::endl;
        }

        // Write the frame.
        if ( tinfo )
        {
            Abc::Time abcTime( thisTime.as( MTime::kSeconds ), 1.0 );
            top.writeSample( abcTime );
            std::cout << "AlembicAbcExport: Wrote frame: " << frame << std::endl;
        }
        
        if ( computation.isInterruptRequested() )
        {
            break;
        }
    }
    computation.endComputation();

    top.close();
    std::cout << "AlembicAbcExport: Closed Asset." << std::endl;

    return MS::kSuccess;
}

} // End namespace AlembicAbcExport

//-*****************************************************************************
MStatus initializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin plugin( obj, "Alembic", "1.0", "Any" );

    status = plugin.registerCommand( "AlembicAbcExport",
                                     AlembicAbcExport::AbcExport::creator,
                                     AlembicAbcExport::AbcExport::createSyntax );

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

    status = plugin.deregisterCommand( "AlembicAbcExport" );
    if ( !status )
    {
        status.perror( "deregisterCommand" );
    }

    return status;
}
