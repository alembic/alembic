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

#ifndef _Alembic_AbcGeom_IArbAttrs_h_
#define _Alembic_AbcGeom_IArbAttrs_h_

#include <Alembic/AbcGeom/Foundation.h>

//-*****************************************************************************
template <class VAL_T>
class OArbAttr : public Abc::OCompoundProperty
{
public:
    OArbAttr() {}

    template <class CPROP>
    OArbAttr( CPROP iParent,
              const std::string &iName
              const OArgument &iArg0 = OArgument(),
              const OArgument &iArg1 = OArgument(),
              const OArgument &iArg2 = OArgument() )
    {
        init( iParent, iName, iArg0, iArg1, iArg2 );
    }

    template <class CPROP>
    OArbAttr( CPROP iSelf,
              WrapExistingFlag iFlag,
              const OArgument &iArg0 = OArgument(),
              const OArgument &iArg1 = OArgument(),
              const OArgument &iArg2 = OArgument() );

    const std::string &getName() const;

    void setScope( GeometryScope iScope );
    GeometryScope getScope() const;

    TimeSamplingType getTimeSamplingType() const { return m_timeSamplingType; }

    void setIndexed( bool iIndexed ) { m_isIndexed = iIndexed; }
    bool isIndexed() const { return m_isIndexed; }




    Abc::TypedArraySample<T> getExpandedValue(const Abc::ISampleSelector &iSS);

    IndexedArbitraryParameterSample getIndexedValue(const Abc::ISampleSelector &iSS);

private:
    template <class CPROP>
    void init( CPROP iParent, const std::string &iName,
               const OArgument &iArg0,
               const OArgument &iArg1,
               const OArgument &iArg2 );

protected:
    bool m_isIndexed;
    GeometryScope m_scope;
    TimeSamplingType m_timeSamplingType;

    Abc::OTypedArrayProperty<VAL_T> m_valArray;
    Abc::OInt32ArrayProperty m_indices;
};

class IndexedArbitraryParameterSample
{
public:
    Abc::TypedArraySample<T> getValue();
    Abc::Int32ArraySamplePtr getIndices();
};
