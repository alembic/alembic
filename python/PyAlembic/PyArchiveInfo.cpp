//-*****************************************************************************
//
// Copyright (c) 2012-2016,
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

#include <Foundation.h>

using namespace boost::python;

//-*****************************************************************************
static Abc::OArchive CreateArchiveWithInfoWrapper(
    const std::string &iFileName,
    const std::string &iApplicationWriter,
    const std::string &iUserDescription,
    const Abc::Argument &iArg0,
    const Abc::Argument &iArg1,
    bool asOgawa = true )
{
    if ( asOgawa == true )
    {
        return Abc::CreateArchiveWithInfo(
            ::Alembic::AbcCoreOgawa::WriteArchive(),
            iFileName,
            iApplicationWriter,
            iUserDescription,
            iArg0,
            iArg1 );
    }
    else
    {
#ifdef ALEMBIC_WITH_HDF5
        return Abc::CreateArchiveWithInfo(
            ::Alembic::AbcCoreHDF5::WriteArchive(),
            iFileName,
            iApplicationWriter,
            iUserDescription,
            iArg0,
            iArg1 );
#else
        throwPythonException( "Unsupported core type: HDF5" );
#endif
    }

    return Abc::OArchive();
}

//-*****************************************************************************
static dict GetArchiveInfoWrapper( Abc::IArchive& iArchive )
{
    std::string appName;
    std::string libraryVersionString;
    AbcU::uint32_t libraryVersion;
    std::string whenWritten;
    std::string userDescription;

    Abc::GetArchiveInfo( iArchive,
                         appName,
                         libraryVersionString,
                         libraryVersion,
                         whenWritten,
                         userDescription );
    dict info;
    info["appName"] = appName;
    info["libraryVersionString"] = libraryVersionString;
    info["libraryVersion"] = libraryVersion;
    info["whenWritten"] = whenWritten;
    info["userDescription"] = userDescription;

    return info;
}

//-*****************************************************************************
void register_archiveinfo()
{
    def( "CreateArchiveWithInfo",
         CreateArchiveWithInfoWrapper,
         ( arg( "fileName" ), arg( "ApplicationWriter" ),
           arg( "UserDescription" ), arg( "argument" ) = Abc::Argument(),
           arg( "argument" ) = Abc::Argument(), arg( "asOgawa" ) = true ),
         "Create an OArchive with the passed arguments" );
    def( "GetArchiveInfo",
         GetArchiveInfoWrapper,
         ( arg( "IArchive" ) ),
         "Return a dictionary that contains info of the given IArchive" );

    def( "GetLibraryVersionShort",
         AbcA::GetLibraryVersionShort,
         "Return the version number of Alembic Library and date built in a "
         "string"
         );
    def( "GetLibraryVersion",
         AbcA::GetLibraryVersion,
         "Return the version number of Alembic Library in a string" );
}
