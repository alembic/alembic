//-*****************************************************************************
//
// Copyright (c) 2012,
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

#include <Foundation.h>
#include <PyISchema.h>
#include <PyISchemaObject.h>
#include <PyTypeBindingUtil.h>

using namespace boost::python;

//-*****************************************************************************
// IMaterialSchema Overlaods
//-*****************************************************************************
struct SchemaOverloads
{
    typedef AbcM::IMaterialSchema IMaterialSchema;
    typedef AbcM::IMaterialSchema::NetworkNode NetworkNode;

    static object getTargetNames( IMaterialSchema& iSchema )
    {
        std::vector<std::string> targetNames;
        iSchema.getTargetNames( targetNames );

        return ConvertStrings( targetNames );
    }

    static object getShaderTypesForTarget( IMaterialSchema& iSchema,
                                           const std::string& targetName )
    {
        std::vector<std::string> shaderTypeNames;
        iSchema.getShaderTypesForTarget( targetName, shaderTypeNames );

        return ConvertStrings( shaderTypeNames );
    }

    static std::string getShader( IMaterialSchema& iSchema,
                                  const std::string& target,
                                  const std::string& shaderType )
    {
        std::string result;
        iSchema.getShader( target, shaderType, result );

        return result;
    }

    static object getNetworkNodeNames( IMaterialSchema& iSchema )
    {
        std::vector<std::string> names;
        iSchema.getNetworkNodeNames( names );

        return ConvertStrings( names );
    }

    static NetworkNode getNetworkNodeByIndex( IMaterialSchema& iSchema,
                                              size_t iIndex )
    {
        return iSchema.getNetworkNode( iIndex );
    }

    static NetworkNode getNetworkNodeByName( IMaterialSchema& iSchema,
                                             const std::string& iName )
    {
        return iSchema.getNetworkNode( iName );
    }

    static object getNetworkTerminalTargetNames( IMaterialSchema& iSchema )
    {
        std::vector<std::string> targetNames;
        iSchema.getNetworkTerminalTargetNames( targetNames );

        return ConvertStrings( targetNames );
    }

    static object getNetworkTerminalShaderTypesForTarget(
                                            IMaterialSchema& iSchema,
                                            const std::string& iTargetName )
    {
        std::vector<std::string> shaderTypeNames;
        iSchema.getNetworkTerminalShaderTypesForTarget( iTargetName,
                                                        shaderTypeNames );

        return ConvertStrings( shaderTypeNames );
    }

    static dict getNetworkTerminal( IMaterialSchema& iSchema,
                                      const std::string& iTarget,
                                      const std::string& iShaderType )
    {
        std::string nodeName, outputName;

        iSchema.getNetworkTerminal( iTarget, iShaderType, nodeName,
                                    outputName );

        dict ret;
        ret["nodeName"] = nodeName;
        ret["outputName"] = outputName;

        return ret;
    }

    static dict getNetworkInterfaceParameterMappingByIndex(
                                                IMaterialSchema& iSchema,
                                                size_t index )
    {
        std::string interfaceParamName, mapToNodeName, mapToParamName;

        iSchema.getNetworkInterfaceParameterMapping( index,
                                                     interfaceParamName,
                                                     mapToNodeName,
                                                     mapToParamName );
        dict ret;
        ret["interfaceParamName"] = interfaceParamName;
        ret["mapToNodeName"] = mapToNodeName;
        ret["mapToParamName"] = mapToParamName;

        return ret;
    }

    static dict getNetworkInterfaceParameterMappingByName(
                                        IMaterialSchema& iSchema,
                                        const std::string& iInterfaceParamName )
    {
        std::string mapToNodeName, mapToParamName;

        iSchema.getNetworkInterfaceParameterMapping( iInterfaceParamName,
                                                     mapToNodeName,
                                                     mapToParamName );
        dict ret;
        ret["mapToNodeName"] = mapToNodeName;
        ret["mapToParamName"] = mapToParamName;

        return ret;
    }

    static object getNetworkInterfaceParameterMappingNames(
                                        IMaterialSchema& iSchema )
    {
        std::vector<std::string> names;
        iSchema.getNetworkInterfaceParameterMappingNames( names );

        return ConvertStrings( names );
    }
};

//-*****************************************************************************
// IMaterialSchema::NetworkNode Overlaods
//-*****************************************************************************
struct NetworkNodeOverloads
{
    typedef AbcM::IMaterialSchema::NetworkNode NetworkNode;

    static std::string getTarget( NetworkNode& iNode )
    {
        std::string result;
        iNode.getTarget( result );
        
        return result;
    }

    static std::string getNodeType( NetworkNode& iNode )
    {
        std::string result;
        iNode.getNodeType( result );

        return result;
    }

