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

#ifndef _Alembic_AbcGeom_ICurves_h_
#define _Alembic_AbcGeom_ICurves_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/Basis.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/IGeomParam.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
class ICurvesSchema : public Abc::ISchema<CurvesSchemaInfo>
{
public:
    class Sample
    {
    public:
        typedef Sample this_type;

        // Users don't ever create this data directly.
        Sample() {}

        Abc::V3fArraySamplePtr getPositions() const { return m_positions; }

        std::string getType() const { return m_type; }
        int getNCurves() const { return m_nCurves; }
        Abc::Int32ArraySamplePtr getNVertices() const { return m_nVertices; }
        std::string getWrap() const { return m_wrap; }

        BasisType getUBasis() const { return static_cast<BasisType>( m_uBasis ); }
        BasisType getVBasis() const { return static_cast<BasisType>( m_vBasis ); }

        Abc::V2fArraySamplePtr getWidths() const { return m_widths; }
        Abc::V2fArraySamplePtr getUVs() const { return m_uvs; }
        Abc::V3fArraySamplePtr getNormals() const { return m_normals; }


        Abc::Box3d getSelfBounds() const { return m_selfBounds; }
        Abc::Box3d getChildBounds() const { return m_childBounds; }

        bool valid() const
        {
            return m_positions;
        }

        void reset()
        {
            m_positions.reset();

            m_widths.reset();
            m_uvs.reset();
            m_normals.reset();

            m_nCurves = 1;

            m_type = "cubic";
            m_nVertices.reset();
            m_wrap = "nonperiodic";

            m_uBasis = kBezierBasis;
            m_vBasis = kBezierBasis;

            m_selfBounds.makeEmpty();
            m_childBounds.makeEmpty();
        }

        ALEMBIC_OPERATOR_BOOL( valid() );

    protected:
        friend class ICurvesSchema;
        Abc::V3fArraySamplePtr m_positions;

        Abc::Box3d m_selfBounds;
        Abc::Box3d m_childBounds;

        // type, wrap, and nVertices
        std::string m_type;
        int m_nCurves;
        Abc::Int32ArraySamplePtr m_nVertices;
        std::string m_wrap;

        uint8_t m_uBasis;
        uint8_t m_vBasis;

        Abc::V2fArraySamplePtr m_widths;
        Abc::V2fArraySamplePtr m_uvs;
        Abc::V3fArraySamplePtr m_normals;

    };

    //-*************************************************************************
    // POLY MESH SCHEMA
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef ICurvesSchema this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OPolyMeshSchema
    //! ...
    ICurvesSchema() {}

