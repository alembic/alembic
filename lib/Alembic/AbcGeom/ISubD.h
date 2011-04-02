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

#ifndef _Alembic_AbcGeom_ISubD_h_
#define _Alembic_AbcGeom_ISubD_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/SchemaInfoDeclarations.h>
#include <Alembic/AbcGeom/IGeomParam.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
class ISubDSchema : public Abc::ISchema<SubDSchemaInfo>
{
public:
    //-*************************************************************************
    // POLY MESH SCHEMA SAMPLE TYPE
    //-*************************************************************************
    class Sample
    {
    public:
        typedef Sample this_type;

        //! Users never create this data directly
        Sample() {}

        // main stuff
        Abc::V3fArraySamplePtr getPositions() const { return m_positions; }
        Abc::Int32ArraySamplePtr getFaceIndices() const { return m_faceIndices; }
        Abc::Int32ArraySamplePtr getFaceCounts() const { return m_faceCounts; }

        // misc subd stuff
        int32_t getFaceVaryingInterpolateBoundary() const
        { return m_faceVaryingInterpolateBoundary; }

        int32_t getFaceVaryingPropagateCorners() const
        { return m_faceVaryingPropagateCorners; }

        int32_t getInterpolateBoundary() const
        { return m_interpolateBoundary; }

        // creases
        Abc::Int32ArraySamplePtr getCreaseIndices() const
        { return m_creaseIndices; }

        Abc::Int32ArraySamplePtr getCreaseLengths() const
        { return m_creaseLengths; }

        Abc::FloatArraySamplePtr getCreaseSharpnesses() const
        { return m_creaseSharpnesses; }

        // corners
        Abc::Int32ArraySamplePtr getCornerIndices() const
        { return m_cornerIndices; }

        Abc::FloatArraySamplePtr getCornerSharpnesses() const
        { return m_cornerSharpnesses; }

        // Holes
        Abc::Int32ArraySamplePtr getHoles() const { return m_holes; }

        // subdivision scheme
        std::string getSubdivisionScheme() const
        { return m_subdScheme; }

        // bounds
        Abc::Box3d getSelfBounds() const { return m_selfBounds; }
        Abc::Box3d getChildBounds() const { return m_childBounds; }


        bool valid() const
        {
            return m_positions && m_faceIndices && m_faceCounts;
        }

        void reset()
        {
            m_positions.reset();
            m_faceIndices.reset();
            m_faceCounts.reset();

            m_faceVaryingInterpolateBoundary = 0;
            m_faceVaryingPropagateCorners = 0;
            m_interpolateBoundary = 0;

            m_creaseIndices.reset();
            m_creaseLengths.reset();
            m_creaseSharpnesses.reset();

            m_cornerIndices.reset();
            m_cornerSharpnesses.reset();

            m_holes.reset();

            m_subdScheme = "catmull-clark";

            m_selfBounds.makeEmpty();
            m_childBounds.makeEmpty();
        }

        ALEMBIC_OPERATOR_BOOL( valid() );

    protected:
        friend class ISubDSchema;

        Abc::V3fArraySamplePtr m_positions;
        Abc::Int32ArraySamplePtr m_faceIndices;
        Abc::Int32ArraySamplePtr m_faceCounts;

        int32_t m_faceVaryingInterpolateBoundary;
        int32_t m_faceVaryingPropagateCorners;
        int32_t m_interpolateBoundary;

        // Creases
        Abc::Int32ArraySamplePtr    m_creaseIndices;
        Abc::Int32ArraySamplePtr    m_creaseLengths;
        Abc::FloatArraySamplePtr  m_creaseSharpnesses;

        // Corners
        Abc::Int32ArraySamplePtr    m_cornerIndices;
        Abc::FloatArraySamplePtr  m_cornerSharpnesses;

        // Holes
        Abc::Int32ArraySamplePtr    m_holes;

        // subdivision scheme
        std::string m_subdScheme;

        // bounds
        Abc::Box3d m_selfBounds;
        Abc::Box3d m_childBounds;

    }; // end ISubDSchema::Sample

    //-*************************************************************************
    // SUBD SCHEMA
    //-*************************************************************************
public:
    //! By convention we always define this_type in AbcGeom classes.
    //! Used by unspecified-bool-type conversion below
    typedef ISubDSchema this_type;

    //-*************************************************************************
    // CONSTRUCTION, DESTRUCTION, ASSIGNMENT
    //-*************************************************************************

    //! The default constructor creates an empty ISubDSchema
    //! ...
    ISubDSchema() {}

    //! This templated, primary constructor creates a new subd reader.
    //! The first argument is any Abc (or AbcCoreAbstract) object
    //! which can intrusively be converted to an CompoundPropertyWriterPtr
    //! to use as a parent, from which the error handler policy for
    //! inheritance is also derived.  The remaining optional arguments
    //! can be used to override the ErrorHandlerPolicy and to specify
    //! schema interpretation matching.
    template <class CPROP_PTR>
    ISubDSchema( CPROP_PTR iParentObject,
                 const std::string &iName,

                 const Abc::IArgument &iArg0 = Abc::IArgument(),
                 const Abc::IArgument &iArg1 = Abc::IArgument() )
      : Abc::ISchema<SubDSchemaInfo>( iParentObject, iName,
                                      iArg0, iArg1 )
    {
        init(  iArg0, iArg1 );
    }

