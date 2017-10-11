//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#ifndef _Alembic_AbcGeom_OPoints_h_
#define _Alembic_AbcGeom_OPoints_h_

#include <Alembic/Util/Export.h>
#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/OGeomParam.h>
#include <Alembic/AbcGeom/OGeomBase.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ALEMBIC_EXPORT OPointsSchema : public OGeomBaseSchema<PointsSchemaInfo>
{
public:
    //-*************************************************************************
    // POINTS SCHEMA SAMPLE TYPE
    //-*************************************************************************
    class Sample
    {
    public:
        //! Creates a default sample with no data in it.
        //! ...
        Sample() { reset(); }

        //! Creates a sample with position data but no id
        //! data. For specifying samples after the first one
        Sample( const Abc::P3fArraySample &iPos,
                const Abc::V3fArraySample &iVelocities = Abc::V3fArraySample(),
                const OFloatGeomParam::Sample &iWidths = \
                OFloatGeomParam::Sample() )
          : m_positions( iPos )
          , m_velocities( iVelocities )
          , m_widths( iWidths )
        {}

        //! Creates a sample with position data and id data. The first
        //! sample must be full like this. Subsequent samples may also
        //! be full like this, which would indicate a change of topology
        Sample( const Abc::P3fArraySample &iPos,
                const Abc::UInt64ArraySample &iId,
                const Abc::V3fArraySample &iVelocities = Abc::V3fArraySample(),
                const OFloatGeomParam::Sample &iWidths = \
                OFloatGeomParam::Sample() )
          : m_positions( iPos )
          , m_velocities( iVelocities )
          , m_ids( iId )
          , m_widths( iWidths )
        {}

        // positions accessor
        const Abc::P3fArraySample &getPositions() const { return m_positions; }
        void setPositions( const Abc::P3fArraySample &iSmp )
        { m_positions = iSmp; }

        // ids accessor
        const Abc::UInt64ArraySample &getIds() const { return m_ids; }
        void setIds( const Abc::UInt64ArraySample &iSmp )
        { m_ids = iSmp; }

        // velocities accessor
        const Abc::V3fArraySample &getVelocities() const { return m_velocities; }
        void setVelocities( const Abc::V3fArraySample &iVelocities )
        { m_velocities = iVelocities; }

        // widths accessor
        const OFloatGeomParam::Sample &getWidths() const { return m_widths; }
        void setWidths( const OFloatGeomParam::Sample &iWidths )
        { m_widths = iWidths; }

        const Abc::Box3d &getSelfBounds() const { return m_selfBounds; }
        void setSelfBounds( const Abc::Box3d &iBnds )
        { m_selfBounds = iBnds; }

        void reset()
        {
            m_positions.reset();
            m_velocities.reset();
            m_ids.reset();
            m_widths.reset();

            m_selfBounds.makeEmpty();
        }

        bool isPartialSample() const
        {
            if( !m_positions.getData() )
            {
                return true;
            }

            return false;
        }

    protected:
        Abc::P3fArraySample m_positions;
        Abc::V3fArraySample m_velocities;
        Abc::UInt64ArraySample m_ids;
        OFloatGeomParam::Sample m_widths;

        Abc::Box3d m_selfBounds;
    };

    //-*************************************************************************
    // POINTS SCHEMA
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef OPointsSchema this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OPointsSchema
    //! ...
    OPointsSchema()
    {
        m_selectiveExport = false;
        m_numSamples = 0;
        m_timeSamplingIndex = 0;
    }

    //! This constructor creates a new poly mesh writer.
    //! The first argument is an CompoundPropertyWriterPtr to use as a parent.
    //! The next is the name to give the schema which is usually the default
    //! name given by OFaceSet (.geom)   The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, specify sparse sampling and to set TimeSampling.
    OPointsSchema( AbcA::CompoundPropertyWriterPtr iParent,
                     const std::string &iName,
                     const Abc::Argument &iArg0 = Abc::Argument(),
                     const Abc::Argument &iArg1 = Abc::Argument(),
                     const Abc::Argument &iArg2 = Abc::Argument(),
                     const Abc::Argument &iArg3 = Abc::Argument() );

    //! This constructor creates a new poly mesh writer.
    //! The first argument is an OCompundProperty to use as a parent, and from
    //! which the ErrorHandlerPolicy is derived.  The next is the name to give
    //! the schema which is usually the default name given by OFaceSet (.geom)
    //! The remaining optional arguments can be used to specify MetaData,
    //! specify sparse sampling and to set TimeSampling.
    OPointsSchema( Abc::OCompoundProperty iParent,
                     const std::string &iName,
                     const Abc::Argument &iArg0 = Abc::Argument(),
                     const Abc::Argument &iArg1 = Abc::Argument(),
                     const Abc::Argument &iArg2 = Abc::Argument() );

    //! Copy constructor.
    OPointsSchema(const OPointsSchema& iCopy)
        : OGeomBaseSchema<PointsSchemaInfo>()
    {
        *this = iCopy;
    }

    //! Default assignment operator used.

    //-*************************************************************************
    // SCHEMA STUFF
    //-*************************************************************************

    //! Return the time sampling
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
    size_t getNumSamples() const { return m_numSamples; }

    //! Set a sample
    void set( const Sample &iSamp );

    //! Set from previous sample. Will apply to each of positions,
    //! ids, velocities, and widths
    void setFromPrevious( );

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
        m_idsProperty.reset();
        m_velocitiesProperty.reset();
        m_widthsParam.reset();

        OGeomBaseSchema<PointsSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( ( OGeomBaseSchema<PointsSchemaInfo>::valid() &&
                 m_positionsProperty.valid() &&
                 m_idsProperty.valid() )
                 || m_selectiveExport );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( OPointsSchema::valid() );

private:
    void init( uint32_t iTsIdx, bool isSparse );

    //! Set only some property data. Does not need to be a valid schema sample
    //! This is to be used when created a file which will be layered in to
    //! another file.
    void selectiveSet( const Sample &iSamp );

    void createPositionProperty();
    void createIdProperty();
    void createVelocityProperty();
    void createWidthsProperty( const Sample &iSamp );

    Abc::OP3fArrayProperty m_positionsProperty;
    Abc::OUInt64ArrayProperty m_idsProperty;
    Abc::OV3fArrayProperty m_velocitiesProperty;
    OFloatGeomParam m_widthsParam;

    // Write out only some properties (UVs, normals).
    // This is to export data to layer into another file later.
    bool m_selectiveExport;

    // Number of times OPolyMeshSchema::set() has been called
    size_t m_numSamples;

    uint32_t m_timeSamplingIndex;

};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::OSchemaObject<OPointsSchema> OPoints;

typedef Util::shared_ptr< OPoints > OPointsPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
