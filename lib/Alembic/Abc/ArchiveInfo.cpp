//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#include <Alembic/Abc/ArchiveInfo.h>

namespace Alembic {
namespace Abc {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
void
GetArchiveInfo(
    IArchive & iArchive,
    std::string & oApplicationWriter,
    std::string & oAlembicVersion,
    Util::uint32_t & oAlembicApiVersion,
    std::string & oDateWritten,
    std::string & oUserDescription,
    double & oDCCFPS)
{
    if ( ! iArchive.getPtr() )
    {
        return;
    }

    AbcA::MetaData md = iArchive.getPtr()->getMetaData();
    oApplicationWriter = md.get( kApplicationNameKey );
    oAlembicVersion = md.get( "_ai_AlembicVersion" );
    oAlembicApiVersion = iArchive.getArchiveVersion();

    oDateWritten = md.get( kDateWrittenKey );
    oUserDescription = md.get( kUserDescriptionKey );
    oDCCFPS = atof( md.get( kDCCFPSKey ).c_str() );
}

void
GetArchiveInfo(
    IArchive & iArchive,
    std::string & oApplicationWriter,
    std::string & oAlembicVersion,
    Util::uint32_t & oAlembicApiVersion,
    std::string & oDateWritten,
    std::string & oUserDescription)
{
    double unusedFps;
    GetArchiveInfo( iArchive, oApplicationWriter, oAlembicVersion,
        oAlembicApiVersion, oDateWritten, oUserDescription, unusedFps );
}

void
GetArchiveStartAndEndTime(
    IArchive & iArchive,
    double & oStartTime,
    double & oEndTime )
{
    double startTime = DBL_MAX;
    double endTime = -DBL_MAX;

    double startSingleTime = DBL_MAX;
    double endSingleTime = -DBL_MAX;

    double startDefaultTime = DBL_MAX;
    double endDefaultTime = -DBL_MAX;

    for ( uint32_t i = 0; i < iArchive.getNumTimeSamplings(); ++i )
    {
        index_t idx = iArchive.getMaxNumSamplesForTimeSamplingIndex( i );

        if ( idx == INDEX_UNKNOWN )
        {
            continue;
        }

        AbcA::TimeSamplingPtr ts = iArchive.getTimeSampling( i );
        if ( !ts )
        {
            continue;
        }

        if ( idx > 1 && i != 0 )
        {
            startTime = std::min( ts->getSampleTime( 0 ), startTime );
            endTime = std::max( ts->getSampleTime( idx - 1 ), endTime );
        }
        else if ( idx == 1 && i != 0 )
        {
            startSingleTime = std::min( ts->getSampleTime( 0 ),
                                        startSingleTime );
            endSingleTime = std::max( ts->getSampleTime( 0 ),
                                      endSingleTime );
        }
        else if ( idx > 0 && i == 0 )
        {
            startDefaultTime = ts->getSampleTime( 0 );
            endDefaultTime = ts->getSampleTime( idx - 1 );
        }
    }

    // if we had a valid animated start and end time we will use only that
    if ( startTime != DBL_MAX && endTime != -DBL_MAX )
    {
        oStartTime = startTime;
        oEndTime = endTime;
    }
    // this is for the cases where we have custom time samplings but
    // they only have 1 sample
    else if ( startSingleTime != DBL_MAX && endSingleTime != -DBL_MAX )
    {
        oStartTime = startSingleTime;
        oEndTime = endSingleTime;
    }
    // no other valid time was yet to be found, but we found a valid
    // default time with at least one sample, so we will use it.
    else if ( startDefaultTime != DBL_MAX && endDefaultTime != -DBL_MAX )
    {
        oStartTime = startDefaultTime;
        oEndTime = endDefaultTime;
    }
    else
    {
        oStartTime = DBL_MAX;
        oEndTime = -DBL_MAX;
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Abc
} // End namespace Alembic
