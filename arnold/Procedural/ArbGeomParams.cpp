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

#include "ArbGeomParams.h"
#include <sstream>

std::string GetArnoldTypeString( GeometryScope scope, int arnoldAPIType)
{
    std::ostringstream buffer;
    
    switch (scope)
    {
    case kUniformScope:
        buffer << "uniform";
        break;
    case kVaryingScope:
    case kVertexScope:
        buffer << "varying";
        break;
    case kFacevaryingScope:
        return ""; //arnold doesn't support facevarying user-data
    case kConstantScope:
    default:
        buffer << "constant";
    }
    
    buffer << " ";
    
    switch ( arnoldAPIType )
    {
        case AI_TYPE_INT:
            buffer << "INT";
            break;
        case AI_TYPE_FLOAT:
            buffer << "FLOAT";
            break;
        case AI_TYPE_STRING:
            buffer << "STRING";
            break;
        case AI_TYPE_RGB:
            buffer << "RGB";
            break;
        case AI_TYPE_RGBA:
            buffer << "RGBA";
            break;
        case AI_TYPE_POINT:
            buffer << "POINT";
            break;
        case AI_TYPE_VECTOR:
            buffer << "VECTOR";
            break;
        case AI_TYPE_POINT2:
            buffer << "POINT2";
            break;
        case AI_TYPE_MATRIX:
            buffer << "MATRIX";
            break;
        default:
            // For now, only support the above types
            return "";
    }
    
    
    return buffer.str();
}

//-*****************************************************************************

template <typename T>
void AddArbitraryGeomParam( ICompoundProperty & parent,
                            const PropertyHeader &propHeader,
                            ISampleSelector &sampleSelector,
                            AtNode * primNode,
                            int arnoldAPIType)
{
    T param( parent, propHeader.getName() );
    
    if ( !param.valid() )
    {
        //TODO error message?
        return;
    }
    
    std::string declStr = GetArnoldTypeString( param.getScope(),
            arnoldAPIType );
    if ( declStr.empty() )
    {
        return;
    }
    
    // TODO For now, don't support user-defined arrays.
    // It's reasonable to support these for kConstantScope
    if ( param.getArrayExtent() > 1 )
    {
        return;
    }
    
    if ( !AiNodeDeclare( primNode, param.getName().c_str(), declStr.c_str() ) )
    {
        //TODO, AiWarning
        return;
    }
    
    if ( param.getScope() == kConstantScope ||
            param.getScope() == kUnknownScope)
    {
        //Set scalars directly based on arnoldAPIType since we're
        //not yet support array types here
        
        typename T::prop_type::sample_ptr_type valueSample =
                param.getExpandedValue( sampleSelector ).getVals();
        
        switch ( arnoldAPIType )
        {
            case AI_TYPE_INT:
                AiNodeSetInt( primNode, param.getName().c_str(),
                        reinterpret_cast<const int32_t *>(
                                valueSample->get() )[0] );
                
                break;
            case AI_TYPE_FLOAT:
                AiNodeSetFlt( primNode, param.getName().c_str(),
                        reinterpret_cast<const float32_t *>(
                                valueSample->get() )[0] );
                break;
            case AI_TYPE_STRING:
                
                AiNodeSetStr( primNode, param.getName().c_str(),
                        reinterpret_cast<const std::string *>(
                                valueSample->get() )[0].c_str() );
                
                break;
            case AI_TYPE_RGB:
            {
                const float32_t * data = 
                        reinterpret_cast<const float32_t *>(
                                valueSample->get() );
                
                AiNodeSetRGB( primNode, param.getName().c_str(),
                        data[0], data[1], data[2]);
                
                break;
            }
            case AI_TYPE_RGBA:
            {
                const float32_t * data = 
                        reinterpret_cast<const float32_t *>(
                                valueSample->get() );
                
                AiNodeSetRGBA( primNode, param.getName().c_str(),
                        data[0], data[1], data[2], data[3]);
                
                break;
            }
            case AI_TYPE_POINT:
            {
                const float32_t * data = 
                        reinterpret_cast<const float32_t *>(
                                valueSample->get() );
                
                AiNodeSetPnt( primNode, param.getName().c_str(),
                        data[0], data[1], data[2]);
                
                break;
            }
            case AI_TYPE_VECTOR:
            {
                const float32_t * data = 
                        reinterpret_cast<const float32_t *>(
                                valueSample->get() );
                
                AiNodeSetVec( primNode, param.getName().c_str(),
                        data[0], data[1], data[2] );
                
                break;
            }
            case AI_TYPE_POINT2:
            {
                const float32_t * data = 
                        reinterpret_cast<const float32_t *>(
                                valueSample->get() );
                
                AiNodeSetPnt2( primNode, param.getName().c_str(),
                        data[0], data[1] );
                break;
            }
            case AI_TYPE_MATRIX:
            {
                const float32_t * data = 
                        reinterpret_cast<const float32_t *>(
                                valueSample->get() );
                
                AtMatrix m;
                for ( size_t i = 0; i < 16; ++i )
                {
                    *((&m[0][0])+i) = data[i];
                }
                AiNodeSetMatrix( primNode, param.getName().c_str(), m);
                
                
                break;
            }
            default:
                // For now, only support the above types
                break;
        }
    }
    else
    {
        // Always set arrays for other scopes
        typename T::prop_type::sample_ptr_type valueSample =
                param.getExpandedValue( sampleSelector ).getVals();
        
        AiNodeSetArray( primNode, param.getName().c_str(),
                ArrayConvert( valueSample->size(), 1, arnoldAPIType,
                        (void *) valueSample->get() ) );
    }
    
    
}


