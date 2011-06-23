//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <Alembic/AbcGeom/ICurves.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
MeshTopologyVariance ICurvesSchema::getTopologyVariance()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ICurvesSchema::getTopologyVariance()" );

    if ( m_positions.isConstant() && m_nVertices.isConstant() &&
         m_basisAndType.isConstant() )
    {
        return kConstantTopology;
    }

    else if ( m_basisAndType.isConstant() )
    {
        return kHomogenousTopology;
    }
    else
    {
        return kHeterogenousTopology;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw
    return kHeterogenousTopology;
}

//-*****************************************************************************
void ICurvesSchema::init( const Abc::Argument &iArg0,
                          const Abc::Argument &iArg1 )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ICurvesSchema::init()" );

    Abc::Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );

    AbcA::CompoundPropertyReaderPtr _this = this->getPtr();

    m_positions = Abc::IV3fArrayProperty( _this, "P",
                                          args.getSchemaInterpMatching() );

    m_nVertices = Abc::IInt32ArrayProperty( _this, "nVertices",
                                            args.getSchemaInterpMatching());

    m_basisAndType = Abc::IScalarProperty( _this, "curveBasisAndType",
                                          args.getSchemaInterpMatching());

    m_selfBounds = Abc::IBox3dProperty( _this, ".selfBnds", iArg0, iArg1 );

    if ( this->getPropertyHeader( ".childBnds" ) != NULL )
    {
        m_childBounds = Abc::IBox3dProperty( _this, ".childBnds", iArg0,
                                             iArg1 );
    }

    // none of the things below here are guaranteed to exist
    if ( this->getPropertyHeader( "uv" ) != NULL )
    {
        m_uvs = IV2fGeomParam( _this, "uv", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader( "N" ) != NULL )
    {
        m_normals = IN3fGeomParam( _this, "N", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader( "width" ) != NULL )
    {
        m_widths = IFloatGeomParam( _this, "width", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader( ".arbGeomParams" ) != NULL )
    {
        m_arbGeomParams = Abc::ICompoundProperty( _this, ".arbGeomParams",
                                                  args.getErrorHandlerPolicy()
                                                );
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void ICurvesSchema::get( ICurvesSchema::Sample &oSample,
                         const Abc::ISampleSelector &iSS )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ICurvesSchema::get()" );

    if ( ! valid() ) { return; }

    m_positions.get( oSample.m_positions, iSS );
    m_nVertices.get( oSample.m_nVertices, iSS );

    Alembic::Util::uint8_t basisAndType[4];
    m_basisAndType.get( basisAndType, iSS );

    oSample.m_type = static_cast<CurveType>( basisAndType[0] );
    oSample.m_wrap = static_cast<CurvePeriodicity>( basisAndType[1] );
    oSample.m_basis = static_cast<BasisType>( basisAndType[2] );
    // we ignore basisAndType[3] since it is the same as basisAndType[2]

    if ( m_selfBounds )
    {
        m_selfBounds.get( oSample.m_selfBounds, iSS );
    }

    if ( m_childBounds && m_childBounds.getNumSamples() > 0 )
    {
        m_childBounds.get( oSample.m_childBounds, iSS );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

} // End namespace AbcGeom
} // End namespace Alembic
