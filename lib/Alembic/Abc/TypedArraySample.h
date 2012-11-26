//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#ifndef _Alembic_Abc_TypedArraySample_h_
#define _Alembic_Abc_TypedArraySample_h_

#include <Alembic/Abc/Foundation.h>
#include <Alembic/Abc/TypedPropertyTraits.h>

namespace Alembic {
namespace Abc {
namespace ALEMBIC_VERSION_NS {

using Alembic::Util::Dimensions;

//-*****************************************************************************
template <class TRAITS>
class TypedArraySample : public AbcA::ArraySample
{
public:
    typedef TRAITS traits_type;
    typedef TypedArraySample<TRAITS> this_type;
    typedef typename TRAITS::value_type value_type;
    typedef typename std::vector<value_type> value_vector;

    //-*************************************************************************
    // Default
    TypedArraySample()
      : AbcA::ArraySample( NULL, TRAITS::dataType(), Dimensions() ) {}

    //-*************************************************************************
    // From pointer+num or pointer+dims
    TypedArraySample( const value_type *iValues, size_t iNumVals )
      : AbcA::ArraySample( reinterpret_cast<const void *>( iValues ),
                           TRAITS::dataType(), Dimensions( iNumVals ) ) {}

    TypedArraySample( const value_type *iValues, const Dimensions &iDims )
      : AbcA::ArraySample( reinterpret_cast<const void *>( iValues ),
                           TRAITS::dataType(), iDims ) {}

    //-*************************************************************************
    // From std::vector
    TypedArraySample( const value_vector &iVec )
      : AbcA::ArraySample( reinterpret_cast<const void *>( iVec.size() > 0 ?
                                                           &iVec.front() :
                                                           NULL ),
                           TRAITS::dataType(), Dimensions( iVec.size() ) ) {}

    // This is for the case in which the data is multi-dimensional
    TypedArraySample( const value_vector &iVec,
                      const Dimensions &iDims )
      : AbcA::ArraySample( reinterpret_cast<const void *>( iVec.size() > 0 ?
                                                           &iVec.front() :
                                                           NULL ),
                           TRAITS::dataType(), iDims )
    {
        ABCA_ASSERT( iDims.numPoints() == iVec.size(),
                     "Invalid dimensions in TypedArraySample" );
    }

    //-*************************************************************************
    // COPY & ASSIGMENT
    //-*************************************************************************

    // From base copy
    TypedArraySample( const AbcA::ArraySample &iCopy )
      : AbcA::ArraySample( iCopy )
    {
        ABCA_ASSERT( iCopy.getDataType() == TRAITS::dataType(),
                     "Invalid DataType in TypedArraySample. Expected: "
                     << TRAITS::dataType()
                     << ", but got: " << iCopy.getDataType() );
    }

    TypedArraySample<TRAITS> operator=( const AbcA::ArraySample &iCopy )
    {
        ArraySample::operator=( iCopy );

        ABCA_ASSERT( iCopy.getDataType() == TRAITS::dataType(),
                     "Invalid DataType in TypedArraySample. Expected: "
                     << TRAITS::dataType()
                     << ", but got: " << iCopy.getDataType() );

        return *this;
    }

    const value_type *get() const
    {
        return reinterpret_cast<const value_type *>( getData() );
    }

    const value_type *operator->() const { return this->get(); }

    //-*************************************************************************
    const value_type &operator[]( const size_t i ) const
    {
        return this->get()[i];
    }

    //-*************************************************************************
    size_t size() const { return this->getDimensions().numPoints(); }

    static TypedArraySample<TRAITS> emptySample()
    {
        static std::vector<value_type> emptyVec;
        return TypedArraySample<TRAITS>( emptyVec );
    }

