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

#ifndef _Alembic_AbcGeom_OXform_h_
#define _Alembic_AbcGeom_OXform_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>

#include <Alembic/AbcGeom/XformSample.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace Alembic {
namespace AbcGeom {

//! The default value for determining whether a property is actually
//! different from the default.  If it's within this tolerance, the
//! default value is used, which allows Alembic to more efficiently
//! store the data, resulting in smaller Archive size.
static const double kXFORM_DELTA_TOLERANCE = 1.0e-9;

//-*****************************************************************************
class OXformSchema : public Abc::OSchema<XformSchemaInfo>
{
    //-*************************************************************************
    // XFORM SCHEMA
    //-*************************************************************************
public:

    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef Abc::OSchema<XformSchemaInfo> super_type;
    typedef OXformSchema this_type;
    typedef XformSample sample_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OPolyMeshSchema
    //! ...
    OXformSchema() {}

    //! This templated, primary constructor creates a new xform writer.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyWriterPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, and to set TimeSamplingType.
    template <class CPROP_PTR>
    OXformSchema( CPROP_PTR iParentObject,
                  const std::string &iName,
                  const Abc::OArgument &iArg0 = Abc::OArgument(),
                  const Abc::OArgument &iArg1 = Abc::OArgument(),
                  const Abc::OArgument &iArg2 = Abc::OArgument() )
      : Abc::OSchema<XformSchemaInfo>( iParentObject, iName,
                                       iArg0, iArg1, iArg2 )
    {
        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        init( Abc::GetTimeSamplingType( iArg0, iArg1, iArg2 ) );
    }

    //! This constructor does the same as the above, but uses the default
    //! name from the XformSchemaInfo struct.
    template <class CPROP_PTR>
    explicit OXformSchema( CPROP_PTR iParentObject,
                           const Abc::OArgument &iArg0 = Abc::OArgument(),
                           const Abc::OArgument &iArg1 = Abc::OArgument(),
                           const Abc::OArgument &iArg2 = Abc::OArgument() )
      : Abc::OSchema<XformSchemaInfo>( iParentObject,
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

    AbcA::TimeSamplingType getTimeSamplingType() const
    { return m_timeSamplingType; }

    //-*************************************************************************
    // SAMPLE STUFF
    //-*************************************************************************

    //! Get number of samples written so far.
    //! ...
    size_t getNumSamples() const { return m_numSetSamples; }

    //! Set an animated sample.  On first call to set, the sample is modified,
    //! so it can't be const.
    void set( XformSample &ioSamp,
              const Abc::OSampleSelector &iSS = Abc::OSampleSelector() );


    //! Set from previous sample. Will hold the animated channels.
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
        m_childBounds.reset();
        m_timeSamplingType = AbcA::TimeSamplingType();
        m_inherits.reset();
        m_numSetSamples = 0;
        m_sampID = m_nilGen();
        m_ops.reset();
        m_props.clear();
        m_props.resize( 0 );
        super_type::reset();
    }

    //! Valid returns whether this function set is valid.
    bool valid() const
    {
        return ( m_ops && super_type::valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( this_type::valid() );


private:
    void init( const AbcA::TimeSamplingType &iTst );
    boost::uuids::nil_generator m_nilGen;

protected:
    //-*************************************************************************
    // HELPER CLASS
    //-*************************************************************************

    //! The defaulted double property will only create a property
    //! and only bother setting a value when it the value differs from a
    //! known default value. This allows transforms to disappear when they
    //! are identity.
    //! It has some Xform-specific stuff in here, so not worth
    //! making general (yet).
    class ODefaultedDoubleProperty
    {
    public:
        void reset()
        {
            m_parent.reset();
            m_name = "";
            m_errorHandlerPolicy = Abc::ErrorHandler::kThrowPolicy;
            m_default = 0.0;
            m_epsilon = kXFORM_DELTA_TOLERANCE;
            m_property.reset();
        }

        ODefaultedDoubleProperty() { reset(); }

        ODefaultedDoubleProperty( AbcA::CompoundPropertyWriterPtr iParent,
                                  const std::string &iName,
                                  Abc::ErrorHandler::Policy iPolicy,
                                  double iDefault,
                                  double iEpsilon=kXFORM_DELTA_TOLERANCE )
          : m_parent( Abc::GetCompoundPropertyWriterPtr( iParent ) )
          , m_name( iName )
          , m_errorHandlerPolicy( iPolicy )
          , m_default( iDefault )
          , m_epsilon( iEpsilon )
        {
            // We don't build the property until we need it for sure.
        }

        void set( const double &iSamp,
                  const Abc::OSampleSelector &iSS,
                  const std::size_t &iNumSampsSoFar );

        void setFromPrevious( const Abc::OSampleSelector &iSS );

        double getDefaultValue() const { return m_default; }

        std::string getName() const { return m_name; }

    protected:
        // Parent.
        AbcA::CompoundPropertyWriterPtr m_parent;

        // We cache the init stuff.
        std::string m_name;
        Abc::ErrorHandler::Policy m_errorHandlerPolicy;
        double m_default;
        double m_epsilon;

        // The "it". This may not exist.
        Abc::ODoubleProperty m_property;
    }; // END DEFAULTED DOUBLE PROPERTY CLASS DECLARATION


protected:
    // Number of set samples.
    std::size_t m_numSetSamples;

    Abc::OBox3dProperty m_childBounds;

    AbcA::TimeSamplingType m_timeSamplingType;

    Abc::OUcharArrayProperty m_ops;

    std::vector<ODefaultedDoubleProperty> m_props;

    Abc::OBoolProperty m_inherits;

    // ensure that our sample is kept pristine.
    boost::uuids::uuid m_sampID;
};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::OSchemaObject<OXformSchema> OXform;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
