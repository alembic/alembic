//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <Alembic/Util/Export.h>
#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/Basis.h>
#include <Alembic/AbcGeom/CurveType.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/IGeomParam.h>
#include <Alembic/AbcGeom/IGeomBase.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ALEMBIC_EXPORT ICurvesSchema : public IGeomBaseSchema<CurvesSchemaInfo>
{
public:
    class Sample
    {
    public:
        typedef Sample this_type;

        // Users don't ever create this data directly.
        Sample() { reset(); }

        Abc::P3fArraySamplePtr getPositions() const { return m_positions; }

        std::size_t getNumCurves() const
        {
            if ( m_nVertices ) { return m_nVertices->size(); }
            else { return 0; }
        }

        Abc::Int32ArraySamplePtr getCurvesNumVertices() const
        { return m_nVertices; }

        CurveType getType() const { return m_type; }
        CurvePeriodicity getWrap() const { return m_wrap; }
        BasisType getBasis() const { return m_basis; }

        // the should not be null if the curve type is kVariableOrder
        Abc::UcharArraySamplePtr getOrders() { return m_orders; }
        Abc::FloatArraySamplePtr getKnots() { return m_knots; }

        // if this is NULL then the weight value of the position for each
        // point is 1
        Abc::FloatArraySamplePtr getPositionWeights() const
        { return m_positionWeights; }

        Abc::Box3d getSelfBounds() const { return m_selfBounds; }
        Abc::V3fArraySamplePtr getVelocities() const { return m_velocities; }

        bool valid() const
        {
            return m_positions.get() != 0 &&
                (m_type != kVariableOrder || m_orders);
        }

        void reset()
        {
            m_positions.reset();
            m_nVertices.reset();
            m_positionWeights.reset();

            m_type = kCubic;
            m_wrap = kNonPeriodic;
            m_basis = kBezierBasis;

            m_orders.reset();
            m_knots.reset();

            m_selfBounds.makeEmpty();
        }

        ALEMBIC_OPERATOR_BOOL( valid() );

    protected:
        friend class ICurvesSchema;
        Abc::P3fArraySamplePtr m_positions;
        Abc::V3fArraySamplePtr m_velocities;
        Abc::FloatArraySamplePtr m_positionWeights;

        Abc::Box3d m_selfBounds;

        // type, wrap, and nVertices
        Abc::Int32ArraySamplePtr m_nVertices;
        Abc::UcharArraySamplePtr m_orders;
        Abc::FloatArraySamplePtr m_knots;

        CurveType m_type;
        BasisType m_basis;
        CurvePeriodicity m_wrap;
    };

    //-*************************************************************************
    // CURVE SCHEMA
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef ICurvesSchema this_type;

    typedef ICurvesSchema::Sample sample_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty ICurvesSchema
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
    ICurvesSchema( CPROP_PTR iParent,
                     const std::string &iName,
                     const Abc::Argument &iArg0 = Abc::Argument(),
                     const Abc::Argument &iArg1 = Abc::Argument() )
      : IGeomBaseSchema<CurvesSchemaInfo>( iParent, iName, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! This constructor is the same as above, but with default
    //! schema name used.
    template <class CPROP_PTR>
    explicit ICurvesSchema( CPROP_PTR iParent,
                            const Abc::Argument &iArg0 = Abc::Argument(),
                            const Abc::Argument &iArg1 = Abc::Argument() )
      : IGeomBaseSchema<CurvesSchemaInfo>( iParent, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Wrap an existing schema object
    template <class CPROP_PTR>
    ICurvesSchema( CPROP_PTR iThis,
                   Abc::WrapExistingFlag iFlag,
                   const Abc::Argument &iArg0 = Abc::Argument(),
                   const Abc::Argument &iArg1 = Abc::Argument() )
      : IGeomBaseSchema<CurvesSchemaInfo>( iThis, iFlag, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Default assignment operator used.

    ICurvesSchema( const ICurvesSchema &iCopy )
      : IGeomBaseSchema<CurvesSchemaInfo>()
    {
        *this = iCopy;
    }

    size_t getNumSamples() const
    { return m_positionsProperty.getNumSamples(); }

    //! Return the topological variance.
    //! This indicates how the mesh may change.
    MeshTopologyVariance getTopologyVariance() const;

    //! Ask if we're constant - no change in value amongst samples,
    //! regardless of the time sampling.
    bool isConstant() const
    { return getTopologyVariance() == kConstantTopology; }

    //! Time sampling type.
    //!
    AbcA::TimeSamplingPtr getTimeSampling() const
    {
        return m_positionsProperty.getTimeSampling();
    }

    //-*************************************************************************
    void get( sample_type &oSample,
              const Abc::ISampleSelector &iSS = Abc::ISampleSelector() ) const;

    sample_type getValue( const Abc::ISampleSelector &iSS =
                          Abc::ISampleSelector() ) const
    {
        sample_type smp;
        get( smp, iSS );
        return smp;
    }

    Abc::IV3fArrayProperty getVelocitiesProperty() const
    {
        return m_velocitiesProperty;
    }

    Abc::IP3fArrayProperty getPositionsProperty() const
    {
        return m_positionsProperty;
    }

    Abc::IInt32ArrayProperty getNumVerticesProperty() const
    {
        return m_nVerticesProperty;
    }

    // if this property is invalid then the weight for every point is 1
    Abc::IFloatArrayProperty getPositionWeightsProperty() const
    {
        return m_positionWeightsProperty;
    }

    IV2fGeomParam getUVsParam() const
    {
        return m_uvsParam;
    }

    IN3fGeomParam getNormalsParam() const
    {
        return m_normalsParam;
    }

    IFloatGeomParam getWidthsParam() const
    {
        return m_widthsParam;
    }

    Abc::IUcharArrayProperty getOrdersProperty() const
    {
        return m_ordersProperty;
    }

    Abc::IFloatArrayProperty getKnotsProperty() const
    {
        return m_knotsProperty;
    }

    //-*************************************************************************
    // ABC BASE MECHANISMS
    // These functions are used by Abc to deal with errors, rewrapping,
    // and so on.
    //-*************************************************************************

    //! Reset returns this function set to an empty, default
    //! state.
    void reset()
    {
        m_positionsProperty.reset();
        m_velocitiesProperty.reset();
        m_nVerticesProperty.reset();

        m_positionWeightsProperty.reset();
        m_ordersProperty.reset();
        m_knotsProperty.reset();

        m_uvsParam.reset();
        m_normalsParam.reset();
        m_widthsParam.reset();

        m_basisAndTypeProperty.reset();

        IGeomBaseSchema<CurvesSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( IGeomBaseSchema<CurvesSchemaInfo>::valid() &&
                 m_positionsProperty.valid() && m_nVerticesProperty.valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( this_type::valid() );

protected:
    void init( const Abc::Argument &iArg0, const Abc::Argument &iArg1 );

    Abc::IP3fArrayProperty m_positionsProperty;
    Abc::IV3fArrayProperty m_velocitiesProperty;
    Abc::IInt32ArrayProperty m_nVerticesProperty;

    // contains type, wrap, ubasis, and vbasis.
    Abc::IScalarProperty m_basisAndTypeProperty;

    IFloatGeomParam m_widthsParam;
    IV2fGeomParam m_uvsParam;
    IN3fGeomParam m_normalsParam;

    // optional
    Abc::IFloatArrayProperty m_positionWeightsProperty;
    Abc::IUcharArrayProperty m_ordersProperty;
    Abc::IFloatArrayProperty m_knotsProperty;
};

//-*****************************************************************************
typedef Abc::ISchemaObject<ICurvesSchema> ICurves;

typedef Util::shared_ptr< ICurves > ICurvesPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
