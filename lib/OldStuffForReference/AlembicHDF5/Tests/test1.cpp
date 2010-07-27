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

#include <AlembicHDF5/HDF5HelloWorld.h>
#include <exception>
#include <iostream>
#include <stdlib.h>

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    try
    {
        // Make an empty file.
        AlembicHDF5::CreateEmptyFile();
        std::cout << "Created HDF5 file: AlembicHDF5_EmptyFile.h5"
                  << std::endl;
        
        AlembicHDF5::CreateEmptyDataSet();
        std::cout << "Created HDF5 file: AlembicHDF5_EmptyDataSet.h5"
                  << std::endl;

        AlembicHDF5::ReadWriteSmallDataSet();
        std::cout << "Created HDF5 file: AlembicHDF5_SmallDataSet.h5"
                  << std::endl;

        AlembicHDF5::CreateAttribute();
        std::cout << "Created HDF5 file: AlembicHDF5_Attribute.h5"
                  << std::endl;
        
        AlembicHDF5::CreateEmptyGroup();
        std::cout << "Created HDF5 file: AlembicHDF5_EmptyGroup.h5"
                  << std::endl;
        
        AlembicHDF5::CreateSeveralGroups();
        std::cout << "Created HDF5 file: AlembicHDF5_SeveralGroups.h5"
                  << std::endl;
        
        AlembicHDF5::CreateDataSetInGroups();
        std::cout << "Created HDF5 file: AlembicHDF5_DataSetInGroups.h5"
                  << std::endl;
        
        AlembicHDF5::CreateHardLink();
        std::cout << "Created HDF5 file: AlembicHDF5_HardLink.h5"
                  << std::endl;

        AlembicHDF5::CheckDataTypeCopying();
        std::cout << "Tested Datatype copying" << std::endl;
    }
    catch ( std::exception &exc )
    {
        std::cerr << "ERROR: EXCEPTION: " << exc.what() << std::endl;
        exit( -1 );
    }
    catch ( ... )
    {
        std::cerr << "ERROR: UNKNOWN EXCEPTION" << std::endl;
        exit( -1 );
    }

    return 0;
}
