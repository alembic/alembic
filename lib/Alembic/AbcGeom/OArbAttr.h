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

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
template <class TRAITS>
class OTypedArbAttr : public Abc::OCompoundProperty
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
                   const OArgument &iArg0 = OArgument(),
                   const OArgument &iArg1 = OArgument(),
                   const OArgument &iArg2 = OArgument() )
      : Abc::OCompoundProperty( iParent, iName, iArg0, iArg1, iArg2 )
      , m_isIndexed( false )
      , m_timeSamplingType( Abc::GetTimeSamplingType( iArg0, iArg1, iArg2 ) )
      , m_scope( kUnknownScope )
    {
        // nothing else; the value and index properties get created on
        // first call to set
    }

    void set( const sample_type &iVal,
              const OSampleSelector &iSS = OSampleSelector() );

    void setFromPrevious( const OSampleSelector &iSS );

    size_t getNumSamples();

    const AbcA::DataType &getDataType() { return TRAITS::dataType(); }

    bool isIndexed() { return m_isIndexed; }

    void setScope( GeometryScope iScope ) { m_scope = iScope; }
    GeometryScope getScope() { return m_scope; }

    TimeSamplingType getTimeSamplingType() { return m_timeSamplingType; }

    bool valid() const
    {
        return ( Abc::OCompoundProperty::valid() && m_valProp.valid()
                 && ( ( ! m_isIndexed ) || m_indices ) );
    }

    ALEMBIC_OVERRIDE_OPERATOR_BOOL( this_type::valid() );

    void reset()
    {
        m_valProp.reset();
        m_indices.reset();
        m_isIndexed = false;
        Abc::OCompoundProperty::reset();
    }

private:
    void init( const AbcA::TimeSamplingType &iTST );


protected:
    prop_type m_valProp;
    OInt32ArrayProperty m_indices;
    bool m_isIndexed;
    TimeSamplingType m_timeSamplingType;

    GeometryScope m_scope;
};

} // namespace AbcGeom
} // namespace Alembic

#endif
