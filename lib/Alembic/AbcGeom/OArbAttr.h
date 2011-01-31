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

#ifndef _Alembic_AbcGeom_OArbAttr_h_
#define _Alembic_AbcGeom_OArbAttr_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/ArbAttrSample.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
template <class TRAITS>
class OTypedArbAttr
{
public:
    typedef OTypedArbAttr<TRAITS> this_type;
    typedef typename TRAITS::value_type value_type;
    typedef TypedArbAttrSample<TRAITS> sample_type;
    typedef OTypedArrayProperty<TRAITS> prop_type;

    OTypedArbAttr() {}

    template <class CPROP>
    OTypedArbAttr( CPROP iParent,
                   const std::string &iName,
                   bool iIsIndexed,
                   GeometryScope iScope,
                   const OArgument &iArg0 = OArgument(),
                   const OArgument &iArg1 = OArgument(),
                   const OArgument &iArg2 = OArgument()
                 )
      : m_name( iName )
      , m_isIndexed( iIsIndexed )
      , m_timeSamplingType( Abc::GetTimeSamplingType( iArg0, iArg1, iArg2 ) )
      , m_scope( iScope )
    {
        AbcA::MetaData md = Abc::GetMetaData( iArg0, iArg1, iArg2 );
        SetGeometryScope( md, iScope );

        Abc::ErrorHandler::Policy ehp(
            Abc::GetErrorHandlerPolicy( iParent, iArg0, iArg1, iArg2 ) );

        if ( m_isIndexed )
        {
            m_cprop = Abc::OCompoundProperty( iParent, iName, md, ehp );

            m_valProp = prop_type( m_cprop, ".vals", md, ehp,
                                   m_timeSamplingType );

            m_indices = OInt32ArrayProperty( m_cprop, ".indices",
                                             m_timeSamplingType );
        }
        else
        {
            m_valProp = prop_type( iParent, iName, md, ehp,
                                   m_timeSamplingType );
        }
    }

    void set( const sample_type &iSamp,
              const OSampleSelector &iSS = OSampleSelector() )
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OTypedArbAttr::set()" );

        if ( iSS.getIndex() == 0 )
        {
            if ( m_isIndexed )
            {
                m_indices.set( iSamp.getIndices(), iSS );
                m_valProp.set( iSamp.getIndexedVals(), iSS );
            }
            else
            {
                m_valProp.set( iSamp.getExpandedVals(), iSS );
            }
        }
        else
        {
            if ( m_isIndexed )
            {
                SetPropUsePrevIfNull( m_indices, iSamp.getIndices(), iSS );
                SetPropUsePrevIfNull( m_valProp, iSamp.getIndexedVals(), iSS );
            }
            else
            {
                SetPropUsePrevIfNull( m_valProp, iSamp.getExpandedVals(), iSS );
            }
        }

        ALEMBIC_ABC_SAFE_CALL_END_RESET();

    }
    void setFromPrevious( const OSampleSelector &iSS )
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OTypedArbAttr::setFromPrevious()" );

        m_valProp.setFromPrevious( iSS );

        if ( m_isIndexed ) { m_indices.setFromPrevious( iSS ); }

        ALEMBIC_ABC_SAFE_CALL_END();
    }

    size_t getNumSamples()
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OTypedArbAttr::getNumSamples()" );

        if ( m_isIndexed )
        {
            if ( m_indices )
            {
                return std::max( m_indices.getNumSamples(),
                                 m_valProp.getNumSamples() );
            }
            else { return 0; }
        }
        else
        {
            if ( m_valProp ) { return m_valProp.getNumSamples(); }
            else { return 0; }
        }

        ALEMBIC_ABC_SAFE_CALL_END();

        return 0;
    }

    const AbcA::DataType &getDataType() { return TRAITS::dataType(); }

    bool isIndexed() { return m_isIndexed; }

    GeometryScope getScope() { return m_scope; }

    TimeSamplingType getTimeSamplingType() { return m_timeSamplingType; }

    bool valid() const
    {
        return ( m_valProp.valid()
                 && ( ( ! m_isIndexed ) || m_indices ) );
    }

    ALEMBIC_OPERATOR_BOOL( this_type::valid() );

    void reset()
    {
        m_timeSamplingType = AbcA::TimeSamplingType();
        m_name = "";
        m_valProp.reset();
        m_indices.reset();
        m_cprop.reset();
        m_scope = kUnknownScope;
        m_isIndexed = false;
    }

private:
    Abc::ErrorHandler &getErrorHandler() const
    { return m_valProp.getErrorHandler(); }

protected:
    std::string m_name;

    prop_type m_valProp;
    OInt32ArrayProperty m_indices;
    bool m_isIndexed;
    TimeSamplingType m_timeSamplingType;

    GeometryScope m_scope;

    // if the GeomParam is not indexed, this will not exist.
    Abc::OCompoundProperty m_cprop;
};

