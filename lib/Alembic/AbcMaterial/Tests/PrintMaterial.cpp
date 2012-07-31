#include "PrintMaterial.h"
#include <set>
#include <sstream>

namespace Abc =  Alembic::Abc;
namespace Mat = Alembic::AbcMaterial;


std::string getPropertyString(
        Abc::ICompoundProperty & parent, const Abc::PropertyHeader &propHeader)
{
    std::ostringstream buffer;
    
    //NOTE: only supporting expected types here
    //real code would need to support more types
    //and animation, etc
    
    if (propHeader.isScalar())
    {
        if (Abc::IStringProperty::matches(propHeader))
        {
            Abc::IStringProperty prop(parent,
                    propHeader.getName());
            buffer << prop.getValue();
            
        }
        else if (Abc::IFloatProperty::matches(propHeader))
        {
            Abc::IFloatProperty prop(parent,
                    propHeader.getName());
            
            buffer << prop.getValue();
            
            
        }
    }
    
    return buffer.str();

}


void walkFromNode(Mat::MaterialFlatten & mafla,
        const std::string & nodeName,
        std::set<std::string> & alreadyWalked)
{
    if (alreadyWalked.find(nodeName) != alreadyWalked.end())
    {
        return;
    }
    alreadyWalked.insert(nodeName);
    
    Mat::MaterialFlatten::NetworkNode node = mafla.getNetworkNode(nodeName);
    if (!node.valid())
    {
        return;
    }
    
    std::string target("<undefined>"), nodeType("<undefined>");
    
    node.getTarget(target);
    node.getNodeType(nodeType);
    
    std::cout << "      NODE: " << node.getName() << std::endl;
    std::cout << "        target: " << target << std::endl;
    std::cout << "        nodeType: " << nodeType << std::endl;
    std::cout << "        parameters:" << std::endl;
    
    Mat::MaterialFlatten::ParameterEntryVector parameters;
    node.getParameters(parameters);
    
    for (Mat::MaterialFlatten::ParameterEntryVector::iterator I =
            parameters.begin(); I != parameters.end(); ++I)
    {
        std::cout << "           name: " << (*I).name << " ";
        std::cout << getPropertyString((*I).parent, *(*I).header) << std::endl;
    }
    
    std::vector<std::string> connectedNodeNames;
    
    Mat::MaterialFlatten::NetworkNode::ConnectionVector connections;
    node.getConnections(connections);
    
    std::cout << "        connections:" << std::endl;
    
    for (Mat::MaterialFlatten::NetworkNode::ConnectionVector::iterator I =
            connections.begin(); I != connections.end(); ++I)
    {
        std::cout << "           input: " << (*I).inputName;
        std::cout << ", node: " << (*I).connectedNodeName << ", output: ";
        std::cout <<  (*I).connectedOutputName << std::endl;
        
        connectedNodeNames.push_back((*I).connectedNodeName);
    }
    
    for (std::vector<std::string>::iterator I = connectedNodeNames.begin();
            I != connectedNodeNames.end(); ++I)
    {
    
        walkFromNode(mafla, (*I), alreadyWalked);
    }
    
}


void printFlattenedMafla(Mat::MaterialFlatten & mafla)
{
    std::vector<std::string> targetNames;
    
    mafla.getTargetNames(targetNames);
    
    if (!targetNames.empty())
    {
        std::cout << "monolithic targets: " << std::endl;
        for (std::vector<std::string>::iterator I = targetNames.begin();
                I != targetNames.end(); ++I)
        {
            const std::string & targetName = (*I);
            
            std::cout << "  " << targetName << std::endl;
            
            std::vector<std::string> shaderTypeNames;
            
            mafla.getShaderTypesForTarget(
                    targetName, shaderTypeNames);
            
            for (std::vector<std::string>::iterator I =
                    shaderTypeNames.begin(); I != shaderTypeNames.end(); ++I)
            {
                const std::string & shaderTypeName = (*I);
                
                std::string shaderName("<undefined>");
                
                mafla.getShader(targetName, shaderTypeName,
                        shaderName);
                
                std::cout << "    " << shaderTypeName << ": ";
                std::cout << shaderName << std::endl;
                
                Mat::MaterialFlatten::ParameterEntryVector paramEntries;
                
                mafla.getShaderParameters(targetName, shaderTypeName,
                         paramEntries);
                
                for (Mat::MaterialFlatten::ParameterEntryVector::iterator I =
                        paramEntries.begin(); I != paramEntries.end(); ++I)
                {
                    Mat::MaterialFlatten::ParameterEntry & propEntry = (*I);
                    
                    const Abc::PropertyHeader &propHeader =
                            *(propEntry.header);
                    
                    Abc::ICompoundProperty & params = propEntry.parent;
                    
                    std::cout << "      " << propEntry.name;
                    std::cout << std::endl;
                    
                    std::cout << "        " << getPropertyString(params, propHeader) << std::endl;
                    
                }
            }
        }
    }
    
    mafla.getNetworkTerminalTargetNames(targetNames);
    
    if (!targetNames.empty())
    {
        std::cout << "network terminals: " << std::endl;
        for (std::vector<std::string>::iterator I = targetNames.begin();
                I != targetNames.end(); ++I)
        {
            const std::string & targetName = (*I);
            
            std::cout << "  " << targetName << std::endl;
            
            std::vector<std::string> shaderTypeNames;
            
            mafla.getNetworkTerminalShaderTypesForTarget(
                    targetName, shaderTypeNames);
            
            for (std::vector<std::string>::iterator I =
                    shaderTypeNames.begin(); I != shaderTypeNames.end(); ++I)
            {
                const std::string & shaderTypeName = (*I);
                
                std::string nodeName("<undefined>");
                std::string outputName("<undefined>");
                
                mafla.getNetworkTerminal(targetName, shaderTypeName,
                        nodeName, outputName);
                
                std::cout << "    " << shaderTypeName << ": ";
                std::cout << "NODE: " << nodeName << ", OUTPUT: " <<  outputName << std::endl;
                
                std::set<std::string> alreadyWalkedNodes;
                
                
                walkFromNode(mafla, nodeName, alreadyWalkedNodes);
                
                
            }
            
        }
        
        
        
        
        
        
        
        
        
    }
    
    
    
    
    
    
}


void printFlattenedMaterial(Mat::IMaterial & material)
{
    std::cout << "---------------------------\n";
    std::cout << material.getFullName() << std::endl;
    std::cout << "---------------------------\n";
    
    Mat::MaterialFlatten mafla(material);
    
    printFlattenedMafla(mafla);
}


void printMaterialSchema(Mat::IMaterialSchema & schema)
{
    Mat::MaterialFlatten mafla(schema);
    
    printFlattenedMafla(mafla);
}


    
