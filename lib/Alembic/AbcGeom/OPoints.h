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

#ifndef _Alembic_AbcGeom_OPoints_h_
#define _Alembic_AbcGeom_OPoints_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
class OPointsSchema : public Abc::OSchema<PointsSchemaInfo>
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
        Sample() {}

        //! Creates a sample with position data but no index
        //! data. For specifying samples after the first one
        Sample( const Abc::V3fArraySample &iPos )
          : m_positions( iPos ) {}

        //! Creates a sample with position data and id data. The first
        //! sample must be full like this. Subsequent samples may also
        //! be full like this, which would indicate a change of topology
        Sample( const Abc::V3fArraySample &iPos,
                const Abc::UInt64ArraySample &iId )
          : m_positions( iPos )
          , m_ids( iId ) {}

        const Abc::V3fArraySample &getPositions() const { return m_positions; }
        void setPositions( const Abc::V3fArraySample &iSmp )
        { m_positions = iSmp; }

        const Abc::UInt64ArraySample &getIds() const { return m_ids; }
        void setIds( const Abc::UInt64ArraySample &iSmp )
        { m_ids = iSmp; }

        const Abc::Box3d &getSelfBounds() const { return m_selfBounds; }
        void setSelfBounds( const Abc::Box3d &iBnds )
        { m_selfBounds = iBnds; }

        const Abc::Box3d &getChildBounds() const { return m_childBounds; }
        void setChildBounds( const Abc::Box3d &iBnds )
        { m_childBounds = iBnds; }


        void reset()
        {
            m_positions.reset();
            m_ids.reset();

            m_selfBounds.makeEmpty();
            m_childBounds.makeEmpty();
        }

    protected:
        Abc::V3fArraySample m_positions;
        Abc::UInt64ArraySample m_ids;

        Abc::Box3d m_selfBounds;
        Abc::Box3d m_childBounds;
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
    OPointsSchema() {}

    //! This templated, primary constructor creates a new poly mesh writer.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyWriterPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, and to set TimeSamplingType.
    template <class CPROP_PTR>
    OPointsSchema( CPROP_PTR iParentObject,
                   const std::string &iName,

                   const Abc::Argument &iArg0 = Abc::Argument(),
                   const Abc::Argument &iArg1 = Abc::Argument(),
                   const Abc::Argument &iArg2 = Abc::Argument() )
      : Abc::OSchema<PointsSchemaInfo>( iParentObject, iName,
                                          iArg0, iArg1, iArg2 )
    {
        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        init( Abc::GetTimeSamplingType( iArg0, iArg1, iArg2 ) );
    }

    template <class CPROP_PTR>
    explicit OPointsSchema( CPROP_PTR iParentObject,
                            const Abc::Argument &iArg0 = Abc::Argument(),
                            const Abc::Argument &iArg1 = Abc::Argument(),
                            const Abc::Argument &iArg2 = Abc::Argument() )
      : Abc::OSchema<PointsSchemaInfo>( iParentObject,
                                     iArg0, iArg1, iArg2 )
    {
        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        init( Abc::GetTimeSamplingType( iArg0, iArg1, iArg2 ) );
    }

    //! Copy constructor.
    OPointsSchema(const OPointsSchema& iCopy)
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
    //! positions, ids and counts.
    void set( const Sample &iSamp,
              const Abc::OSampleSelector &iSS = Abc::OSampleSelector() );

    //! Set from previous sample. Will apply to each of positions,
    //! ids, and counts.
    void setFromPrevious( const Abc::OSampleSelector &iSS );

    //! A container for arbitrary geom params (pseudo-properties settable and
    //! gettable as indexed or not).
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
        m_ids.reset();

        m_selfBounds.reset();
        m_childBounds.reset();

        m_arbGeomParams.reset();

        Abc::OSchema<PointsSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( Abc::OSchema<PointsSchemaInfo>::valid() &&
                 m_positions.valid() &&
                 m_ids.valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( OPointsSchema::valid() );

protected:
    void init( const AbcA::TimeSamplingType &iTst );

    Abc::OV3fArrayProperty m_positions;
    Abc::OUInt64ArrayProperty m_ids;

    Abc::OBox3dProperty m_selfBounds;
    Abc::OBox3dProperty m_childBounds;

    Abc::OCompoundProperty m_arbGeomParams;
};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::OSchemaObject<OPointsSchema> OPoints;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
