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

#ifndef _Alembic_AbcGeom_ArbAttrSample_h_
#define _Alembic_AbcGeom_ArbAttrSample_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
template <class TRAITS>
class TypedArbAttrSample
{
public:
    typedef TRAITS traits_type;
    typedef TypedArbAttrSample<TRAITS> this_type;
    typedef Abc::TypedArraySample<TRAITS> samp_type;
    typedef typename TRAITS::value_type value_type;

    //-*************************************************************************
    // Default
    TypedArbAttrSample() {}

    //-*************************************************************************
    // Construct from an Abc::TypedArarySample
    TypedArbAttrSample( const samp_type &iValues,
                        GeometryScope iScope = kUnknownScope )
      : m_vals( iValues )
      , m_scope( iScope )
    {}

    //-*************************************************************************
    // Construct from a typed array sample and an array of indices
    TypedArbAttrSample( const samp_type &iValues,
                        const Abc::Int32ArraySample &iIndices,
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

    const Abc::Int32ArraySample &getIndices() const { return m_indices; }
    void setIndices( const Abc::Int32ArraySample &iIndices )
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
    Abc::Int32ArraySample m_indices;
    GeometryScope m_scope;
};

//-*****************************************************************************
// TYPEDEFS
//-*****************************************************************************
typedef TypedArbAttrSample<BooleanTPTraits> BoolArbAttrSample;
typedef TypedArbAttrSample<Uint8TPTraits> UcharArbAttrSample;
typedef TypedArbAttrSample<Int8TPTraits> CharArbAttrSample;
typedef TypedArbAttrSample<Uint16TPTraits> UInt16ArbAttrSample;
typedef TypedArbAttrSample<Int16TPTraits> Int16ArbAttrSample;
typedef TypedArbAttrSample<Uint32TPTraits> UInt32ArbAttrSample;
typedef TypedArbAttrSample<Int32TPTraits> Int32ArbAttrSample;
typedef TypedArbAttrSample<Uint64TPTraits> UInt64ArbAttrSample;
typedef TypedArbAttrSample<Int64TPTraits> Int64ArbAttrSample;
typedef TypedArbAttrSample<Float16TPTraits> HalfArbAttrSample;
typedef TypedArbAttrSample<Float32TPTraits> FloatArbAttrSample;
typedef TypedArbAttrSample<Float64TPTraits> DoubleArbAttrSample;
typedef TypedArbAttrSample<StringTPTraits> StringArbAttrSample;
typedef TypedArbAttrSample<WstringTPTraits> WstringArbAttrSample;

typedef TypedArbAttrSample<V2sTPTraits> V2sArbAttrSample;
typedef TypedArbAttrSample<V2iTPTraits> V2iArbAttrSample;
typedef TypedArbAttrSample<V2fTPTraits> V2fArbAttrSample;
typedef TypedArbAttrSample<V2dTPTraits> V2dArbAttrSample;

typedef TypedArbAttrSample<V3sTPTraits> V3sArbAttrSample;
typedef TypedArbAttrSample<V3iTPTraits> V3iArbAttrSample;
typedef TypedArbAttrSample<V3fTPTraits> V3fArbAttrSample;
typedef TypedArbAttrSample<V3dTPTraits> V3dArbAttrSample;

typedef TypedArbAttrSample<P2sTPTraits> P2sArbAttrSample;
typedef TypedArbAttrSample<P2iTPTraits> P2iArbAttrSample;
typedef TypedArbAttrSample<P2fTPTraits> P2fArbAttrSample;
typedef TypedArbAttrSample<P2dTPTraits> P2dArbAttrSample;

typedef TypedArbAttrSample<P3sTPTraits> P3sArbAttrSample;
typedef TypedArbAttrSample<P3iTPTraits> P3iArbAttrSample;
typedef TypedArbAttrSample<P3fTPTraits> P3fArbAttrSample;
typedef TypedArbAttrSample<P3dTPTraits> P3dArbAttrSample;

typedef TypedArbAttrSample<Box3sTPTraits> Box3sArbAttrSample;
typedef TypedArbAttrSample<Box3iTPTraits> Box3iArbAttrSample;
typedef TypedArbAttrSample<Box3fTPTraits> Box3fArbAttrSample;
typedef TypedArbAttrSample<Box3dTPTraits> Box3dArbAttrSample;

typedef TypedArbAttrSample<M33fTPTraits> M33fArbAttrSample;
typedef TypedArbAttrSample<M33dTPTraits> M33dArbAttrSample;

typedef TypedArbAttrSample<M44fTPTraits> M44fArbAttrSample;
typedef TypedArbAttrSample<M44dTPTraits> M44dArbAttrSample;

typedef TypedArbAttrSample<QuatfTPTraits> QuatfArbAttrSample;
typedef TypedArbAttrSample<QuatdTPTraits> QuatdArbAttrSample;

typedef TypedArbAttrSample<C3hTPTraits> C3hArbAttrSample;
typedef TypedArbAttrSample<C3fTPTraits> C3fArbAttrSample;
typedef TypedArbAttrSample<C3cTPTraits> C3cArbAttrSample;

typedef TypedArbAttrSample<C4hTPTraits> C4hArbAttrSample;
typedef TypedArbAttrSample<C4fTPTraits> C4fArbAttrSample;
typedef TypedArbAttrSample<C4cTPTraits> C4cArbAttrSample;

typedef TypedArbAttrSample<N2fTPTraits> N2fArbAttrSample;
typedef TypedArbAttrSample<N2dTPTraits> N2dArbAttrSample;

typedef TypedArbAttrSample<N3fTPTraits> N3fArbAttrSample;
typedef TypedArbAttrSample<N3dTPTraits> N3dArbAttrSample;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
