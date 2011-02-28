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

#ifndef _Alembic_AbcGeom_IXform_h_
#define _Alembic_AbcGeom_IXform_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/XformOp.h>
#include <Alembic/AbcGeom/XformSample.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
class IXformSchema : public Abc::ISchema<XformSchemaInfo>
{

    //-*************************************************************************
    // XFORM SCHEMA
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef IXformSchema this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty IXformSchema
    //! ...
    IXformSchema() {}

    //! This templated, explicit function creates a new scalar property reader.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyReaderPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy and to specify
    //! schema interpretation matching.
    template <class CPROP_PTR>
    IXformSchema( CPROP_PTR iParentObject,
                     const std::string &iName,

                     const Abc::IArgument &iArg0 = Abc::IArgument(),
                     const Abc::IArgument &iArg1 = Abc::IArgument() )
      : Abc::ISchema<XformSchemaInfo>( iParentObject, iName, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! This constructor is the same as above, but with default
    //! schema name used.
    template <class CPROP_PTR>
    explicit IXformSchema( CPROP_PTR iParentObject,
                              const Abc::IArgument &iArg0 = Abc::IArgument(),
                              const Abc::IArgument &iArg1 = Abc::IArgument() )
      : Abc::ISchema<XformSchemaInfo>( iParentObject, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Default copy constructor used.
    //! Default assignment operator used.

    //! Return the number of samples contained in the animated property.
    //! This can be any number, including zero.
    //! This returns the number of samples that were written, independently
    //! of whether or not they were constant.
    size_t getNumAnimSamples()
    {
        if ( m_anim.valid() )
            return m_anim.getNumSamples();

        return 0;
    }

    //! Return the number of samples contained in the inherits property.
    //! This can be any number, including zero.
    //! This returns the number of samples that were written, independently
    //! of whether or not they were constant.
    size_t getNumInheritsSamples()
    {
        if ( m_inherits.valid() )
            return m_inherits.getNumSamples();

        return 0;
    }

    //! Time sampling type for inherits property
    AbcA::TimeSamplingType getInheritsTimeSamplingType() const
    {
        if ( m_inherits.valid() )
            return m_inherits.getTimeSamplingType();

        return AbcA::TimeSamplingType();
    }

    //! Time sampling type.
    AbcA::TimeSamplingType getTimeSamplingType() const
    {
        if ( m_anim.valid() )
            return m_anim.getTimeSamplingType();

        return AbcA::TimeSamplingType();
    }

    //! Time information.
    //! Defaults to Identity.
    AbcA::TimeSampling getInheritsTimeSampling()
    {
        if ( m_inherits.valid() )
            return m_inherits.getTimeSampling();

        return AbcA::TimeSampling();
    }

    //! Time information for inherits property.
    //! Defaults to Identity.
    AbcA::TimeSampling getTimeSampling()
    {
        if ( m_anim.valid() )
            return m_anim.getTimeSampling();

        return AbcA::TimeSampling();
    }

    //-*************************************************************************
    Abc::DoubleArraySamplePtr getAnimData(
        const Abc::ISampleSelector &iSS = Abc::ISampleSelector() )
    {
        Abc::DoubleArraySamplePtr ret;

        ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::getAnim()" );

        m_anim.get( ret, iSS );

        // Could error check here.

        ALEMBIC_ABC_SAFE_CALL_END();
        return ret;
    }

    bool inherits(
        const Abc::ISampleSelector &iSS = Abc::ISampleSelector() )
    {
        // if m_inherits doesn't exist we'll default to true
        Alembic::Util::bool_t ret = true;

        ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::inherits()" );

        if ( m_inherits.valid() )
            m_inherits.get( ret, iSS );

        // Could error check here.

        ALEMBIC_ABC_SAFE_CALL_END();
        return ret;
    }

    Abc::M44d getMatrix(
        const Abc::ISampleSelector &iSS = Abc::ISampleSelector() );

    Abc::DoubleArraySamplePtr getStaticData() { return m_static; }

    const XformOpVec & getOps() { return m_ops; }

    //! Returns the total number of operations.
    size_t getNumOps() const { return m_ops.size(); }

    //! Returns true if a particular op has no animated components.
    bool isOpStatic( size_t iIndex ) const;

    void get(XformSample & oVec,
        const Abc::ISampleSelector &iSS = Abc::ISampleSelector());

    XformSample getValue(
        const Abc::ISampleSelector &iSS = Abc::ISampleSelector() )
    {
        XformSample smp;
        get( smp, iSS );
        return smp;
    }

    Abc::Box3d getChildBounds( const Abc::ISampleSelector &iSS =
                               Abc::ISampleSelector() )
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "IXformSchema::getChildBounds()" );

        return m_childBounds.getValue( iSS );

        ALEMBIC_ABC_SAFE_CALL_END();

        Abc::Box3d ret;
        return ret;
    }

    //-*************************************************************************
    // ABC BASE MECHANISMS
    // These functions are used by Abc to deal with errors, rewrapping,
    // and so on.
    //-*************************************************************************

    //! Reset returns this function set to an empty, default state.
    void reset()
    {
        m_ops.clear();
        m_static.reset();

        m_anim.reset();
        m_inherits.reset();

        m_childBounds.reset();

        Abc::ISchema<XformSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is valid.
    bool valid() const
    {
        return ( Abc::ISchema<XformSchemaInfo>::valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( IXformSchema::valid() );

protected:
    void init( const Abc::IArgument &iArg0,
               const Abc::IArgument &iArg1 );

    Abc::IDoubleArrayProperty m_anim;
    Abc::IBoolProperty m_inherits;
    XformOpVec m_ops;
    Abc::DoubleArraySamplePtr m_static;

    Abc::IBox3dProperty m_childBounds;

};

//-*****************************************************************************
typedef Abc::ISchemaObject<IXformSchema> IXform;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
