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

#ifndef _Alembic_AbcGeom_OSimpleXform_h_
#define _Alembic_AbcGeom_OSimpleXform_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SimpleXformSample.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>

#include <set>

namespace Alembic {
namespace AbcGeom {

//! The default value for determining whether a property is actually
//! different from the default.  If it's within this tolerance, the
//! default value is used, which allows Alembic to more efficiently
//! store the data, resulting in smaller Archive size.
static const double kSIMPLE_XFORM_DELTA_TOLERANCE = 1.0e-9;

//-*****************************************************************************
//-*****************************************************************************
// THE SCHEMA
//-*****************************************************************************
//-*****************************************************************************
class OSimpleXformSchema : public Abc::OSchema<SimpleXformSchemaInfo>
{
public:
    //-*************************************************************************
    // TYPEDEFS AND IDENTIFIERS
    //-*************************************************************************
    typedef Abc::OSchema<SimpleXformSchemaInfo> super_type;
    typedef OSimpleXformSchema this_type;
    typedef SimpleXformSample sample_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION & ASSIGNMENT
    //-*************************************************************************

    //! Default constructor creates empty
    //! SimpleXform function set
    OSimpleXformSchema() {}

    //! This templated, primary constructor creates a new simple xform writer.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyWriterPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, and to set TimeSamplingType.
    template <class COMPOUND_PTR>
    OSimpleXformSchema( COMPOUND_PTR iParent,
                        const std::string &iName,
                        const Abc::OArgument &iArg0 = Abc::OArgument(),
                        const Abc::OArgument &iArg1 = Abc::OArgument(),
                        const Abc::OArgument &iArg2 = Abc::OArgument() )
      : Abc::OSchema<SimpleXformSchemaInfo>( iParent, iName,
                                               iArg0, iArg1, iArg2 )
    {
        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        init( Abc::GetTimeSamplingType( iArg0, iArg1, iArg2 ) );
    }

    //! Same as above but with the default name being used.
    //! ...
    template <class COMPOUND_PTR>
    explicit OSimpleXformSchema( COMPOUND_PTR iParent,
                                 const Abc::OArgument &iArg0 = Abc::OArgument(),
                                 const Abc::OArgument &iArg1 = Abc::OArgument(),
                                 const Abc::OArgument &iArg2 = Abc::OArgument() )
      : Abc::OSchema<SimpleXformSchemaInfo>( iParent,
                                             iArg0, iArg1, iArg2 )
    {
        // Meta data and error handling are eaten up by
        // the super type, so all that's left is time sampling.
        init( Abc::GetTimeSamplingType( iArg0, iArg1, iArg2 ) );
    }

    //-*************************************************************************
    // SAMPLE STUFF
    //-*************************************************************************

    void set( const SimpleXformSample &iSamp,
              const Abc::OSampleSelector &iSS = Abc::OSampleSelector() );

    void setFromPrevious( const Abc::OSampleSelector &iSS );

    //-*************************************************************************
    // ABC STUFF
    //-*************************************************************************

    void reset()
    {
        m_times.clear();

        m_scaleX.reset();
        m_scaleY.reset();
        m_scaleZ.reset();

        m_shear0.reset();
        m_shear1.reset();
        m_shear2.reset();

        m_rotateX.reset();
        m_rotateY.reset();
        m_rotateZ.reset();

        m_translateX.reset();
        m_translateY.reset();
        m_translateZ.reset();

        m_childBounds.reset();

        super_type::reset();
    }

private:
    void init( const AbcA::TimeSamplingType &iTst );

protected:

    //-*************************************************************************
    // HELPER CLASS
    //-*************************************************************************

    //! The defaulted double property will only create a property
    //! and only bother setting a value when it the value differs from a
    //! known default value. This allows transforms to disappear when they
    //! are identity.
    //! It has some SimpleXform-specific stuff in here, so not worth
    //! making general (yet).
    class ODefaultedDoubleProperty
    {
    public:
        void reset()
        {
            m_parent.reset();
            m_name = "";
            m_errorHandlerPolicy = Abc::ErrorHandler::kThrowPolicy;
            m_timeSamplingType = AbcA::TimeSamplingType();
            m_default = 0.0;
            m_epsilon = kSIMPLE_XFORM_DELTA_TOLERANCE;
            m_property.reset();
        }

        ODefaultedDoubleProperty() { reset(); }

        ODefaultedDoubleProperty( AbcA::CompoundPropertyWriterPtr iParent,
                                  const std::string &iName,
                                  Abc::ErrorHandler::Policy iPolicy,
                                  const AbcA::TimeSamplingType &iTst,
                                  double iDefault,
                                  double iEpsilon=kSIMPLE_XFORM_DELTA_TOLERANCE )
          : m_parent( Abc::GetCompoundPropertyWriterPtr( iParent ) )
          , m_name( iName )
          , m_errorHandlerPolicy( iPolicy )
          , m_timeSamplingType( iTst )
          , m_default( iDefault )
          , m_epsilon( iEpsilon )
        {
            // We don't build the property until we need it for sure.
        }

        // Returns whether or not the property exists, or whether
        // it is still default value.
        void set( const double &iSamp,
                  const Abc::OSampleSelector &iSS,

                  // If we haven't made a property yet,
                  // these time samples correspond to the times
                  // not yet sampled.
                  // Otherwise, these will be NULL.
                  const std::vector<chrono_t> &iTimeSamples );

        // Returns whether or not the property exists, or whether
        // it is still default value.
        void setFromPrevious( const Abc::OSampleSelector &iSS,

                              // If we haven't made a property yet,
                              // these time samples correspond to the times
                              // not yet sampled.
                              // Otherwise, these will be NULL.
                              const std::vector<chrono_t> &iTimeSamples );

        double getDefaultValue() const { return m_default; }

    protected:
        // Parent.
        AbcA::CompoundPropertyWriterPtr m_parent;

        // We cache the init stuff.
        std::string m_name;
        Abc::ErrorHandler::Policy m_errorHandlerPolicy;
        AbcA::TimeSamplingType m_timeSamplingType;
        double m_default;
        double m_epsilon;

        // The "it". This may not exist.
        Abc::ODoubleProperty m_property;
    };

protected:
    // Times for the properties set as non-default
    std::vector<chrono_t> m_times;

    // The components.
    ODefaultedDoubleProperty m_scaleX;
    ODefaultedDoubleProperty m_scaleY;
    ODefaultedDoubleProperty m_scaleZ;

    ODefaultedDoubleProperty m_shear0;
    ODefaultedDoubleProperty m_shear1;
    ODefaultedDoubleProperty m_shear2;

    ODefaultedDoubleProperty m_rotateX;
    ODefaultedDoubleProperty m_rotateY;
    ODefaultedDoubleProperty m_rotateZ;

    ODefaultedDoubleProperty m_translateX;
    ODefaultedDoubleProperty m_translateY;
    ODefaultedDoubleProperty m_translateZ;

    OBox3dProperty m_childBounds;
};

//-*****************************************************************************
//-*****************************************************************************
// THE SCHEMA OBJECT
//-*****************************************************************************
//-*****************************************************************************
typedef Abc::OSchemaObject<OSimpleXformSchema> OSimpleXform;

} // End namespace AbcGeom
} // End namespace Alembic

#endif

