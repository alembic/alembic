//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include "ArbAttrUtil.h"

//-*****************************************************************************
void ParamListBuilder::add( const std::string & declaration, RtPointer value,
                            ArraySamplePtr sampleToRetain )
{
    m_declarations.push_back( declaration );

    m_outputDeclarations.push_back(
        const_cast<char *>( m_declarations.back().c_str() ) );

    m_values.push_back( value );

    if ( sampleToRetain )
    {
        m_retainedSamples.push_back( sampleToRetain );
    }
}

//-*****************************************************************************
RtInt ParamListBuilder::n()
{
    return (RtInt) m_values.size();
}

//-*****************************************************************************
RtToken* ParamListBuilder::nms()
{
    if ( m_outputDeclarations.empty() ) { return 0; }

    return &m_outputDeclarations.front();
}

//-*****************************************************************************
RtPointer* ParamListBuilder::vals()
{
    if ( m_values.empty() ) { return NULL; };

    return &m_values.front();
}

//-*****************************************************************************
RtPointer ParamListBuilder::addStringValue( const std::string &value,
                                            bool retainLocally )
{
    if ( retainLocally )
    {
        m_retainedStrings.push_back( value );

        m_convertedStrings.push_back(
            const_cast<RtString>( m_retainedStrings.back().c_str() ) );
    }
    else
    {
        m_convertedStrings.push_back(
            const_cast<RtString>( value.c_str() ) );
    }
    return (RtPointer) &m_convertedStrings.back();
}

//-*****************************************************************************
std::string GetPrmanScopeString( GeometryScope scope )
{
    switch (scope)
    {
    case kUniformScope:
        return "uniform";
    case kVaryingScope:
        return "varying";
    case kVertexScope:
        return "vertex";
    case kFacevaryingScope:
        return "facevarying";
    case kConstantScope:
    default:
        return "constant";
    }
}

//-*****************************************************************************
template <typename T>
void AddArbitraryPropertyToParamListBuilder( ICompoundProperty & parent,
                                             const PropertyHeader &propHeader,
                                             ISampleSelector &sampleSelector,
                                             const std::string &rmanBaseType,
                                             ParamListBuilder &ParamListBuilder
                                           )
{
    T prop( parent, propHeader.getName() );

    if ( ! prop.valid() )
    {
        //TODO error message?
        return;
    }

    std::string rmanType = GetPrmanScopeString(
        GetGeometryScope( propHeader.getMetaData() ) ) + " ";

    rmanType += rmanBaseType + " " + propHeader.getName();

    typename T::sample_ptr_type propSample = prop.getValue( sampleSelector );

    ParamListBuilder.add( rmanType, (RtPointer)propSample->get(), propSample );
}

//-*****************************************************************************
void AddArbitraryStringPropertyToParamListBuilder(
    ICompoundProperty &parent,
    const PropertyHeader &propHeader,
    ISampleSelector &sampleSelector,
    ParamListBuilder &ParamListBuilder
                                                 )
{
    IStringArrayProperty prop( parent, propHeader.getName() );
    if ( ! prop.valid() )
    {
        //error message?
        return;
    }

    std::string rmanType = GetPrmanScopeString(
        GetGeometryScope( propHeader.getMetaData() ) ) + " ";
    rmanType += "string " + propHeader.getName();

    StringArraySamplePtr propSample = prop.getValue( sampleSelector );

    RtPointer dataStart = NULL;
    for ( size_t i = 0; i < propSample->size(); ++i )
    {
        RtPointer data = ParamListBuilder.addStringValue( (*propSample)[i] );
        if ( i == 0 ) { dataStart = data; }
    }

    ParamListBuilder.add(rmanType, dataStart, propSample);

}

//-*****************************************************************************
void AddArbitraryProperties( ICompoundProperty &parent,
                             ISampleSelector &sampleSelector,
                             ParamListBuilder &ParamListBuilder,
                             const std::set<std::string> * excludeNames
                           )
{
    for ( size_t i = 0; i < parent.getNumProperties(); ++i )
    {
        const PropertyHeader &propHeader = parent.getPropertyHeader( i );
        const std::string &propName = propHeader.getName();

        if (propName.empty()
            || propName[0] == '.'
            || ( excludeNames
                 && excludeNames->find( propName ) != excludeNames->end() )
            || propHeader.getMetaData().get( "geoScope" ).empty()
            || propHeader.isCompound() // skip compounds for now
            || propHeader.isScalar() // also skip scalars
           ) { continue; }

        if ( IN3fArrayProperty::matches( propHeader ) )
        {
            AddArbitraryPropertyToParamListBuilder<IN3fArrayProperty>(
                parent,
                propHeader,
                sampleSelector,
                "normal",
                ParamListBuilder );
        }
        else if ( IV2fArrayProperty::matches( propHeader ) )
        {
            AddArbitraryPropertyToParamListBuilder<IV2fArrayProperty>(
                parent,
                propHeader,
                sampleSelector,
                "float[2]",
                ParamListBuilder );
        }
        else if ( IV3fArrayProperty::matches( propHeader ) )
        {
            AddArbitraryPropertyToParamListBuilder<IV3fArrayProperty>(
                parent,
                propHeader,
                sampleSelector,
                "point",
                ParamListBuilder );
        }
        else if ( IM44fArrayProperty::matches( propHeader ) )
        {
            AddArbitraryPropertyToParamListBuilder<IM44fArrayProperty>(
                parent,
                propHeader,
                sampleSelector,
                "matrix",
                ParamListBuilder );
        }
        else if ( IFloatArrayProperty::matches( propHeader ) )
        {
            AddArbitraryPropertyToParamListBuilder<IFloatArrayProperty>(
                parent,
                propHeader,
                sampleSelector,
                "float",
                ParamListBuilder );
        }
        else if ( IInt32ArrayProperty::matches( propHeader ) )
        {
            AddArbitraryPropertyToParamListBuilder<IInt32ArrayProperty>(
                parent,
                propHeader,
                sampleSelector,
                "int",
                ParamListBuilder );
        }
        else if ( IStringArrayProperty::matches( propHeader ) )
        {
            AddArbitraryStringPropertyToParamListBuilder(
                parent,
                propHeader,
                sampleSelector,
                ParamListBuilder );
        }

    }
}
