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

#ifndef _Alembic_AbcGeom_OGeomParam_h_
#define _Alembic_AbcGeom_OGeomParam_h_

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
template <class TRAITS>
class OTypedGeomParam
{
public:
    typedef typename TRAITS::value_type value_type;
    typedef OTypedArrayProperty<TRAITS> prop_type;

    class Sample
    {
    public:
        typedef Sample this_type;

        Sample()
          : m_scope( kUnknownScope )
        {}

        Sample( const Abc::TypedArraySample<TRAITS> &iVals,
                GeometryScope iScope )
          : m_vals( iVals )
          , m_scope( iScope )
        {}

        Sample( const Abc::TypedArraySample<TRAITS> &iVals,
                const Abc::UInt32ArraySample &iIndices,
                GeometryScope iScope )
          : m_vals( iVals )
          , m_indices( iIndices )
          , m_scope ( iScope )
        {}

        void setVals( const Abc::TypedArraySample<TRAITS> &iVals )
        { m_vals = iVals; }
        const Abc::TypedArraySample<TRAITS> &getVals() const
        { return m_vals; }

        void setIndices( const Abc::UInt32ArraySample &iIndices )
        { m_indices = iIndices; }
        const Abc::UInt32ArraySample &getIndices() const
        { return m_indices; }

        void setScope( GeometryScope iScope )
        { m_scope = iScope; }
        GeometryScope getScope() const
        { return m_scope; }

        void reset()
        {
            m_vals.reset();
            m_indices.reset();
            m_scope = kUnknownScope;
        }

        bool valid() { return m_vals; }

        ALEMBIC_OPERATOR_BOOL( valid() );

    protected:
        Abc::TypedArraySample<TRAITS> m_vals;
        Abc::UInt32ArraySample m_indices;
        GeometryScope m_scope;
    };

    //-*************************************************************************
    typedef OTypedGeomParam<TRAITS> this_type;
    typedef typename this_type::Sample sample_type;


    OTypedGeomParam() {}

