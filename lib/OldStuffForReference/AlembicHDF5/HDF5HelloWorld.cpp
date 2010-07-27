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
#include <AlembicHDF5/Foundation.h>
#include <iostream>
#include <stdlib.h>

namespace AlembicHDF5 {

//-*****************************************************************************
// From h5_crtfile18.c
void CreateEmptyFile( const std::string &fileName )
{
    // File identifier.
    hid_t       file_id;
    herr_t      status;
    
    // Create a new file using default properties.
    file_id = H5Fcreate( fileName.c_str(),
                         H5F_ACC_TRUNC, // This means replace if existing
                         H5P_DEFAULT,
                         H5P_DEFAULT );
    
    // Terminate access to the file.
    status = H5Fclose( file_id );
}

//-*****************************************************************************
// From h5_crtdat18.c
void CreateEmptyDataSet( const std::string &fileName )
{
    // Identifiers
    hid_t       file_id, dataset_id, dataspace_id;
    hsize_t     dims[2];
    herr_t      status;

    // Create a new file using default properties.
    file_id = H5Fcreate( fileName.c_str(),
                         H5F_ACC_TRUNC,
                         H5P_DEFAULT,
                         H5P_DEFAULT );
    
    // Create the data space for the dataset.
    dims[0] = 4; 
    dims[1] = 6; 
    dataspace_id = H5Screate_simple( 2, dims, NULL );

    
    // Create the dataset.
    dataset_id = H5Dcreate(
        // Location ID.
        // This may be a file identifier, or a group identifier within
        // that file. 
        file_id,

        // Name. This may be either an absolute path in the file or
        // a relative path from loc_id naming the dataset
        "/dset",

        // Data Type ID. Here using a built-in.
        H5T_STD_I32BE,

        // Data space ID. We built that above.
        dataspace_id,

        // Link creation property list:
        // "Governs the creation of the link(s) by which the new dataset
        // is accessed and the creation of any intermediate groups that
        // may be missing"
        H5P_DEFAULT,

        // Dataset creation property list
        H5P_DEFAULT,

        // Dataset access property list
        H5P_DEFAULT );

    // End access to the dataset and release resources used by it.
    status = H5Dclose( dataset_id );

    // Terminate access to the data space.
    status = H5Sclose( dataspace_id );

    // Close the file.
    status = H5Fclose( file_id );
}

//-*****************************************************************************
// From h5_rdwt18.c
void ReadWriteSmallDataSet( const std::string &fileName )
{
    // Identifiers
    hid_t       file_id, dataset_id;
    herr_t      status;
    int         i, j, dset_data[4][6];

    // Okay, so we're going to use the CreateEmptyDataSet function above
    // to make the file first.
    CreateEmptyDataSet( fileName );

    // Initialize the dataset.
    for ( i = 0; i < 4; i++ )
    {
        for ( j = 0; j < 6; j++ )
        {
            dset_data[i][j] = i * 6 + j + 1;
        }
    }

    // Open an existing file.
    // This does not create a file if it does not already exist.
    // This could presumably fail.
    file_id = H5Fopen( fileName.c_str(),

                       // Read/Write
                       H5F_ACC_RDWR,
                       
                       H5P_DEFAULT );

    // Open an existing dataset.
    // This could presumably fail.
    dataset_id = H5Dopen( file_id, "/dset", H5P_DEFAULT );

    // Write the dataset.
    // This function writes raw data from a buffer to a dataset.
    status = H5Dwrite(

        // ID of the dataset to write to.
        dataset_id,
        
        // Datatype of the data in memory 
        H5T_NATIVE_INT,

        // "mem_space_id". Used to specify both the memory dataspace
        // and the selection within that dataspace. mem_space_id can
        // be the constant "H5S_ALL", in which case the file dataspace
        // is used for the memory dataspace and the selection defined
        // with file_space_id is used for the selection within that
        // dataspace.
        H5S_ALL,

        // "file_space_id" is used to specify only the selection within
        // the file dataset's dataspace. Any dataspace specificed in
        // file_space_id is ignored by the library and the dataset's
        // dataspace is always used. "file_space_id" can be the constant
        // "H5S_ALL", which indicates that the entire file dataspace,
        // as defined by the current dimensions of the dataspace, is to
        // be selected.
        H5S_ALL,

        // Transfer property list.
        // This controls how data conversion is done.
        // H5P_DEFAULT uses defaults.
        H5P_DEFAULT,

        // const void *buf
        // This is the data to be written.
        ( const void * )dset_data );

    // Now, read it back?
    // This function reads raw data from a buffer to a dataset.
    status = H5Dread(

        // ID of the dataset to write to.
        dataset_id,

        // Datatype of the data in memory (the memory buffer)
        H5T_NATIVE_INT,

        // "mem_space_id" is used to specify both the memory dataspace
        // and the selection within that dataspace. Can be the constant
        // H5S_ALL, in which case the file dataspace is used for the
        // memory dataspace and the selection defined with file_space_id
        // is used for the selection within that dataspace.
        H5S_ALL,

        // "file_space_id" is used to specify ONLY the selection within
        // the file dataset's dataspace. Any dataspace specified in
        // file_space_id is ignored by the library and the dataset's
        // dataspace is always used.  Can be the constant "H5S_ALL",
        // which indicates that the entire file dataspace, as defined
        // by the current dimensions of the dataset, is to be selected.
        H5S_ALL,

        // Transfer property list.
        H5P_DEFAULT,

        // buffer to write into
        ( void * )dset_data );

    // Close the dataset.
    status = H5Dclose( dataset_id );

    // Close the file.
    status = H5Fclose( file_id );
}

//-*****************************************************************************
// From h5_crtatt18.c
// This uses the previous stuff.
void CreateAttribute( const std::string &fileName )
{
    // Identifiers
    hid_t       file_id, dataset_id, attribute_id, dataspace_id;
    hsize_t     dims;
    int         attr_data[2];
    herr_t      status;

    // We need to establish the existing file using above routines.
    ReadWriteSmallDataSet( fileName );

    // Initialize the attribute data.
    attr_data[0] = 100;
    attr_data[1] = 200;

    // Open an existing file.
    file_id = H5Fopen( fileName.c_str(), H5F_ACC_RDWR, H5P_DEFAULT );

    // Open an existing dataset.
    dataset_id = H5Dopen( file_id, "/dset", H5P_DEFAULT );

    // Create the data space for the attribute.
    dims = 2;
    dataspace_id = H5Screate_simple( 1, &dims, NULL );

    // Create a dataset attribute.
    attribute_id = H5Acreate(

        // This is the location id. It refers to the object
        // that the attribute is being added to.
        // In this case, we're adding attributes to the dataset.
        dataset_id,

        // The attribute name.
        "Units",
        
        // The attribute datatype identifier
        H5T_STD_I32BE,

        // The attribute dataspace identifier
        dataspace_id, 

        // The attribute creation property list identifier.
        H5P_DEFAULT,

        // The attribute access property list identifier.
        H5P_DEFAULT );

    // Write the attribute data.
    status = H5Awrite(

        // Attribute id
        attribute_id,

        // Memory type id
        H5T_NATIVE_INT,

        // ( const void * )buf
        // The actual data.
        ( void * )attr_data );

    // Close the attribute.
    status = H5Aclose( attribute_id );

    // Close the dataspace.
    status = H5Sclose( dataspace_id );

    /* Close to the dataset. */
    status = H5Dclose(dataset_id);

    // Close the file.
    status = H5Fclose( file_id );
}

//-*****************************************************************************
// From h5_crtgrp18.c
void CreateEmptyGroup( const std::string &fileName )
{
    // Identifiers
    hid_t       file_id, group_id;
    herr_t      status;
    
    // Create a new file using default properties.
    file_id = H5Fcreate( fileName.c_str(),

                         // Create and obliterate
                         H5F_ACC_TRUNC,
                         
                         H5P_DEFAULT,
                         H5P_DEFAULT);
    
    // Create a group named "/MyGroup" in the file.
    group_id = H5Gcreate(

        // Location - object in which to create a group. Must
        // be another group?
        file_id,

        // Name of the group
        "/MyGroup",

        // Property list for link creation
        H5P_DEFAULT,

        // Property list for group creation
        H5P_DEFAULT,

        // Property list for group access. (Must be H5P_DEFAULT)
        H5P_DEFAULT );
    
    // Close the group.
    status = H5Gclose( group_id );
    
    // Terminate access to the file. 
    status = H5Fclose( file_id );
}

//-*****************************************************************************
// From h5_crtgrpar18.c
void CreateSeveralGroups( const std::string &fileName )
{
    // Identifiers
    hid_t       file_id, group1_id, group2_id, group3_id;
    herr_t      status;

    // Create a new file using default properties.
    file_id = H5Fcreate( fileName.c_str(),
                         H5F_ACC_TRUNC,
                         H5P_DEFAULT,
                         H5P_DEFAULT);

    // Create group "MyGroup" in the root group using absolute name.
    group1_id = H5Gcreate( file_id, "/MyGroup",
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );

    // Create group "Group_A" in group "MyGroup" using absolute name.
    group2_id = H5Gcreate( file_id, "/MyGroup/Group_A",
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );

    // Create group "Group_B" in group "MyGroup" using relative name
    group3_id = H5Gcreate( group1_id, "Group_B",
                           H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );

    // Close groups.
    status = H5Gclose( group1_id );
    status = H5Gclose( group2_id );
    status = H5Gclose( group3_id );

    // Close the file.
    status = H5Fclose( file_id );
}

//-*****************************************************************************
// From h5_crtgrpd18.c
void CreateDataSetInGroups( const std::string &fileName )
{
    // Identifiers
    hid_t       file_id, group_id, dataset_id, dataspace_id;
    hsize_t     dims[2];
    herr_t      status;
    int         i, j, dset1_data[3][3], dset2_data[2][10];

    // Call the previous function.
    CreateSeveralGroups( fileName );

    // Initialize the first dataset.
    for ( i = 0; i < 3; i++ )
    {
        for ( j = 0; j < 3; j++ )
        {
            dset1_data[i][j] = j + 1;
        }
    }

    // Initialize the second dataset.
    for ( i = 0; i < 2; i++ )
    {
        for ( j = 0; j < 10; j++ )
        {
            dset2_data[i][j] = j + 1;
        }
    }

    // Open an existing file.
    file_id = H5Fopen( fileName.c_str(), H5F_ACC_RDWR, H5P_DEFAULT );

    // Create the data space for the first dataset.
    dims[0] = 3;
    dims[1] = 3;
    dataspace_id = H5Screate_simple( 2, dims, NULL );

    // Create a dataset in group "MyGroup".
    dataset_id = H5Dcreate( file_id,
                            "/MyGroup/dset1",
                            H5T_STD_I32BE,
                            dataspace_id,
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );

    // Write the first dataset.
    status = H5Dwrite( dataset_id,
                       H5T_NATIVE_INT,
                       H5S_ALL, H5S_ALL,
                       H5P_DEFAULT,
                       ( const void * )dset1_data );

    // Close the data space for the first dataset.
    status = H5Sclose( dataspace_id );

    // Close the first dataset. 
    status = H5Dclose( dataset_id );

    // Open an existing group of the specified file.
    group_id = H5Gopen( file_id, "/MyGroup/Group_A", H5P_DEFAULT );

    // Create the data space for the second dataset.
    dims[0] = 2;
    dims[1] = 10;
    dataspace_id = H5Screate_simple( 2, dims, NULL );

    // Create the second dataset in group "Group_A".
    dataset_id = H5Dcreate( group_id,
                            "dset2",
                            H5T_STD_I32BE,
                            dataspace_id, 
                            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT );

    // Write the second dataset.
    status = H5Dwrite( dataset_id,
                       H5T_NATIVE_INT,
                       H5S_ALL, H5S_ALL,
                       H5P_DEFAULT,
                       ( const void * )dset2_data );

    // Close the data space for the second dataset.
    status = H5Sclose( dataspace_id );

    // Close the second dataset
    status = H5Dclose( dataset_id );

    // Close the group.
    status = H5Gclose( group_id );

    // Close the file.
    status = H5Fclose( file_id );
}

//-*****************************************************************************
// Testing the creation of hard links.
void CreateHardLink( const std::string &fileName )
{
    // Identifiers
    hid_t file_id;
    herr_t status;

    // Call previous function
    CreateDataSetInGroups( fileName );

    // Open an existing file.
    file_id = H5Fopen( fileName.c_str(),
                       H5F_ACC_RDWR, H5P_DEFAULT );

    // Create a hard link using absolute names.
    status = H5Lcreate_hard(
        // What to link to
        file_id, "/MyGroup/dset1",

        // What to call the new link
        file_id, "/MyGroup/Group_B/linked_dset1",
        
        H5P_DEFAULT, H5P_DEFAULT );
    
    if ( status < 0 )
    {
        std::cerr << "ERROR: Failed at creating hard link." << std::endl
                  << "STATUS: " << status << std::endl;
        exit( -1 );
    }

    // Close the file.
    status = H5Fclose( file_id );
    if ( status < 0 )
    {
        std::cerr << "ERROR: Couldn't close HDF5 file: " << fileName
                  << std::endl;
        exit( -1 );
    }
}

//-*****************************************************************************
void CheckDataTypeCopying( void )
{
    // Copy the datatype.
    hid_t copied_datatype_id = H5Tcopy( H5T_STD_I32LE );
    if ( copied_datatype_id < 0 )
    {
        std::cerr << "ERROR: Couldn't copy H5T_STD_I32LE"
                  << std::endl;
        exit( -1 );
    }
    std::cout << "Copied H5T_STD_I32LE datatype" << std::endl;

    // Now see if they're equal.
    htri_t eq = H5Tequal( copied_datatype_id,
                          H5T_STD_I32LE );
    std::cout << "Results of H5Tequal: " << ( int )eq << std::endl;
}

} // End namespace AlembicHDF5
