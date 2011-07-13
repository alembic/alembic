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

#ifndef _Alembic_AbcGeom_INuPatch_h_
#define _Alembic_AbcGeom_INuPatch_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/IGeomParam.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
class INuPatchSchema : public Abc::ISchema<NuPatchSchemaInfo>
{
public:
    class Sample
    {
    public:
        typedef Sample this_type;

        // Users don't ever create this data directly.
        Sample() { reset(); }

        Abc::V3fArraySamplePtr getPositions() const { return m_positions; }
        int32_t getNumU() const { return m_numU; }
        int32_t getNumV() const { return m_numV; }
        int32_t getUOrder() const { return m_uOrder; }
        int32_t getVOrder() const { return m_vOrder; }
        Abc::FloatArraySamplePtr getUKnot() const { return m_uKnot; }
        Abc::FloatArraySamplePtr getVKnot() const { return m_vKnot; }

        // if this is NULL then the weight value of the position for each
        // point is 1
        Abc::FloatArraySamplePtr getPositionWeights() const { return m_positionWeights; }

        Abc::Box3d getSelfBounds() const { return m_selfBounds; }
        Abc::Box3d getChildBounds() const { return m_childBounds; }

        // trim curve
        int32_t getTrimNumLoops() const { return m_trimNumLoops; }
        Abc::Int32ArraySamplePtr getTrimNumVertices() const { return m_trimNumVertices; }
        Abc::Int32ArraySamplePtr getTrimNumCurves() const { return m_trimNumCurves; }
        Abc::Int32ArraySamplePtr getTrimOrders() const { return m_trimOrder; }
        Abc::FloatArraySamplePtr getTrimKnots() const { return m_trimKnot; }
        Abc::FloatArraySamplePtr getTrimMins() const { return m_trimMin; }
        Abc::FloatArraySamplePtr getTrimMaxes() const { return m_trimMax; }
        Abc::FloatArraySamplePtr getTrimU() const { return m_trimU; }
        Abc::FloatArraySamplePtr getTrimV() const { return m_trimV; }
        Abc::FloatArraySamplePtr getTrimW() const { return m_trimW; }

        bool hasTrimCurve() const { return m_trimNumLoops != 0; }

        bool valid() const
        {
            return m_positions && m_numU && m_numV && m_uOrder && m_vOrder &&
                m_uKnot && m_vKnot;
        }

        void reset()
        {
            m_positions.reset();
            m_numU = 0;
            m_numV = 0;
            m_uOrder = 0;
            m_vOrder = 0;
            m_uKnot.reset();
            m_vKnot.reset();
            m_positionWeights.reset();

            m_selfBounds.makeEmpty();
            m_childBounds.makeEmpty();

            // trim curve
            m_trimNumLoops = 0;
            m_trimNumCurves.reset();
            m_trimNumVertices.reset();
            m_trimOrder.reset();
            m_trimKnot.reset();
            m_trimMin.reset();
            m_trimMax.reset();
            m_trimU.reset();
            m_trimV.reset();
            m_trimW.reset();
        }

        ALEMBIC_OPERATOR_BOOL( valid() );

    protected:

        friend class INuPatchSchema;

        Abc::V3fArraySamplePtr m_positions;
        int32_t m_numU;
        int32_t m_numV;
        int32_t m_uOrder;
        int32_t m_vOrder;
        Abc::FloatArraySamplePtr m_uKnot;
        Abc::FloatArraySamplePtr m_vKnot;
        Abc::FloatArraySamplePtr m_positionWeights;

        // trim curve
        int32_t m_trimNumLoops;
        Abc::Int32ArraySamplePtr m_trimNumCurves;
        Abc::Int32ArraySamplePtr m_trimNumVertices;
        Abc::Int32ArraySamplePtr m_trimOrder;
        Abc::FloatArraySamplePtr m_trimKnot;
        Abc::FloatArraySamplePtr m_trimMin;
        Abc::FloatArraySamplePtr m_trimMax;
        Abc::FloatArraySamplePtr m_trimU;
        Abc::FloatArraySamplePtr m_trimV;
        Abc::FloatArraySamplePtr m_trimW;
        bool m_hasTrimCurve;

        // bounds
        Abc::Box3d m_selfBounds;
        Abc::Box3d m_childBounds;

    };

    //-*************************************************************************
    // NuPatch Schema
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef INuPatchSchema this_type;
    typedef Sample sample_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor
    INuPatchSchema() {}

    //! copy constructor
    INuPatchSchema(const INuPatchSchema& iCopy)
    {
        *this = iCopy;
    }

