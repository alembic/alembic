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

#ifndef Alembic_AbcGeom_OPolyMesh_h
#define Alembic_AbcGeom_OPolyMesh_h

#include <Alembic/Util/Export.h>
#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/OFaceSet.h>
#include <Alembic/AbcGeom/OGeomParam.h>
#include <Alembic/AbcGeom/OGeomBase.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ALEMBIC_EXPORT OPolyMeshSchema
    : public OGeomBaseSchema<PolyMeshSchemaInfo>
{
public:
    //-*************************************************************************
    // POLY MESH SCHEMA SAMPLE TYPE
    //-*************************************************************************
    class Sample
    {
    public:
        //! Creates a default sample with no data in it.
        //! ...
        Sample() { reset(); }

        //! Creates a sample with position data but no index
        //! or count data. For specifying samples after the first one
        Sample( const Abc::P3fArraySample &iPos )
          : m_positions( iPos ) {}


        //! Creates a sample with position data, index data, count data,
        //! and optional UV and Normals data.
        //! For specifying samples with an explicit topology. The first
        //! sample must be full like this. Subsequent samples may also
        //! be full like this, which would indicate a change of topology
        Sample( const Abc::P3fArraySample &iPos,
                const Abc::Int32ArraySample &iInd,
                const Abc::Int32ArraySample &iCnt,
                const OV2fGeomParam::Sample &iUVs = OV2fGeomParam::Sample(),
                const ON3fGeomParam::Sample &iNormals = ON3fGeomParam::Sample() )
          : m_positions( iPos )
          , m_indices( iInd )
          , m_counts( iCnt )
          , m_uvs( iUVs )
          , m_normals( iNormals )
        {}

        const Abc::P3fArraySample &getPositions() const { return m_positions; }
        void setPositions( const Abc::P3fArraySample &iSmp )
        { m_positions = iSmp; }

        // velocities accessor
        const Abc::V3fArraySample &getVelocities() const { return m_velocities; }
        void setVelocities( const Abc::V3fArraySample &iVelocities )
        { m_velocities = iVelocities; }

        const Abc::Int32ArraySample &getFaceIndices() const { return m_indices; }
        void setFaceIndices( const Abc::Int32ArraySample &iSmp )
        { m_indices = iSmp; }

        const Abc::Int32ArraySample &getFaceCounts() const { return m_counts; }
        void setFaceCounts( const Abc::Int32ArraySample &iCnt )
        { m_counts = iCnt; }

        const Abc::Box3d &getSelfBounds() const { return m_selfBounds; }
        void setSelfBounds( const Abc::Box3d &iBnds )
        { m_selfBounds = iBnds; }

        const OV2fGeomParam::Sample &getUVs() const { return m_uvs; }
        void setUVs( const OV2fGeomParam::Sample &iUVs )
        { m_uvs = iUVs; }

        const ON3fGeomParam::Sample &getNormals() const { return m_normals; }
        void setNormals( const ON3fGeomParam::Sample &iNormals )
        { m_normals = iNormals; }

        void reset()
        {
            m_positions.reset();
            m_indices.reset();
            m_counts.reset();

            m_selfBounds.makeEmpty();

            m_velocities.reset();
            m_uvs.reset();
            m_normals.reset();
        }

        bool isPartialSample() const
        {
            if( !m_positions.getData() && !m_indices.getData() && !m_counts.getData() )
            {
                if( m_uvs.getVals() || m_normals.getVals() || m_velocities.getData() )
                {
                    return true;
                }
            }

            return false;
        }

    protected:
        Abc::P3fArraySample m_positions;
        Abc::Int32ArraySample m_indices;
        Abc::Int32ArraySample m_counts;

        Abc::Box3d m_selfBounds;

        Abc::V3fArraySample m_velocities;
        OV2fGeomParam::Sample m_uvs;
        ON3fGeomParam::Sample m_normals;

    };
    //-*************************************************************************
    // POLY MESH SCHEMA
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef OPolyMeshSchema this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OPolyMeshSchema
    //! ...
    OPolyMeshSchema()
    {
        m_selectiveExport = false;
        m_numSamples = 0;
        m_timeSamplingIndex = 0;
    }

    //! This constructor creates a new poly mesh writer.
    //! The first argument is an CompoundPropertyWriterPtr to use as a parent.
    //! The next is the name to give the schema which is usually the default
    //! name given by OPolyMesh (.geom)   The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, specify sparse sampling and to set TimeSampling.
    OPolyMeshSchema( AbcA::CompoundPropertyWriterPtr iParent,
                     const std::string &iName,
                     const Abc::Argument &iArg0 = Abc::Argument(),
                     const Abc::Argument &iArg1 = Abc::Argument(),
                     const Abc::Argument &iArg2 = Abc::Argument(),
                     const Abc::Argument &iArg3 = Abc::Argument() );

    //! This constructor creates a new poly mesh writer.
    //! The first argument is an OCompundProperty to use as a parent, and from
    //! which the ErrorHandlerPolicy is derived.  The next is the name to give
    //! the schema which is usually the default name given by OPolyMesh (.geom)
    //! The remaining optional arguments can be used to specify MetaData,
    //! specify sparse sampling and to set TimeSampling.
    OPolyMeshSchema( Abc::OCompoundProperty iParent,
                     const std::string &iName,
                     const Abc::Argument &iArg0 = Abc::Argument(),
                     const Abc::Argument &iArg1 = Abc::Argument(),
                     const Abc::Argument &iArg2 = Abc::Argument() );

    //! Default assignment and copy operator used.

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

    //-*************************************************************************
    // SAMPLE STUFF
    //-*************************************************************************

    //! Get number of samples written so far.
    //! ...
    size_t getNumSamples() const
    { return m_numSamples; }

    //! Set a sample! Sample zero has to have non-degenerate
    //! positions, indices and counts.
    void set( const Sample &iSamp );

    //! Set from previous sample. Will apply to each of positions,
    //! indices, and counts.
    void setFromPrevious();

    void setTimeSampling( uint32_t iIndex );
    void setTimeSampling( AbcA::TimeSamplingPtr iTime );

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
        m_velocitiesProperty.reset();
        m_indicesProperty.reset();
        m_countsProperty.reset();
        m_uvsParam.reset();
        m_normalsParam.reset();

        m_faceSets.clear();

        OGeomBaseSchema<PolyMeshSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( ( OGeomBaseSchema<PolyMeshSchemaInfo>::valid() &&
                   m_positionsProperty.valid() &&
                   m_indicesProperty.valid() &&
                   m_countsProperty.valid() ) ) ||
                   m_selectiveExport;
    }

    // FaceSet stuff
    OFaceSet & createFaceSet( const std::string &iFaceSetName );
    //! Appends the names of any FaceSets for this PolyMesh.
    void getFaceSetNames (std::vector <std::string> & oFaceSetNames);
    OFaceSet getFaceSet( const std::string &iFaceSetName );
    bool hasFaceSet( const std::string &iFaceSetName );

    //! Optional source name for the UV param.
    //! Must be set before the first UV sample is set.
    void setUVSourceName(const std::string & iName);

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( OPolyMeshSchema::valid() );

