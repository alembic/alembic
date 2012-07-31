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

#include "WriteMaterial.h"
#ifdef PRMAN_USE_ABCMATERIAL

#include "ArbAttrUtil.h"

void ApplyObjectMaterial(IObject object, ProcArgs &args)
{
    //TODO, check args
    
    Mat::MaterialFlatten mafla(object);
    
    if (!mafla.empty())
    {
        WriteMaterial(mafla, args);
    }
    
}


void WriteMaterial(Mat::MaterialFlatten & materialInput, ProcArgs & args)
{
    if (materialInput.empty())
    {
        return;
    }
    
    std::vector<std::string> shaderTypes;
    materialInput.getShaderTypesForTarget("prman", shaderTypes);
    
    
    Mat::MaterialFlatten::ParameterEntryVector paramEntries;
    
    
    ISampleSelector sampleSelector(args.frame / args.fps);
    
    for (std::vector<std::string>::iterator I = shaderTypes.begin();
            I != shaderTypes.end(); ++I)
    {
        const std::string & shaderType = (*I);
        
        std::string shaderName;
        if (!materialInput.getShader("prman", shaderType, shaderName))
        {
            continue;
        }
        
        if (shaderName.empty())
        {
            continue;
        }
        
        ParamListBuilder plb;
        
        materialInput.getShaderParameters("prman", shaderType, paramEntries);
        
        for (Mat::MaterialFlatten::ParameterEntryVector::iterator I =
                paramEntries.begin(); I != paramEntries.end(); ++I)
        {
            //TODO
            Mat::MaterialFlatten::ParameterEntry & entry = (*I);
            
            //#float, double, point, vector, #color, matrix, normal, #string
            
            if (entry.header->isScalar())
            {
                //no ints in RenderMan shaders, make everything a float
                
                if (Abc::IFloatProperty::matches(*entry.header))
                {
                    AddScalarPropertyAsFloatToParamListBuilder<
                            Abc::IFloatProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "float",
                                    plb);
                }
                else if (Abc::IC3fProperty::matches(*entry.header))
                {
                    AddScalarPropertyAsFloatToParamListBuilder<
                            Abc::IC3fProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "color",
                                    plb);
                }
                else if (Abc::IStringProperty::matches(*entry.header))
                {
                    AddScalarPropertyAsStringToParamListBuilder<
                            Abc::IStringProperty>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    plb);
                }
                else if (Abc::IInt32Property::matches(*entry.header))
                {
                    AddScalarPropertyAsFloatToParamListBuilder<
                            Abc::IInt32Property, Abc::uint32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "float",
                                    plb);
                }
                else if (Abc::IBoolProperty::matches(*entry.header))
                {
                    AddScalarPropertyAsFloatToParamListBuilder<
                            Abc::IBoolProperty, Abc::bool_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "float",
                                    plb);
                }
                else if (Abc::IN3fProperty::matches(*entry.header))
                {
                    AddScalarPropertyAsFloatToParamListBuilder<
                            Abc::IN3fProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "normal",
                                    plb);
                }
                else if (Abc::IM44fProperty::matches(*entry.header))
                {
                    AddScalarPropertyAsFloatToParamListBuilder<
                            Abc::IM44fProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "matrix",
                                    plb);
                }
                else if (Abc::IP3fProperty::matches(*entry.header))
                {
                    AddScalarPropertyAsFloatToParamListBuilder<
                            Abc::IP3fProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "point",
                                    plb);
                }
                else if (Abc::IV3fProperty::matches(*entry.header))
                {
                    AddScalarPropertyAsFloatToParamListBuilder<
                            Abc::IV3fProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "vector",
                                    plb);
                }
                
            }
            else if (entry.header->isArray())
            {
                if (Abc::IFloatArrayProperty::matches(*entry.header))
                {
                    AddArrayPropertyAsFloatToParamListBuilder<
                            Abc::IFloatArrayProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "float",
                                    plb);
                }
                else if (Abc::IC3fArrayProperty::matches(*entry.header))
                {
                    AddArrayPropertyAsFloatToParamListBuilder<
                            Abc::IC3fArrayProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "color",
                                    plb);
                }
                else if (Abc::IP3fArrayProperty::matches(*entry.header))
                {
                    AddArrayPropertyAsFloatToParamListBuilder<
                            Abc::IP3fArrayProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "point",
                                    plb);
                }
                else if (Abc::IStringArrayProperty::matches(*entry.header))
                {
                    AddArrayPropertyAsStringToParamListBuilder<
                            Abc::IStringArrayProperty>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    plb);
                }
                else if (Abc::IV3fArrayProperty::matches(*entry.header))
                {
                    AddArrayPropertyAsFloatToParamListBuilder<
                            Abc::IV3fArrayProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "vector",
                                    plb);
                }
                else if (Abc::IN3fArrayProperty::matches(*entry.header))
                {
                    AddArrayPropertyAsFloatToParamListBuilder<
                            Abc::IN3fArrayProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "normal",
                                    plb);
                }
                else if (Abc::IM44fArrayProperty::matches(*entry.header))
                {
                    AddArrayPropertyAsFloatToParamListBuilder<
                            Abc::IM44fArrayProperty, Abc::float32_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "matrix",
                                    plb);
                }
                else if (Abc::IBoolArrayProperty::matches(*entry.header))
                {
                    AddArrayPropertyAsFloatToParamListBuilder<
                            Abc::IBoolArrayProperty, Abc::bool_t>(
                                    entry.parent,
                                    *entry.header,
                                    sampleSelector,
                                    entry.name,
                                    "float",
                                    plb);
                }
                
                
            }
            else
            {
                //skip
            }
        }
        
        if (shaderType == "surface")
        {
            RiSurfaceV(const_cast<RtToken>(shaderName.c_str()),
                    plb.n(), plb.nms(), plb.vals());
        }
        else if (shaderType == "displacement")
        {
            RiDisplacementV(const_cast<RtToken>(shaderName.c_str()),
                    plb.n(), plb.nms(), plb.vals());
        
        }
        else if (shaderType == "interior")
        {
            RiInteriorV(const_cast<RtToken>(shaderName.c_str()),
                    plb.n(), plb.nms(), plb.vals());
        }
        else if (shaderType == "exterior")
        {
            RiExteriorV(const_cast<RtToken>(shaderName.c_str()),
                    plb.n(), plb.nms(), plb.vals());
        }
        else
        {
            std::string coshaderName = shaderType; 
            
            if (shaderType.size() > 9 && shaderType.substr(0, 9) == "coshader_")
            {
                coshaderName = shaderType.substr(9);
            }
            
            RiShader(const_cast<RtToken>(shaderName.c_str()),
                    const_cast<RtToken>(coshaderName.c_str()),
                            plb.n(), plb.nms(), plb.vals());
        }
        
    }
    
    //surface
    //displacement
    //interior
    //exterior
    //coshaders
    
    
    
    
    
    
    
    
    
    
}









#endif
