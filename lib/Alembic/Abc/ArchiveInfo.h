//-*****************************************************************************
//
// Copyright (c) 2009-2013,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#ifndef Alembic_Abc_ArchiveInfo_h
#define Alembic_Abc_ArchiveInfo_h

#include <Alembic/Util/Export.h>
#include <Alembic/Abc/Foundation.h>
#include <Alembic/Abc/IArchive.h>
#include <Alembic/Abc/OArchive.h>
#include <cfloat>
#include <time.h>

namespace Alembic {
namespace Abc {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
// Alembic archive information:
// Useful utilities that readers and writers can use to provide
// useful annotations into the alembic file.

//-*****************************************************************************
// Some MetaData key constants
static ALEMBIC_EXPORT_CONST char * kApplicationNameKey = "_ai_Application";
static ALEMBIC_EXPORT_CONST char * kDateWrittenKey = "_ai_DateWritten";
static ALEMBIC_EXPORT_CONST char * kUserDescriptionKey = "_ai_Description";
static ALEMBIC_EXPORT_CONST char * kDCCFPSKey = "_ai_DCC_FPS";

//-*****************************************************************************
template <class ARCHIVE_CTOR>
OArchive CreateArchiveWithInfo(
    //! We need to pass in a constructor which provides
    //! an explicit link to the concrete implementation of
    //! AbcCoreAbstract that we're using.
    ARCHIVE_CTOR iCtor,

    //! File name
    const std::string &iFileName,

    //! Application specific information about what is writing the file
    const std::string & iApplicationWriter,

    //! Extra information, could be arguments to the tool that is
    //! writing the file.
    const std::string & iUserDescription,

    //! Optional meta data or error handling policy
    const Argument &iArg0 = Argument(),

    //! Optional meta data or error handling policy
    const Argument &iArg1 = Argument() );

//-*****************************************************************************
template <class ARCHIVE_CTOR>
OArchive CreateArchiveWithInfo(
    //! We need to pass in a constructor which provides
    //! an explicit link to the concrete implementation of
    //! AbcCoreAbstract that we're using.
    ARCHIVE_CTOR iCtor,

    //! File name
    const std::string &iFileName,

    //! Optional FPS hint that the DCC used at write time
    double iDCCFPS,

    //! Application specific information about what is writing the file
    const std::string & iApplicationWriter,

    //! Extra information, could be arguments to the tool that is
    //! writing the file.
    const std::string & iUserDescription,

    //! Optional meta data or error handling policy
    const Argument &iArg0 = Argument(),

    //! Optional meta data or error handling policy
    const Argument &iArg1 = Argument() );

//-*****************************************************************************
ALEMBIC_EXPORT void
GetArchiveInfo(
    //! The Archive whose meta data will be inspected
    IArchive & iArchive,

    //! Application specific information about what wrote the file
    std::string & oApplicationWriter,

    //! What version of Alembic wrote the file (and when it was built)
    std::string & oAlembicVersion,

    //! Numeric version of the Alembic API that wrote the file
    Util::uint32_t & oAlembicApiVersion,

    //! The data that the file was originally written
    std::string & oDateWritten,

    //! Extra information, could be arguments to the tool that wrote the file.
    std::string & oUserDescription );

//-*****************************************************************************
ALEMBIC_EXPORT void
GetArchiveInfo(
    //! The Archive whose meta data will be inspected
    IArchive & iArchive,

    //! Application specific information about what wrote the file
    std::string & oApplicationWriter,

    //! What version of Alembic wrote the file (and when it was built)
    std::string & oAlembicVersion,

    //! Numeric version of the Alembic API that wrote the file
    Util::uint32_t & oAlembicApiVersion,

    //! The data that the file was originally written
    std::string & oDateWritten,

    //! Extra information, could be arguments to the tool that wrote the file.
    std::string & oUserDescription,

    //! Optional hint about what FPS was being used by the DCC when this archive
    //! was created.
    double & oDCCFPS);

//-*****************************************************************************
//! Convenience function which gets a start and end time for the archive using
//! only IArchive::getMaxNumSamplesForTimeSamplingIndex.  The hierarchy is
//! NOT walked.
//! If the archive was written prior to 1.1.3 or no samples were written for
//! any of the properties DBL_MAX will be set for oStartTime
//! and -DBL_MAX will be set for oEndTime.
ALEMBIC_EXPORT void
GetArchiveStartAndEndTime(
    //! The Archive to be inspected
    IArchive & iArchive,
    //! The start time for the archive in seconds
    double & oStartTime,
    //! The end time for the archive in seconds
    double & oEndTime );

//-*****************************************************************************
template <class ARCHIVE_CTOR>
OArchive CreateArchiveWithInfo(
    ARCHIVE_CTOR iCtor,
    const std::string &iFileName,
    double iDCCFPS,
    const std::string &iApplicationWriter,
    const std::string &iUserDescription,
    const Argument &iArg0,
    const Argument &iArg1 )
{
    AbcA::MetaData md = GetMetaData( iArg0, iArg1 );
    ErrorHandler::Policy policy = GetErrorHandlerPolicyFromArgs( iArg0, iArg1 );

    if ( iApplicationWriter != "" )
    {
        md.set( kApplicationNameKey, iApplicationWriter );
    }

    time_t rawtimeNow;
    time( &rawtimeNow );
    char dateBuf [128];
#if defined _WIN32 || defined _WIN64
    ctime_s( dateBuf, 128, &rawtimeNow);
#else
    ctime_r( &rawtimeNow, dateBuf );
#endif

    std::size_t bufLen = strlen( dateBuf );
    if ( bufLen > 0 && dateBuf[bufLen - 1] == '\n' )
    {
        dateBuf[bufLen - 1] = '\0';
    }
    md.set( kDateWrittenKey, dateBuf );

    if ( iUserDescription != "" )
    {
        md.set( kUserDescriptionKey, iUserDescription );
    }

    if ( iDCCFPS > 0.0 )
    {
        md.set( kDCCFPSKey, std::to_string( iDCCFPS ) );
    }

    return OArchive( iCtor, iFileName, md, policy );

}

template <class ARCHIVE_CTOR>
OArchive CreateArchiveWithInfo(
    ARCHIVE_CTOR iCtor,
    const std::string &iFileName,
    const std::string &iApplicationWriter,
    const std::string &iUserDescription,
    const Argument &iArg0,
    const Argument &iArg1 )
{
    return CreateArchiveWithInfo( iCtor, iFileName, 0, iApplicationWriter,
        iUserDescription, iArg0, iArg1 );
}

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Abc
} // End namespace Alembic

#endif
