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

#ifndef _Alembic_AbcGeom_IPoints_h_
#define _Alembic_AbcGeom_IPoints_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
class IPointsSchema : public Abc::ISchema<PointsSchemaInfo>
{
public:
    class Sample
    {
    public:
        typedef Sample this_type;

        // Users don't ever create this data directly.
        Sample() {}

        Abc::V3fArraySamplePtr getPositions() const { return m_positions; }
        Abc::UInt64ArraySamplePtr getIds() const { return m_ids; }

        Abc::Box3d getSelfBounds() const { return m_selfBounds; }
        Abc::Box3d getChildBounds() const { return m_childBounds; }

        bool valid() const
        {
            return m_positions && m_ids;
        }

        void reset()
        {
            m_positions.reset();
            m_ids.reset();

            m_selfBounds.makeEmpty();
            m_childBounds.makeEmpty();
        }

        ALEMBIC_OPERATOR_BOOL( valid() );

    protected:
        friend class IPointsSchema;
        Abc::V3fArraySamplePtr m_positions;
        Abc::UInt64ArraySamplePtr m_ids;

        Abc::Box3d m_selfBounds;
        Abc::Box3d m_childBounds;
    };

    //-*************************************************************************
    // POLY MESH SCHEMA
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef IPointsSchema this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OPointsSchema
    //! ...
    IPointsSchema() {}

    //! This templated, explicit function creates a new scalar property reader.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyReaderPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, and to set TimeSamplingType.
    template <class CPROP_PTR>
    IPointsSchema( CPROP_PTR iParentObject,
                   const std::string &iName,

                   const Abc::IArgument &iArg0 = Abc::IArgument(),
                   const Abc::IArgument &iArg1 = Abc::IArgument() )
      : Abc::ISchema<PointsSchemaInfo>( iParentObject, iName,
                                          iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! This constructor is the same as above, but with default
    //! schema name used.
    template <class CPROP_PTR>
    explicit IPointsSchema( CPROP_PTR iParentObject,
                            const Abc::IArgument &iArg0 = Abc::IArgument(),
                            const Abc::IArgument &iArg1 = Abc::IArgument() )
      : Abc::ISchema<PointsSchemaInfo>( iParentObject,
                                     iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Default copy constructor used.
    //! Default assignment operator used.

    //-*************************************************************************
    // SCALAR PROPERTY READER FUNCTIONALITY
    //-*************************************************************************

    //! Return the number of samples contained in the property.
    //! This can be any number, including zero.
    //! This returns the number of samples that were written, independently
    //! of whether or not they were constant.
    size_t getNumSamples()
    { return std::max( m_positions.getNumSamples(),
                       m_ids.getNumSamples() ); }

    //! Ask if we're constant - no change in value amongst samples,
    //! regardless of the time sampling.
    bool isConstant() { return m_positions.isConstant() && m_ids.isConstant(); }

    //! Time sampling type.
    //!
    AbcA::TimeSamplingType getTimeSamplingType() const
    {
        return m_positions.getTimeSamplingType();
    }

    //! Time information.
    //! ...
    AbcA::TimeSampling getTimeSampling()
    {
        if ( !m_positions.getTimeSampling().isStatic() )
        { return m_positions.getTimeSampling(); }
        else { return m_ids.getTimeSampling(); }
    }

    //-*************************************************************************
    void get( Sample &iSample,
              const Abc::ISampleSelector &iSS = Abc::ISampleSelector() )
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "IPointsSchema::get()" );

        m_positions.get( iSample.m_positions, iSS );
        m_ids.get( iSample.m_ids, iSS );

        m_selfBounds.get( iSample.m_selfBounds, iSS );
        m_childBounds.get( iSample.m_childBounds, iSS );

        // Could error check here.

        ALEMBIC_ABC_SAFE_CALL_END();
    }

    Sample getValue( const Abc::ISampleSelector &iSS = Abc::ISampleSelector() )
    {
        Sample smp;
        get( smp, iSS );
        return smp;
    }

    Abc::ICompoundProperty getArbGeomParams() { return m_arbGeomParams; }

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

        Abc::ISchema<PointsSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( Abc::ISchema<PointsSchemaInfo>::valid() &&
                 m_positions.valid() &&
                 m_ids.valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( IPointsSchema::valid() );

protected:
    void init( const Abc::IArgument &iArg0,
               const Abc::IArgument &iArg1 );

    Abc::IV3fArrayProperty m_positions;
    Abc::IUInt64ArrayProperty m_ids;

    Abc::IBox3dProperty m_selfBounds;
    Abc::IBox3dProperty m_childBounds;

    Abc::ICompoundProperty m_arbGeomParams;
};

//-*****************************************************************************
typedef Abc::ISchemaObject<IPointsSchema> IPoints;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
