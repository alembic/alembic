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

#ifndef _FluidSimDemo_MeshDrwHelper_h_
#define _FluidSimDemo_MeshDrwHelper_h_

#include <FluidSimDemo/Foundation.h>
#include <FluidSimDemo/DrawContext.h>

namespace FluidSimDemo {

//-*****************************************************************************
//! \brief Both the SubD and PolyMesh classes draw in the same way, so we
//! create this helper class to do the common work.
class MeshDrwHelper
{
public:
    // These, hopefully, will change to managed vectors soon.
    typedef std::vector<float> FloatArray;
    typedef std::vector<int> IndexArray;
    typedef boost::iterator_range<const float*> ConstFloatArray;
    typedef boost::iterator_range<const int*> ConstIndexArray;
    typedef Imath::Vec3<unsigned int> Tri;
    typedef std::vector<Tri> TriArray;

    // Default constructor
    MeshDrwHelper();
    
    // Destructor
    ~MeshDrwHelper();

    // This is a "full update" of all parameters.
    // If N is null, normals will be computed.
    void updateMesh( ConstFloatArray * i_P,
                     ConstFloatArray * i_N,
                     ConstIndexArray * i_vertexIndices,
                     ConstIndexArray * i_elementFirstIndex );

    // This is an update of just P & N.
    // If you want to update the whole mesh, just make a new
    // mesh drw helper.
    void updateMesh( ConstFloatArray * i_P,
                     ConstFloatArray * i_N );

    // This returns validity.
    bool valid() const { return m_valid; }

    // This returns the bounds
    const Box3d &getBounds() const { return m_bounds; }

    // And, finally, this draws.
    void draw( const DrawContext & i_ctx ) const;

    // This is a weird thing. Just makes the helper invalid
    // by nulling everything out. For internal use.
    void makeInvalid();

    bool first;
protected:
    void updateMeshFinal();
    
    FloatArray m_inputP;
    FloatArray m_inputN;
    IndexArray m_inputIndices;
    IndexArray m_inputStarts;

    bool m_valid;

    Box3d m_bounds;

    TriArray m_triangles;
    FloatArray m_customNormals;
};

} // End namespace FluidSimDemo

#endif