//-*****************************************************************************

void AddArbitraryStringGeomParam( ICompoundProperty & parent,
                            const PropertyHeader &propHeader,
                            ISampleSelector &sampleSelector,
                            AtNode * primNode)
{
    IStringGeomParam param( parent, propHeader.getName() );
    
    if ( !param.valid() )
    {
        //TODO error message?
        return;
    }
    
    std::string declStr = GetArnoldTypeString( param.getScope(),
            AI_TYPE_STRING );
    if ( declStr.empty() )
    {
        return;
    }
    
    
    // TODO, remove this restriction and support arrays for constant values
    if ( param.getArrayExtent() > 1 )
    {
        return;
    }
    
    if ( !AiNodeDeclare( primNode, param.getName().c_str(), declStr.c_str() ) )
    {
        //TODO, AiWarning
        return;
    }
    
    IStringGeomParam::prop_type::sample_ptr_type valueSample =
                param.getExpandedValue( sampleSelector ).getVals();
    
    if ( param.getScope() == kConstantScope ||
            param.getScope() == kUnknownScope)
    {
        AiNodeSetStr( primNode, param.getName().c_str(),
                        reinterpret_cast<const std::string *>(
                                valueSample->get() )[0].c_str() );
    }
    else
    {
        std::vector<const char *> strPtrs;
        strPtrs.reserve( valueSample->size() );
        for ( size_t i = 0; i < valueSample->size(); ++i )
        {
            strPtrs.push_back( valueSample->get()[i].c_str() );
        }
        
        AiNodeSetArray( primNode, param.getName().c_str(),
                ArrayConvert( valueSample->size(), 1, AI_TYPE_STRING,
                        (void *) &strPtrs[0] ) );
        
    
    }
    
}

//-*****************************************************************************

//UserDefDeclare(node, name.c_str(), userType.c_str())) continue;
//SetUserData(node, name.c_str(), dataSize, apiType, dataStart);  


void AddArbitraryGeomParams( ICompoundProperty &parent,
                             ISampleSelector &sampleSelector,
                             AtNode * primNode,
                             const std::set<std::string> * excludeNames
                           )
{
    if ( primNode == NULL || !parent.valid() )
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
            AddArbitraryGeomParam<IFloatGeomParam>(
                    parent,
                    propHeader,
                    sampleSelector,
                    primNode,
                    AI_TYPE_FLOAT);
        }
        else if ( IInt32GeomParam::matches( propHeader ) )
        {
            AddArbitraryGeomParam<IInt32GeomParam>(
                    parent,
                    propHeader,
                    sampleSelector,
                    primNode,
                    AI_TYPE_INT);
        }
        else if ( IStringGeomParam::matches( propHeader ) )
        {
            AddArbitraryStringGeomParam(
                    parent,
                    propHeader,
                    sampleSelector,
                    primNode);
        }
        else if ( IV2fGeomParam::matches( propHeader ) )
        {
            AddArbitraryGeomParam<IV2fGeomParam>(
                    parent,
                    propHeader,
                    sampleSelector,
                    primNode,
                    AI_TYPE_POINT2);
        }
        else if ( IV3fGeomParam::matches( propHeader ) )
        {
            AddArbitraryGeomParam<IV3fGeomParam>(
                    parent,
                    propHeader,
                    sampleSelector,
                    primNode,
                    AI_TYPE_VECTOR);
        }
        else if ( IP3fGeomParam::matches( propHeader ) )
        {
            AddArbitraryGeomParam<IP3fGeomParam>(
                    parent,
                    propHeader,
                    sampleSelector,
                    primNode,
                    AI_TYPE_POINT);
        }
        else if ( IN3fGeomParam::matches( propHeader ) )
        {
            AddArbitraryGeomParam<IN3fGeomParam>(
                    parent,
                    propHeader,
                    sampleSelector,
                    primNode,
                    AI_TYPE_VECTOR);
        }
        else if ( IC3fGeomParam::matches( propHeader ) )
        {
            AddArbitraryGeomParam<IC3fGeomParam>(
                    parent,
                    propHeader,
                    sampleSelector,
                    primNode,
                    AI_TYPE_RGB);
        }
        else if ( IC4fGeomParam::matches( propHeader ) )
        {
            AddArbitraryGeomParam<IC4fGeomParam>(
                    parent,
                    propHeader,
                    sampleSelector,
                    primNode,
                    AI_TYPE_RGBA);
        }
        if ( IM44fGeomParam::matches( propHeader ) )
        {
            AddArbitraryGeomParam<IM44fGeomParam>(
                    parent,
                    propHeader,
                    sampleSelector,
                    primNode,
                    AI_TYPE_MATRIX);
        }
        
        
    }
}