    //-*************************************************************************
    ALEMBIC_OPERATOR_BOOL( ArraySample::valid() );
};

//-*****************************************************************************
// TYPEDEFS
//-*****************************************************************************
typedef TypedArraySample<BooleanTPTraits> BoolArraySample;
typedef TypedArraySample<Uint8TPTraits> UcharArraySample;
typedef TypedArraySample<Int8TPTraits> CharArraySample;
typedef TypedArraySample<Uint16TPTraits> UInt16ArraySample;
typedef TypedArraySample<Int16TPTraits> Int16ArraySample;
typedef TypedArraySample<Uint32TPTraits> UInt32ArraySample;
typedef TypedArraySample<Int32TPTraits> Int32ArraySample;
typedef TypedArraySample<Uint64TPTraits> UInt64ArraySample;
typedef TypedArraySample<Int64TPTraits> Int64ArraySample;
typedef TypedArraySample<Float16TPTraits> HalfArraySample;
typedef TypedArraySample<Float32TPTraits> FloatArraySample;
typedef TypedArraySample<Float64TPTraits> DoubleArraySample;
typedef TypedArraySample<StringTPTraits> StringArraySample;
typedef TypedArraySample<WstringTPTraits> WstringArraySample;

typedef TypedArraySample<V2sTPTraits> V2sArraySample;
typedef TypedArraySample<V2iTPTraits> V2iArraySample;
typedef TypedArraySample<V2fTPTraits> V2fArraySample;
typedef TypedArraySample<V2dTPTraits> V2dArraySample;

typedef TypedArraySample<V3sTPTraits> V3sArraySample;
typedef TypedArraySample<V3iTPTraits> V3iArraySample;
typedef TypedArraySample<V3fTPTraits> V3fArraySample;
typedef TypedArraySample<V3dTPTraits> V3dArraySample;

typedef TypedArraySample<P2sTPTraits> P2sArraySample;
typedef TypedArraySample<P2iTPTraits> P2iArraySample;
typedef TypedArraySample<P2fTPTraits> P2fArraySample;
typedef TypedArraySample<P2dTPTraits> P2dArraySample;

typedef TypedArraySample<P3sTPTraits> P3sArraySample;
typedef TypedArraySample<P3iTPTraits> P3iArraySample;
typedef TypedArraySample<P3fTPTraits> P3fArraySample;
typedef TypedArraySample<P3dTPTraits> P3dArraySample;

typedef TypedArraySample<Box2sTPTraits> Box2sArraySample;
typedef TypedArraySample<Box2iTPTraits> Box2iArraySample;
typedef TypedArraySample<Box2fTPTraits> Box2fArraySample;
typedef TypedArraySample<Box2dTPTraits> Box2dArraySample;
  
typedef TypedArraySample<Box3sTPTraits> Box3sArraySample;
typedef TypedArraySample<Box3iTPTraits> Box3iArraySample;
typedef TypedArraySample<Box3fTPTraits> Box3fArraySample;
typedef TypedArraySample<Box3dTPTraits> Box3dArraySample;

typedef TypedArraySample<M33fTPTraits> M33fArraySample;
typedef TypedArraySample<M33dTPTraits> M33dArraySample;

typedef TypedArraySample<M44fTPTraits> M44fArraySample;
typedef TypedArraySample<M44dTPTraits> M44dArraySample;

typedef TypedArraySample<QuatfTPTraits> QuatfArraySample;
typedef TypedArraySample<QuatdTPTraits> QuatdArraySample;

typedef TypedArraySample<C3hTPTraits> C3hArraySample;
typedef TypedArraySample<C3fTPTraits> C3fArraySample;
typedef TypedArraySample<C3cTPTraits> C3cArraySample;

typedef TypedArraySample<C4hTPTraits> C4hArraySample;
typedef TypedArraySample<C4fTPTraits> C4fArraySample;
typedef TypedArraySample<C4cTPTraits> C4cArraySample;

typedef TypedArraySample<N2fTPTraits> N2fArraySample;
typedef TypedArraySample<N2dTPTraits> N2dArraySample;

typedef TypedArraySample<N3fTPTraits> N3fArraySample;
typedef TypedArraySample<N3dTPTraits> N3dArraySample;

//-*****************************************************************************
typedef Alembic::Util::shared_ptr<BoolArraySample> BoolArraySamplePtr;
typedef Alembic::Util::shared_ptr<UcharArraySample> UcharArraySamplePtr;
typedef Alembic::Util::shared_ptr<CharArraySample> CharArraySamplePtr;
typedef Alembic::Util::shared_ptr<UInt16ArraySample> UInt16ArraySamplePtr;
typedef Alembic::Util::shared_ptr<Int16ArraySample> Int16ArraySamplePtr;
typedef Alembic::Util::shared_ptr<UInt32ArraySample> UInt32ArraySamplePtr;
typedef Alembic::Util::shared_ptr<Int32ArraySample> Int32ArraySamplePtr;
typedef Alembic::Util::shared_ptr<UInt64ArraySample> UInt64ArraySamplePtr;
typedef Alembic::Util::shared_ptr<Int64ArraySample> Int64ArraySamplePtr;
typedef Alembic::Util::shared_ptr<HalfArraySample> HalfArraySamplePtr;
typedef Alembic::Util::shared_ptr<FloatArraySample> FloatArraySamplePtr;
typedef Alembic::Util::shared_ptr<DoubleArraySample> DoubleArraySamplePtr;
typedef Alembic::Util::shared_ptr<StringArraySample> StringArraySamplePtr;
typedef Alembic::Util::shared_ptr<WstringArraySample> WstringArraySamplePtr;

typedef Alembic::Util::shared_ptr<V2sArraySample> V2sArraySamplePtr;
typedef Alembic::Util::shared_ptr<V2iArraySample> V2iArraySamplePtr;
typedef Alembic::Util::shared_ptr<V2fArraySample> V2fArraySamplePtr;
typedef Alembic::Util::shared_ptr<V2dArraySample> V2dArraySamplePtr;

typedef Alembic::Util::shared_ptr<V3sArraySample> V3sArraySamplePtr;
typedef Alembic::Util::shared_ptr<V3iArraySample> V3iArraySamplePtr;
typedef Alembic::Util::shared_ptr<V3fArraySample> V3fArraySamplePtr;
typedef Alembic::Util::shared_ptr<V3dArraySample> V3dArraySamplePtr;

typedef Alembic::Util::shared_ptr<P2sArraySample> P2sArraySamplePtr;
typedef Alembic::Util::shared_ptr<P2iArraySample> P2iArraySamplePtr;
typedef Alembic::Util::shared_ptr<P2fArraySample> P2fArraySamplePtr;
typedef Alembic::Util::shared_ptr<P2dArraySample> P2dArraySamplePtr;

typedef Alembic::Util::shared_ptr<P3sArraySample> P3sArraySamplePtr;
typedef Alembic::Util::shared_ptr<P3iArraySample> P3iArraySamplePtr;
typedef Alembic::Util::shared_ptr<P3fArraySample> P3fArraySamplePtr;
typedef Alembic::Util::shared_ptr<P3dArraySample> P3dArraySamplePtr;

typedef Alembic::Util::shared_ptr<Box2sArraySample> Box2sArraySamplePtr;
typedef Alembic::Util::shared_ptr<Box2iArraySample> Box2iArraySamplePtr;
typedef Alembic::Util::shared_ptr<Box2fArraySample> Box2fArraySamplePtr;
typedef Alembic::Util::shared_ptr<Box2dArraySample> Box2dArraySamplePtr;

typedef Alembic::Util::shared_ptr<Box3sArraySample> Box3sArraySamplePtr;
typedef Alembic::Util::shared_ptr<Box3iArraySample> Box3iArraySamplePtr;
typedef Alembic::Util::shared_ptr<Box3fArraySample> Box3fArraySamplePtr;
typedef Alembic::Util::shared_ptr<Box3dArraySample> Box3dArraySamplePtr;

typedef Alembic::Util::shared_ptr<M33fArraySample> M33fArraySamplePtr;
typedef Alembic::Util::shared_ptr<M33dArraySample> M33dArraySamplePtr;

typedef Alembic::Util::shared_ptr<M44fArraySample> M44fArraySamplePtr;
typedef Alembic::Util::shared_ptr<M44dArraySample> M44dArraySamplePtr;

typedef Alembic::Util::shared_ptr<QuatfArraySample> QuatfArraySamplePtr;
typedef Alembic::Util::shared_ptr<QuatdArraySample> QuatdArraySamplePtr;

typedef Alembic::Util::shared_ptr<C3hArraySample> C3hArraySamplePtr;
typedef Alembic::Util::shared_ptr<C3fArraySample> C3fArraySamplePtr;
typedef Alembic::Util::shared_ptr<C3cArraySample> C3cArraySamplePtr;

typedef Alembic::Util::shared_ptr<C4hArraySample> C4hArraySamplePtr;
typedef Alembic::Util::shared_ptr<C4fArraySample> C4fArraySamplePtr;
typedef Alembic::Util::shared_ptr<C4cArraySample> C4cArraySamplePtr;

typedef Alembic::Util::shared_ptr<N2fArraySample> N2fArraySamplePtr;
typedef Alembic::Util::shared_ptr<N2dArraySample> N2dArraySamplePtr;

typedef Alembic::Util::shared_ptr<N3fArraySample> N3fArraySamplePtr;
typedef Alembic::Util::shared_ptr<N3dArraySample> N3dArraySamplePtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace Abc
} // End namespace Alembic

#endif
