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

#ifndef _Alembic_AbcGeom_OCurves_h_
#define _Alembic_AbcGeom_OCurves_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/Basis.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/OGeomParam.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
// Curves definition - Similar in form to the Geometric primitive used to specify
// curves in renderman.
// "type"   - linear or cubic, one type for all curves
// "wrap"   - periodic or nonperiodic, one mode for all curves
// ---
// "P"      - vertexes for the curves being written
// "width"  - can be constant or can vary
// "N"      - (just like PolyMesh, via a geom parameter) Normals
// "uv"     - (just like PolyMesh, via a geom parameter) u-v coordinates
class OCurvesSchema : public Abc::OSchema<CurvesSchemaInfo>
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
        Sample() {}

        //! Creates a sample with position data but no index
        //! or count data. For specifying samples after the first one
        Sample( const Abc::V3fArraySample &iPos )
          : m_positions( iPos ) {}


        //! Creates a sample with position data, index data, count data,
        //! and optional UV and Normals data.
        //! For specifying samples with an explicit topology. The first
        //! sample must be full like this. Subsequent samples may also
        //! be full like this, which would indicate a change of topology
        Sample(
                const Abc::V3fArraySample &iPos,
                const std::string &iType = "cubic",
                const Abc::Int32ArraySample &iNVertices = Abc::Int32ArraySample(),
                const std::string &iWrap = "nonperiodic",
                const Abc::V2fArraySample &iWidths = Abc::V2fArraySample(),
                const Abc::V2fArraySample &iUVs = Abc::V2fArraySample(),
                const Abc::V3fArraySample &iNormals = Abc::V3fArraySample(),
		const BasisType &iUBasis = kBezierBasis,
		const BasisType &iVBasis = kBezierBasis )

          : m_positions( iPos ),
            m_type( iType ),
            m_nVertices( iNVertices ),
            m_wrap( iWrap ),
            m_uvs( iUVs ),
            m_normals( iNormals ),
            m_widths( iWidths ),
	    m_uBasis( iUBasis ),
	    m_vBasis( iVBasis )
        {}

        // widths accessor
        const Abc::V2fArraySample &getWidths() const { return m_widths; }
        void setWidths( const Abc::V2fArraySample &iWidths )
        { m_widths = iWidths; }

        // positions accessor
        const Abc::V3fArraySample &getPositions() const { return m_positions; }
        void setPositions( const Abc::V3fArraySample &iSmp )
        { m_positions = iSmp; }

        // type accessors
        void setType( const std::string &iType )
        { m_type = iType; }
        const std::string getType() const { return m_type; }

        // wrap accessors
        void setWrap( const std::string &iWrap )
        { m_wrap = iWrap; }
        const std::string &getWrap() const { return m_wrap; }

        const std::size_t getNumCurves() const { return m_nVertices.size(); }

        //! an array of ints that corresponds to the number
        //! of vertices per curve
        void setCurvesNumVertices( const Abc::Int32ArraySample &iNVertices)
        { m_nVertices = iNVertices; }
        const Abc::Int32ArraySample &getCurvesNumVertices() const
        { return m_nVertices; }

        // getUVs getter
        const Abc::V2fArraySample &getUVs() const { return m_uvs; }

        // setUvs setter
        void setUVs( const Abc::V2fArraySample &iUVs )
        { m_uvs = iUVs; }

        // bounding box accessors
        const Abc::Box3d &getSelfBounds() const { return m_selfBounds; }
        void setSelfBounds( const Abc::Box3d &iBnds )
        { m_selfBounds = iBnds; }

        const Abc::Box3d &getChildBounds() const { return m_childBounds; }
        void setChildBounds( const Abc::Box3d &iBnds )
        { m_childBounds = iBnds; }

        // normal accessors
        const Abc::V3fArraySample &getNormals() const { return m_normals; }
        void setNormals( const Abc::V3fArraySample &iNormals )
        { m_normals = iNormals; }

	// basis accessors
	const BasisType &getUBasis() const { return m_uBasis; }
        void setUBasis( const BasisType &iUBasis )
        { m_uBasis = iUBasis; }

	const BasisType &getVBasis() const { return m_vBasis; }
        void setVBasis( const BasisType &iVBasis )
        { m_vBasis = iVBasis; }


        void reset()
        {
            m_positions.reset();
            m_uvs.reset();
            m_normals.reset();
            m_widths.reset();

            m_nVertices.reset();
            m_type = "cubic";
            m_wrap = "nonperiodic";

            m_selfBounds.makeEmpty();
            m_childBounds.makeEmpty();

	    m_uBasis = kBezierBasis;
	    m_vBasis = kBezierBasis;
        }

    protected:

        // properties
        Abc::V3fArraySample m_positions;
        Abc::Int32ArraySample m_nVertices;

        Abc::V2fArraySample m_uvs;
        Abc::V3fArraySample m_normals;
        Abc::V2fArraySample m_widths;

        std::string m_type;
        std::string m_wrap;

	BasisType m_uBasis;
	BasisType m_vBasis;

        // bounding box attributes
        Abc::Box3d m_selfBounds;
        Abc::Box3d m_childBounds;
    };

    //-*************************************************************************
    // CURVE SCHEMA
    //-*************************************************************************

