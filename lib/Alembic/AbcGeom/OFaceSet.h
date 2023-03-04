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

#ifndef Alembic_AbcGeom_OFaceSet_h
#define Alembic_AbcGeom_OFaceSet_h

#include <Alembic/Util/Export.h>
#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/OGeomParam.h>
#include <Alembic/AbcGeom/FaceSetExclusivity.h>
#include <Alembic/AbcGeom/OGeomBase.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class ALEMBIC_EXPORT OFaceSetSchema : public OGeomBaseSchema<FaceSetSchemaInfo>
{
public:
    //-*************************************************************************
    // Parition SAMPLE
    //-*************************************************************************
    class Sample
    {
    public:
        typedef Sample this_type;

        //! Creates a default sample with no data in it.
        //! ...
        Sample() { reset(); }

        //! Creates a sample with the list of faces that are in this
        //! faceset.
        //! The array of face numbers MUST be ordered by face number.
        //! Code that reads and works with facesets depends on this for efficency.
        //! e.g. call std::sort (myVecOfFaces.begin (), myVecOfFaces.end ());
        //! if you need to.
        //! The sample must be complete like this. Subsequent samples may also
        //! consist of faces which allows you to change of topology
        //! of the faceset.
        Sample( const Abc::Int32ArraySample &iFaceNums)
          : m_faces( iFaceNums )
        {}

        /* main accessors */
        // Faces
        const Abc::Int32ArraySample &getFaces() const { return m_faces; }
        void setFaces( const Abc::Int32ArraySample &iFaces)
        { m_faces = iFaces; }

        // Bounding boxes
        const Abc::Box3d &getSelfBounds() const { return m_selfBounds; }
        void setSelfBounds( const Abc::Box3d &iBnds )
        { m_selfBounds = iBnds; }

        void reset()
        {
            m_faces.reset();

            m_selfBounds.makeEmpty();
        }

    protected:
        Abc::Int32ArraySample   m_faces;

        // bounds
        Abc::Box3d              m_selfBounds;
    }; // end OFaceSetSchema::Sample


    //-*************************************************************************
    // FaceSet SCHEMA
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef OFaceSetSchema this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************
    //! The default constructor creates an empty OFaceSetSchema.
    //! OFaceSetSchema instances created this evaluate to a boolean value of false.
    OFaceSetSchema() {}

    //! This constructor creates a new faceset writer.
    //! The first argument is an CompoundPropertyWriterPtr to use as a parent.
    //! The next is the name to give the schema which is usually the default
    //! name given by OFaceSet (.faceset)   The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy, to specify
    //! MetaData, specify sparse sampling and to set TimeSampling.
    OFaceSetSchema( AbcA::CompoundPropertyWriterPtr iParent,
                    const std::string &iName,
                    const Abc::Argument &iArg0 = Abc::Argument(),
                    const Abc::Argument &iArg1 = Abc::Argument(),
                    const Abc::Argument &iArg2 = Abc::Argument(),
                    const Abc::Argument &iArg3 = Abc::Argument() );

    //! This constructor creates a new faceset writer.
    //! The first argument is an OCompundProperty to use as a parent, and from
    //! which the ErrorHandlerPolicy is derived.  The next is the name to give
    //! the schema which is usually the default name given by OFaceSet
    //! (.faceset) The remaining optional arguments can be used to specify
    //! MetaData, specify sparse sampling and to set TimeSampling.
    OFaceSetSchema( Abc::OCompoundProperty iParent,
                    const std::string &iName,
                    const Abc::Argument &iArg0 = Abc::Argument(),
                    const Abc::Argument &iArg1 = Abc::Argument(),
                    const Abc::Argument &iArg2 = Abc::Argument() );

    //! Default assignment and copy operator used.

    //-*************************************************************************
    // SCHEMA STUFF
    //-*************************************************************************

    //-*************************************************************************
    // SAMPLE STUFF
    //-*************************************************************************

    //! Get number of samples written so far.
    //! ...
    size_t getNumSamples() const
    { return m_facesProperty.getNumSamples(); }

    //! Set a sample! First sample must have the list of faces in the faceset.
    void set( const Sample &iSamp );

    void setTimeSampling( uint32_t iTimeSamplingID );
    void setTimeSampling( AbcA::TimeSamplingPtr iTime );

    void setFaceExclusivity( FaceSetExclusivity iFacesExclusive );
    FaceSetExclusivity getFaceExclusivity() { return m_facesExclusive; }
    //-*************************************************************************
    // ABC BASE MECHANISMS
    // These functions are used by Abc to deal with errors, validity,
    // and so on.
    //-*************************************************************************

    //! Reset returns this function set to an empty, default
    //! state.
    void reset()
    {
        m_facesProperty.reset();

        OGeomBaseSchema<FaceSetSchemaInfo>::reset();
    }

    //! Valid returns whether this instance holds real data.
    bool valid() const
    {
        return ( OGeomBaseSchema<FaceSetSchemaInfo>::valid() &&
                 m_facesProperty.valid()
                 );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( OFaceSetSchema::valid() );

protected:
    void _recordExclusivityHint();

    void init( AbcA::CompoundPropertyWriterPtr iParent,
               const Abc::Argument &iArg0, const Abc::Argument &iArg1,
               const Abc::Argument &iArg2, const Abc::Argument &iArg3 );

    Abc::OInt32ArrayProperty    m_facesProperty;

    Abc::OUInt32Property        m_facesExclusiveProperty;
    FaceSetExclusivity          m_facesExclusive;
};


//-*****************************************************************************
// Nice to use typedef for users of this class.
//-*****************************************************************************
typedef Abc::OSchemaObject<OFaceSetSchema> OFaceSet;

typedef Util::shared_ptr< OFaceSet > OFaceSetPtr;

Abc::Box3d computeBoundsFromPositionsByFaces (const Int32ArraySample & faces,
    const Int32ArraySample & meshFaceCounts,
    const Int32ArraySample & vertexIndices,
    const P3fArraySample & meshP);

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
