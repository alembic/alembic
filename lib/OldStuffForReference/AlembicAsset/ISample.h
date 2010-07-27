//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#ifndef _AlembicAsset_ISample_h_
#define _AlembicAsset_ISample_h_

#include <AlembicAsset/Foundation.h>
#include <AlembicAsset/Base/ISlabCache.h>
#include <AlembicAsset/Base/Slab.h>
#include <AlembicAsset/Base/TypedPropertyTraits.h>
#include <AlembicUtil/OperatorBool.h>

namespace AlembicAsset {

//-*****************************************************************************
class ISample
{
public:
    typedef ISample this_type;
    
    ISample()
      : m_handle(),
        m_pointer( NULL ),
        m_dimensions() {}
    
    explicit ISample( ISlabCache::Handle hnd )
      : m_handle( hnd ),
        m_pointer( NULL ),
        m_dimensions()
    {
        if ( m_handle )
        {
            m_pointer = m_handle->slab().rawData();
            m_dimensions = m_handle->slab().dimensions();
        }
    }

    ISample( const void *ptr,
             const Dimensions &dims )
      : m_handle(),
        m_pointer( ptr ),
        m_dimensions( dims ) {}

    ISample( const ISample &copy )
      : m_handle( copy.m_handle ),
        m_pointer( copy.m_pointer ),
        m_dimensions( copy.m_dimensions ) {}

    ISample &operator=( const ISample &copy )
    {
        m_handle = copy.m_handle;
        m_pointer = copy.m_pointer;
        m_dimensions = copy.m_dimensions;
        return *this;
    }

    bool isMulti() const { return ( bool )m_handle; }
    bool isSingular() const { return !( bool )m_handle; }
    
    const void *getRaw() const { return m_pointer; }
    const Dimensions &dimensions() const { return m_dimensions; }
    size_t numPoints() const { return m_dimensions.numPoints(); }
    size_t size() const { return m_dimensions.numPoints(); }

    ISlabCache::Handle handle() const { return m_handle; }

    ALEMBIC_OPERATOR_BOOL_NOTHROW( getRaw() );

    void release()
    {
        m_handle.reset();
        m_pointer = NULL;
        m_dimensions = Dimensions();
    }
    
protected:
    ISlabCache::Handle m_handle;
    const void *m_pointer;
    Dimensions m_dimensions;
};

//-*****************************************************************************
template <class T>
class ITypedSample : public ISample
{
public:
    typedef ITypedSample<T> this_type;
    typedef T value_type;

    ITypedSample() : ISample() {}
    
    explicit ITypedSample( ISlabCache::Handle hnd )
      : ISample( hnd ) {}

    ITypedSample( const T *ptr,
                  const Dimensions &dims )
      : ISample( ( const void * )ptr, dims ) {}

    ITypedSample( const ITypedSample<T> &copy )
      : ISample( copy ) {}

    ITypedSample<T> &operator=( const ITypedSample<T> &copy )
    {
        ISample::operator=( copy );
        return *this;
    }

    const T &operator[]( size_t i ) const
    { return (( const T * )m_pointer)[i]; }
    const T *get() const { return ( const T * )m_pointer; }
};


//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// TYPEDEFS
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

typedef ITypedSample<uint8_t>           IUcharSample;
typedef ITypedSample<int8_t>            ICharSample;
typedef ITypedSample<uint16_t>          IUshortSample;
typedef ITypedSample<int16_t>           IShortSample;
typedef ITypedSample<uint32_t>          IUintSample;
typedef ITypedSample<int32_t>           IIntSample;
typedef ITypedSample<float32_t>         IFloatSample;
typedef ITypedSample<float64_t>         IDoubleSample;

typedef ITypedSample<V2s>               IV2sSample;
typedef ITypedSample<V2i>               IV2iSample;
typedef ITypedSample<V2f>               IV2fSample;
typedef ITypedSample<V2d>               IV2dSample;

typedef ITypedSample<V3s>               IV3sSample;
typedef ITypedSample<V3i>               IV3iSample;
typedef ITypedSample<V3f>               IV3fSample;
typedef ITypedSample<V3d>               IV3dSample;

typedef ITypedSample<Box2s>             IBox2sSample;
typedef ITypedSample<Box2i>             IBox2iSample;
typedef ITypedSample<Box2f>             IBox2fSample;
typedef ITypedSample<Box2d>             IBox2dSample;

typedef ITypedSample<Box3s>             IBox3sSample;
typedef ITypedSample<Box3i>             IBox3iSample;
typedef ITypedSample<Box3f>             IBox3fSample;
typedef ITypedSample<Box3d>             IBox3dSample;

typedef ITypedSample<M33f>              IM33fSample;
typedef ITypedSample<M33d>              IM33dSample;
typedef ITypedSample<M44f>              IM44fSample;
typedef ITypedSample<M44d>              IM44dSample;

typedef ITypedSample<Quatf>             IQuatfSample;
typedef ITypedSample<Quatd>             IQuatdSample;

} // End namespace AlembicAsset

#endif
