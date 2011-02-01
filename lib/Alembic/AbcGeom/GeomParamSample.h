//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#ifndef _Alembic_AbcGeom_GeomParamSample_h_
#define _Alembic_AbcGeom_GeomParamSample_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
template <class TRAITS>
class TypedGeomParamSample
{
public:
    typedef TRAITS traits_type;
    typedef TypedGeomParamSample<TRAITS> this_type;
    typedef Abc::TypedArraySample<TRAITS> samp_type;
    typedef typename TRAITS::value_type value_type;

    //-*************************************************************************
    // Default
    TypedGeomParamSample() {}

    //-*************************************************************************
    // Construct from an Abc::TypedArarySample
    TypedGeomParamSample( const samp_type &iValues,
                          GeometryScope iScope = kUnknownScope )
      : m_vals( iValues )
      , m_scope( iScope )
    {}

    //-*************************************************************************
    // Construct from a typed array sample and an array of indices
    TypedGeomParamSample( const samp_type &iValues,
                          const Abc::UInt32ArraySample &iIndices,
                          GeometryScope iScope = kUnknownScope )
      : m_vals( iValues )
      , m_indices( iIndices )
      , m_scope( iScope )
    {}

    const samp_type &getIndexedVals() const { return m_vals; }
    const samp_type &getExpandedVals() const
    {
        if ( ! m_indices ) { return m_vals; } // easy!
        else // not as easy!
        {
            if ( ! m_vals ) { return m_vals; } // can't expand an empty sample

            std::vector<value_type> ev;

            ev.reserve( m_indices.size() );

            for ( size_t i = 0 ; i < m_indices.size() ; ++i )
            {
                ev.push_back( m_vals[m_indices[i]] );
            }

            static const samp_type ret( ev );
            return ret;
        }
    }

    void setVals( const samp_type &iVals ) { m_vals = iVals; }

    GeometryScope getScope() const { return m_scope; }
    void setScope( GeometryScope iScope ) { m_scope = iScope; }

    const Abc::UInt32ArraySample &getIndices() const { return m_indices; }
    void setIndices( const Abc::UInt32ArraySample &iIndices )
    { m_indices = iIndices; }

    bool valid() const
    {
        return ( m_vals || m_indices );
    }

    ALEMBIC_OPERATOR_BOOL( this_type::valid() );

    void reset()
    {
        m_vals.reset();
        m_indices.reset();
        m_scope = kUnknownScope;
    }

protected:
    samp_type m_vals;
    Abc::UInt32ArraySample m_indices;
    GeometryScope m_scope;
};

//-*****************************************************************************
// TYPEDEFS
//-*****************************************************************************
typedef TypedGeomParamSample<BooleanTPTraits> BoolGeomParamSample;
typedef TypedGeomParamSample<Uint8TPTraits> UcharGeomParamSample;
typedef TypedGeomParamSample<Int8TPTraits> CharGeomParamSample;
typedef TypedGeomParamSample<Uint16TPTraits> UInt16GeomParamSample;
typedef TypedGeomParamSample<Int16TPTraits> Int16GeomParamSample;
typedef TypedGeomParamSample<Uint32TPTraits> UInt32GeomParamSample;
typedef TypedGeomParamSample<Int32TPTraits> Int32GeomParamSample;
typedef TypedGeomParamSample<Uint64TPTraits> UInt64GeomParamSample;
typedef TypedGeomParamSample<Int64TPTraits> Int64GeomParamSample;
typedef TypedGeomParamSample<Float16TPTraits> HalfGeomParamSample;
typedef TypedGeomParamSample<Float32TPTraits> FloatGeomParamSample;
typedef TypedGeomParamSample<Float64TPTraits> DoubleGeomParamSample;
typedef TypedGeomParamSample<StringTPTraits> StringGeomParamSample;
typedef TypedGeomParamSample<WstringTPTraits> WstringGeomParamSample;

typedef TypedGeomParamSample<V2sTPTraits> V2sGeomParamSample;
typedef TypedGeomParamSample<V2iTPTraits> V2iGeomParamSample;
typedef TypedGeomParamSample<V2fTPTraits> V2fGeomParamSample;
typedef TypedGeomParamSample<V2dTPTraits> V2dGeomParamSample;

typedef TypedGeomParamSample<V3sTPTraits> V3sGeomParamSample;
typedef TypedGeomParamSample<V3iTPTraits> V3iGeomParamSample;
typedef TypedGeomParamSample<V3fTPTraits> V3fGeomParamSample;
typedef TypedGeomParamSample<V3dTPTraits> V3dGeomParamSample;

typedef TypedGeomParamSample<P2sTPTraits> P2sGeomParamSample;
typedef TypedGeomParamSample<P2iTPTraits> P2iGeomParamSample;
typedef TypedGeomParamSample<P2fTPTraits> P2fGeomParamSample;
typedef TypedGeomParamSample<P2dTPTraits> P2dGeomParamSample;

typedef TypedGeomParamSample<P3sTPTraits> P3sGeomParamSample;
typedef TypedGeomParamSample<P3iTPTraits> P3iGeomParamSample;
typedef TypedGeomParamSample<P3fTPTraits> P3fGeomParamSample;
typedef TypedGeomParamSample<P3dTPTraits> P3dGeomParamSample;

typedef TypedGeomParamSample<Box3sTPTraits> Box3sGeomParamSample;
typedef TypedGeomParamSample<Box3iTPTraits> Box3iGeomParamSample;
typedef TypedGeomParamSample<Box3fTPTraits> Box3fGeomParamSample;
typedef TypedGeomParamSample<Box3dTPTraits> Box3dGeomParamSample;

typedef TypedGeomParamSample<M33fTPTraits> M33fGeomParamSample;
typedef TypedGeomParamSample<M33dTPTraits> M33dGeomParamSample;

typedef TypedGeomParamSample<M44fTPTraits> M44fGeomParamSample;
typedef TypedGeomParamSample<M44dTPTraits> M44dGeomParamSample;

typedef TypedGeomParamSample<QuatfTPTraits> QuatfGeomParamSample;
typedef TypedGeomParamSample<QuatdTPTraits> QuatdGeomParamSample;

typedef TypedGeomParamSample<C3hTPTraits> C3hGeomParamSample;
typedef TypedGeomParamSample<C3fTPTraits> C3fGeomParamSample;
typedef TypedGeomParamSample<C3cTPTraits> C3cGeomParamSample;

typedef TypedGeomParamSample<C4hTPTraits> C4hGeomParamSample;
typedef TypedGeomParamSample<C4fTPTraits> C4fGeomParamSample;
typedef TypedGeomParamSample<C4cTPTraits> C4cGeomParamSample;

typedef TypedGeomParamSample<N2fTPTraits> N2fGeomParamSample;
typedef TypedGeomParamSample<N2dTPTraits> N2dGeomParamSample;

typedef TypedGeomParamSample<N3fTPTraits> N3fGeomParamSample;
typedef TypedGeomParamSample<N3dTPTraits> N3dGeomParamSample;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
