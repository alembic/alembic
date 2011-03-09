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
#include <Alembic/AbcGeom/XformOp.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
class OXformSchema : public Abc::OSchema<XformSchemaInfo>
{
    //-*************************************************************************
    // XFORM SCHEMA
    //-*************************************************************************
public:

    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef OXformSchema this_type;

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

    //! Return the time sampling type, which is stored on each of the
    //! sub properties.
    AbcA::TimeSamplingType getTimeSamplingType() const
    { return m_anim.getTimeSamplingType(); }

    //-*************************************************************************
    // SAMPLE STUFF
    //-*************************************************************************

    //! Get number of samples written so far.
    //! ...
    size_t getNumSamples()
    { return m_anim.getNumSamples(); }

    //! Set an animated sample.  setXform needs to be called first.
    void set( const Abc::DoubleArraySample & iAnim,
              const Abc::OSampleSelector &iSS = Abc::OSampleSelector() );

    //! Set the inherits transform hint
    void setInherits(bool iInherits,
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
        m_anim.reset();
        m_inherits.reset();
        m_childBounds.reset();
        m_timeSamplingType = AbcA::TimeSamplingType();
        Abc::OSchema<XformSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is valid.
    bool valid() const
    {
        return ( Abc::OSchema<XformSchemaInfo>::valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( OXformSchema::valid() );

protected:
    void init( const AbcA::TimeSamplingType &iTst );

    Abc::OBox3dProperty m_childBounds;

private:
    AbcA::TimeSamplingType m_timeSamplingType;
};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::OSchemaObject<OXformSchema> OXform;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
