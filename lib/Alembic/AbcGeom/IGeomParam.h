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

#ifndef _Alembic_AbcGeom_IGeomParam_h_
#define _Alembic_AbcGeom_IGeomParam_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
template <class TRAITS>
class ITypedGeomParam
{
public:
    typedef typename TRAITS::value_type value_type;
    typedef Abc::ITypedArrayProperty<TRAITS> prop_type;

    class Sample
    {
    public:
        typedef Sample this_type;
        typedef boost::shared_ptr< Abc::TypedArraySample<TRAITS> > samp_ptr_type;

        Sample()
        {}

        Abc::UInt32ArraySamplePtr getIndices() { return m_indices; }
        samp_ptr_type getVals() { return m_vals; }
        GeometryScope getScope() { return m_scope; }
        bool isIndexed() { return m_isIndexed; }

        void reset()
        {
            m_vals.reset();
            m_indices.reset();
            m_scope = kUnknownScope;
            m_isIndexed = false;
        }

        bool valid()
        { return m_vals; }

        ALEMBIC_OPERATOR_BOOL( valid() );

    protected:
        friend class ITypedGeomParam<TRAITS>;
        samp_ptr_type m_vals;
        Abc::UInt32ArraySamplePtr m_indices;
        GeometryScope m_scope;
        bool m_isIndexed;
    };

    //-*************************************************************************
    typedef ITypedGeomParam<TRAITS> this_type;
    typedef typename this_type::Sample sample_type;


    ITypedGeomParam() {}

    template <class CPROP>
    ITypedGeomParam( CPROP iParent,
                     const std::string &iName,
                     const Abc::IArgument &iArg0 = Abc::IArgument() );

    void getIndexed( sample_type &iSamp,
                     const Abc::ISampleSelector &iSS = Abc::ISampleSelector() );

    void getExpanded( sample_type &iSamp,
                      const Abc::ISampleSelector &iSS = Abc::ISampleSelector() );

    sample_type getIndexedValue( const Abc::ISampleSelector &iSS = \
                          Abc::ISampleSelector() )
    {
        sample_type ret;
        getIndexed( ret, iSS );
        return ret;
    }

    sample_type getExpandedValue( const Abc::ISampleSelector &iSS = \
                                  Abc::ISampleSelector() )
    {
        sample_type ret;
        getExpanded( ret, iSS );
        return ret;
    }

    size_t getNumSamples();

    AbcA::DataType getDataType() { return TRAITS::dataType(); }

    bool isIndexed() { return m_isIndexed; }

    GeometryScope getScope()
    { return GetGeometryScope( m_valProp.getMetaData() ); }

    AbcA::TimeSampling getTimeSampling();

    const std::string &getName();

    bool valid() const
    {
        return ( m_valProp.valid()
                 && ( ( ! m_cprop ) || m_indices ) );
    }

    ALEMBIC_OPERATOR_BOOL( this_type::valid() );

private:
    Abc::ErrorHandler &getErrorHandler() const
    { return m_valProp.getErrorHandler(); }

protected:
    prop_type m_valProp;

    // if the GeomParam is not indexed, these will not exist.
    Abc::IUInt32ArrayProperty m_indices;
    Abc::ICompoundProperty m_cprop;

    bool m_isIndexed;
};

