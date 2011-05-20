//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#ifndef _Alembic_AbcGeom_ONuPatch_h_
#define _Alembic_AbcGeom_ONuPatch_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/Basis.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/OGeomParam.h>

namespace Alembic {
namespace AbcGeom {

class ONuPatchSchema : public Abc::OSchema<NuPatchSchemaInfo>
{
public:
    //-*************************************************************************
    // NuPatch SCHEMA SAMPLE TYPE
    //-*************************************************************************
    class Sample
    {
    public:
        //! Creates a default sample with no data in it.
        //! ...
        Sample() {}

        Sample(
                const Abc::V3fArraySample &iPos,
                const uint64_t &iNumU,
                const uint64_t &iNumV,
                const uint64_t &iUOrder,
                const uint64_t &iVOrder,
                const Abc::FloatArraySample &iUKnot,
                const Abc::FloatArraySample &iVKnot,
                const ON3fGeomParam::Sample &iNormals = ON3fGeomParam::Sample(),
                const OV2fGeomParam::Sample &iUVs = OV2fGeomParam::Sample()
              ): m_positions( iPos )
               , m_numU( iNumU )
               , m_numV( iNumV )
               , m_uOrder( iUOrder )
               , m_vOrder( iVOrder )
               , m_uKnot( iUKnot )
               , m_vKnot( iVKnot )
               , m_normals( iNormals )
               , m_uvs( iUVs )
               , m_trimNumLoops( 0 )
               , m_trimNumVertices( Abc::UInt64ArraySample() )
               , m_trimOrder( Abc::UInt64ArraySample() )
               , m_trimKnot( Abc::FloatArraySample() )
               , m_trimMin( Abc::FloatArraySample() )
               , m_trimMax( Abc::FloatArraySample() )
               , m_trimU( Abc::FloatArraySample() )
               , m_trimV( Abc::FloatArraySample() )
               , m_trimW( Abc::FloatArraySample() )
            {}


        // positions
        const Abc::V3fArraySample &getPositions() const { return m_positions; }
        void setPositions( const Abc::V3fArraySample &iSmp )
        { m_positions = iSmp; }

        // nu
        const uint64_t &getNu() const { return m_numV; }
        void setNu( const int &iNu )
        { m_numU = iNu; }

        // nv
        const uint64_t &getNv() const { return m_numV; }
        void setNv( const int &iNv )
        { m_numV = iNv; }

        // uOrder
        const uint64_t &getUOrder() const { return m_uOrder; }
        void setUOrder( const int &iUOrder )
        { m_uOrder = iUOrder; }

        // vOrder
        const uint64_t &getVOrder() const { return m_vOrder; }
        void setVOrder( const int &iVOrder )
        { m_vOrder = iVOrder; }

        // uKnot
        const Abc::FloatArraySample &getUKnot() const { return m_uKnot; }
        void setUKnot( const Abc::FloatArraySample &iUKnot )
        { m_uKnot = iUKnot; }

        // vKnot
        const Abc::FloatArraySample &getVKnot() const { return m_vKnot; }
        void setVKnot( const Abc::FloatArraySample &iVKnot )
        { m_vKnot = iVKnot; }

        // uvs
        const OV2fGeomParam::Sample &getUVs() const { return m_uvs; }
        void setUVs( const OV2fGeomParam::Sample &iUVs )
        { m_uvs = iUVs; }

        // normals
        const ON3fGeomParam::Sample &getNormals() const { return m_normals; }
        void setNormals( const ON3fGeomParam::Sample &iNormals )
        { m_normals = iNormals; }

        // bounds
        const Abc::Box3d &getSelfBounds() const { return m_selfBounds; }
        void setSelfBounds( const Abc::Box3d &iBnds )
        { m_selfBounds = iBnds; }

        const Abc::Box3d &getChildBounds() const { return m_childBounds; }
        void setChildBounds( const Abc::Box3d &iBnds )
        { m_childBounds = iBnds; }

