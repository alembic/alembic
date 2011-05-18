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

#include <Alembic/AbcGeom/INuPatch.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
bool INuPatchSchema::trimCurveTopologyIsConstant()
{
    return m_trimNumLoops.isConstant() && m_trimNumVertices.isConstant() &&
           m_trimNumCurves.isConstant() && m_trimOrder.isConstant() &&
           m_trimKnot.isConstant() && m_trimMin.isConstant() &&
           m_trimMax.isConstant() && m_trimU.isConstant() &&
           m_trimV.isConstant() && m_trimW.isConstant();
}

//-*****************************************************************************
bool INuPatchSchema::trimCurveTopologyIsHomogenous()
{
    return m_trimNumLoops.isConstant() && m_trimNumVertices.isConstant() &&
           m_trimNumCurves.isConstant() && m_trimOrder.isConstant() &&
           m_trimMin.isConstant() && m_trimMax.isConstant();
}

//-*****************************************************************************
MeshTopologyVariance INuPatchSchema::getTopologyVariance()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "INuPatch::getTopologyVariance()" );

    // check for constant topology.
    // if the surface has trim curves, we must also check those of tology
    // variance.
    if ( m_positions.isConstant() && m_uOrder.isConstant() &&
         m_vOrder.isConstant() && m_uKnot.isConstant() &&
         m_vKnot.isConstant() )
    {
        if ( this -> hasTrimCurve() )
        {
            if ( this -> trimCurveTopologyIsConstant() )
            {
                return kConstantTopology;
            }
            else if ( this -> trimCurveTopologyIsHomogenous() )
            {
                return kHomogenousTopology;
            }
            else
            {
                return kHeterogenousTopology;
            }
        }
        else
        {
            return kConstantTopology;        
        }
    }
    else if ( m_numU.isConstant() && m_numV.isConstant() &&
              m_uOrder.isConstant() & m_vOrder.isConstant() &&
              m_uKnot.isConstant() && m_vKnot.isConstant() )
    {
        
        if ( this -> hasTrimCurve() )
        {
            if ( this -> trimCurveTopologyIsHomogenous() )
            {
                return kHomogenousTopology;
            }
            else
            {
                return kHeterogenousTopology;
            }
        }
        else
        {
            return kHomogenousTopology;
        }
    }


    ALEMBIC_ABC_SAFE_CALL_END();

    return kHeterogenousTopology;
}

//-*****************************************************************************
bool INuPatchSchema::hasTrimCurve()
{
    return this->getPropertyHeader( "trim_nloops" ) != NULL &&
           this->getPropertyHeader( "trim_n" ) != NULL &&
           this->getPropertyHeader( "trim_order" ) != NULL &&
           this->getPropertyHeader( "trim_knot" ) != NULL &&
           this->getPropertyHeader( "trim_min" ) != NULL &&
           this->getPropertyHeader( "trim_max" ) != NULL &&
           this->getPropertyHeader( "trim_u" ) != NULL &&
           this->getPropertyHeader( "trim_v" ) != NULL &&
           this->getPropertyHeader( "trim_w" ) != NULL;
}

