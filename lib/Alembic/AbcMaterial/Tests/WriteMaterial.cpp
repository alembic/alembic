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

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include <Alembic/AbcMaterial/OMaterial.h>
#include <Alembic/AbcMaterial/IMaterial.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

namespace Abc =  Alembic::Abc;
namespace Mat = Alembic::AbcMaterial;


void write()
{
    Abc::OArchive archive(
            Alembic::AbcCoreHDF5::WriteArchive(), "Material.abc" );
    
    Abc::OObject root(archive, Abc::kTop);
    
    //make a dummy enclosing object
    Abc::OObject materials(root, "materials");
    
    Mat::OMaterial matObj(materials, "material1");
    
    matObj.getSchema().setShader("prman", "surface", "paintedplastic");
    matObj.getSchema().setShader("prman", "displacement", "knobby");
    
    matObj.getSchema().setShader("arnold", "surface", "paintedplastic");
    
    
    {
        Abc::OFloatProperty prop(
                matObj.getSchema().getShaderParameters("prman", "surface"),
                        "Kd");
        prop.set(0.5);
    }
    
    {
        Abc::OStringProperty prop(
                matObj.getSchema().getShaderParameters("prman", "surface"),
                        "texname");
        prop.set("taco");
    }
}

void read()
{
    Abc::IArchive archive(Alembic::AbcCoreHDF5::ReadArchive(), "Material.abc");
    
    
    Abc::IObject materialsObject(archive.getTop(), "materials");
    
    
    Mat::IMaterial matObj(materialsObject, "material1");
    
    std::vector<std::string> targetNames;
    std::vector<std::string> shaderTypeNames;
    
    matObj.getSchema().getTargetNames(targetNames);
    TESTING_ASSERT( targetNames.size() == 2 );
    TESTING_ASSERT((targetNames[0] == "arnold" && targetNames[1] == "prman")
                || (targetNames[1] == "arnold" && targetNames[0] == "prman"));

    std::cout << "targets: " << std::endl;
    for (std::vector<std::string>::iterator I = targetNames.begin();
            I != targetNames.end(); ++I)
    {
        const std::string & targetName = (*I);
        
        std::cout << "  " << targetName << std::endl;
        
        matObj.getSchema().getShaderTypesForTarget(
                targetName, shaderTypeNames);

        if (targetName == "prman")
        {
            TESTING_ASSERT(shaderTypeNames.size() == 2 &&
                ((shaderTypeNames[0] == "surface" && 
                  shaderTypeNames[1] == "displacement") ||
                 (shaderTypeNames[1] == "surface" && 
                  shaderTypeNames[0] == "displacement")));
        }
        else
        {
            TESTING_ASSERT(targetName == "arnold" && 
                shaderTypeNames.size() == 1 && shaderTypeNames[0] == "surface");
        }

        for (std::vector<std::string>::iterator I = shaderTypeNames.begin();
                I != shaderTypeNames.end(); ++I)
        {
            const std::string & shaderTypeName = (*I);
            
            std::string shaderName("<undefined>");
            
            matObj.getSchema().getShader(targetName, shaderTypeName,
                    shaderName);
            
            if (targetName == "prman" && shaderTypeName == "surface")
            {
                TESTING_ASSERT(shaderName == "paintedplastic");
            }
            else if (targetName == "prman" && shaderTypeName == "displacement")
            {
                TESTING_ASSERT(shaderName == "knobby");
            }
            else if (targetName == "arnold" && shaderTypeName == "surface")
            {
                TESTING_ASSERT(shaderName == "paintedplastic");
            }

            std::cout << "    " << shaderTypeName << ": ";
            std::cout << shaderName << std::endl;
            
            Abc::ICompoundProperty params =
                    matObj.getSchema().getShaderParameters(
                            targetName, shaderTypeName);
            
            if (params.valid())
            {
                TESTING_ASSERT(targetName == "prman" &&
                    shaderTypeName == "surface");

                for (size_t i = 0; i < params.getNumProperties(); ++i)
                {
                    const Abc::PropertyHeader &propHeader =
                            params.getPropertyHeader(i);
                    
                    std::cout << "      " << propHeader.getName();
                    std::cout << std::endl;
                    //NOTE: only supporting expected types here
                    //real code would need to support more types
                    //and animation, etc
                    if (propHeader.isScalar())
                    {
                        if (Abc::IStringProperty::matches(propHeader))
                        {
                            Abc::IStringProperty prop(params,
                                    propHeader.getName());
                            std::cout << "        " << prop.getValue();
                            std::cout << std::endl;
                            TESTING_ASSERT(propHeader.getName() == "texname" &&
                                prop.getValue() == "taco");
                        }
                        
                        else if (Abc::IFloatProperty::matches(propHeader))
                        {
                            Abc::IFloatProperty prop(params,
                                    propHeader.getName());
                            
                            std::cout << "        " << prop.getValue();
                            std::cout << std::endl;
                            TESTING_ASSERT(propHeader.getName() == "Kd" &&
                                prop.getValue() == 0.5);
                        }
                    }
                }
            }
        }
    }
}


int main( int argc, char *argv[] )
{
    write();
    read();
    
    
    return 0;
}
