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

//-*****************************************************************************
// Chris - this is a messy version for now, using internal guts.
//-*****************************************************************************

#include <AlembicAsset/Handles.h>
#include <exception>
#include <string>

namespace Abc = AlembicAsset;

//-*****************************************************************************
// Trivially simple mesh class:
//
// protocol: AlembicSimpleMesh
//   property: double[16] globalMatrix;
//   
//   property: double[3] vertices[];
//   property: int faceCounts[];
//   property: int vertIndices[];
//
struct MeshObject
{
    //-*************************************************************************
    // Initialize the object
    void init( Abc::OAssetHandle asset,
               const std::string &meshName,
               int startFrame,
               int endFrame,
               double fps )
    {
        //-*********************************************************************
        // Make the mesh
        m_abcMeshObj = Abc::MakeOObjectHandle(
            // Parent asset
            asset,
            // Name of object
            meshName,
            // "protocol" - hint as to what this thing is
            "AlembicSimpleMesh" );

        //-*********************************************************************
        // Assuming the global matrices are animated.
        // Make a uniform time sampling for them
        size_t numSamps = 1 + ( endFrame - startFrame );
        Abc::SharedOTimeSampling tsamp(
            new Abc::OUniformTimeSampling(
                Abc::Time( ( double )startFrame, fps ),
                numSamps ) );
        saveStartFrame = startFrame;

        //-*********************************************************************
        // Make the global matrix property
        globalMatrixAttr = Abc::MakeSPropertyHandle(
            abcMeshobj,
            "globalMatrix",
            "m44d",
            Abc::DataType( Abc::kFloat64POD, 16 ),
            tsamp );
        
        //-*********************************************************************
        // Make the mesh properties. Not animated.
        vertices = Abc::MakeMPropertyHandle(
            abcMeshObj,
            "vertices",
            "v3d",
            Abc::DataType( Abc::kFloat64POD, 3 ) );
        
        counts = Abc::MakeMPropertyHandle(
            abcMeshObj,
            "faceCounts",
            "none",
            Abc::DataType( Abc::kInt32POD, 1 ) );
        
        indices = Abc::MakeMPropertyHandle(
            abcMeshObj,
            "vertexIndices",
            "none",
            Abc::DataType( Abc::kInt32POD, 1 ) );
    }

    //-*****************************************************************************
    void writeRestPose( )
    {
        // Get global matrix at rest
        Abc::float64_t restGlobalMatrix[16];
        // GetGlobalMatrix();
        
        // Somehow get the vertices, indices, and counts
        size_t NumVertices; // Get somehow
        size_
        Abc::Slab vertexSlab( Abc::DataType( Abc::kFloat64POD, 3 ),
                              
    

    Abc::OObjectHandle m_abcMeshObj;

                              
    int m_tartFrame;

    // The xform attr
    Abc::OSPropertyHandle m_globalMatrix;

    // The vertices, element counts, and indices.
    Abc::OMPropertyHandle vertices;
    Abc::OMPropertyHandle counts;
    Abc::OMPropertyHandle indices;
};

//-*****************************************************************************
void doAThing( const std::string &fileName )
{
    try
    {
        Abc::OAssetHandle asset = Abc::MakeOAssetHandle( fileName );

        doScanOfScene( asset );
    }
    catch ( std::exception &exc )
    {
        std::cerr << "EXCEPTION: " << exc.what() << std::endl;
    }
    catch ( ... )
    {
        std::cerr << "UNKNOWN EXCEPTION: " << exc.what() << std::endl;
    }
}