private:
    void init( uint32_t iTsIdx, bool isSparse );

    //! Set only some property data. Does not need to be a valid schema sample
    //! This is to be used when created a file which will be layered in to
    //! another file.
    void selectiveSet( const Sample &iSamp );

    Abc::OP3fArrayProperty m_positionsProperty;
    Abc::OV3fArrayProperty m_velocitiesProperty;
    Abc::OInt32ArrayProperty m_indicesProperty;
    Abc::OInt32ArrayProperty m_countsProperty;

    // FaceSets created on this PolyMesh
    std::map <std::string, OFaceSet>  m_faceSets;

    OV2fGeomParam m_uvsParam;
    ON3fGeomParam m_normalsParam;

    // optional source name for the UVs
    std::string m_uvSourceName;

    // Write out only some properties (UVs, normals).
    // This is to export data to layer into another file later.
    bool m_selectiveExport;

    // Number of times OPolyMeshSchema::set() has been called
    size_t m_numSamples;

    uint32_t m_timeSamplingIndex;

    void createPositionsProperty();

    void createVelocitiesProperty();

    void createUVsProperty( const Sample &iSamp );

    void createNormalsProperty( const Sample &iSamp );

    // self and child bounds and ArbGeomParams and UserProperties
    // all come from OGeomBaseSchema
};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::OSchemaObject<OPolyMeshSchema> OPolyMesh;

typedef Util::shared_ptr< OPolyMesh > OPolyMeshPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