//-*****************************************************************************
// TEMPLATED METHODS AND CONSTRUCTORS
//-*****************************************************************************
template <class TRAITS>
template <class CPROP>
ITypedGeomParam<TRAITS>::ITypedGeomParam( CPROP iParent,
                                          const std::string &iName,
                                          const IArgument &iArg0 )
{
    IArguments args( GetErrorHandlerPolicy( iParent ) );
    iArg0.setInto( args );

    getErrorHandler().setPolicy( args.getErrorHandlerPolicy() );

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ITypedGeomParam::ITypedGeomParam()" );

    AbcA::CompoundPropertyReaderPtr parent =
        GetCompoundPropertyReaderPtr( iParent );
    ABCA_ASSERT( parent != NULL,
                 "NULL CompoundPropertyReader passed into "
                 << "ITypedGeomParam ctor" );

    const AbcA::PropertyHeader *pheader =
        parent->getPropertyHeader( iName );
    ABCA_ASSERT( pheader != NULL,
                 "Nonexistent GeomParam: " << iName );

    if ( pheader->isCompound() )
    {
        // we're indexed
        m_cprop = ICompoundProperty( iParent, iName, iArg0 );
        m_indices = IUInt32ArrayProperty( m_cprop, ".indices", iArg0 );
        m_valProp = ITypedArrayProperty<TRAITS>( m_cprop, ".vals", iArg0 );
        m_isIndexed = true;
    }
    else if ( pheader->isArray() )
    {
        // not indexed
        m_valProp = ITypedArrayProperty<TRAITS>( iParent, iName, iArg0 );
        m_isIndexed = false;
    }
    else
    {
        ABCA_ASSERT( false, "Invalid ITypedGeomParam: " << iName );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
template <class TRAITS>
void
ITypedGeomParam<TRAITS>::getIndexed( ITypedGeomParam<TRAITS>::Sample &iSamp,
                                     const Abc::ISampleSelector &iSS )
{
    m_valProp.get( iSamp.m_vals, iSS );
    if ( m_indices ) { m_indices.get( iSamp.m_indices, iSS ); }
    else
    {
        uint32_t size = iSamp.m_vals->size();

        uint32_t *v = new uint32_t[size];

        for ( uint32_t i = 0 ; i < size ; ++i )
        {
            v[i] = i;
        }

        const Alembic::Util::Dimensions dims( size );

        iSamp.m_indices.reset( new Abc::UInt32ArraySample( v, dims ),
                               AbcA::TArrayDeleter<uint32_t>() );
    }

    iSamp.m_scope = this->getScope();
    iSamp.m_isIndexed = m_isIndexed;
}


//-*****************************************************************************
template <class TRAITS>
void
ITypedGeomParam<TRAITS>::getExpanded( ITypedGeomParam<TRAITS>::Sample &iSamp,
                                      const Abc::ISampleSelector &iSS )
{
    typedef typename TRAITS::value_type value_type;

    iSamp.m_scope = this->getScope();
    iSamp.m_isIndexed = m_isIndexed;

    if ( ! m_indices )
    {
        m_valProp.get( iSamp.m_vals, iSS );
    }
    else
    {
        boost::shared_ptr< Abc::TypedArraySample<TRAITS> > valPtr = \
            m_valProp.getValue( iSS );
        Abc::UInt32ArraySamplePtr idxPtr = m_indices.getValue( iSS );

        std::size_t size = idxPtr->size();

        value_type *v = new value_type[size];

        for ( size_t i = 0 ; i < size ; ++i )
        {
            v[i] = ( (*valPtr)[(*idxPtr)[i]] );
        }

        const Alembic::Util::Dimensions dims( size );

        iSamp.m_vals.reset( new Abc::TypedArraySample<TRAITS>( v, dims ),
                            AbcA::TArrayDeleter<value_type>() );
    }

}

//-*****************************************************************************
template <class TRAITS>
size_t ITypedGeomParam<TRAITS>::getNumSamples()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ITypedGeomParam::getNumSamples()" );

    if ( m_isIndexed )
    {
        return std::max( m_indices.getNumSamples(),
                         m_valProp.getNumSamples() );
    }
    else
    {
        if ( m_valProp ) { return m_valProp.getNumSamples(); }
        else { return 0; }
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return 0;
}

//-*****************************************************************************
template <class TRAITS>
const std::string &ITypedGeomParam<TRAITS>::getName()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "ITypedGeomParam::getName()" );

    if ( m_isIndexed ) { return m_cprop.getName(); }
    else { return m_valProp.getName(); }

    ALEMBIC_ABC_SAFE_CALL_END();

    static const std::string ret( "" );
    return ret;
}

//-*****************************************************************************
template <class TRAITS>
AbcA::TimeSampling ITypedGeomParam<TRAITS>::getTimeSampling()
{
    if ( m_indices && m_valProp )
    {
        AbcA::TimeSamplingType itst =
            m_indices.getTimeSampling().getTimeSamplingType();
        AbcA::TimeSamplingType vtst =
            m_valProp.getTimeSampling().getTimeSamplingType();

        if ( itst.getNumSamplesPerCycle() > vtst.getNumSamplesPerCycle() )
        { return m_indices.getTimeSampling(); }
        else
        { return m_valProp.getTimeSampling(); }
    }
    else if ( m_indices ) { return m_indices.getTimeSampling(); }
    else { return m_valProp.getTimeSampling(); }
}

//-*****************************************************************************
// TYPEDEFS
//-*****************************************************************************

