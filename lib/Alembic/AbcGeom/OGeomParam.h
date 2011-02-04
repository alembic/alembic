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
#include <Alembic/AbcGeom/GeomParamSample.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
template <class TRAITS>
class OTypedGeomParam : public Abc::OTypedArrayProperty<TRAITS>
{
public:
    typedef OTypedGeomParam<TRAITS> this_type;
    typedef typename TRAITS::value_type value_type;
    typedef TypedGeomParamSample<TRAITS> sample_type;
    typedef typename Abc::OTypedArrayProperty<TRAITS> prop_type;
    typedef typename Abc::OUInt32ArrayProperty indices_type;

    //! Return the interpretation expected of this
    //! property. An empty interpretation matches everything
    static const std::string &getInterpretation()
    {
        static std::string sInterpretation = TRAITS::interpretation();
        return sInterpretation;
    }

    //! This will check whether or not a given entity (as represented by
    //! a metadata) strictly matches the interpretation of this
    //! GeomParam
    static bool matches( const AbcA::MetaData &iMetaData,
                         SchemaInterpMatching iMatching = kStrictMatching )
    {
        return ( getInterpretation() == "" ||
                 ( iMetaData.get( "interpretation" ) ==
                   getInterpretation() &&
                   iMetaData.get( "isGeomParam" ) != "" ) );
    }

    //! This will check whether or not a given object (as represented by
    //! an property header) strictly matches the interpretation of this
    //! GeomParam, as well as the data type.
    static bool matches( const AbcA::PropertyHeader &iHeader,
                         SchemaInterpMatching iMatching = kStrictMatching )
    {
        return ( iHeader.getDataType() == TRAITS::dataType() ) &&
            matches( iHeader.getMetaData(), iMatching );
    }

    OTypedGeomParam() {}

    template <class CPROP>
    OTypedGeomParam( CPROP iParent,
                     const std::string &iName,
                     bool iIsIndexed,
                     GeometryScope iScope = kUnknownScope,
                     const OArgument &iArg0 = OArgument(),
                     const OArgument &iArg1 = OArgument(),
                     const OArgument &iArg2 = OArgument() );

    void set( const sample_type &iSamp,
              const OSampleSelector &iSS = OSampleSelector() )
    {
        if ( m_isIndexed )
        {
            indices_type::sample_type idxsamp( iSamp.getIndices() );
            typename prop_type::sample_type valsamp( iSamp.getIndexedVals() );

            m_indices.set( idxsamp, iSS );
            prop_type::set( valsamp, iSS );
        }
        else
        {
            typename prop_type::sample_type valsamp( iSamp.getExpandedVals() );
            prop_type::set( valsamp, iSS );
        }
    }

    void setFromPrevious( const OSampleSelector &iSS )
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OTypedGeomParam::setFromPrevious()" );

        prop_type::setFromPrevious( iSS );

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
                                 prop_type::getNumSamples() );
            }
            else { return 0; }
        }
        else
        {
            return prop_type::getNumSamples();
        }

        ALEMBIC_ABC_SAFE_CALL_END();

        return 0;
    }

    bool isIndexed() { return m_isIndexed; }

    GeometryScope getScope() { return m_scope; }

    void reset()
    {
        m_scope = kUnknownScope;
        m_isIndexed = false;
        m_indices.reset();
        prop_type::reset();
    }

    bool valid() const
    {
        return ( prop_type::valid()
                 && ( ( ! m_isIndexed ) || m_indices ) );
    }

    //ALEMBIC_OPERATOR_BOOL( this_type::valid() );

private:
    std::string getIndexedName()
    {
        std::string name( "._gp_" );
        return name + prop_type::getName() + "_idxs";
    }

    Abc::ErrorHandler &getErrorHandler() const
    { return prop_type::getErrorHandler(); }

protected:
    indices_type m_indices;
    bool m_isIndexed;
    GeometryScope m_scope;
    AbcA::ArrayPropertyWriterPtr m_property;
};

//-*****************************************************************************
template <class TRAITS>
template <class CPROP>
OTypedGeomParam<TRAITS>::OTypedGeomParam( CPROP iParent,
                                          const std::string &iName,
                                          bool iIsIndexed,
                                          GeometryScope iScope,
                                          const OArgument &iArg0,
                                          const OArgument &iArg1,
                                          const OArgument &iArg2 )
  : m_isIndexed( iIsIndexed )
  , m_scope( iScope )
{
    OArguments args( Abc::GetErrorHandlerPolicy( iParent ) );
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );

    getErrorHandler().setPolicy( args.getErrorHandlerPolicy() );

    AbcA::MetaData md = Abc::GetMetaData( iArg0, iArg1, iArg2 );
    SetGeometryScope( md, iScope );

    md.set( "isGeomParam", "true" );

    Abc::ErrorHandler::Policy ehp(
        Abc::GetErrorHandlerPolicy( iParent, iArg0, iArg1, iArg2 ) );

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OTypedGeomParam()" );

    AbcA::CompoundPropertyWriterPtr parent =
        GetCompoundPropertyWriterPtr( iParent );
    ABCA_ASSERT( parent, "NULL CompoundPropertyWriterPtr" );

    if ( m_isIndexed )
    {
        const std::string idxName = getIndexedName();

        md.set( "indexedBy", idxName );

        m_indices = OUInt32ArrayProperty( iParent, idxName,
                                          args.getTimeSamplingType() );
    }

    AbcA::PropertyHeader ph( iName, AbcA::kArrayProperty, md,
                             TRAITS::dataType(),
                             args.getTimeSamplingType() );

    m_property = parent->createArrayProperty( ph );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}


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
