//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include <Alembic/AbcGeom/IPolyMesh.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
MeshTopologyVariance IPolyMeshSchema::getTopologyVariance()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IPolyMeshSchema::getTopologyVariance()" );

    if ( m_indices.isConstant() && m_counts.isConstant() )
    {
        if ( m_positions.isConstant() )
        {
            return kConstantTopology;
        }
        else
        {
            return kHomogenousTopology;
        }
    }
    else
    {
        return kHeterogenousTopology;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw
    return kConstantTopology;
}

//-*****************************************************************************
void IPolyMeshSchema::init( const Abc::IArgument &iArg0,
                            const Abc::IArgument &iArg1 )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IPolyMeshTrait::init()" );

    Abc::IArguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );

    m_positions = Abc::IV3fArrayProperty( *this, "P",
                                          args.getSchemaInterpMatching() );
    m_indices = Abc::IInt32ArrayProperty( *this, ".faceIndices",
                                        args.getSchemaInterpMatching() );
    m_counts = Abc::IInt32ArrayProperty( *this, ".faceCounts",
                                       args.getSchemaInterpMatching() );

    m_uvs = IV2fGeomParam( *this, "uv", args.getSchemaInterpMatching() );

    m_normals = IN3fGeomParam( *this, "N", args.getSchemaInterpMatching() );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
