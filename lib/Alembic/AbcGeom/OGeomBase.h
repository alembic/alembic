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

#ifndef _Alembic_AbcGeom_OGeometrySchema_h_
#define _Alembic_AbcGeom_OGeometrySchema_h_

#include <Alembic/Abc/OSchema.h>
#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/OGeomParam.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {


//! This class holds properties common to all classes with a physical volume.
//! - selfBounds
//! - childBounds (optional)
//! - GeomParams (optional)
//! - UserProperties (optional)
//!
//! This class is used to encapsulate common functionality of the
//! real Geometry schema classes, like OPoints and OPolyMesh and so on
template <class INFO>
class OGeomBaseSchema : public Abc::OSchema<INFO>
{
public:
    //-*************************************************************************
    // TYPEDEFS AND IDENTIFIERS
    //-*************************************************************************

    typedef INFO info_type;


    //-*************************************************************************
    // Constructors that pass through to OSchema
    //-*************************************************************************
    //
    //! The default constructor creates an empty OSchema.
    //! Used to create "NULL/invalid" instances.
    OGeomBaseSchema() {}

    //! Delegates to Abc/OSchema, and then creates always-present
    //! properties
    OGeomBaseSchema( AbcA::CompoundPropertyWriterPtr iParent,
                     const std::string &iName,
                     const Argument &iArg0 = Argument(),
                     const Argument &iArg1 = Argument(),
                     const Argument &iArg2 = Argument(),
                     const Argument &iArg3 = Argument() )
       : Abc::OSchema<info_type>( iParent, iName, iArg0, iArg1, iArg2, iArg3 )
    {
        AbcA::TimeSamplingPtr tsPtr =
            Abc::GetTimeSampling( iArg0, iArg1, iArg2, iArg3 );
        uint32_t tsIndex =
            Abc::GetTimeSamplingIndex( iArg0, iArg1, iArg2, iArg3 );

        // if we specified a valid TimeSamplingPtr, use it to determine the
        // index otherwise use the default index of 0 - uniform.
        if ( tsPtr )
        {
            tsIndex = iParent->getObject()->getArchive()->addTimeSampling(
                *tsPtr );
        }

        // Create our always present property
        init( tsIndex, Abc::IsSparse( iArg0, iArg1, iArg2, iArg3 ) );
    }

    //! Copy constructor
    OGeomBaseSchema( const OGeomBaseSchema& iCopy )
      : Abc::OSchema<info_type>()
    {
        *this = iCopy;
    }

    virtual void reset ()
    {
        m_selfBoundsProperty.reset();
        m_childBoundsProperty.reset();
        m_arbGeomParams.reset();
        m_userProperties.reset();
        Abc::OSchema<info_type>::reset();
    }

    virtual bool valid() const
    {
        return ( Abc::OSchema<info_type>::valid() );
    }

    Abc::OCompoundProperty getArbGeomParams()
    {
        // Accessing the ArbGeomParams will create its compound
        // property if needed.
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OGeomBaseSchema::getArbGeomParams()" );

        if ( ! m_arbGeomParams )
        {
            m_arbGeomParams = Abc::OCompoundProperty( this->getPtr(),
                                                      ".arbGeomParams" );
        }

        return m_arbGeomParams;

        ALEMBIC_ABC_SAFE_CALL_END();

        Abc::OCompoundProperty ret;
        return ret;
    }

    Abc::OCompoundProperty getUserProperties()
    {
        // Accessing UserProperties will create its compound
        // property if needed.
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OGeomBaseSchema::getUserProperties()" );

        if ( ! m_userProperties )
        {
            m_userProperties = Abc::OCompoundProperty( this->getPtr(),
                                                       ".userProperties" );
        }

        return m_userProperties;

        ALEMBIC_ABC_SAFE_CALL_END();

        Abc::OCompoundProperty ret;
        return ret;
    }

    Abc::OBox3dProperty getChildBoundsProperty()
    {
        // Accessing Child Bounds Property will create it if needed
        ALEMBIC_ABC_SAFE_CALL_BEGIN(
            "OGeomBaseSchema::getChildBoundsProperty()" );

        if ( ! m_childBoundsProperty )
        {
            AbcA::CompoundPropertyWriterPtr _this = this->getPtr();

            // for now, use the self bounds time sampling, this
            // can and should be changed depending on how the children
            // are sampled
            m_childBoundsProperty = Abc::OBox3dProperty( _this,
                ".childBnds", m_selfBoundsProperty.getTimeSampling() );

        }

        ALEMBIC_ABC_SAFE_CALL_END();
        return m_childBoundsProperty;
    }

protected:

    void init( uint32_t iTsIndex, bool isSparse)
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OGeomBaseSchema::init()" );

        if( !isSparse )
        {
           createSelfBoundsProperty(iTsIndex, 0);
        }

        ALEMBIC_ABC_SAFE_CALL_END_RESET();
    }

    //! Creates the self bounds
    void createSelfBoundsProperty(uint32_t iTsIndex, size_t iNumSamples)
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OGeomBaseSchema::createSelfBoundsProperty()" );

        if ( m_selfBoundsProperty.valid() )
        {
            return;
        }

        m_selfBoundsProperty = Abc::OBox3dProperty( this->getPtr(), ".selfBnds",
                                                    iTsIndex );

        Abc::Box3d bnds;
        for ( size_t i = 0; i < iNumSamples; ++i )
        {
            m_selfBoundsProperty.set( bnds );
        }
        ALEMBIC_ABC_SAFE_CALL_END();
    }

    // Only selfBounds is required, all others are optional
    Abc::OBox3dProperty m_selfBoundsProperty;
    Abc::OBox3dProperty m_childBoundsProperty;

    Abc::OCompoundProperty m_arbGeomParams;
    Abc::OCompoundProperty m_userProperties;

};


} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