//-*****************************************************************************
void INuPatchSchema::get( sample_type &oSample,
                          const Abc::ISampleSelector &iSS)
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "INuPatch::get()" );

    m_positions.get( oSample.m_positions, iSS );
    m_numU.get( oSample.m_numU, iSS );
    m_numV.get( oSample.m_numV, iSS );
    m_uOrder.get( oSample.m_uOrder, iSS );
    m_vOrder.get( oSample.m_vOrder, iSS );
    m_uKnot.get( oSample.m_uKnot, iSS );
    m_vKnot.get( oSample.m_vKnot, iSS );
    
    if ( m_selfBounds )
    {
        m_selfBounds.get( oSample.m_selfBounds, iSS );
    }
    
    if ( m_childBounds && m_childBounds.getNumSamples() > 0 )
    {
        m_childBounds.get( oSample.m_childBounds, iSS );
    }
    
    // handle trim curves
    if ( this->hasTrimCurve() )
    {
        m_trimNumLoops.get( oSample.m_trimNumLoops, iSS );
        m_trimNumCurves.get( oSample.m_trimNumCurves, iSS );
        m_trimNumVertices.get( oSample.m_trimNumVertices, iSS );
        m_trimOrder.get( oSample.m_trimOrder, iSS );
        m_trimKnot.get( oSample.m_trimKnot, iSS );
        m_trimMin.get( oSample.m_trimMin, iSS );
        m_trimMax.get( oSample.m_trimMax, iSS );
        m_trimU.get( oSample.m_trimU, iSS );
        m_trimV.get( oSample.m_trimV, iSS );
        m_trimW.get( oSample.m_trimW, iSS );
    }


    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void INuPatchSchema::init( const Abc::Argument &iArg0,
                            const Abc::Argument &iArg1 )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ICurvesTrait::init()" );

    Abc::Arguments args;
    iArg0.setInto( args );
    iArg1.setInto( args );

    AbcA::CompoundPropertyReaderPtr _this = this->getPtr();

    // required properties
    m_positions = Abc::IV3fArrayProperty( _this, "P",
                                          args.getSchemaInterpMatching() );

    m_numU = Abc::IUInt64Property( _this, "nu",
                                args.getSchemaInterpMatching() );

    m_numV = Abc::IUInt64Property( _this, "nv",
                                args.getSchemaInterpMatching() );

    m_uOrder = Abc::IUInt64Property( _this, "uOrder",
                                args.getSchemaInterpMatching() );

    m_vOrder = Abc::IUInt64Property( _this, "vOrder",
                                args.getSchemaInterpMatching() );

    m_uKnot = Abc::IFloatArrayProperty( _this, "uKnot",
                                args.getSchemaInterpMatching() );    

    m_vKnot = Abc::IFloatArrayProperty( _this, "vKnot",
                                args.getSchemaInterpMatching() ); 

    // optional properties
    if ( this->getPropertyHeader( ".selfBnds" ) != NULL )
    {
        m_selfBounds = Abc::IBox3dProperty( _this, ".selfBnds", iArg0, iArg1 );
    }

    if ( this->getPropertyHeader( ".childBnds" ) != NULL )
    {
        m_childBounds = Abc::IBox3dProperty( _this, ".childBnds", iArg0,
                                             iArg1 );
    }

    // none of the things below here are guaranteed to exist
    if ( this->getPropertyHeader( "N" ) != NULL )
    {
        m_normals = IN3fGeomParam( _this, "N", iArg0, iArg1 );
    }

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
    
    if ( this->hasTrimCurve() )
    {
        m_trimNumLoops = Abc::IUInt64Property( _this, "trim_nloops",
                                              args.getErrorHandlerPolicy() );
        m_trimNumCurves = Abc::IUInt64ArrayProperty( _this, "trim_ncurves",
                                                   args.getErrorHandlerPolicy() );
        m_trimNumVertices = Abc::IUInt64ArrayProperty( _this, "trim_n",
                                             args.getErrorHandlerPolicy() );
        m_trimOrder = Abc::IUInt64ArrayProperty( _this, "trim_order",
                                                 args.getErrorHandlerPolicy() );
        m_trimKnot = Abc::IFloatArrayProperty( _this, "trim_knot",
                                                args.getErrorHandlerPolicy() );
        m_trimMin = Abc::IFloatArrayProperty( _this, "trim_min",
                                               args.getErrorHandlerPolicy() );
        m_trimMax = Abc::IFloatArrayProperty( _this, "trim_max",
                                               args.getErrorHandlerPolicy() );
        m_trimU = Abc::IFloatArrayProperty( _this, "trim_u",
                                             args.getErrorHandlerPolicy() );
        m_trimV = Abc::IFloatArrayProperty( _this, "trim_v",
                                             args.getErrorHandlerPolicy() );
        m_trimW = Abc::IFloatArrayProperty( _this, "trim_w",
                                             args.getErrorHandlerPolicy() );
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace AbcGeom
} // End namespace Alembic
