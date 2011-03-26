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
                                this->getPtr(),
                                ph.getName() ).getNumSamples() );
        }
        else if ( ph.isScalar() )
        {
            max = std::max( max,
                            Abc::IScalarProperty(
                                this->getPtr(),
                                ph.getName() ).getNumSamples() );
        }
    }

    return max;
}

//-*****************************************************************************
void ISubDSchema::get( ISubDSchema::Sample &oSample,
                       const Abc::ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ISubDSchema::get()" );

    m_positions.get( oSample.m_positions, iSS );
    m_faceIndices.get( oSample.m_faceIndices, iSS );
    m_faceCounts.get( oSample.m_faceCounts, iSS );

    m_faceVaryingInterpolateBoundary.get(
        oSample.m_faceVaryingInterpolateBoundary, iSS );
    m_faceVaryingPropagateCorners.get(
        oSample.m_faceVaryingPropagateCorners, iSS );
    m_interpolateBoundary.get( oSample.m_interpolateBoundary, iSS );

    m_selfBounds.get( oSample.m_selfBounds, iSS );

    if ( m_creaseIndices )
    { m_creaseIndices.get( oSample.m_creaseIndices, iSS ); }

    if ( m_creaseLengths )
    { m_creaseLengths.get( oSample.m_creaseLengths, iSS ); }

    if ( m_creaseSharpnesses )
    { m_creaseSharpnesses.get( oSample.m_creaseSharpnesses, iSS ); }

    if ( m_cornerIndices )
    { m_cornerIndices.get( oSample.m_cornerIndices, iSS ); }

    if ( m_cornerSharpnesses )
    { m_cornerSharpnesses.get( oSample.m_cornerSharpnesses, iSS ); }

    if ( m_holes )
    { m_holes.get( oSample.m_holes, iSS ); }

    m_subdScheme.get( oSample.m_subdScheme, iSS );

    if ( m_childBounds.getNumSamples() > 0 )
    { m_childBounds.get( oSample.m_childBounds, iSS ); }

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

    AbcA::CompoundPropertyReaderPtr _this = this->getPtr();

    m_positions = Abc::IV3fArrayProperty( _this, "P",
                                          args.getSchemaInterpMatching() );
    m_faceIndices = Abc::IInt32ArrayProperty( _this, ".faceIndices",
                                            args.getSchemaInterpMatching() );
    m_faceCounts = Abc::IInt32ArrayProperty( _this, ".faceCounts",
                                           args.getSchemaInterpMatching() );

    m_faceVaryingInterpolateBoundary =
        Abc::IInt32Property( _this, ".faceVaryingInterpolateBoundary",
                           args.getSchemaInterpMatching() );
    m_faceVaryingPropagateCorners =
        Abc::IInt32Property( _this, ".faceVaryingPropagateCorners",
                           args.getSchemaInterpMatching() );
    m_interpolateBoundary =
        Abc::IInt32Property( _this, ".interpolateBoundary",
                           args.getSchemaInterpMatching() );

    // creases, corners, and holes optionally exist
    if ( this->getPropertyHeader(".creaseIndices") != NULL)
    {
        m_creaseIndices = Abc::IInt32ArrayProperty( _this, ".creaseIndices",
                                                    args.getSchemaInterpMatching() );
    }

    if ( this->getPropertyHeader(".creaseLengths") != NULL)
    {
        m_creaseLengths = Abc::IInt32ArrayProperty( _this, ".creaseLengths",
                                                    args.getSchemaInterpMatching() );
    }

    if ( this->getPropertyHeader(".creaseSharpnesses") != NULL)
    {
        m_creaseSharpnesses =
             Abc::IFloatArrayProperty( _this,
                                       ".creaseSharpnesses",
                                       args.getSchemaInterpMatching() );
    }

    if ( this->getPropertyHeader(".cornerIndices") != NULL)
    {
        m_cornerIndices =
            Abc::IInt32ArrayProperty( _this, ".cornerIndices",
                                      args.getSchemaInterpMatching() );
    }

    if ( this->getPropertyHeader(".cornerSharpnesses") != NULL)
    {
        m_cornerSharpnesses =
            Abc::IFloatArrayProperty( _this, ".cornerSharpnesses",
                                      args.getSchemaInterpMatching() );
    }

    if ( this->getPropertyHeader(".holes") != NULL)
    {
        m_holes =
            Abc::IInt32ArrayProperty( _this, ".holes",
                                      args.getSchemaInterpMatching() );
    }

    m_subdScheme = Abc::IStringProperty( _this, ".scheme",
                                         args.getSchemaInterpMatching() );

    m_selfBounds = Abc::IBox3dProperty( _this, ".selfBnds", iArg0, iArg1 );
    m_childBounds = Abc::IBox3dProperty( _this, ".childBnds", iArg0, iArg1 );

    // none of the things below here are guaranteed to exist
    if ( this->getPropertyHeader( "uv" ) != NULL )
    {
        m_uvs = IV2fGeomParam( _this, "uv", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader( ".arbGeomParams" ) != NULL )
    {
        m_arbGeomParams = Abc::ICompoundProperty( _this, ".arbGeomParams",
                                                  args.getErrorHandlerPolicy()
                                                );
    }


    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