    //! This templated, explicit function creates a new scalar property reader.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyReaderPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy and to specify
    //! schema interpretation matching.
    template <class CPROP_PTR>
    INuPatchSchema( CPROP_PTR iParent,
                    const std::string &iName,
                    const Abc::Argument &iArg0 = Abc::Argument(),
                    const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<NuPatchSchemaInfo>( iParent, iName,
                                         iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! This constructor is the same as above, but with default
    //! schema name used.
    template <class CPROP_PTR>
    explicit INuPatchSchema( CPROP_PTR iParent,
                             const Abc::Argument &iArg0 = Abc::Argument(),
                             const Abc::Argument &iArg1 = Abc::Argument() )

      : Abc::ISchema<NuPatchSchemaInfo>( iParent,
                                         iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Wrap an existing schema object
    template <class CPROP_PTR>
    INuPatchSchema( CPROP_PTR iThis,
                    Abc::WrapExistingFlag iFlag,
                    const Abc::Argument &iArg0 = Abc::Argument(),
                    const Abc::Argument &iArg1 = Abc::Argument() )

      : Abc::ISchema<NuPatchSchemaInfo>( iThis, iFlag, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

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

    //! Time information.
    AbcA::TimeSamplingPtr getTimeSampling()
    {
        return m_positions.getTimeSampling();
    }

    void get( sample_type &oSample,
              const Abc::ISampleSelector &iSS = Abc::ISampleSelector() );

    Sample getValue( const Abc::ISampleSelector &iSS = Abc::ISampleSelector() )
    {
        Sample smp;
        get( smp, iSS );
        return smp;
    }

    Abc::IV3fArrayProperty getPositions(){ return m_positions; }

    // if this property is invalid then the weight for every point is 1
    Abc::IFloatArrayProperty getPositionWeights(){ return m_positionWeights; }
    Abc::IFloatArrayProperty getUKnots(){ return m_uKnot; }
    Abc::IFloatArrayProperty getVKnots(){ return m_vKnot; }

    Abc::IBox3dProperty getSelfBounds() { return m_selfBounds; }
    Abc::IBox3dProperty getChildBounds() { return m_childBounds; }

    bool hasTrimCurve() { return m_hasTrimCurve; }
    bool trimCurveTopologyIsHomogenous();
    bool trimCurveTopologyIsConstant();

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
        m_numU.reset();
        m_numV.reset();
        m_uOrder.reset();
        m_vOrder.reset();
        m_uKnot.reset();
        m_vKnot.reset();
        m_positionWeights.reset();

        m_normals.reset();
        m_uvs.reset();

        m_selfBounds.reset();
        m_childBounds.reset();

        // reset trim curve attributes
        m_trimNumLoops.reset();
        m_trimNumCurves.reset();
        m_trimNumVertices.reset();
        m_trimOrder.reset();
        m_trimKnot.reset();
        m_trimMin.reset();
        m_trimMax.reset();
        m_trimU.reset();
        m_trimV.reset();
        m_trimW.reset();

        Abc::ISchema<NuPatchSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( Abc::ISchema<NuPatchSchemaInfo>::valid() &&
                 m_positions.valid() &&
                 m_numU.valid() &&
                 m_numV.valid() &&
                 m_uOrder.valid() &&
                 m_vOrder.valid() &&
                 m_uKnot.valid() &&
                 m_vKnot.valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( INuPatchSchema::valid() );

protected:
    bool hasTrimProps();

protected:
    void init( const Abc::Argument &iArg0,
               const Abc::Argument &iArg1 );

    // required properties
    Abc::IV3fArrayProperty m_positions;
    Abc::IInt32Property m_numU;
    Abc::IInt32Property m_numV;
    Abc::IInt32Property m_uOrder;
    Abc::IInt32Property m_vOrder;
    Abc::IFloatArrayProperty m_uKnot;
    Abc::IFloatArrayProperty m_vKnot;

    // optional
    Abc::IFloatArrayProperty m_positionWeights;
    IN3fGeomParam m_normals;
    IV2fGeomParam m_uvs;

    // optional trim curve properties
    Abc::IInt32Property m_trimNumLoops;
    Abc::IInt32ArrayProperty m_trimNumVertices;
    Abc::IInt32ArrayProperty m_trimNumCurves;
    Abc::IInt32ArrayProperty m_trimOrder;
    Abc::IFloatArrayProperty m_trimKnot;
    Abc::IFloatArrayProperty m_trimMin;
    Abc::IFloatArrayProperty m_trimMax;
    Abc::IFloatArrayProperty m_trimU;
    Abc::IFloatArrayProperty m_trimV;
    Abc::IFloatArrayProperty m_trimW;

    // bounds
    Abc::IBox3dProperty m_selfBounds;
    Abc::IBox3dProperty m_childBounds;

    Abc::ICompoundProperty m_arbGeomParams;

    bool m_hasTrimCurve;
};

//-*****************************************************************************
typedef Abc::ISchemaObject<INuPatchSchema> INuPatch;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
