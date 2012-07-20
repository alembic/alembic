#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include <Alembic/AbcMaterial/OMaterial.h>
#include <Alembic/AbcMaterial/IMaterial.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

#include "PrintMaterial.h"

namespace Abc =  Alembic::Abc;
namespace Mat = Alembic::AbcMaterial;


void write()
{
    Abc::OArchive archive(
            Alembic::AbcCoreHDF5::WriteArchive(), "MaterialNetworkNodes.abc" );
    
    Abc::OObject root(archive, Abc::kTop);
    
    //make a dummy enclosing object
    Abc::OObject materials(root, "materials");
    
    Mat::OMaterial matObj(materials, "material1");
    
    matObj.getSchema().addNetworkNode("mainshader", "abc", "blinn");
    matObj.getSchema().addNetworkNode("colormap", "abc", "texture_read");
    
    matObj.getSchema().setNetworkNodeConnection("mainshader", "Cs",
            "colormap", "color_out");
    
    {
        Abc::OFloatProperty prop(
                matObj.getSchema().getNetworkNodeParameters("mainshader"),
                        "Kd");
        prop.set(0.5);
    }
    
    
    
    {
        Abc::OStringProperty prop(
                matObj.getSchema().getNetworkNodeParameters("colormap"),
                        "map_name");
        prop.set("/tmp/original.tx");
    }
    
    matObj.getSchema().setNetworkTerminal(
            "abc",
            "surface",
            "mainshader",
            "out");
    
    matObj.getSchema().setNetworkInterfaceParameterMapping(
            "ColorMapName", "colormap", "map_name");
    
    {
        
        Abc::OStringProperty prop(
                matObj.getSchema().getNetworkInterfaceParameters(),
                        "ColorMapName");
        prop.set("/from/public.tx");
    }
    
    



}

