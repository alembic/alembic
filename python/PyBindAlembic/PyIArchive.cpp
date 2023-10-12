//-*****************************************************************************
//
// Copyright (c) 2009-2014,
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

using namespace py;

const std::string kHDF5 = "HDF5";
const std::string kOgawa = "Ogawa";
const std::string kLayer = "Layer";
const std::string kUnknown = "Unknown";

//-*****************************************************************************
static Abc::IArchive* mkIArchive( const std::string &iName )
{
    Abc::IArchive archive;
    AbcF::IFactory factory;
    factory.setPolicy(Abc::ErrorHandler::kQuietNoopPolicy);
    AbcF::IFactory::CoreType coreType;
    archive = factory.getArchive(iName, coreType);

    if ( coreType == AbcF::IFactory::kUnknown ) {
        throwPythonException( "Unknown core type" );
    }
#ifndef ALEMBIC_WITH_HDF5
    else if ( coreType == AbcF::IFactory::kHDF5 ) {
        throwPythonException( "Unsupported core type: HDF5" );
    }
#endif
    return new Abc::IArchive( archive );
}

//-*****************************************************************************
static Abc::IArchive* mkLayeredIArchive( py::list &iNames )
{
    std::vector< std::string > files;
    for ( int i = 0; i < len(iNames); ++i )
    {
        files.push_back( iNames[i].cast<std::string>() );
    }

    Abc::IArchive archive;
    AbcF::IFactory factory;
    factory.setPolicy(Abc::ErrorHandler::kQuietNoopPolicy);
    AbcF::IFactory::CoreType coreType;
    archive = factory.getArchive( files, coreType );

    if ( coreType == AbcF::IFactory::kUnknown ) {
        throwPythonException( "Unknown core type" );
    }
#ifndef ALEMBIC_WITH_HDF5
    else if ( coreType == AbcF::IFactory::kHDF5 ) {
        throwPythonException( "Unsupported core type: HDF5" );
    }
#endif
    return new Abc::IArchive( archive );
}

//-*****************************************************************************
static std::string getCoreType( Abc::IArchive& archive )
{
    AbcF::IFactory factory;
    factory.setPolicy(Abc::ErrorHandler::kQuietNoopPolicy);
    AbcF::IFactory::CoreType coreType;
    archive = factory.getArchive(archive.getName(), coreType);

    if ( coreType == AbcF::IFactory::kOgawa ) {
        return kOgawa;
    } else if ( coreType == AbcF::IFactory::kHDF5 ) {
        return kHDF5;
    } else {
        return kUnknown;
    };
}

//-*****************************************************************************
void register_iarchive(py::module_& module_handle)
{

    // export the CoreType constants
    //
    module_handle.attr("kHDF5") = kHDF5;
    module_handle.attr("kOgawa") = kOgawa;
    module_handle.attr("kLayer") = kLayer;
    module_handle.attr("kUnknown") = kUnknown;

    // IArchive
    //
    py::class_< Abc::IArchive >(
        module_handle,
        "IArchive",
        "The IArchive class opens an existing Alembic archive for read access")
        .def( py::init(&mkIArchive), py::arg( "fileName" ),
             "Create an IArchive with the given file name" )
         .def( py::init(&mkLayeredIArchive), py::arg( "fileList" ),
              "Create a layered IArchive with the given file name list" )
        .def( "getName",
              &Abc::IArchive::getName,
              "Return the file name" )
        .def( "getTop",
              &Abc::IArchive::getTop,
              "Return the single top-level IObject",
              py::keep_alive<0,1>() )
        .def( "getCoreType",
              &getCoreType,
              "Return the archive CoreType" )
        .def( "getTimeSampling",
              &Abc::IArchive::getTimeSampling,
              py::arg( "index" ) ,
              "Return the TimeSampling with the given index" )
        .def( "getNumTimeSamplings",
              &Abc::IArchive::getNumTimeSamplings,
              "Return the total number of TimeSampling in the Archive" )
        .def( "getMaxNumSamplesForTimeSamplingIndex",
              &Abc::IArchive::getMaxNumSamplesForTimeSamplingIndex,
              py::arg( "index" ),
              "Returns the max number of samples set for the TimeSampling at the given index." )
        .def( "valid", &Abc::IArchive::valid )
        .def( ALEMBIC_PYTHON_BOOL_NAME, &Abc::IArchive::valid )
        .def( "__str__", &Abc::IArchive::getName )
        ;
}
