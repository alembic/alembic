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

#ifndef _Alembic_AbcGeom_OPolyMesh_h_
#define _Alembic_AbcGeom_OPolyMesh_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/OGeomParam.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
class OPolyMeshSchema : public Abc::OSchema<PolyMeshSchemaInfo>
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
        Sample( const Abc::V3fArraySample &iPos,
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

        const Abc::V3fArraySample &getPositions() const { return m_positions; }
        void setPositions( const Abc::V3fArraySample &iSmp )
        { m_positions = iSmp; }

        const Abc::Int32ArraySample &getIndices() const { return m_indices; }
        void setIndices( const Abc::Int32ArraySample &iSmp )
        { m_indices = iSmp; }

        const Abc::Int32ArraySample &getCounts() const { return m_counts; }
        void setCounts( const Abc::Int32ArraySample &iCnt )
        { m_counts = iCnt; }

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

            m_uvs.reset();
            m_normals.reset();
        }

    protected:
        Abc::V3fArraySample m_positions;
        Abc::Int32ArraySample m_indices;
        Abc::Int32ArraySample m_counts;

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
    OPolyMeshSchema() {}

    //! This templated, primary constructor creates a new poly mesh writer.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyWriterPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, and to set TimeSamplingType.
    template <class CPROP_PTR>
    OPolyMeshSchema( CPROP_PTR iParentObject,
                     const std::string &iName,

                     const Abc::OArgument &iArg0 = Abc::OArgument(),
                     const Abc::OArgument &iArg1 = Abc::OArgument(),
                     const Abc::OArgument &iArg2 = Abc::OArgument() )
      : Abc::OSchema<PolyMeshSchemaInfo>( iParentObject, iName,
                                            iArg0, iArg1, iArg2 )
    {
        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        init( Abc::GetTimeSamplingType( iArg0, iArg1, iArg2 ) );
    }

    template <class CPROP_PTR>
    explicit OPolyMeshSchema( CPROP_PTR iParentObject,
                              const Abc::OArgument &iArg0 = Abc::OArgument(),
                              const Abc::OArgument &iArg1 = Abc::OArgument(),
                              const Abc::OArgument &iArg2 = Abc::OArgument() )
      : Abc::OSchema<PolyMeshSchemaInfo>( iParentObject,
                                            iArg0, iArg1, iArg2 )
    {
        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        init( Abc::GetTimeSamplingType( iArg0, iArg1, iArg2 ) );
    }

    //! Default copy constructor used.
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
        m_indices.reset();
        m_counts.reset();
        m_uvs.reset();
        m_normals.reset();
        m_arbAttrs.reset();
        Abc::OSchema<PolyMeshSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( Abc::OSchema<PolyMeshSchemaInfo>::valid() &&
                 m_positions.valid() &&
                 m_indices.valid() &&
                 m_counts.valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( OPolyMeshSchema::valid() );

protected:
    void init( const AbcA::TimeSamplingType &iTst );

    Abc::OV3fArrayProperty m_positions;
    Abc::OInt32ArrayProperty m_indices;
    Abc::OInt32ArrayProperty m_counts;

    OV2fGeomParam m_uvs;
    ON3fGeomParam m_normals;

    Abc::OCompoundProperty m_arbAttrs;
};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::OSchemaObject<OPolyMeshSchema> OPolyMesh;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