        // trim curves
        void setTrimCurve( const uint64_t &i_trim_nLoops,
                           const Abc::UInt64ArraySample &i_trim_nCurves,
                           const Abc::UInt64ArraySample &i_trim_n,
                           const Abc::UInt64ArraySample &i_trim_order,
                           const Abc::FloatArraySample &i_trim_knot,
                           const Abc::FloatArraySample &i_trim_min,
                           const Abc::FloatArraySample &i_trim_max,
                           const Abc::FloatArraySample &i_trim_u,
                           const Abc::FloatArraySample &i_trim_v,
                           const Abc::FloatArraySample &i_trim_w)
        {
            m_trimNumLoops = i_trim_nLoops;
            m_trimNumCurves = i_trim_nCurves;
            m_trimNumVertices = i_trim_n;
            m_trimOrder = i_trim_order;
            m_trimKnot = i_trim_knot;
            m_trimMin = i_trim_min;
            m_trimMax = i_trim_max;
            m_trimU = i_trim_u;
            m_trimV = i_trim_v;
            m_trimW = i_trim_w;

            m_hasTrimCurve = true;
        }

        const uint64_t &getTrimNumLoops() const { return m_trimNumLoops; }
        const Abc::UInt64ArraySample &getTrimNumCurves() const { return m_trimNumCurves; }
        const Abc::UInt64ArraySample &getTrimNumVertices() const { return m_trimNumVertices; }
        const Abc::UInt64ArraySample &getTrimOrder() const { return m_trimOrder; }
        const Abc::FloatArraySample &getTrimKnot() const { return m_trimKnot; }
        const Abc::FloatArraySample &getTrimMin() const { return m_trimMin; }
        const Abc::FloatArraySample &getTrimMax() const { return m_trimMax; }
        const Abc::FloatArraySample &getTrimU() const { return m_trimU; }
        const Abc::FloatArraySample &getTrimV() const { return m_trimV; }
        const Abc::FloatArraySample &getTrimW() const { return m_trimW; }

        const bool hasTrimCurve() const { return m_trimNumLoops &&
                                            m_trimNumCurves &&
                                            m_trimNumVertices &&
                                            m_trimOrder &&
                                            m_trimKnot &&
                                            m_trimMin &&
                                            m_trimMax &&
                                            m_trimU &&
                                            m_trimV &&
                                            m_trimW; }

        void reset()
        {
            m_positions.reset();
            m_numU = 1;
            m_numV = 1;
            m_uOrder = 1;
            m_vOrder = 1;
            m_uKnot.reset();
            m_vKnot.reset();
            m_normals.reset();
            m_uvs.reset();
            m_selfBounds.makeEmpty();
            m_childBounds.makeEmpty();

            // reset trim curves
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
            m_hasTrimCurve = false;
        }

    protected:

        // required properties
        Abc::V3fArraySample m_positions;
        uint64_t m_numU;
        uint64_t m_numV;
        uint64_t m_uOrder;
        uint64_t m_vOrder;
        Abc::FloatArraySample m_uKnot;
        Abc::FloatArraySample m_vKnot;

        // optional properties
        ON3fGeomParam::Sample m_normals;
        OV2fGeomParam::Sample m_uvs;

        // optional trim curves
        uint64_t m_trimNumLoops;
        Abc::UInt64ArraySample m_trimNumCurves;
        Abc::UInt64ArraySample m_trimNumVertices;
        Abc::UInt64ArraySample m_trimOrder;
        Abc::FloatArraySample m_trimKnot;
        Abc::FloatArraySample m_trimMin;
        Abc::FloatArraySample m_trimMax;
        Abc::FloatArraySample m_trimU;
        Abc::FloatArraySample m_trimV;
        Abc::FloatArraySample m_trimW;
        bool m_hasTrimCurve;

        // bounds
        Abc::Box3d m_selfBounds;
        Abc::Box3d m_childBounds;
    };

    //-*************************************************************************
    // NuPatch SCHEMA
    //-*************************************************************************

public:

    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef ONuPatchSchema this_type;
    typedef ONuPatchSchema::Sample sample_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty ONuPatchSchema
    //! ...
    ONuPatchSchema() {}

    //! This templated, primary constructor creates a new poly mesh writer.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyWriterPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, and to set TimeSamplingType.
    template <class CPROP_PTR>
    ONuPatchSchema( CPROP_PTR iParentObject,
                     const std::string &iName,
                     const Abc::Argument &iArg0 = Abc::Argument(),
                     const Abc::Argument &iArg1 = Abc::Argument(),
                     const Abc::Argument &iArg2 = Abc::Argument() )
      : Abc::OSchema<NuPatchSchemaInfo>( iParentObject, iName,
                                            iArg0, iArg1, iArg2 )
    {

        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        AbcA::TimeSamplingPtr tsPtr =
            Abc::GetTimeSampling( iArg0, iArg1, iArg2 );

        AbcA::index_t tsIndex =
            Abc::GetTimeSamplingIndex( iArg0, iArg1, iArg2 );

        if ( tsPtr )
        {
            tsIndex = iParentObject->getObject()->getArchive()->
                addTimeSampling( *tsPtr );
        }

        m_timeSamplingIndex = tsIndex;

        init( tsIndex );
    }