public:

    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef OCurvesSchema this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OCurvesSchema
    //! ...
    OCurvesSchema() {}

    //! This templated, primary constructor creates a new poly mesh writer.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyWriterPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, and to set TimeSamplingType.
    template <class CPROP_PTR>
    OCurvesSchema( CPROP_PTR iParentObject,
                   const std::string &iName,
                   const Abc::Argument &iArg0 = Abc::Argument(),
                   const Abc::Argument &iArg1 = Abc::Argument(),
                   const Abc::Argument &iArg2 = Abc::Argument() )
      : Abc::OSchema<CurvesSchemaInfo>( iParentObject, iName,
                                        iArg0, iArg1, iArg2 )
    {
        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        init( Abc::GetTimeSamplingType( iArg0, iArg1, iArg2 ) );
    }

    template <class CPROP_PTR>
    explicit OCurvesSchema( CPROP_PTR iParentObject,
                            const Abc::Argument &iArg0 = Abc::Argument(),
                            const Abc::Argument &iArg1 = Abc::Argument(),
                            const Abc::Argument &iArg2 = Abc::Argument() )
      : Abc::OSchema<CurvesSchemaInfo>( iParentObject,
                                        iArg0, iArg1, iArg2 )
    {
        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        init( Abc::GetTimeSamplingType( iArg0, iArg1, iArg2 ) );
    }

    OCurvesSchema( const OCurvesSchema& iCopy )
    {
        *this = iCopy;
    }

    //! Default assignment operator used.

    //-*************************************************************************
    // SCHEMA STUFF
    //-*************************************************************************

    //! Return the time sampling type, which is stored on each of the
    //! sub properties.
    AbcA::TimeSamplingType getTimeSamplingType() const
    { return m_positions.getTimeSamplingType(); }

    //-*************************************************************************
    // SAMPLE STUFF
    //-*************************************************************************

    //! Get number of samples written so far.
    //! ...
    size_t getNumSamples()
    { return m_positions.getNumSamples(); }

    //! Set a sample! Sample zero has to have non-degenerate
    //! positions, indices and counts.
    void set( const Sample &iSamp,
              const Abc::OSampleSelector &iSS = Abc::OSampleSelector() );

    //! Set from previous sample. Will apply to each of positions,
    //! indices, and counts.
    void setFromPrevious( const Abc::OSampleSelector &iSS );

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
        //m_nVertices.reset();
        //m_type.reset();
        //m_wrap.reset();

        m_positions.reset();
        m_uvs.reset();
        m_normals.reset();
        m_widths.reset();
        m_arbGeomParams.reset();
        m_nVertices.reset();

        m_uBasis.reset();
        m_vBasis.reset();


        m_selfBounds.reset();
        m_childBounds.reset();

        Abc::OSchema<CurvesSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( Abc::OSchema<CurvesSchemaInfo>::valid() &&
                 m_positions.valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( OCurvesSchema::valid() );

protected:
    void init(  const AbcA::TimeSamplingType &iTst);

    // point data
    Abc::OV3fArrayProperty m_positions;

    // m_type represents the type of the curve, i.e. "linear" "cubic" etc.
    Abc::OStringProperty m_type;
    Abc::OInt32ArrayProperty m_nVertices;
    Abc::OStringProperty m_wrap;

    // per-point data
    Abc::OV2fArrayProperty m_uvs;
    Abc::OV3fArrayProperty m_normals;
    Abc::OV2fArrayProperty m_widths;

    Abc::OCompoundProperty m_arbGeomParams;

    // basis properties
    Abc::OUcharProperty m_uBasis;
    Abc::OUcharProperty m_vBasis;

    // bounding box attributes
    Abc::OBox3dProperty m_selfBounds;
    Abc::OBox3dProperty m_childBounds;
};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::OSchemaObject<OCurvesSchema> OCurves;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
