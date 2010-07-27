//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#ifndef _AlembicHDF5_HDF5HelloWorld_h_
#define _AlembicHDF5_HDF5HelloWorld_h_

#include <string>

namespace AlembicHDF5 {

//-*****************************************************************************
// This function will just create an empty h5 file with the given filename.
void CreateEmptyFile( const std::string &fileName =
                      "AlembicHDF5_EmptyFile.h5" );

void CreateEmptyDataSet( const std::string &fileName =
                         "AlembicHDF5_EmptyDataSet.h5" );

void ReadWriteSmallDataSet( const std::string &fileName =
                            "AlembicHDF5_SmallDataSet.h5" );

void CreateAttribute( const std::string &fileName =
                      "AlembicHDF5_Attribute.h5" );

void CreateEmptyGroup( const std::string &fileName =
                       "AlembicHDF5_EmptyGroup.h5" );

void CreateSeveralGroups( const std::string &fileName =
                          "AlembicHDF5_SeveralGroups.h5" );

void CreateDataSetInGroups( const std::string &fileName =
                            "AlembicHDF5_DataSetInGroups.h5" );

void CreateHardLink( const std::string &fileName =
                     "AlembicHDF5_HardLink.h5" );

void CheckDataTypeCopying( void );

} // End namespace AlembicHDF5

#endif