    template <class CPROP_PTR>
    explicit ONuPatchSchema( CPROP_PTR iParent,
                              const Abc::Argument &iArg0 = Abc::Argument(),
                              const Abc::Argument &iArg1 = Abc::Argument(),
                              const Abc::Argument &iArg2 = Abc::Argument() )
      : Abc::OSchema<NuPatchSchemaInfo>( iParent, iArg0, iArg1, iArg2 )
    {
        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        AbcA::TimeSamplingPtr tsPtr =
            Abc::GetTimeSampling( iArg0, iArg1, iArg2 );

        AbcA::index_t tsIndex =
            Abc::GetTimeSamplingIndex( iArg0, iArg1, iArg2 );

        if ( tsPtr )
        {
            tsIndex = iParent->getObject()->getArchive()->
                addTimeSampling( *tsPtr );
        }

        m_timeSamplingIndex = tsIndex;

        init( tsIndex );
    }

    //! Copy constructor.
    ONuPatchSchema(const ONuPatchSchema& iCopy)
    {
        *this = iCopy;
    }

    //-*************************************************************************
    // SCHEMA STUFF
    //-*************************************************************************

    //! Return the time sampling type, which is stored on each of the
    //! sub properties.
    AbcA::TimeSamplingPtr getTimeSampling()
    { return m_positions.getTimeSampling(); }

    //-*************************************************************************
    // SAMPLE STUFF
    //-*************************************************************************

    //! Get number of samples written so far.
    //! ...
    size_t getNumSamples()
    { return m_positions.getNumSamples(); }

    //! Set a sample! 
    void set( const sample_type &iSamp );

    //! Set from previous sample. Will apply to each of positions,
    //! indices, and counts.
    void setFromPrevious();

    Abc::OCompoundProperty getArbGeomParams();

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
        
        m_normals.reset();
        m_uvs.reset();

        m_selfBounds.reset();
        m_childBounds.reset();

        // reset trim curve attributes
        m_trimNumLoops.reset();
        m_trimNumVertices.reset();
        m_trimOrder.reset();
        m_trimKnot.reset();
        m_trimMin.reset();
        m_trimMax.reset();
        m_trimU.reset();
        m_trimV.reset();
        m_trimW.reset();

        Abc::OSchema<NuPatchSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( Abc::OSchema<NuPatchSchemaInfo>::valid() &&
                 m_positions.valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( ONuPatchSchema::valid() );

protected:
    void init( const AbcA::index_t iTsIdx );

    AbcA::index_t m_timeSamplingIndex;

    // point data
    Abc::OV3fArrayProperty m_positions;

    // required properties
    Abc::OUInt64Property m_numU;
    Abc::OUInt64Property m_numV;
    Abc::OUInt64Property m_uOrder;
    Abc::OUInt64Property m_vOrder;
    Abc::OFloatArrayProperty m_uKnot;
    Abc::OFloatArrayProperty m_vKnot;

    // optional properties
    ON3fGeomParam m_normals;
    OV2fGeomParam m_uvs;

    // optional trim curves
    Abc::OUInt64Property m_trimNumLoops;
    Abc::OUInt64ArrayProperty m_trimNumCurves;
    Abc::OUInt64ArrayProperty m_trimNumVertices;
    Abc::OUInt64ArrayProperty m_trimOrder;
    Abc::OFloatArrayProperty m_trimKnot;
    Abc::OFloatArrayProperty m_trimMin;
    Abc::OFloatArrayProperty m_trimMax;
    Abc::OFloatArrayProperty m_trimU;
    Abc::OFloatArrayProperty m_trimV;
    Abc::OFloatArrayProperty m_trimW;


    // bounds
    Abc::OBox3dProperty m_selfBounds;
    Abc::OBox3dProperty m_childBounds;

    Abc::OCompoundProperty m_arbGeomParams;
};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::OSchemaObject<ONuPatchSchema> ONuPatch;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
