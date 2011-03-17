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

#ifndef _Alembic_AbcGeom_ISimpleXform_h_
#define _Alembic_AbcGeom_ISimpleXform_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/SimpleXformSample.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
//-*****************************************************************************
// THE SIMPLE TRANSFORM SCHEMA
//-*****************************************************************************
//-*****************************************************************************
class ISimpleXformSchema  : public Abc::ISchema<SimpleXformSchemaInfo>
{
public:
    //-*************************************************************************
    // TYPEDEFS AND IDENTIFIERS
    //-*************************************************************************
    typedef Abc::ISchema<SimpleXformSchemaInfo> super_type;
    typedef ISimpleXformSchema this_type;
    typedef SimpleXformSample sample_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION & ASSIGNMENT
    //-*************************************************************************

    //! Default constructor creates empty
    //! SimpleXform function set
    ISimpleXformSchema() {}

    //! This templated, primary constructor creates a new simple xform writer.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyReaderPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, and to set
    //! SchemaMatching.
    template <class COMPOUND_PTR>
    ISimpleXformSchema( COMPOUND_PTR iParent,
                        const std::string &iName,
                        const Abc::IArgument &iArg0 = Abc::IArgument(),
                        const Abc::IArgument &iArg1 = Abc::IArgument() )
      : Abc::ISchema<SimpleXformSchemaInfo>( iParent, iName, iArg0, iArg1 )
    {
        // Error Handling is eaten up by the super type, all that's
        // left is SchemaInterpMatching, which we don't internally
        init( Abc::GetSchemaInterpMatching( iArg0, iArg1 ) );
    }

    //! Same as above, but using the default schema name
    //! ...
    template <class COMPOUND_PTR>
    explicit ISimpleXformSchema( COMPOUND_PTR iParent,
                                 const Abc::IArgument &iArg0 = Abc::IArgument(),
                                 const Abc::IArgument &iArg1 = Abc::IArgument() )
      : Abc::ISchema<SimpleXformSchemaInfo>( iParent, iArg0, iArg1 )
    {
        // Error Handling is eaten up by the super type, all that's
        // left is SchemaInterpMatching
        init( Abc::GetSchemaInterpMatching( iArg0, iArg1 ) );
    }

    //! wrap an existing ISimpleXform object
    template <class CPROP_PTR>
    ISimpleXformSchema( CPROP_PTR iThis,
                        Abc::WrapExistingFlag iFlag,

                        const Abc::IArgument &iArg0 = Abc::IArgument(),
                        const Abc::IArgument &iArg1 = Abc::IArgument() )
      : Abc::ISchema<SimpleXformSchemaInfo>( iThis, iFlag, iArg0, iArg1 )
    {
        init( Abc::GetSchemaInterpMatching( iArg0, iArg1 ) );
    }

    //-*************************************************************************
    // SAMPLE STUFF
    //-*************************************************************************

    size_t getNumSamples() const { return m_numSamples; }

    bool isConstant() const { return m_isConstant; }

    TimeSampling getTimeSampling() const
    {
        return m_timeSampling;
    }

    void get( SimpleXformSample &oSamp,
              const Abc::ISampleSelector &iSS = Abc::ISampleSelector() );

    SimpleXformSample getValue( const Abc::ISampleSelector &iSS =
                                Abc::ISampleSelector() )
    {
        SimpleXformSample smp;
        this->get( smp, iSS );
        return smp;
    }

    //-*************************************************************************
    // ABC STUFF
    //-*************************************************************************

    void reset()
    {
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
    void init( Abc::SchemaInterpMatching iMatching );

protected:
    void _getTimeData( Abc::IDoubleProperty& iProp );

    // Any of these properties might not exist, in which case their
    // default values will be used.

    Abc::IDoubleProperty m_scaleX;
    Abc::IDoubleProperty m_scaleY;
    Abc::IDoubleProperty m_scaleZ;

    Abc::IDoubleProperty m_shear0;
    Abc::IDoubleProperty m_shear1;
    Abc::IDoubleProperty m_shear2;

    Abc::IDoubleProperty m_rotateX;
    Abc::IDoubleProperty m_rotateY;
    Abc::IDoubleProperty m_rotateZ;

    Abc::IDoubleProperty m_translateX;
    Abc::IDoubleProperty m_translateY;
    Abc::IDoubleProperty m_translateZ;

    Abc::IBox3dProperty m_childBounds;

    // Time stuff, cached.
    size_t m_numSamples;
    bool m_isConstant;
    AbcA::TimeSampling m_timeSampling;
};

//-*****************************************************************************
//-*****************************************************************************
// THE SCHEMA OBJECT
//-*****************************************************************************
//-*****************************************************************************
typedef Abc::ISchemaObject<ISimpleXformSchema> ISimpleXform;


} // End namespace AbcGeom
} // End namespace Alembic

#endif