    //! This templated, explicit function creates a new scalar property reader.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyReaderPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy and to specify
    //! schema interpretation matching.
    template <class CPROP_PTR>
    ICurvesSchema( CPROP_PTR iParentObject,
                     const std::string &iName,
                     const Abc::Argument &iArg0 = Abc::Argument(),
                     const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<PolyMeshSchemaInfo>( iParentObject, iName,
                                            iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! This constructor is the same as above, but with default
    //! schema name used.
    template <class CPROP_PTR>
    explicit ICurvesSchema( CPROP_PTR iParentObject,
                              const Abc::Argument &iArg0 = Abc::Argument(),
                              const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<CurvesSchemaInfo>( iParentObject,
                                            iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Wrap an existing schema object
    template <class CPROP_PTR>
    ICurvesSchema( CPROP_PTR iThis,
                     Abc::WrapExistingFlag iFlag,

                     const Abc::Argument &iArg0 = Abc::Argument(),
                     const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<CurvesSchemaInfo>( iThis, iFlag, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Default copy constructor used.
    //! Default assignment operator used.


    //! Return the number of samples contained in the property.
    //! This can be any number, including zero.
    //! This returns the number of samples that were written, independently
    //! of whether or not they were constant.
    size_t getNumSamples()
    { return m_positions.getNumSamples(); }

    //! Return the topological variance.
    //! This indicates how the mesh may change.
    MeshTopologyVariance getTopologyVariance();

    //! Ask if we're constant - no change in value amongst samples,
    //! regardless of the time sampling.
    bool isConstant() { return getTopologyVariance() == kConstantTopology; }

    //! Time sampling type.
    //!
    AbcA::TimeSamplingType getTimeSamplingType() const
    {
        return m_positions.getTimeSamplingType();
    }

    //! Time information.
    AbcA::TimeSampling getTimeSampling()
    {
        return m_positions.getTimeSampling();
    }

    //-*************************************************************************
    void get( Sample &oSample,
              const Abc::ISampleSelector &iSS = Abc::ISampleSelector() )
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "ICurvesSchema::get()" );

        m_positions.get( oSample.m_positions, iSS );

        m_type.get( oSample.m_type, iSS);
        m_nCurves.get( oSample.m_nCurves, iSS );
        m_nVertices.get( oSample.m_nVertices, iSS );
        m_wrap.get( oSample.m_wrap, iSS );

        if ( m_uBasis )
        {
            m_uBasis.get( oSample.m_uBasis, iSS );
        }

        if ( m_vBasis )
        {
            m_vBasis.get( oSample.m_vBasis, iSS );
        }

        if ( m_normals )
        {
            m_normals.get( oSample.m_normals, iSS);
        }

        if ( m_uvs )
        {
            m_uvs.get( oSample.m_uvs, iSS);
        }

        if ( m_widths )
        {
            m_widths.get( oSample.m_widths, iSS);
        }

        if ( m_selfBounds )
        {
            m_selfBounds.get( oSample.m_selfBounds, iSS );
        }

        if ( m_childBounds && m_childBounds.getNumSamples() > 0 )
        {
            m_childBounds.get( oSample.m_childBounds, iSS );
        }

        // Could error check here.

        ALEMBIC_ABC_SAFE_CALL_END();
    }

    Sample getValue( const Abc::ISampleSelector &iSS = Abc::ISampleSelector() )
    {
        Sample smp;
        get( smp, iSS );
        return smp;
    }


    Abc::IV3fArrayProperty &getPositions(){ return m_positions; }
    Abc::IV2fArrayProperty &getUVs() { return m_uvs; }
    Abc::IV3fArrayProperty &getNormals() { return m_normals; }
    Abc::IV2fArrayProperty &getWidths() { return m_widths; }

    Abc::IUcharProperty &getUBasis() { return m_uBasis; }
    Abc::IUcharProperty &getVBasis() { return m_vBasis; }

    // compound property to use as parent for any arbitrary GeomParams
    // underneath it
    ICompoundProperty getArbGeomParams() { return m_arbGeomParams; }

    //-*************************************************************************
    // ABC BASE MECHANISMS
    // These functions are used by Abc to deal with errors, rewrapping,
    // and so on.
    //-*************************************************************************

    //! Reset returns this function set to an empty, default
    //! state.
    void reset()
    {
        m_positions.reset();

        m_selfBounds.reset();
        m_childBounds.reset();

        m_type.reset();
        m_nCurves.reset();
        m_nVertices.reset();
        m_wrap.reset();

        m_uvs.reset();
        m_normals.reset();
        m_widths.reset();

        m_uBasis.reset();
        m_vBasis.reset();

        m_arbGeomParams.reset();

        Abc::ISchema<CurvesSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( Abc::ISchema<CurvesSchemaInfo>::valid() &&
                 m_positions.valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( ICurvesSchema::valid() );

protected:
    void init( const Abc::Argument &iArg0,
               const Abc::Argument &iArg1 );

    Abc::IV3fArrayProperty m_positions;

    Abc::IV2fArrayProperty m_widths;
    Abc::IV2fArrayProperty m_uvs;
    Abc::IV3fArrayProperty m_normals;

    Abc::IBox3dProperty m_selfBounds;
    Abc::IBox3dProperty m_childBounds;

    Abc::IStringProperty m_type;
    Abc::IInt32Property m_nCurves;
    Abc::IInt32ArrayProperty  m_nVertices;
    Abc::IStringProperty m_wrap;

    Abc::ICompoundProperty m_arbGeomParams;

    Abc::IUcharProperty m_uBasis;
    Abc::IUcharProperty m_vBasis;

};

//-*****************************************************************************
typedef Abc::ISchemaObject<ICurvesSchema> ICurves;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
