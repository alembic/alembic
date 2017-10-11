//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#ifndef _Alembic_AbcGeom_OCurves_h_
#define _Alembic_AbcGeom_OCurves_h_

#include <Alembic/Util/Export.h>
#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/Basis.h>
#include <Alembic/AbcGeom/CurveType.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/OGeomParam.h>
#include <Alembic/AbcGeom/OGeomBase.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
// Curves definition - Similar in form to the Geometric primitive used to
// specify curves in renderman.
// "type"   - linear or cubic, one type for all curves
// "wrap"   - periodic or nonperiodic, one mode for all curves
// ---
// "P"      - vertexes for the curves being written
// "width"  - can be constant or can vary
// "N"      - (just like PolyMesh, via a geom parameter) Normals
// "uv"     - (just like PolyMesh, via a geom parameter) u-v coordinates
class ALEMBIC_EXPORT OCurvesSchema : public OGeomBaseSchema<CurvesSchemaInfo>
{
public:
    //-*************************************************************************
    // CURVE SCHEMA SAMPLE TYPE
    //-*************************************************************************
    class Sample
    {
    public:
        //! Creates a default sample with no data in it.
        //! ...
        Sample()
        {
            // even though this might not be written out
            // (unless curvesNumVertices and points is set) give some reasonable
            // and predictable defaults
            reset();
            m_type = kCubic;
            m_wrap = kNonPeriodic;
            m_basis = kBezierBasis;
        }

        //! Creates a sample with position data but no index
        //! or count data. For specifying samples after the first one
        Sample( const Abc::P3fArraySample &iPos )
          : m_positions( iPos )
        {
            // even though this might not be written out
            // (unless curvesNumVertices is set) give some reasonable
            // and predictable defaults
            m_type = kCubic;
            m_wrap = kNonPeriodic;
            m_basis = kBezierBasis;
        }


        //! Creates a sample with position data, index data, count data,
        //! and optional UV and Normals data.
        //! For specifying samples with an explicit topology. The first
        //! sample must be full like this. Subsequent samples may also
        //! be full like this, which would indicate a change of topology
        Sample(
                const Abc::P3fArraySample &iPos,
                const Abc::Int32ArraySample &iNVertices,
                const CurveType &iType = kCubic,
                const CurvePeriodicity iWrap = kNonPeriodic,
                const OFloatGeomParam::Sample &iWidths = \
                OFloatGeomParam::Sample(),
                const OV2fGeomParam::Sample &iUVs = OV2fGeomParam::Sample(),
                const ON3fGeomParam::Sample &iNormals = ON3fGeomParam::Sample(),
                const BasisType &iBasis = kBezierBasis,
                const Abc::FloatArraySample &iPosWeight = \
                Abc::FloatArraySample(),
                const Abc::UcharArraySample &iOrders = Abc::UcharArraySample(),
                const Abc::FloatArraySample &iKnots = Abc::FloatArraySample()
        ): m_positions( iPos ),
            m_nVertices( iNVertices ),
            m_type( iType ),
            m_wrap( iWrap ),
            m_widths( iWidths ),
            m_uvs( iUVs ),
            m_normals( iNormals ),
            m_basis( iBasis ),
            m_positionWeights( iPosWeight ),
            m_orders( iOrders ),
            m_knots( iKnots ) {}

        // widths accessor
        const OFloatGeomParam::Sample &getWidths() const { return m_widths; }
        void setWidths( const OFloatGeomParam::Sample &iWidths )
        { m_widths = iWidths; }

        // positions accessor
        const Abc::P3fArraySample &getPositions() const { return m_positions; }
        void setPositions( const Abc::P3fArraySample &iSmp )
        { m_positions = iSmp; }

        // position weights, if it isn't set, it's 1 for every point
        const Abc::FloatArraySample &getPositionWeights() const
        { return m_positionWeights; }

        // type accessors
        void setType( const CurveType &iType )
        { m_type = iType; }
        CurveType getType() const { return m_type; }

        // wrap accessors
        void setWrap( const CurvePeriodicity &iWrap )
        { m_wrap = iWrap; }
        CurvePeriodicity getWrap() const { return m_wrap; }

        std::size_t getNumCurves() const { return m_nVertices.size(); }

