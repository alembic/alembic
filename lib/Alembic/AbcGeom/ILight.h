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

#ifndef _Alembic_AbcGeom_ILight_h_
#define _Alembic_AbcGeom_ILight_h_

#include <Alembic/AbcGeom/ICamera.h>
#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ILightSchema : public Abc::ISchema<LightSchemaInfo>
{
    //-*************************************************************************
    // LIGHT SCHEMA (container schema which has a camera schema)
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef ILightSchema this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty OLightMeshSchema
    //! ...
    ILightSchema() {}

    //! This templated, primary constructor creates a new light writer.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyWriterPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, and to set TimeSampling.
    template <class CPROP_PTR>
    ILightSchema( CPROP_PTR iParent,
                  const std::string &iName,
                  const Abc::Argument &iArg0 = Abc::Argument(),
                  const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<LightSchemaInfo>( iParent, iName, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    template <class CPROP_PTR>
    explicit ILightSchema( CPROP_PTR iParent,
                           const Abc::Argument &iArg0 = Abc::Argument(),
                           const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<LightSchemaInfo>( iParent, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Wrap an existing ILight object
    template <class CPROP_PTR>
    explicit ILightSchema( CPROP_PTR iThis,
                           Abc::WrapExistingFlag iFlag,
                           const Abc::Argument &iArg0 = Abc::Argument(),
                           const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<LightSchemaInfo>( iThis, iFlag, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Copy constructor.
    ILightSchema(const ILightSchema& iCopy)
        : Abc::ISchema<LightSchemaInfo>()
    {
        *this = iCopy;
    }

    //! Access to the camera schema.
    ICameraSchema getCameraSchema() const { return m_cameraSchema; }

    //! Access to the child bounds property
    Abc::IBox3dProperty getChildBoundsProperty() const
    {
        return m_childBoundsProperty;
    }

    AbcA::TimeSamplingPtr getTimeSampling() const;

    bool isConstant() const;

    size_t getNumSamples() const;

    // compound property to use as parent for any arbitrary GeomParams
    // underneath it
    ICompoundProperty getArbGeomParams() const { return m_arbGeomParams; }
    ICompoundProperty getUserProperties() const { return m_userProperties; }

    //! Reset returns this function set to an empty, default
    //! state.
    void reset()
    {
        m_childBoundsProperty.reset();
        m_arbGeomParams.reset();
        m_userProperties.reset();
        m_cameraSchema.reset();
        Abc::ISchema<LightSchemaInfo>::reset();
    }

    //! Returns whether this function set is valid.
    bool valid() const
    {
        return ( Abc::ISchema<LightSchemaInfo>::valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( ILightSchema::valid() );

  protected:
    void init( const Abc::Argument& iArg0,
               const Abc::Argument& iArg1 );

    Abc::IBox3dProperty    m_childBoundsProperty;
    Abc::ICompoundProperty m_arbGeomParams;
    Abc::ICompoundProperty m_userProperties;

    Alembic::AbcGeom::ICameraSchema m_cameraSchema;
};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::ISchemaObject< ILightSchema > ILight;

typedef Util::shared_ptr< ILight > ILightPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
