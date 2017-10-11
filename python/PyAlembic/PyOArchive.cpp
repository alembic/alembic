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
static Abc::OArchive* mkOArchive( const std::string &iName,
                                  bool asOgawa = true )
{
    if ( !asOgawa )
    {
#ifdef ALEMBIC_WITH_HDF5
        return new Abc::OArchive( AbcH::WriteArchive(), iName );
#else
        throwPythonException( "Unsupported core type: HDF5" );
#endif
    }

    return new Abc::OArchive( AbcO::WriteArchive(), iName );
}

//-*****************************************************************************
void register_oarchive()
{
    class_< Abc::OArchive>(
         "OArchive",
         "The OArchive class creates an Alembic archive for write access",
         no_init )
        .def( "__init__",
              make_constructor(
                  mkOArchive,
                  default_call_policies(),
                  ( arg( "fileName" ), arg( "asOgawa" ) = true ) ),
              "Create an OArchive with the given file name" )
        .def( "getName",
              &Abc::OArchive::getName,
              "Return the file Name" )
        .def( "getTop",
              &Abc::OArchive::getTop,
              "Return the single top-level OObject",
              with_custodian_and_ward_postcall<0,1>() )
        .def( "getCompressionHint",
              &Abc::OArchive::getCompressionHint,
              "Return the compression applied to array properties\n" )
        .def( "setCompressionHint",
              &Abc::OArchive::setCompressionHint,
              ( arg( "compressionHint" ) ),
              "Set the compression applied to array properties\n"
              "Value of -1 means uncompressed, and values of 0-9 indicate "
              "increasingly compressed data, at the expense of time" )
        .def( "addTimeSampling",
              &Abc::OArchive::addTimeSampling,
              ( arg( "timeSampling" ) ),
              "Add the given TimeSampling to the Archive TimeSampling pool\n"
              "Returns the index assigned to the added TimeSampling\n"
              "index 0 is reserved for uniform time sampling with a start "
              "time of 0 and time per cycle of 1" )
        .def( "getTimeSampling",
              &Abc::OArchive::getTimeSampling,
              "Return the TimeSampling with the given index" )
        .def( "getNumTimeSamplings",
              &Abc::OArchive::getNumTimeSamplings,
              "Return the total number of TimeSampling in the Archive "
              "TimeSampling pool" )
        .def( "valid", &Abc::OArchive::valid )
        .def( "reset", &Abc::OArchive::reset )
        .def( "__nonzero__", &Abc::OArchive::valid )
        .def( "__str__", &Abc::OArchive::getName )
        ;
}
