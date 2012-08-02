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

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

using namespace boost::python;

//-*****************************************************************************
// MaterialFlatten Overlaods
//-*****************************************************************************
struct Overloads
{
    typedef AbcM::IMaterial IMaterial;
    typedef AbcM::IMaterialSchema IMaterialSchema;
    typedef AbcM::MaterialFlatten MaterialFlatten;
    typedef AbcM::MaterialFlatten::NetworkNode NetworkNode;

    typedef MaterialFlatten::ParameterEntryVector ParameterEntryVector;

    static void appendMaterialSchema( MaterialFlatten& iMatfl,
                                      IMaterialSchema materialSchema )
    {
        iMatfl.append( materialSchema );
    }

    static void appendMaterial( MaterialFlatten& iMatfl, IMaterial material )
    {
        iMatfl.append( material );
    }

    static object getTargetNames( MaterialFlatten& iMatfl )
    {
        std::vector<std::string> targetNames;
        iMatfl.getTargetNames( targetNames );

        return ConvertStrings( targetNames );
    }

    static object getShaderTypesForTarget( MaterialFlatten& iMatfl,
                                           const std::string& targetName )
    {
        std::vector<std::string> shaderTypeNames;
        iMatfl.getShaderTypesForTarget( targetName, shaderTypeNames );

        return ConvertStrings( shaderTypeNames );
    }

    static std::string getShader( MaterialFlatten& iMatfl,
                                  const std::string& target,
                                  const std::string& shaderType )
    {
        std::string result;
        iMatfl.getShader( target, shaderType, result );

        return result;
    }

    static ParameterEntryVector getShaderParameters(
                                    MaterialFlatten& iMatfl,
                                    const std::string& iTarget,
                                    const std::string& iShaderType )
    {
        ParameterEntryVector result;
        iMatfl.getShaderParameters( iTarget, iShaderType, result );

        return result;
    }

    static object getNetworkTerminalTargetNames( MaterialFlatten& iMatfl )
    {
        std::vector<std::string> targetNames;
        iMatfl.getNetworkTerminalTargetNames( targetNames );

        return ConvertStrings( targetNames );
    }

    static object getNetworkTerminalShaderTypesForTarget(
                                            MaterialFlatten& iMatfl,
                                            const std::string& iTargetName )
    {
        std::vector<std::string> shaderTypeNames;
        iMatfl.getNetworkTerminalShaderTypesForTarget( iTargetName,
                                                       shaderTypeNames );

        return ConvertStrings( shaderTypeNames );
    }

    static dict getNetworkTerminal( MaterialFlatten& iMatfl,
                                      const std::string& iTarget,
                                      const std::string& iShaderType )
    {
        std::string nodeName, outputName;

        iMatfl.getNetworkTerminal( iTarget, iShaderType, nodeName,
                                    outputName );

        dict result;
        result["nodeName"] = nodeName;
        result["outputName"] = outputName;

        return result;
    }

    static NetworkNode getNetworkNodeByIndex( MaterialFlatten& iMatfl,
                                              size_t iIndex )
    {
        return iMatfl.getNetworkNode( iIndex );
    }

    static NetworkNode getNetworkNodeByName( MaterialFlatten& iMatfl,
                                             const std::string& iName )
    {
        return iMatfl.getNetworkNode( iName );
    }
};

//-*****************************************************************************
// MaterialFlatten::NetworkNode Overlaods
//-*****************************************************************************
struct NetworkNodeOverloads
{
    typedef AbcM::MaterialFlatten::NetworkNode NetworkNode;
    typedef AbcM::MaterialFlatten::ParameterEntryVector ParameterEntryVector;
    typedef NetworkNode::ConnectionVector ConnectionVector;

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

    static ParameterEntryVector getParameters( NetworkNode& iNode )
    {
        ParameterEntryVector result;
        iNode.getParameters( result );

        return result;
    }

    static ConnectionVector getConnections( NetworkNode& iNode )
    {
        ConnectionVector result;
        iNode.getConnections( result );

        return result;
    }
};