//-*****************************************************************************
// TYPEDEFS
//-*****************************************************************************

typedef OTypedArbAttr<BooleanTPTraits>         OBoolArbAttr;
typedef OTypedArbAttr<Uint8TPTraits>           OUcharArbAttr;
typedef OTypedArbAttr<Int8TPTraits>            OCharArbAttr;
typedef OTypedArbAttr<Uint16TPTraits>          OUInt16ArbAttr;
typedef OTypedArbAttr<Int16TPTraits>           OInt16ArbAttr;
typedef OTypedArbAttr<Uint32TPTraits>          OUInt32ArbAttr;
typedef OTypedArbAttr<Int32TPTraits>           OInt32ArbAttr;
typedef OTypedArbAttr<Uint64TPTraits>          OUInt64ArbAttr;
typedef OTypedArbAttr<Int64TPTraits>           OInt64ArbAttr;
typedef OTypedArbAttr<Float16TPTraits>         OHalfArbAttr;
typedef OTypedArbAttr<Float32TPTraits>         OFloatArbAttr;
typedef OTypedArbAttr<Float64TPTraits>         ODoubleArbAttr;
typedef OTypedArbAttr<StringTPTraits>          OStringArbAttr;
typedef OTypedArbAttr<WstringTPTraits>         OWstringArbAttr;

typedef OTypedArbAttr<V2sTPTraits>             OV2sArbAttr;
typedef OTypedArbAttr<V2iTPTraits>             OV2iArbAttr;
typedef OTypedArbAttr<V2fTPTraits>             OV2fArbAttr;
typedef OTypedArbAttr<V2dTPTraits>             OV2dArbAttr;

typedef OTypedArbAttr<V3sTPTraits>             OV3sArbAttr;
typedef OTypedArbAttr<V3iTPTraits>             OV3iArbAttr;
typedef OTypedArbAttr<V3fTPTraits>             OV3fArbAttr;
typedef OTypedArbAttr<V3dTPTraits>             OV3dArbAttr;

typedef OTypedArbAttr<P2sTPTraits>             OP2sArbAttr;
typedef OTypedArbAttr<P2iTPTraits>             OP2iArbAttr;
typedef OTypedArbAttr<P2fTPTraits>             OP2fArbAttr;
typedef OTypedArbAttr<P2dTPTraits>             OP2dArbAttr;

typedef OTypedArbAttr<P3sTPTraits>             OP3sArbAttr;
typedef OTypedArbAttr<P3iTPTraits>             OP3iArbAttr;
typedef OTypedArbAttr<P3fTPTraits>             OP3fArbAttr;
typedef OTypedArbAttr<P3dTPTraits>             OP3dArbAttr;

typedef OTypedArbAttr<Box2sTPTraits>           OBox2sArbAttr;
typedef OTypedArbAttr<Box2iTPTraits>           OBox2iArbAttr;
typedef OTypedArbAttr<Box2fTPTraits>           OBox2fArbAttr;
typedef OTypedArbAttr<Box2dTPTraits>           OBox2dArbAttr;

typedef OTypedArbAttr<Box3sTPTraits>           OBox3sArbAttr;
typedef OTypedArbAttr<Box3iTPTraits>           OBox3iArbAttr;
typedef OTypedArbAttr<Box3fTPTraits>           OBox3fArbAttr;
typedef OTypedArbAttr<Box3dTPTraits>           OBox3dArbAttr;

typedef OTypedArbAttr<M33fTPTraits>            OM33fArbAttr;
typedef OTypedArbAttr<M33dTPTraits>            OM33dArbAttr;
typedef OTypedArbAttr<M44fTPTraits>            OM44fArbAttr;
typedef OTypedArbAttr<M44dTPTraits>            OM44dArbAttr;

typedef OTypedArbAttr<QuatfTPTraits>           OQuatfArbAttr;
typedef OTypedArbAttr<QuatdTPTraits>           OQuatdArbAttr;

typedef OTypedArbAttr<C3hTPTraits>             OC3hArbAttr;
typedef OTypedArbAttr<C3fTPTraits>             OC3fArbAttr;
typedef OTypedArbAttr<C3cTPTraits>             OC3cArbAttr;

typedef OTypedArbAttr<C4hTPTraits>             OC4hArbAttr;
typedef OTypedArbAttr<C4fTPTraits>             OC4fArbAttr;
typedef OTypedArbAttr<C4cTPTraits>             OC4cArbAttr;

typedef OTypedArbAttr<N2fTPTraits>             ON2fArbAttr;
typedef OTypedArbAttr<N2dTPTraits>             ON2dArbAttr;

typedef OTypedArbAttr<N3fTPTraits>             ON3fArbAttr;
typedef OTypedArbAttr<N3dTPTraits>             ON3dArbAttr;

} // namespace AbcGeom
} // namespace Alembic

#endif
