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

#ifndef _Alembic_AbcGeom_IFaceSet_h_
#define _Alembic_AbcGeom_IFaceSet_h_

#include <Alembic/Util/Export.h>
#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/IGeomParam.h>
#include <Alembic/AbcGeom/FaceSetExclusivity.h>
#include <Alembic/AbcGeom/IGeomBase.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ALEMBIC_EXPORT IFaceSetSchema : public IGeomBaseSchema<FaceSetSchemaInfo>
{
public:
    //-*************************************************************************
    // FACESET MESH SCHEMA SAMPLE TYPE
    //-*************************************************************************
    class Sample
    {
    public:
        typedef Sample this_type;

        //! Users never create this data directly
        Sample() { reset(); }

        // main stuff
        Abc::Int32ArraySamplePtr getFaces() const { return m_faces; }

        // bounds
        Abc::Box3d getSelfBounds() const { return m_selfBounds; }

        bool valid() const
        {
            return m_faces.get() != 0;
        }

        void reset()
        {
            m_faces.reset();

            m_selfBounds.makeEmpty();
        }

        ALEMBIC_OPERATOR_BOOL( valid() );

    protected:
        friend class IFaceSetSchema;

        Abc::Int32ArraySamplePtr    m_faces;


        // bounds
        Abc::Box3d m_selfBounds;

    }; // end IFaceSetSchema::Sample

    //-*************************************************************************
    // FACESET SCHEMA
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef IFaceSetSchema this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty IFaceSetSchema
    //! ...
    IFaceSetSchema() {}

    //! This templated, primary constructor creates a new faceset reader.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyWriterPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy and to specify
    //! schema interpretation matching.
    template <class CPROP_PTR>
    IFaceSetSchema( CPROP_PTR iParent,
                 const std::string &iName,

                 const Abc::Argument &iArg0 = Abc::Argument(),
                 const Abc::Argument &iArg1 = Abc::Argument() )
      : IGeomBaseSchema<FaceSetSchemaInfo>( iParent, iName,
                                      iArg0, iArg1 )
    {
        init(  iArg0, iArg1 );
    }

    //! Same constructor as above, but use the default schema name, ie,
    //! ".geom".
    template <class CPROP_PTR>
    explicit IFaceSetSchema( CPROP_PTR iParent,
                          const Abc::Argument &iArg0 = Abc::Argument(),
                          const Abc::Argument &iArg1 = Abc::Argument() )
      : IGeomBaseSchema<FaceSetSchemaInfo>( iParent,
                                      iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! wrap an existing schema object
    template <class CPROP_PTR>
    IFaceSetSchema( CPROP_PTR iThis,
                 Abc::WrapExistingFlag iFlag,

                 const Abc::Argument &iArg0 = Abc::Argument(),
                 const Abc::Argument &iArg1 = Abc::Argument() )
      : IGeomBaseSchema<FaceSetSchemaInfo>( iThis, iFlag, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Copy constructor. (explicit copy ctor for msvc bug workaround)
    IFaceSetSchema(const IFaceSetSchema& iCopy)
        : IGeomBaseSchema<FaceSetSchemaInfo>()
    {
        *this = iCopy;
    }

    //! Default assignment operator used.

    //-*************************************************************************
    // SCHEMA STUFF
    //-*************************************************************************


    //! if isConstant() is true, the mesh contains no time-varying values
    bool isConstant() const { return (m_facesProperty.isConstant ()); }

    //-*************************************************************************
    // SAMPLE STUFF
    //-*************************************************************************

    //! Get number of samples written so far.
    //! ...
    size_t getNumSamples() const;

    //! Return the time sampling
    AbcA::TimeSamplingPtr getTimeSampling() const
    {
        if ( m_facesProperty.valid() )
            return m_facesProperty.getTimeSampling();
        return getObject().getArchive().getTimeSampling(0);
    }

    void get( Sample &iSamp,
              const Abc::ISampleSelector &iSS = Abc::ISampleSelector() ) const;

    Sample getValue( const Abc::ISampleSelector &iSS = Abc::ISampleSelector() ) const
    {
        Sample smp;
        get( smp, iSS );
        return smp;
    }

    FaceSetExclusivity getFaceExclusivity() const;

    Abc::IInt32ArrayProperty getFacesProperty() const
    {
        return m_facesProperty;
    }

    //-*************************************************************************
    // ABC BASE MECHANISMS
    // These functions are used by Abc to deal with errors, rewrapping,
    // and so on.
    //-*************************************************************************

    //! Reset returns this function set to an empty, default
    //! state.
    void reset()
    {
        m_facesProperty.reset();

        IGeomBaseSchema<FaceSetSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( IGeomBaseSchema<FaceSetSchemaInfo>::valid() &&
                 m_facesProperty.valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( IFaceSetSchema::valid() );

protected:
    void init( const Abc::Argument &iArg0, const Abc::Argument &iArg1 );

    Abc::IInt32ArrayProperty    m_facesProperty;
};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::ISchemaObject<IFaceSetSchema> IFaceSet;

typedef Util::shared_ptr< IFaceSet > IFaceSetPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