    //! Same constructor as above, but use the default schema name, ie,
    //! ".geom".
    template <class CPROP_PTR>
    explicit ISubDSchema( CPROP_PTR iParentObject,
                          const Abc::IArgument &iArg0 = Abc::IArgument(),
                          const Abc::IArgument &iArg1 = Abc::IArgument() )
      : Abc::ISchema<SubDSchemaInfo>( iParentObject,
                                      iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! wrap an existing schema object
    template <class CPROP_PTR>
    ISubDSchema( CPROP_PTR iThis,
                 Abc::WrapExistingFlag iFlag,

                 const Abc::IArgument &iArg0 = Abc::IArgument(),
                 const Abc::IArgument &iArg1 = Abc::IArgument() )
      : Abc::ISchema<SubDSchemaInfo>( iThis, iFlag, iArg0, iArg1 )
    {
        init( iArg0, iArg1 );
    }

    //! Copy constructor.
    ISubDSchema(const ISubDSchema& iCopy)
    {
        *this = iCopy;
    }

    //! Default assignment operator used.

    //-*************************************************************************
    // SCHEMA STUFF
    //-*************************************************************************


    MeshTopologyVariance getTopologyVariance();

    //! if isConstant() is true, the mesh contains no time-varying values
    bool isConstant() { return getTopologyVariance() == kConstantTopology; }

    //-*************************************************************************
    // SAMPLE STUFF
    //-*************************************************************************

    //! Get number of samples written so far.
    //! ...
    size_t getNumSamples();

    //! Return the time sampling type, which is stored on each of the
    //! sub properties.
    AbcA::TimeSamplingType getTimeSamplingType() const
    { return m_positions.getTimeSamplingType(); }


    //! Time information.
    //! If the time sampling ptr is null, it means we don't have any explicit
    //! mapping from sample indices to times (identity time sampling). If
    //! a time needs to be associated with an index, it will simply be
    //! ( chrono_t )index_t, and similarly ( index_t )chrono_t.
    AbcA::TimeSampling getTimeSampling()
    {
        if ( !m_positions.getTimeSampling().isStatic() )
        {
            return m_positions.getTimeSampling();
        }
        else if ( !m_faceIndices.getTimeSampling().isStatic() )
        {
            return m_faceIndices.getTimeSampling();
        }
        else { return m_faceCounts.getTimeSampling(); }
    }

    void get( Sample &iSamp,
              const Abc::ISampleSelector &iSS = Abc::ISampleSelector() );

    Sample getValue( const Abc::ISampleSelector &iSS = Abc::ISampleSelector() )
    {
        Sample smp;
        get( smp, iSS );
        return smp;
    }

    Abc::IV3fArrayProperty getPositions()
    {
        return m_positions;
    }

    IV2fGeomParam &getUVs() { return m_uvs; }

    ICompoundProperty getArbGeomParams() { return m_arbGeomParams; }

    //-*************************************************************************
    // ABC BASE MECHANISMS
    // These functions are used by Abc to deal with errors, rewrapping,
    // and so on.
    //-*************************************************************************

    //! Reset returns this function set to an empty, default
    //! state.
    void reset()
    {
        m_positions.reset();
        m_faceIndices.reset();
        m_faceCounts.reset();

        m_faceVaryingInterpolateBoundary.reset();
        m_faceVaryingPropagateCorners.reset();
        m_interpolateBoundary.reset();

        m_creaseIndices.reset();
        m_creaseLengths.reset();
        m_creaseSharpnesses.reset();

        m_cornerIndices.reset();
        m_cornerSharpnesses.reset();

        m_holes.reset();

        m_subdScheme.reset();

        m_uvs.reset();

        m_arbGeomParams.reset();

        Abc::ISchema<SubDSchemaInfo>::reset();
    }

    //! Valid returns whether this function set is
    //! valid.
    bool valid() const
    {
        return ( Abc::ISchema<SubDSchemaInfo>::valid() &&
                 m_positions.valid() &&
                 m_faceIndices.valid() &&
                 m_faceCounts.valid() );
    }

    //! unspecified-bool-type operator overload.
    //! ...
    ALEMBIC_OVERRIDE_OPERATOR_BOOL( ISubDSchema::valid() );

protected:
    void init( const Abc::IArgument &iArg0, const Abc::IArgument &iArg1 );

    Abc::IV3fArrayProperty m_positions;
    Abc::IInt32ArrayProperty m_faceIndices;
    Abc::IInt32ArrayProperty m_faceCounts;

    // misc
    Abc::IInt32Property m_faceVaryingInterpolateBoundary;
    Abc::IInt32Property m_faceVaryingPropagateCorners;
    Abc::IInt32Property m_interpolateBoundary;

    // Creases
    Abc::IInt32ArrayProperty    m_creaseIndices;
    Abc::IInt32ArrayProperty    m_creaseLengths;
    Abc::IFloatArrayProperty  m_creaseSharpnesses;

    // Corners
    Abc::IInt32ArrayProperty    m_cornerIndices;
    Abc::IFloatArrayProperty  m_cornerSharpnesses;

    // Holes
    Abc::IInt32ArrayProperty    m_holes;

    // subdivision scheme
    Abc::IStringProperty m_subdScheme;

    // bounds
    Abc::IBox3dProperty m_selfBounds;
    Abc::IBox3dProperty m_childBounds;

    // UVs
    IV2fGeomParam m_uvs;

    // random geometry parameters
    Abc::ICompoundProperty m_arbGeomParams;
};

//-*****************************************************************************
// SCHEMA OBJECT
//-*****************************************************************************
typedef Abc::ISchemaObject<ISubDSchema> ISubD;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
