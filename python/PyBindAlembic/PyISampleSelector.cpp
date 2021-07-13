//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

//-*****************************************************************************
void register_isampleselector(py::module_& module_handle)
{
    // ISampleSelector
    //
    scope ss =
         class_<Abc::ISampleSelector>(
             module_handle,
             "ISampleSelector",
             "The ISampleSelector utilize the selection of the sample index "
             "for the initialized requested index or time")
             .def( init<AbcA::index_t>(),
                 arg( "requestedIndex" ),
                 "Create an ISampleSelector with the given requested index" )
        .def( init<AbcA::chrono_t,
                  Abc::ISampleSelector::TimeIndexType>(),
                  arg( "requestedTime" ),
                  arg( "requestedIndexType "),
                  "Create an ISampleSelector with the given requested time and "
                  " the optional request index type" )
        .def( init<>(),
                  "Create an ISampleSelector with the requested index value of "
                  "0 and the requested index type of kNearIndex" )
        .def( "getIndex",
              &Abc::ISampleSelector::getIndex,
              arg( "timeSampling" ), arg( "numSamples" ),
              "Return the requested index with the given time sampling and "
              "the number of samples" )
        .def( "getRequestedIndex",
              &Abc::ISampleSelector::getRequestedIndex,
              "Return the requested Index" )
        .def( "getRequestedTime",
              &Abc::ISampleSelector::getRequestedTime,
              "Return the requested time" )
        .def( "getRequestedTimeIndexType",
              &Abc::ISampleSelector::getRequestedTimeIndexType,
              "Return the requested time index type" )
        ;

    // Implicit conversions for ISampleSelector
    //
    implicitly_convertible<AbcA::index_t, Abc::ISampleSelector>();
    implicitly_convertible<AbcA::chrono_t, Abc::ISampleSelector>();

    // TimeIndexType
    //
    enum_<Abc::ISampleSelector::TimeIndexType>(module_handle, "TimeIndexType" )
        .value( "kFloorIndex", Abc::ISampleSelector::kFloorIndex )
        .value( "kCeilIndex", Abc::ISampleSelector::kCeilIndex )
        .value( "kNearIndex", Abc::ISampleSelector::kNearIndex )
        .export_values()
        ;

}