    static dict getConnectionByIndex( NetworkNode& iNode, size_t iIndex )
    {
        std::string inputName, connectedNodeName, connectedOutputName;

        iNode.getConnection( iIndex, inputName, connectedNodeName,
                             connectedOutputName );

        dict ret;
        ret["inputName"] = inputName;
        ret["connectedNodeName"] = connectedNodeName;
        ret["connectedOuputName"] = connectedOutputName;

        return ret;
    }

    static dict getConnectionByName( NetworkNode& iNode,
                                     const std::string& iInputName )
    {
        std::string connectedNodeName, connectedOutputName;

        iNode.getConnection( iInputName, connectedNodeName,
                             connectedOutputName );

        dict ret;
        ret["connectedNodeName"] = connectedNodeName;
        ret["connectedOuputName"] = connectedOutputName;

        return ret;
    }

};

//-*****************************************************************************
void register_imaterial()
{
    // IMaterial
    //
    register_ISchemaObject<AbcM::IMaterial>( "IMaterial" );

    // ISchema base class
    //
    register_ISchema<AbcM::MaterialSchemaInfo>( "ISchema_Material" );

    // IMaterialSchema
    //
    class_<AbcM::IMaterialSchema,
           bases<Abc::ISchema<AbcM::MaterialSchemaInfo> > >(
           "IMaterialSchema",
           "The IMaterialSchema class is a xform schema reader",
           init<>() )
        .def( init<Abc::ICompoundProperty,
                   const std::string&,
                   optional<
                   const Abc::Argument&,
                   const Abc::Argument&> >(
                   ( arg( "parent" ), arg( "name" ), 
                     arg( "argument" ), arg( "argument" ) ),
                   "doc") )
        .def( init<Abc::ICompoundProperty,
                   optional<
                   const Abc::Argument&,
                   const Abc::Argument&> >(
                   ( arg( "parent" ), arg( "argument" ), arg( "argument" ) ),
                   "doc") )
        .def( "getTargetNames", 
              &SchemaOverloads::getTargetNames )
        .def( "getShaderTypesForTarget",
              &SchemaOverloads::getShaderTypesForTarget )
        .def( "getShader",
              &SchemaOverloads::getShader )
        .def( "getShaderParameters",
              &AbcM::IMaterialSchema::getShaderParameters )
        .def( "getNumNetworkNodes",
              &AbcM::IMaterialSchema::getNumNetworkNodes )
        .def( "getNetworkNodeNames",
              &SchemaOverloads::getNetworkNodeNames )
        .def( "getNetworkNode",
              &SchemaOverloads::getNetworkNodeByName )
        .def( "getNetworkNode",
              &SchemaOverloads::getNetworkNodeByIndex )
        .def( "getNetworkTerminal",
              &SchemaOverloads::getNetworkTerminal )
        .def( "getNetworkTerminalTargetNames",
              &SchemaOverloads::getNetworkTerminalTargetNames )
        .def( "getNetworkTerminalShaderTypesForTarget",
              &SchemaOverloads::getNetworkTerminalShaderTypesForTarget )
        .def( "getNumNetworkInterfaceParameterMappings",
              &AbcM::IMaterialSchema::getNumNetworkInterfaceParameterMappings )
        .def( "getNetworkInterfaceParameterMapping",
              &SchemaOverloads::getNetworkInterfaceParameterMappingByIndex )
        .def( "getNetworkInterfaceParameterMapping",
              &SchemaOverloads::getNetworkInterfaceParameterMappingByName )
        .def( "getNetworkInterfaceParameterMappingNames",
              &SchemaOverloads::getNetworkInterfaceParameterMappingNames )
        .def( "getNetworkInterfaceParameters",
              &AbcM::IMaterialSchema::getNetworkInterfaceParameters )
        ;

    // IMaterial::NetworkNode
    //
    typedef AbcM::IMaterialSchema::NetworkNode NetworkNode;

    class_<NetworkNode>(
           "IMaterialSchemaNetworkNode",
           "",
           init<>() )
    .def( init<Abc::ICompoundProperty>() )
    .def( init<Abc::ICompoundProperty,const std::string&>() )
    .def( "valid",
          &NetworkNode::valid )
    .def( "getName",
          &NetworkNode::getName )
    .def( "getTarget",
          &NetworkNodeOverloads::getTarget )
    .def( "getNodeType",
          &NetworkNodeOverloads::getNodeType )
    .def( "getParameters",
          &NetworkNode::getParameters )
    .def( "getNumConnections",
          &NetworkNode::getNumConnections )
    .def( "getConnection",
          &NetworkNodeOverloads::getConnectionByIndex )
    .def( "getConnection",
          &NetworkNodeOverloads::getConnectionByName )
    ;
}
