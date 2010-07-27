//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#ifndef _Alembic_Tako_PrivateUtil_h_
#define _Alembic_Tako_PrivateUtil_h_

#include <Alembic/Tako/Foundation.h>
#include <Alembic/Tako/CameraDef.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {

/**
\brief Helper HDF visitor function which adds found frames to a set.
\param pid The HDF parent group id that this function iterates over.
\param name The name of the current child group or dataset.
\param linfo HDF struct that contains information about the link.
\param opdata void pointer to a set of floats.

This function follows the specification for the HDF function H5Literate.
It is used to iterate over all links from a group, and add them to a set
of floats.
*/
herr_t GetFrames(hid_t pid, const char * name, const H5L_info_t *linfo,
    void *opdata);

/**
\brief Helper function that converts a float to a std::string.
\param iVal The value to convert to a std::string.
\return The string value.
*/
std::string floatToString(float iVal);


// older way in which the camera was written out and read
typedef struct {
    FilmFitType filmFit;
    double doubleData[26];
    char orthographic;
    char filmRollOrder;
} HDFCameraCompound_V1;

// used internally for reading and writing the camera data in hdf form
typedef struct {
    FilmFitType filmFit;
    double doubleData[26];
    char orthographic;
    char tumblePivotLocal;
    char filmRollOrder;
} HDFCameraCompound;
}

using namespace TAKO_LIB_VERSION_NS;
}

} // End namespace Alembic

#endif  // _Alembic_Tako_PrivateUtil_h_
