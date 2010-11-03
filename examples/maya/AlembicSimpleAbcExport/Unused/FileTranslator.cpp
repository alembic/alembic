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

#include "FileTranslator.h"
#include "Top.h"
#include <maya/MFnPlugin.h>

namespace AlembicAbcExport {

//-*****************************************************************************
#define DEFAULT_OPTIONS "recurse=1;"                \
                        "recurseLimit=0;"           \
                        "subd=0;"                   \
                        "deforming=1;"              \
                        "userAttr=1;"               \
                        "mayaAttr=0;"               \
                        "fs=1;"                     \
                        "fe=100;"                   \
                        "fps=24.0"

//-*****************************************************************************
FileTranslator::FileTranslator()
{
    // Nothing
    Abc::Init();
}

//-*****************************************************************************
MStatus FileTranslator::writer( const MFileObject &file, 
                                const MString &optionsString,
                                MPxFileTranslator::FileAccessMode mode )
{
    if ( ( mode == MPxFileTranslator::kExportAccessMode ) ||
         ( mode == MPxFileTranslator::kSaveAccessMode ) )
    {
        MGlobal::displayError( "The AlembicAbcExport plugin can only be used "
                               "for Export Selection..." );
        return MS::kFailure;
    }
    
    Parameters params;
    params.startFrame =
        ( int )floor( MAnimControl::currentTime().as( MTime::uiUnit() ) );
    params.endFrame = params.startFrame;

    MStringArray args;
    optionsString.split( ';', args );
    for ( size_t i = 0; i < args.length(); ++i )
    {
        MStringArray thisArg;
        args[i].split( '=', thisArg );
        MString argName( thisArg[0] );
        MString argValue( thisArg[1] );

        if ( argName == "recurse" && argValue == "1" )
        {
            params.maxRecursion = 100000;
        }
        else if ( argName == "deforming" )
        {
            params.deforming = ( bool )argValue.asInt();
        }
        else if ( argName == "subd" )
        {
            params.polysAsSubds = ( bool )argValue.asInt();
        }
        else if ( argName == "userAttr" )
        {
            params.allUserAttributes = ( bool )argValue.asInt();
        }
        else if ( argName == "mayaAttr" )
        {
            params.allMayaAttributes = ( bool )argValue.asInt();
        }
        else if ( argName == "fs" )
        {
            params.startFrame = argValue.asInt();
        }
        else if ( argName == "fe" )
        {
            params.endFrame = argValue.asInt();
        }
        else if ( argName == "recurseLimit" )
        {
            if ( argValue.asInt() > 0 )
            {
                params.maxRecursion = argValue.asInt();
            }
        }
    }

    params.fileName = file.resolvedFullName().asChar();

    params.endFrame = std::max( params.endFrame, params.startFrame );

    try
    {
        return ExportSelected( params );
    }
    catch ( std::exception &exc )
    {
        MGlobal::displayError( exc.what() );
        return MS::kFailure;
    }
    catch ( ... )
    {
        MGlobal::displayError( "AlembicAbcExport: Unknown Error" );
        return MS::kFailure;
    }
}

//-*****************************************************************************
MPxFileTranslator::MFileKind
FileTranslator::identifyFile( const MFileObject &file,
                        const char *magic,
                        short magicSize ) const
{
    std::string fileNameStr = file.resolvedFullName().asChar();

    if ( fileNameStr.find( ".abc" ) && H5Fis_hdf5( fileNameStr.c_str() ) )
    {
        return MPxFileTranslator::kIsMyFileType;
    }

    return MPxFileTranslator::kNotMyFileType;
}

//-*****************************************************************************
void *FileTranslator::creator()
{
    return new FileTranslator;
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// EXPORT SELECTED FUNCTION
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
MStatus ExportSelected( const Parameters &config )
{
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
        factory.makeTree( top, dagPath, config.maxRecursion, tinfo );
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
//-*****************************************************************************
//-*****************************************************************************
// PLUGIN REGISTRATION
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
MStatus initializePlugin( MObject _obj )
{
    MFnPlugin   plugin( _obj, "AlembicAbcExport", "0.1.1" );
    MStatus     status;
    status = plugin.registerFileTranslator(
        "Alembic", "none",
        AlembicAbcExport::FileTranslator::creator,
        "AlembicAbcExport_IOOptions", "",
        false );
    //status = plugin.registerFileTranslator
    //    ( "AlembicAbcExport", "none",
    //      AlembicAbcExport::FileTranslator::creator,
    //      "AlembicAbcExportOptions" );
    CHECK_MAYA_STATUS;
    return status;
}

//-*****************************************************************************
MStatus uninitializePlugin( MObject _obj )                     
{
    MFnPlugin   plugin( _obj );
    MStatus     status;
    status = plugin.deregisterFileTranslator( "AlembicAbcExport" );
    CHECK_MAYA_STATUS;
    return status;
}