//-*****************************************************************************
void register_materialflatten()
{
    // MaterialFlatten
    //
    class_<AbcM::MaterialFlatten>(
        "MaterialFlatten",
        "The MaterialFlatten class is a utility class for IMaterial",
        init<>() )
        .def( init<AbcM::IMaterialSchema>() )
        .def( init<AbcM::IMaterial>() )
        .def( init<Abc::IObject,
                   Abc::IArchive>(
                   ( arg( "object" ),
                     arg( "alternateSearchArchive" ) =Abc::IArchive() ) ) )
        .def( "append",
              &Overloads::appendMaterialSchema )
        .def( "append",
              &Overloads::appendMaterial )
        .def( "empty", 
              &AbcM::MaterialFlatten::empty )
        .def( "getTargetNames",
              &Overloads::getTargetNames )
        .def( "getShaderTypesForTarget",
              &Overloads::getShaderTypesForTarget )
        .def( "getShader",
              &Overloads::getShader )
        .def( "getShaderParameters",
              &Overloads::getShaderParameters )
        .def( "getNetworkTerminalTargetNames",
              &Overloads::getNetworkTerminalTargetNames )
        .def( "getNetworkTerminalShaderTypesForTarget",
              &Overloads::getNetworkTerminalShaderTypesForTarget )
        .def( "getNetworkTerminal",
              &Overloads::getNetworkTerminal )
        .def( "getNumNetworkNodes",
              &AbcM::MaterialFlatten::getNumNetworkNodes )
        .def( "getNetworkNode",
              &Overloads::getNetworkNodeByIndex )
        .def( "getNetworkNode",
              &Overloads::getNetworkNodeByName )
        ;

    // MaterialFlatten::ParameterEntry
    //
    typedef AbcM::MaterialFlatten::ParameterEntry ParameterEntry;
    typedef AbcM::MaterialFlatten::ParameterEntryVector ParameterEntryVector;

    class_<ParameterEntry>(
            "MaterialFlattenParameterEntry"
            "",
            init<>() )
    .def( init<const std::string&,
               Abc::ICompoundProperty,
               AbcA::PropertyHeader*>() )
    .def_readwrite( "name", &ParameterEntry::name )
    .def_readwrite( "parent", &ParameterEntry::parent )
    .def_readwrite( "header", &ParameterEntry::header )
    .def( self == self )
    ;

    // Vector suite for ParameterEntry
    //
    class_<ParameterEntryVector>(
        "ParameterEntryVector",
        "" )
        .def( vector_indexing_suite<ParameterEntryVector>() );

    // MaterialFlatten::NetworkNode
    //
    typedef AbcM::MaterialFlatten::NetworkNode NetworkNode;

    class_<NetworkNode>(
        "MaterialFlattenNetworkNode",
        "",
        init<>() )
        .def( "valid",
              &NetworkNode::valid )
        .def( "getName",
              &NetworkNode::getName )
        .def( "getTarget",
              &NetworkNodeOverloads::getTarget )
        .def( "getNodeType",
              &NetworkNodeOverloads::getNodeType )
        .def( "getParameters",
              &NetworkNodeOverloads::getParameters )
        .def( "getConnections",
              &NetworkNodeOverloads::getConnections )
        ;

    // MaterialFlatten::NetworkNode::Connection
    //
    typedef NetworkNode::Connection Connection;
    typedef NetworkNode::ConnectionVector ConnectionVector;

    class_<Connection>(
        "MaterialFlattenNetworkNodeConnection",
        "",
        init<const std::string&,
             const std::string&,
             const std::string&>() )
        .def_readwrite("inputName", &Connection::inputName ) 
        .def_readwrite("connectedNodeName", &Connection::connectedNodeName ) 
        .def_readwrite("connectedOutputName", &Connection::connectedOutputName )
        .def( self == self )
        ;

    // Vector suite for Connection
    //
    class_<ConnectionVector>(
        "ConnectionVector",
        "" )
        .def( vector_indexing_suite<ConnectionVector>() )
        ;
}