        //! an array of ints that corresponds to the number
        //! of vertices per curve
        void setCurvesNumVertices( const Abc::Int32ArraySample &iNVertices)
        { m_nVertices = iNVertices; }
        const Abc::Int32ArraySample &getCurvesNumVertices() const
        { return m_nVertices; }

        // UVs
        const OV2fGeomParam::Sample &getUVs() const { return m_uvs; }
        void setUVs( const OV2fGeomParam::Sample &iUVs )
        { m_uvs = iUVs; }

        // bounding box accessors
        const Abc::Box3d &getSelfBounds() const { return m_selfBounds; }
        void setSelfBounds( const Abc::Box3d &iBnds )
        { m_selfBounds = iBnds; }

        // velocities accessor
        const Abc::V3fArraySample &getVelocities() const { return m_velocities; }
        void setVelocities( const Abc::V3fArraySample &iVelocities )
        { m_velocities = iVelocities; }

        // normal accessors
        const ON3fGeomParam::Sample &getNormals() const { return m_normals; }
        void setNormals( const ON3fGeomParam::Sample &iNormals )
        { m_normals = iNormals; }

        // basis accessors
        BasisType getBasis() const { return m_basis; }
        void setBasis( const BasisType &iBasis )
        { m_basis = iBasis; }

        // orders accessors
        const Abc::UcharArraySample &getOrders() const { return m_orders; }
        void setOrders( const Abc::UcharArraySample &iOrders)
        { m_orders = iOrders; }

        // knot accessors
        const Abc::FloatArraySample &getKnots() const { return m_knots; }
        void setKnots( const Abc::FloatArraySample &iKnots)
        { m_knots = iKnots; }

        void reset()
        {
            m_positions.reset();
            m_positionWeights.reset();
            m_velocities.reset();
            m_uvs.reset();
            m_normals.reset();
            m_widths.reset();

            m_nVertices.reset();

            m_orders.reset();
            m_knots.reset();

            m_selfBounds.makeEmpty();

            m_type = kCubic;
            m_wrap = kNonPeriodic;
            m_basis = kBezierBasis;
        }

        bool isPartialSample() const
        {
            if( !m_positions.getData() )
            {
                if( m_uvs.getVals() || m_normals.getVals() || m_velocities.getData() )
                {
                    return true;
                }
            }

            return false;
        }

    protected:

        // properties
        Abc::P3fArraySample m_positions;
        Abc::V3fArraySample m_velocities;
        Abc::Int32ArraySample m_nVertices;

        CurveType m_type;
        CurvePeriodicity m_wrap;

        OFloatGeomParam::Sample m_widths;
        OV2fGeomParam::Sample m_uvs;
        ON3fGeomParam::Sample m_normals;

        BasisType m_basis;

        // optional properties
        Abc::FloatArraySample m_positionWeights;
        Abc::UcharArraySample m_orders;
        Abc::FloatArraySample m_knots;

        // bounding box attributes
        Abc::Box3d m_selfBounds;

    };

    //-*************************************************************************
    // CURVE SCHEMA
    //-*************************************************************************

public:

    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef OCurvesSchema this_type;
    typedef OCurvesSchema::Sample sample_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OCurvesSchema
    //! ...
    OCurvesSchema()
    {
        m_selectiveExport = false;
        m_numSamples = 0;
        m_timeSamplingIndex = 0;
    }


    //! This constructor creates a new curves writer.
    //! The first argument is the compound property to use as a parent
    //! The remaining optional arguments are the parents ErrorHandlerPolicy,
    //! an override to the ErrorHandlerPolicy, MetaData, and TimeSampling info.
    OCurvesSchema( AbcA::CompoundPropertyWriterPtr iParent,
                   const std::string &iName,
                   const Abc::Argument &iArg0 = Abc::Argument(),
                   const Abc::Argument &iArg1 = Abc::Argument(),
                   const Abc::Argument &iArg2 = Abc::Argument(),
                   const Abc::Argument &iArg3 = Abc::Argument() )
      : OGeomBaseSchema<CurvesSchemaInfo>( iParent, iName,
                                           iArg0, iArg1, iArg2, iArg3)
    {
        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        AbcA::TimeSamplingPtr tsPtr =
            Abc::GetTimeSampling( iArg0, iArg1, iArg2, iArg3 );

        AbcA::index_t tsIndex =
            Abc::GetTimeSamplingIndex( iArg0, iArg1, iArg2, iArg3 );

        if ( tsPtr )
        {
            tsIndex = GetCompoundPropertyWriterPtr( iParent )->getObject(
                )->getArchive()->addTimeSampling( *tsPtr );
        }

        init( tsIndex, Abc::IsSparse( iArg0, iArg1, iArg2, iArg3 ) );
    }

