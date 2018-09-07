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

#ifndef Alembic_AbcGeom_OGeomParam_h
#define Alembic_AbcGeom_OGeomParam_h

#include <Alembic/AbcGeom/Foundation.h>
#include <Alembic/AbcGeom/GeometryScope.h>

namespace Alembic {
namespace AbcGeom {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
template <class TRAITS>
class OTypedGeomParam
{
public:
    typedef typename TRAITS::value_type value_type;
    typedef OTypedArrayProperty<TRAITS> prop_type;


    //-*************************************************************************
    // inner class for setting
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

        bool valid() const { return m_vals; }

        ALEMBIC_OPERATOR_BOOL( valid() );

    protected:
        Abc::TypedArraySample<TRAITS> m_vals;
        Abc::UInt32ArraySample m_indices;
        GeometryScope m_scope;
    };

    //-*************************************************************************
    typedef OTypedGeomParam<TRAITS> this_type;
    typedef typename this_type::Sample sample_type;

    static const char * getInterpretation()
    {
        return TRAITS::interpretation();
    }

    static bool matches( const AbcA::PropertyHeader &iHeader,
                         SchemaInterpMatching iMatching = kStrictMatching )
    {
        if ( iHeader.isCompound() )
        {
            return ( iHeader.getMetaData().get( "podName" ) ==
                    Alembic::Util::PODName( TRAITS::dataType().getPod() ) &&
                    ( std::string() == getInterpretation() ||
                      atoi(
                        iHeader.getMetaData().get( "podExtent" ).c_str() ) ==
                     TRAITS::dataType().getExtent() ) ) &&
                    prop_type::matches( iHeader.getMetaData(), iMatching );
        }
        else if ( iHeader.isArray() )
        {
            return prop_type::matches( iHeader, iMatching );
        }

        return false;

    }

    OTypedGeomParam() {}

    OTypedGeomParam( OCompoundProperty iParent,
                     const std::string &iName,
                     bool iIsIndexed,
                     GeometryScope iScope,
                     size_t iArrayExtent,
                     const Argument &iArg0 = Argument(),
                     const Argument &iArg1 = Argument(),
                     const Argument &iArg2 = Argument()
                     )
        : m_name( iName )
        , m_isIndexed( iIsIndexed )
        , m_scope( iScope )
    {
        *this = OTypedGeomParam( iParent.getPtr(), iName, iIsIndexed, iScope,
            iArrayExtent, iArg0, iArg1, iArg2 );
    }

    OTypedGeomParam( AbcA::CompoundPropertyWriterPtr iParent,
                     const std::string &iName,
                     bool iIsIndexed,
                     GeometryScope iScope,
                     size_t iArrayExtent,
                     const Argument &iArg0 = Argument(),
                     const Argument &iArg1 = Argument(),
                     const Argument &iArg2 = Argument()
                 )
      : m_name( iName )
      , m_isIndexed( iIsIndexed )
      , m_scope( iScope )
    {
        Arguments args( Abc::GetErrorHandlerPolicy( iParent ) );
        iArg0.setInto( args );
        iArg1.setInto( args );
        iArg2.setInto( args );

        AbcA::MetaData md = args.getMetaData();

        SetGeometryScope( md, iScope );

        md.set( "isGeomParam", "true" );

        std::string podName( Alembic::Util::PODName(
                                 TRAITS::dataType().getPod() ) );

        size_t extent = TRAITS::dataType().getExtent();

        md.set( "podName", podName );

        std::ostringstream extentStrm;
        extentStrm << extent;
        std::string extentStr = extentStrm.str();
        md.set( "podExtent", extentStr );

        std::ostringstream arrayExtentStrm;
        arrayExtentStrm << iArrayExtent;
        std::string arrayExtentStr = arrayExtentStrm.str();
        md.set( "arrayExtent", arrayExtentStr );

        md.set( "interpretation", TRAITS::interpretation() );

        Abc::ErrorHandler::Policy ehp = args.getErrorHandlerPolicy();

        AbcA::TimeSamplingPtr tsPtr = args.getTimeSampling();
        uint32_t tsIndex = args.getTimeSamplingIndex();

        // if we specified a valid TimeSamplingPtr, use it to determine the
        // index otherwise we'll use the index, which defaults to the
        // intrinsic 0 index
        if (tsPtr)
        {
            AbcA::CompoundPropertyWriterPtr parent =
                GetCompoundPropertyWriterPtr( iParent );
            tsIndex =
                parent->getObject()->getArchive()->addTimeSampling(*tsPtr);
        }

        if ( m_isIndexed )
        {
            m_cprop = Abc::OCompoundProperty( iParent, iName, md, ehp );

            m_valProp = prop_type( m_cprop, ".vals", md, ehp, tsIndex );

            m_indicesProperty = Abc::OUInt32ArrayProperty( m_cprop, ".indices",
                tsIndex );
        }
        else
        {
            m_valProp = prop_type( iParent, iName, md, ehp, tsIndex );
        }
    }

public:

