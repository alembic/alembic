//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#ifndef _Alembic_AbcGeom_IGeometrySchema_h_
#define _Alembic_AbcGeom_IGeometrySchema_h_

#include <Alembic/Abc/ISchema.h>
#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/IGeomParam.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {


//! This class holds properties common to all geometric classes that have a
//! physical volume.
//! - selfBounds
//! - childBounds (optional)
//! - GeomParams (optional)
//! - UserProperties (optional)
//!
//! This class is used to encapsulate common functionality of the
//! real Geometry schema classes, like IPoints and IPolyMesh and so on
template <class INFO>
class IGeomBaseSchema : public Abc::ISchema<INFO>
{
public:
    //-*************************************************************************
    // TYPEDEFS AND IDENTIFIERS
    //-*************************************************************************

    typedef INFO info_type;
    typedef ISchema<INFO> this_type;


    //-*************************************************************************
    // Constructors that pass through to ISchema
    //-*************************************************************************
    //
    //! The default constructor creates an empty ISchema.
    //! Used to create "NULL/invalid" instances.
    IGeomBaseSchema() {}

    //! Delegates to Abc/ISchema, and then creates
    //! properties that are present.
    template <class CPROP_PTR>
    IGeomBaseSchema( CPROP_PTR iParentCompound,
             const std::string &iName,

             const Argument &iArg0 = Argument(),
             const Argument &iArg1 = Argument() )
       : ISchema<info_type>( iParentCompound, iName,
                              iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    template <class CPROP_PTR>
    explicit IGeomBaseSchema( CPROP_PTR iParentCompound,

                      const Argument &iArg0 = Argument(),
                      const Argument &iArg1 = Argument() )
      : ISchema<info_type>( iParentCompound,
                            iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Wrap an existing schema object
    template <class CPROP_PTR>
    IGeomBaseSchema( CPROP_PTR iThis,
                   Abc::WrapExistingFlag iFlag,
                   const Abc::Argument &iArg0 = Abc::Argument(),
                   const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<info_type>( iThis, iFlag, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    void init( const Abc::Argument &iArg0, const Abc::Argument &iArg1 )
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "IGeomBaseSchema::init()" );

        Abc::Arguments args;
        iArg0.setInto( args );
        iArg1.setInto( args );

        AbcA::CompoundPropertyReaderPtr _this = this->getPtr();

        m_selfBoundsProperty = Abc::IBox3dProperty( _this, ".selfBnds",
            iArg0, iArg1 );
        if ( this->getPropertyHeader( ".childBnds" ) != NULL )
        {
            m_childBoundsProperty = Abc::IBox3dProperty( _this,
                ".childBnds", iArg0, iArg1 );
        }

        if ( this->getPropertyHeader( ".arbGeomParams" ) != NULL )
        {
            m_arbGeomParams = Abc::ICompoundProperty( _this, ".arbGeomParams",
                args.getErrorHandlerPolicy() );
        }
        if ( this->getPropertyHeader( ".userProperties" ) != NULL )
        {
            m_userProperties = Abc::ICompoundProperty( _this, ".userProperties",
                args.getErrorHandlerPolicy() );
        }

        ALEMBIC_ABC_SAFE_CALL_END();
    }

    virtual void reset ()
    {
        m_selfBoundsProperty.reset();
        m_childBoundsProperty.reset();
        m_arbGeomParams.reset();
        m_userProperties.reset();
        Abc::ISchema<info_type>::reset();
    }

    virtual bool valid() const
    {
        // Only selfBounds is required, all others are optional
        return ( Abc::ISchema<info_type>::valid() &&
                m_selfBoundsProperty.valid() );
    }

    Abc::IBox3dProperty getSelfBoundsProperty()
    {
        return m_selfBoundsProperty;
    }

    Abc::IBox3dProperty getChildBoundsProperty()
    {
        return m_childBoundsProperty;
    }

    // compound property to use as parent for any arbitrary GeomParams
    // underneath it
    ICompoundProperty getArbGeomParams() { return m_arbGeomParams; }

    // compound property to use as parent for any user workflow specific
    // properties
    ICompoundProperty getUserProperties() { return m_userProperties; }

protected:
    // Only selfBounds is required, all others are optional
    Abc::IBox3dProperty m_selfBoundsProperty;
    Abc::IBox3dProperty m_childBoundsProperty;

    Abc::ICompoundProperty m_arbGeomParams;
    Abc::ICompoundProperty m_userProperties;

};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
