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
#ifndef _Alembic_Prman_ArbAttrUtil_h_
#define _Alembic_Prman_ArbAttrUtil_h_

#include <ri.h>

#include <Alembic/AbcGeom/All.h>

#include <set>

using namespace Alembic::AbcGeom;

//-*****************************************************************************
class ParamListBuilder
{
public:
    void add( const std::string & declaration, RtPointer value,
              ArraySamplePtr sampleToRetain = ArraySamplePtr() );

    RtPointer addStringValue( const std::string & value,
                              bool retainLocally = false );

    RtInt n();
    RtToken* nms();
    RtPointer* vals();

private:
    std::vector<std::string> m_declarations;
    std::vector<RtToken> m_outputDeclarations;
    std::vector<RtPointer> m_values;
    std::vector<ArraySamplePtr> m_retainedSamples;

    //Used for converting std::string arrays to RtString arrays
    std::vector<RtString> m_convertedStrings;
    std::vector<std::string> m_retainedStrings;
};

//-*****************************************************************************
std::string GetPrmanScopeString( GeometryScope scope );

//-*****************************************************************************
template <typename T>
void AddGeomParamToParamListBuilder( ICompoundProperty & parent,
                                             const PropertyHeader &propHeader,
                                             ISampleSelector &sampleSelector,
                                             const std::string &rmanBaseType,
                                             ParamListBuilder &ParamListBuilder,
                                             size_t baseArrayExtent = 1,
                                             const std::string & overrideName = ""
                                           )
{
    T param( parent, propHeader.getName() );

    if ( !param.valid() )
    {
        //TODO error message?
        return;
    }

    std::string rmanType = GetPrmanScopeString( param.getScope() ) + " ";

    rmanType += rmanBaseType;

    size_t arrayExtent = baseArrayExtent * param.getArrayExtent();
    if (arrayExtent > 1)
    {
        std::ostringstream buffer;
        buffer << "[" << arrayExtent << "]";
        rmanType += buffer.str();
    }

    rmanType += " " + (
            overrideName.empty() ? propHeader.getName() : overrideName );


    typename T::prop_type::sample_ptr_type valueSample =
            param.getExpandedValue( sampleSelector ).getVals();

    ParamListBuilder.add( rmanType, (RtPointer)valueSample->get(), valueSample );

}

//-*****************************************************************************

void AddArbitraryGeomParams( ICompoundProperty &parent,
                             ISampleSelector &sampleSelector,
                             ParamListBuilder &ParamListBuilder,
                             const std::set<std::string> * excludeNames = NULL
                           );

#endif
