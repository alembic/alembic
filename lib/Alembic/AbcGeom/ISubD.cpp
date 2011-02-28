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

#include <Alembic/AbcGeom/ISubD.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
MeshTopologyVariance ISubDSchema::getTopologyVariance()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISubDSchema::getTopologyVariance()" );

    if ( m_faceIndices.isConstant() && m_faceCounts.isConstant() )
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
size_t ISubDSchema::getNumSamples()
{
    size_t max = 0;

    for ( size_t i = 0 ; i < this->getNumProperties() ; i++ )
    {
        const AbcA::PropertyHeader& ph = this->getPropertyHeader( i );
        if ( ph.isArray() )
        {
            max = std::max( max,
                            Abc::IArrayProperty(
                                *this,
                                ph.getName() ).getNumSamples() );
        }
        else if ( ph.isScalar() )
        {
            max = std::max( max,
                            Abc::IScalarProperty(
                                *this,
                                ph.getName() ).getNumSamples() );
        }
    }

    return max;
}

//-*****************************************************************************
void ISubDSchema::get( ISubDSchema::Sample &iSample,
                       const Abc::ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISubDSchema::get()" );

    m_positions.get( iSample.m_positions, iSS );
    m_faceIndices.get( iSample.m_faceIndices, iSS );
    m_faceCounts.get( iSample.m_faceCounts, iSS );

    m_faceVaryingInterpolateBoundary.get(
        iSample.m_faceVaryingInterpolateBoundary, iSS );
    m_faceVaryingPropagateCorners.get(
        iSample.m_faceVaryingPropagateCorners, iSS );
    m_interpolateBoundary.get( iSample.m_interpolateBoundary, iSS );

    m_creaseIndices.get( iSample.m_creaseIndices, iSS );
    m_creaseLengths.get( iSample.m_creaseLengths, iSS );
    m_creaseSharpnesses.get( iSample.m_creaseSharpnesses, iSS );

    m_cornerIndices.get( iSample.m_cornerIndices, iSS );
    m_cornerSharpnesses.get( iSample.m_cornerSharpnesses, iSS );

    m_holes.get( iSample.m_holes, iSS );

    m_subdScheme.get( iSample.m_subdScheme, iSS );

    m_selfBounds.get( iSample.m_selfBounds, iSS );
    m_childBounds.get( iSample.m_childBounds, iSS );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void ISubDSchema::init( const Abc::IArgument &iArg0,
                        const Abc::IArgument &iArg1 )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISubDSchema::init()" );

    Abc::IArguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );

    m_positions = Abc::IV3fArrayProperty( *this, "P",
                                          args.getSchemaInterpMatching() );
    m_faceIndices = Abc::IInt32ArrayProperty( *this, ".faceIndices",
                                            args.getSchemaInterpMatching() );
    m_faceCounts = Abc::IInt32ArrayProperty( *this, ".faceCounts",
                                           args.getSchemaInterpMatching() );

    m_faceVaryingInterpolateBoundary =
        Abc::IInt32Property( *this, ".faceVaryingInterpolateBoundary",
                           args.getSchemaInterpMatching() );
    m_faceVaryingPropagateCorners =
        Abc::IInt32Property( *this, ".faceVaryingPropagateCorners",
                           args.getSchemaInterpMatching() );
    m_interpolateBoundary =
        Abc::IInt32Property( *this, ".interpolateBoundary",
                           args.getSchemaInterpMatching() );

    m_creaseIndices = Abc::IInt32ArrayProperty( *this, ".creaseIndices",
                                              args.getSchemaInterpMatching() );
    m_creaseLengths =
        Abc::IInt32ArrayProperty( *this, ".creaseLengths",
                                args.getSchemaInterpMatching() );
    m_creaseSharpnesses =
        Abc::IFloatArrayProperty( *this, ".creaseSharpnesses",
                                  args.getSchemaInterpMatching() );

    m_cornerIndices = Abc::IInt32ArrayProperty( *this, ".cornerIndices",
                                              args.getSchemaInterpMatching() );
    m_cornerSharpnesses =
        Abc::IFloatArrayProperty( *this, ".cornerSharpnesses",
                                  args.getSchemaInterpMatching() );


    m_holes = Abc::IInt32ArrayProperty( *this, ".holes",
                                      args.getSchemaInterpMatching() );

    m_subdScheme = Abc::IStringProperty( *this, ".scheme",
                                         args.getSchemaInterpMatching() );

    m_selfBounds = Abc::IBox3dProperty( *this, ".selfBnds", iArg0, iArg1 );
    m_childBounds = Abc::IBox3dProperty( *this, ".childBnds", iArg0, iArg1 );

    // none of the things below here are guaranteed to exist
    if ( this->getPropertyHeader( "uv" ) != NULL )
    {
        m_uvs = IV2fGeomParam( *this, "uv", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader( ".arbGeomParams" ) != NULL )
    {
        m_arbGeomParams = Abc::ICompoundProperty( *this, ".arbGeomParams",
                                                  args.getErrorHandlerPolicy()
                                                );
    }


    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