void read()
{
    Abc::IArchive archive(Alembic::AbcCoreHDF5::ReadArchive(), "MaterialNetworkNodes.abc");
    
    Abc::IObject materialsObject(archive.getTop(), "materials");
    Mat::IMaterial matObj(materialsObject, "material1");
    
    
    std::cout << "----" << std::endl;
    std::cout << "NODES" << std::endl;
    
    std::cout << matObj.getSchema().getNumNetworkNodes() << std::endl;
    TESTING_ASSERT(matObj.getSchema().getNumNetworkNodes() == 2);
    for (size_t i = 0, e = matObj.getSchema().getNumNetworkNodes(); i < e; ++i)
    {
        Mat::IMaterialSchema::NetworkNode node = matObj.getSchema().getNetworkNode(i);
        
        TESTING_ASSERT(node.valid());

        std::cout << "  ----" << std::endl;
        
        std::string target = "<undefined>";
        node.getTarget(target);
        TESTING_ASSERT(target == "abc");
        std::string nodeType = "<undefined>";
        node.getNodeType(nodeType);
        
        std::cout << "  NODE: " << node.getName() << ", TARGET: " <<
                target << ", TYPE: " << nodeType << std::endl;
        TESTING_ASSERT((nodeType == "blinn" && node.getName() == "mainshader")||
            (nodeType == "texture_read" && node.getName() == "colormap"));
        Abc::ICompoundProperty parameters = node.getParameters();
        TESTING_ASSERT(parameters.valid());
        TESTING_ASSERT(parameters.getNumProperties() == 1);
        if (parameters.valid())
        {
            std::cout << "    PARAMETERS:" << std::endl;

            TESTING_ASSERT( (node.getName() == "mainshader" &&
                parameters.getPropertyHeader(0).getName() == "Kd") ||
                (node.getName() == "colormap" &&
                parameters.getPropertyHeader(0).getName() == "map_name") );

            for (size_t i = 0, e = parameters.getNumProperties(); i < e; ++i)
            {
                const Abc::PropertyHeader & header =
                        parameters.getPropertyHeader(i);
                
                std::cout << "      " << header.getName() << std::endl;
            }
        }
        
        size_t numConnections = node.getNumConnections();
        TESTING_ASSERT(
            (node.getName() == "mainshader" && numConnections == 1) ||
            (node.getName() == "colormap" && numConnections == 0) );

        if (numConnections)
        {
            std::string inputName, connectedNodeName, connectedOutputName;
            
            std::cout << "    CONNNECTIONS:" << std::endl;
            
            for (size_t i = 0; i < numConnections; ++i)
            {
                if (node.getConnection(i,
                        inputName, connectedNodeName, connectedOutputName))
                {
                    TESTING_ASSERT( inputName == "Cs" &&
                        connectedNodeName == "colormap" &&
                        connectedOutputName == "color_out" );
                    std::cout << "      " << inputName << " -> NODE: " << connectedNodeName;
                    
                    if (!connectedOutputName.empty())
                    {
                        std::cout << ", PORT: " << connectedOutputName;
                    }
                    std::cout << std::endl;
                
                }
            }
        }
    }
    
    std::cout << "TERMINALS" << std::endl;
    
    std::vector<std::string> targetNames;
    matObj.getSchema().getNetworkTerminalTargetNames(targetNames);
    TESTING_ASSERT(targetNames.size() == 1);
    for (std::vector<std::string>::iterator I = targetNames.begin();
            I != targetNames.end(); ++I)
    {
        const std::string & targetName = (*I);
        
        std::cout << "  TARGET: " << targetName << std::endl;
        
        
        std::vector<std::string> shaderTypeNames;
        
        matObj.getSchema().getNetworkTerminalShaderTypesForTarget(
                targetName, shaderTypeNames);
        
        for (std::vector<std::string>::iterator I = shaderTypeNames.begin();
                I != shaderTypeNames.end(); ++I)
        {
            const std::string & shaderType = (*I);
            std::cout << "    SHADERTYPE: " << shaderType;
            
            std::string connectedNodeName = "<undefined>";
            std::string connectedOutputName = "<undefined>";
            
            if (matObj.getSchema().getNetworkTerminal(
                    targetName, shaderType, connectedNodeName, connectedOutputName))
            {
                TESTING_ASSERT(targetName == "abc" &&
                    shaderType == "surface" &&
                    connectedNodeName == "mainshader" &&
                    connectedOutputName == "out");

                std::cout << ", NODE: " << connectedNodeName;
                
                if (!connectedOutputName.empty())
                {
                    std::cout << ", PORT: " << connectedOutputName;
                }
            }
            
            
            std::cout << std::endl;
            
        }
        
        
        
    }
    
    std::vector<std::string> mappingNames;
    matObj.getSchema().getNetworkInterfaceParameterMappingNames(mappingNames);
    
    std::cout << "INTERFACE MAPPINGS" << std::endl;
    TESTING_ASSERT(mappingNames.size() == 1);
    for (std::vector<std::string>::iterator I = mappingNames.begin();
            I != mappingNames.end(); ++I)
    {
        std::string mapToNodeName;
        std::string mapToParamName;
        if (matObj.getSchema().getNetworkInterfaceParameterMapping(
                (*I), mapToNodeName, mapToParamName))
        {
            TESTING_ASSERT(mapToNodeName == "colormap" &&
                mapToParamName == "map_name");

            std::cout << "  PARAM NAME: " << (*I) << ", MAPTONODE: " <<
                    mapToNodeName << ", MAPTOPARAMNAME: " << mapToParamName;
            std::cout << std::endl;
        }
    }
    
    
    std::cerr << "\n\n\nFROM FLATTENED MATERIAL" << std::endl;
    
    Mat::MaterialFlatten mafla(matObj);
    
    printFlattenedMafla(mafla);
    
    
}


int main( int argc, char *argv[] )
{
    write();
    read();
    
   
    
    return 0;
}

