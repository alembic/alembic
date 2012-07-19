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

#include "ArbAttrUtil.h"
#include <sstream>
#include <cstring>

//-*****************************************************************************

ParamListBuilder::~ParamListBuilder()
{
    for ( std::vector<RtString>::iterator I = m_retainedStrings.begin();
            I != m_retainedStrings.end(); ++I )
    {
        free( (*I) );
    }
    
    m_retainedStrings.clear();
}


//-*****************************************************************************
void ParamListBuilder::add( const std::string & declaration, RtPointer value,
                            ArraySamplePtr sampleToRetain )
{
    //save a copy of the declaration string
    m_retainedStrings.push_back( strdup( declaration.c_str() ) );
    m_outputDeclarations.push_back( m_retainedStrings.back() );
    
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
RtPointer ParamListBuilder::finishStringVector()
{
    RtPointer previous = NULL;
    
    if ( !m_convertedStringVectors.empty() )
    {
        previous = &( (*m_convertedStringVectors.back())[0] );
    }
    
    m_convertedStringVectors.push_back( SharedRtStringVector(
            new std::vector<RtString> ) );
    
    return previous;
}

//-*****************************************************************************
void ParamListBuilder::addStringValue( const std::string &value,
                                            bool retainLocally )
{
    if ( m_convertedStringVectors.empty() )
    {
        finishStringVector();
    }
    
    if ( retainLocally )
    {
        m_retainedStrings.push_back( strdup( value.c_str() ) );
        m_convertedStringVectors.back()->push_back( m_retainedStrings.back() );
    }
    else
    {
        m_convertedStringVectors.back()->push_back(
            const_cast<RtString>( value.c_str() ) );
    }
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
void AddStringGeomParamToParamListBuilder(
        ICompoundProperty &parent,
        const PropertyHeader &propHeader,
        ISampleSelector &sampleSelector,
        ParamListBuilder &paramListBuilder
                                         )
{
    IStringGeomParam param( parent, propHeader.getName() );

    if ( !param.valid() )
    {
        //TODO error message?
        return;
    }

    std::string rmanType = GetPrmanScopeString( param.getScope() ) + " ";
    rmanType += "string";

    if ( param.getArrayExtent() > 1 )
    {
        std::ostringstream buffer;
        buffer << "[" << param.getArrayExtent() << "]";
        rmanType += buffer.str();
    }

    rmanType += " " + propHeader.getName();

    StringArraySamplePtr valueSample = param.getExpandedValue(
            sampleSelector ).getVals();

    
    for ( size_t i = 0; i < valueSample->size(); ++i )
    {
        paramListBuilder.addStringValue( (*valueSample)[i] );
    }
    
    RtPointer dataStart = paramListBuilder.finishStringVector();
    
    paramListBuilder.add(rmanType, dataStart, valueSample);

}


//-*****************************************************************************
void AddArbitraryGeomParams( ICompoundProperty &parent,
                             ISampleSelector &sampleSelector,
                             ParamListBuilder &paramListBuilder,
                             const std::set<std::string> * excludeNames
                           )
{
    if ( !parent.valid() )
    {
        return;
    }

    for ( size_t i = 0; i < parent.getNumProperties(); ++i )
    {
        const PropertyHeader &propHeader = parent.getPropertyHeader( i );
        const std::string &propName = propHeader.getName();

        if (propName.empty()
            || ( excludeNames
                 && excludeNames->find( propName ) != excludeNames->end() ) )
        {
            continue;
        }

        if ( IFloatGeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilder<IFloatGeomParam>(
                parent,
                propHeader,
                sampleSelector,
                "float",
                paramListBuilder);
        }
        else if ( IDoubleGeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilderAsFloat<IDoubleGeomParam, double>(
                parent,
                propHeader,
                sampleSelector,
                "float",
                paramListBuilder);
        }
        else if ( IV3dGeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilderAsFloat<IV3dGeomParam, double>(
                parent,
                propHeader,
                sampleSelector,
                "vector",
                paramListBuilder);
        }
        else if ( IInt32GeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilder<IInt32GeomParam>(
                parent,
                propHeader,
                sampleSelector,
                "int",
                paramListBuilder);
        }
        else if ( IStringGeomParam::matches( propHeader ) )
        {
            AddStringGeomParamToParamListBuilder(
                parent,
                propHeader,
                sampleSelector,
                paramListBuilder);
        }
        else if ( IV2fGeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilder<IV2fGeomParam>(
                parent,
                propHeader,
                sampleSelector,
                "float",
                paramListBuilder,
                2);
        }
        else if ( IV3fGeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilder<IV3fGeomParam>(
                parent,
                propHeader,
                sampleSelector,
                "vector",
                paramListBuilder);
        }
        else if ( IP3fGeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilder<IP3fGeomParam>(
                parent,
                propHeader,
                sampleSelector,
                "point",
                paramListBuilder);
        }
        else if ( IP3dGeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilderAsFloat<IP3dGeomParam, double>(
                parent,
                propHeader,
                sampleSelector,
                "point",
                paramListBuilder);
        }
        else if ( IN3fGeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilder<IN3fGeomParam>(
                parent,
                propHeader,
                sampleSelector,
                "normal",
                paramListBuilder);
        }
        else if ( IC3fGeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilder<IC3fGeomParam>(
                parent,
                propHeader,
                sampleSelector,
                "color",
                paramListBuilder);
        }
        else if ( IM44fGeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilder<IM44fGeomParam>(
                parent,
                propHeader,
                sampleSelector,
                "matrix",
                paramListBuilder);
        }
        else if ( IBoolGeomParam::matches( propHeader ) )
        {
            AddGeomParamToParamListBuilderAsInt<IBoolGeomParam, bool_t>(
                parent,
                propHeader,
                sampleSelector,
                paramListBuilder);
        }

    }
}