    OCurvesSchema( const OCurvesSchema& iCopy )
        : OGeomBaseSchema<CurvesSchemaInfo>()
    {
        *this = iCopy;
    }

    //! Default assignment operator used.

    //-*************************************************************************
    // SCHEMA STUFF
    //-*************************************************************************

    //! Return the time sampling type, which is stored on each of the
    //! sub properties.
    AbcA::TimeSamplingPtr getTimeSampling() const
    {
        if( m_positionsProperty.valid() )
        {
            return m_positionsProperty.getTimeSampling();
        }
        else
        {
            return getObject().getArchive().getTimeSampling( 0 );
        }
    }

    void setTimeSampling( uint32_t iIndex );
    void setTimeSampling( AbcA::TimeSamplingPtr iTime );

    //-*************************************************************************
    // SAMPLE STUFF
    //-*************************************************************************

    //! Get number of samples written so far.
    //! ...
    size_t getNumSamples() const { return m_numSamples; }

    //! Set a sample! Sample zero has to have non-degenerate
    //! positions, indices and counts.
    void set( const sample_type &iSamp );

    //! Set from previous sample. Will apply to each of positions,
    //! indices, and counts.
    void setFromPrevious();

    //-*************************************************************************
    // ABC BASE MECHANISMS
    // These functions are used by Abc to deal with errors, validity,
    // and so on.
    //-*************************************************************************

    //! Reset returns this function set to an empty, default
    //! state.
    void reset()
    {
        m_positionsProperty.reset();
        m_positionWeightsProperty.reset();
        m_uvsParam.reset();
        m_normalsParam.reset();
        m_widthsParam.reset();
        m_nVerticesProperty.reset();
        m_ordersProperty.reset();
        m_knotsProperty.reset();

        m_basisAndTypeProperty.reset();

        OGeomBaseSchema<CurvesSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( ( OGeomBaseSchema<CurvesSchemaInfo>::valid() &&
                     m_positionsProperty.valid() )
                 || m_selectiveExport );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( this_type::valid() );

private:
    void init( const AbcA::index_t iTsIdx, bool isSparse );

    //! Set only some property data. Does not need to be a valid schema sample
    //! This is to be used when created a file which will be layered in to
    //! another file.
    void selectiveSet( const Sample &iSamp );

    // point data
    Abc::OP3fArrayProperty m_positionsProperty;
    Abc::OInt32ArrayProperty m_nVerticesProperty;

    // Write out only some properties (UVs, normals).
    // This is to export data to layer into another file later.
    bool m_selectiveExport;

    // Number of times OPolyMeshSchema::set() has been called
    size_t m_numSamples;

    uint32_t m_timeSamplingIndex;

    void createPositionProperty();
    void createVertexProperties();
    void createVelocityProperty();
    void createUVsProperty( const Sample &iSamp );
    void createNormalsProperty( const Sample &iSamp );
    void createWidthProperty( const Sample &iSamp );
    void createPositionWeightsProperty();
    void createOrdersProperty();
    void createKnotsProperty();
    void calcBasisAndType(Alembic::Util::uint8_t (&basisAndType)[4], const Sample &iSamp);

    // optional data
    OV2fGeomParam m_uvsParam;
    ON3fGeomParam m_normalsParam;
    OFloatGeomParam m_widthsParam;
    Abc::OV3fArrayProperty m_velocitiesProperty;
    Abc::OFloatArrayProperty m_positionWeightsProperty;
    Abc::OUcharArrayProperty m_ordersProperty;
    Abc::OFloatArrayProperty m_knotsProperty;

    Abc::OScalarProperty m_basisAndTypeProperty;
};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::OSchemaObject<OCurvesSchema> OCurves;

typedef Util::shared_ptr< OCurves > OCurvesPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