    template <class CPROP>
    OTypedGeomParam( CPROP iParent,
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

        md.set( "isGeomParam", "true" );

        Abc::ErrorHandler::Policy ehp(
            Abc::GetErrorHandlerPolicy( iParent, iArg0, iArg1, iArg2 ) );

        if ( m_isIndexed )
        {
            m_cprop = Abc::OCompoundProperty( iParent, iName, md, ehp );

            m_valProp = prop_type( m_cprop, ".vals", md, ehp,
                                   m_timeSamplingType );

            m_indices = OUInt32ArrayProperty( m_cprop, ".indices",
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
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OTypedGeomParam::set()" );

        if ( iSS.getIndex() == 0 )
        {
            m_valProp.set( iSamp.getVals(), iSS );
            if ( m_isIndexed ) { m_indices.set( iSamp.getIndices(), iSS ); }
        }
        else
        {
            SetPropUsePrevIfNull( m_valProp, iSamp.getVals(), iSS );
            if ( m_isIndexed )
            {
                SetPropUsePrevIfNull( m_indices, iSamp.getIndices(), iSS );
            }
        }

        ALEMBIC_ABC_SAFE_CALL_END_RESET();
    }

    void setFromPrevious( const OSampleSelector &iSS )
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OTypedGeomParam::setFromPrevious()" );

        m_valProp.setFromPrevious( iSS );

        if ( m_isIndexed ) { m_indices.setFromPrevious( iSS ); }

        ALEMBIC_ABC_SAFE_CALL_END();
    }

    size_t getNumSamples()
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OTypedGeomParam::getNumSamples()" );

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

    const std::string &getName() { return m_name; }

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
    OUInt32ArrayProperty m_indices;
    bool m_isIndexed;
    TimeSamplingType m_timeSamplingType;

    GeometryScope m_scope;

    // if the GeomParam is not indexed, this will not exist.
    Abc::OCompoundProperty m_cprop;
};

//-*****************************************************************************
// TYPEDEFS
//-*****************************************************************************

typedef OTypedGeomParam<BooleanTPTraits>         OBoolGeomParam;
typedef OTypedGeomParam<Uint8TPTraits>           OUcharGeomParam;
typedef OTypedGeomParam<Int8TPTraits>            OCharGeomParam;
typedef OTypedGeomParam<Uint16TPTraits>          OUInt16GeomParam;
typedef OTypedGeomParam<Int16TPTraits>           OInt16GeomParam;
typedef OTypedGeomParam<Uint32TPTraits>          OUInt32GeomParam;
typedef OTypedGeomParam<Int32TPTraits>           OInt32GeomParam;
typedef OTypedGeomParam<Uint64TPTraits>          OUInt64GeomParam;
typedef OTypedGeomParam<Int64TPTraits>           OInt64GeomParam;
typedef OTypedGeomParam<Float16TPTraits>         OHalfGeomParam;
typedef OTypedGeomParam<Float32TPTraits>         OFloatGeomParam;
typedef OTypedGeomParam<Float64TPTraits>         ODoubleGeomParam;
typedef OTypedGeomParam<StringTPTraits>          OStringGeomParam;
typedef OTypedGeomParam<WstringTPTraits>         OWstringGeomParam;

typedef OTypedGeomParam<V2sTPTraits>             OV2sGeomParam;
typedef OTypedGeomParam<V2iTPTraits>             OV2iGeomParam;
typedef OTypedGeomParam<V2fTPTraits>             OV2fGeomParam;
typedef OTypedGeomParam<V2dTPTraits>             OV2dGeomParam;

typedef OTypedGeomParam<V3sTPTraits>             OV3sGeomParam;
typedef OTypedGeomParam<V3iTPTraits>             OV3iGeomParam;
typedef OTypedGeomParam<V3fTPTraits>             OV3fGeomParam;
typedef OTypedGeomParam<V3dTPTraits>             OV3dGeomParam;

typedef OTypedGeomParam<P2sTPTraits>             OP2sGeomParam;
typedef OTypedGeomParam<P2iTPTraits>             OP2iGeomParam;
typedef OTypedGeomParam<P2fTPTraits>             OP2fGeomParam;
typedef OTypedGeomParam<P2dTPTraits>             OP2dGeomParam;

typedef OTypedGeomParam<P3sTPTraits>             OP3sGeomParam;
typedef OTypedGeomParam<P3iTPTraits>             OP3iGeomParam;
typedef OTypedGeomParam<P3fTPTraits>             OP3fGeomParam;
typedef OTypedGeomParam<P3dTPTraits>             OP3dGeomParam;

typedef OTypedGeomParam<Box2sTPTraits>           OBox2sGeomParam;
typedef OTypedGeomParam<Box2iTPTraits>           OBox2iGeomParam;
typedef OTypedGeomParam<Box2fTPTraits>           OBox2fGeomParam;
typedef OTypedGeomParam<Box2dTPTraits>           OBox2dGeomParam;

typedef OTypedGeomParam<Box3sTPTraits>           OBox3sGeomParam;
typedef OTypedGeomParam<Box3iTPTraits>           OBox3iGeomParam;
typedef OTypedGeomParam<Box3fTPTraits>           OBox3fGeomParam;
typedef OTypedGeomParam<Box3dTPTraits>           OBox3dGeomParam;

typedef OTypedGeomParam<M33fTPTraits>            OM33fGeomParam;
typedef OTypedGeomParam<M33dTPTraits>            OM33dGeomParam;
typedef OTypedGeomParam<M44fTPTraits>            OM44fGeomParam;
typedef OTypedGeomParam<M44dTPTraits>            OM44dGeomParam;

typedef OTypedGeomParam<QuatfTPTraits>           OQuatfGeomParam;
typedef OTypedGeomParam<QuatdTPTraits>           OQuatdGeomParam;

typedef OTypedGeomParam<C3hTPTraits>             OC3hGeomParam;
typedef OTypedGeomParam<C3fTPTraits>             OC3fGeomParam;
typedef OTypedGeomParam<C3cTPTraits>             OC3cGeomParam;

typedef OTypedGeomParam<C4hTPTraits>             OC4hGeomParam;
typedef OTypedGeomParam<C4fTPTraits>             OC4fGeomParam;
typedef OTypedGeomParam<C4cTPTraits>             OC4cGeomParam;

typedef OTypedGeomParam<N2fTPTraits>             ON2fGeomParam;
typedef OTypedGeomParam<N2dTPTraits>             ON2dGeomParam;

typedef OTypedGeomParam<N3fTPTraits>             ON3fGeomParam;
typedef OTypedGeomParam<N3dTPTraits>             ON3dGeomParam;

} // namespace AbcGeom
} // namespace Alembic

#endif