    void set( const sample_type &iSamp )
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OTypedGeomParam::set()" );

        if ( m_valProp.getNumSamples() == 0 )
        {
            m_valProp.set( iSamp.getVals() );
            if ( m_isIndexed ) { m_indicesProperty.set( iSamp.getIndices() ); }
        }
        else
        {
            SetPropUsePrevIfNull( m_valProp, iSamp.getVals() );
            if ( m_isIndexed )
            {
                SetPropUsePrevIfNull( m_indicesProperty, iSamp.getIndices() );
            }
        }

        ALEMBIC_ABC_SAFE_CALL_END_RESET();
    }

    void setFromPrevious()
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OTypedGeomParam::setFromPrevious()" );

        m_valProp.setFromPrevious();

        if ( m_isIndexed ) { m_indicesProperty.setFromPrevious(); }

        ALEMBIC_ABC_SAFE_CALL_END();
    }

    void setTimeSampling( uint32_t iIndex )
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN(
            "OTypedGeomParam::setTimeSampling( uint32_t )" );

        m_valProp.setTimeSampling( iIndex );

        if ( m_isIndexed ) { m_indicesProperty.setTimeSampling( iIndex ); }

        ALEMBIC_ABC_SAFE_CALL_END();
    }

    void setTimeSampling( AbcA::TimeSamplingPtr iTime )
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN(
            "OTypedGeomParam::setTimeSampling( TimeSamplingPtr )" );

        if (iTime)
        {
            uint32_t tsIndex =
                m_valProp.getParent().getObject().getArchive().addTimeSampling(
                    *iTime);
            setTimeSampling( tsIndex );
        }

        ALEMBIC_ABC_SAFE_CALL_END();
    }

    size_t getNumSamples() const
    {
        ALEMBIC_ABC_SAFE_CALL_BEGIN( "OTypedGeomParam::getNumSamples()" );

        if ( m_isIndexed )
        {
            if ( m_indicesProperty )
            {
                return std::max( m_indicesProperty.getNumSamples(),
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

    AbcA::DataType getDataType() const { return TRAITS::dataType(); }

    bool isIndexed() const { return m_isIndexed; }

    GeometryScope getScope() const { return m_scope; }

    AbcA::TimeSamplingPtr getTimeSampling() const
    {
        return m_valProp.getTimeSampling();
    }

    const std::string &getName() const { return m_name; }

    bool valid() const
    {
        return ( m_valProp.valid()
                 && ( ( ! m_isIndexed ) || m_indicesProperty ) );
    }

    ALEMBIC_OPERATOR_BOOL( this_type::valid() );

    void reset()
    {
        m_name = "";
        m_valProp.reset();
        m_indicesProperty.reset();
        m_cprop.reset();
        m_scope = kUnknownScope;
        m_isIndexed = false;
    }

    prop_type getValueProperty() const { return m_valProp; }

    OUInt32ArrayProperty getIndexProperty() const { return m_indicesProperty; }

private:
    Abc::ErrorHandler &getErrorHandler() const
    { return m_valProp.getErrorHandler(); }

protected:
    std::string m_name;

    prop_type m_valProp;
    OUInt32ArrayProperty m_indicesProperty;
    bool m_isIndexed;

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

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcGeom
} // End namespace Alembic

#endif