typedef ITypedGeomParam<BooleanTPTraits>         IBoolGeomParam;
typedef ITypedGeomParam<Uint8TPTraits>           IUcharGeomParam;
typedef ITypedGeomParam<Int8TPTraits>            ICharGeomParam;
typedef ITypedGeomParam<Uint16TPTraits>          IUInt16GeomParam;
typedef ITypedGeomParam<Int16TPTraits>           IInt16GeomParam;
typedef ITypedGeomParam<Uint32TPTraits>          IUInt32GeomParam;
typedef ITypedGeomParam<Int32TPTraits>           IInt32GeomParam;
typedef ITypedGeomParam<Uint64TPTraits>          IUInt64GeomParam;
typedef ITypedGeomParam<Int64TPTraits>           IInt64GeomParam;
typedef ITypedGeomParam<Float16TPTraits>         IHalfGeomParam;
typedef ITypedGeomParam<Float32TPTraits>         IFloatGeomParam;
typedef ITypedGeomParam<Float64TPTraits>         IDoubleGeomParam;
typedef ITypedGeomParam<StringTPTraits>          IStringGeomParam;
typedef ITypedGeomParam<WstringTPTraits>         IWstringGeomParam;

typedef ITypedGeomParam<V2sTPTraits>             IV2sGeomParam;
typedef ITypedGeomParam<V2iTPTraits>             IV2iGeomParam;
typedef ITypedGeomParam<V2fTPTraits>             IV2fGeomParam;
typedef ITypedGeomParam<V2dTPTraits>             IV2dGeomParam;

typedef ITypedGeomParam<V3sTPTraits>             IV3sGeomParam;
typedef ITypedGeomParam<V3iTPTraits>             IV3iGeomParam;
typedef ITypedGeomParam<V3fTPTraits>             IV3fGeomParam;
typedef ITypedGeomParam<V3dTPTraits>             IV3dGeomParam;

typedef ITypedGeomParam<P2sTPTraits>             IP2sGeomParam;
typedef ITypedGeomParam<P2iTPTraits>             IP2iGeomParam;
typedef ITypedGeomParam<P2fTPTraits>             IP2fGeomParam;
typedef ITypedGeomParam<P2dTPTraits>             IP2dGeomParam;

typedef ITypedGeomParam<P3sTPTraits>             IP3sGeomParam;
typedef ITypedGeomParam<P3iTPTraits>             IP3iGeomParam;
typedef ITypedGeomParam<P3fTPTraits>             IP3fGeomParam;
typedef ITypedGeomParam<P3dTPTraits>             IP3dGeomParam;

typedef ITypedGeomParam<Box2sTPTraits>           IBox2sGeomParam;
typedef ITypedGeomParam<Box2iTPTraits>           IBox2iGeomParam;
typedef ITypedGeomParam<Box2fTPTraits>           IBox2fGeomParam;
typedef ITypedGeomParam<Box2dTPTraits>           IBox2dGeomParam;

typedef ITypedGeomParam<Box3sTPTraits>           IBox3sGeomParam;
typedef ITypedGeomParam<Box3iTPTraits>           IBox3iGeomParam;
typedef ITypedGeomParam<Box3fTPTraits>           IBox3fGeomParam;
typedef ITypedGeomParam<Box3dTPTraits>           IBox3dGeomParam;

typedef ITypedGeomParam<M33fTPTraits>            IM33fGeomParam;
typedef ITypedGeomParam<M33dTPTraits>            IM33dGeomParam;
typedef ITypedGeomParam<M44fTPTraits>            IM44fGeomParam;
typedef ITypedGeomParam<M44dTPTraits>            IM44dGeomParam;

typedef ITypedGeomParam<QuatfTPTraits>           IQuatfGeomParam;
typedef ITypedGeomParam<QuatdTPTraits>           IQuatdGeomParam;

typedef ITypedGeomParam<C3hTPTraits>             IC3hGeomParam;
typedef ITypedGeomParam<C3fTPTraits>             IC3fGeomParam;
typedef ITypedGeomParam<C3cTPTraits>             IC3cGeomParam;

typedef ITypedGeomParam<C4hTPTraits>             IC4hGeomParam;
typedef ITypedGeomParam<C4fTPTraits>             IC4fGeomParam;
typedef ITypedGeomParam<C4cTPTraits>             IC4cGeomParam;

typedef ITypedGeomParam<N2fTPTraits>             IN2fGeomParam;
typedef ITypedGeomParam<N2dTPTraits>             IN2dGeomParam;

typedef ITypedGeomParam<N3fTPTraits>             IN3fGeomParam;
typedef ITypedGeomParam<N3dTPTraits>             IN3dGeomParam;

} // namespace AbcGeom
} // namespace Alembic

#endif
