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

#include <Alembic/AbcMaterial/OMaterial.h>
#include "InternalUtil.h"


namespace Alembic {
namespace AbcMaterial {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class OMaterialSchema::Data
{
public:

    // writes out the data we were holding onto
    ~Data()
    {
        size_t mapSize = shaderNames.size();
        if ( mapSize > 0 )
        {
            std::vector<std::string> strVec;
            strVec.reserve( mapSize * 2 );
            std::map<std::string, std::string>::iterator i;
            for ( i = shaderNames.begin(); i != shaderNames.end(); ++i )
            {
                strVec.push_back( i->first );
                strVec.push_back( i->second );
            }
            Abc::OStringArrayProperty prop( parent, ".shaderNames" );
            prop.set( Abc::StringArraySample( strVec ) );
        }

        // now do it for terminals
        mapSize = terminals.size();
        if ( mapSize > 0 )
        {
            std::vector<std::string> strVec;
            strVec.reserve( mapSize * 2 );
            std::map<std::string, std::string>::iterator i;
            for ( i = terminals.begin(); i != terminals.end(); ++i )
            {
                strVec.push_back( i->first );
                strVec.push_back( i->second );
            }
            Abc::OStringArrayProperty prop( parent, ".terminals" );
            prop.set( Abc::StringArraySample( strVec ) );
        }

        // now write out the connections for each node
        Data::NodeMap::iterator i;
        for ( i = nodes.begin(); i != nodes.end(); ++i )
        {
            mapSize = i->second.connections.size();

            // don't write anything if our map is empty
            if ( mapSize == 0 )
            {
                continue;
            }

            std::vector<std::string> strVec;
            strVec.reserve( mapSize * 2 );
            std::map<std::string, std::string>::iterator j;
            for ( j = i->second.connections.begin();
                  j != i->second.connections.end(); ++j )
           {
                strVec.push_back( j->first );
                strVec.push_back( j->second );
            }
        Abc::OStringArrayProperty prop( i->second.prop, ".connections" );
        prop.set( Abc::StringArraySample( strVec ) );
    }

    if ( ! interfaceStrVec.empty() )
    {
        Abc::OStringArrayProperty prop( parent, ".interface" );
        prop.set( Abc::StringArraySample( interfaceStrVec ) );
    }
}

    AbcCoreAbstract::CompoundPropertyWriterPtr parent;

    struct Node
    {
        Abc::OCompoundProperty prop;
        Abc::OCompoundProperty params;
        std::map<std::string, std::string> connections;
    };

    typedef std::map<std::string, Node> NodeMap;
    NodeMap nodes;
    std::map<std::string, std::string> shaderNames;
    std::map<std::string, std::string> terminals;

    Abc::OCompoundProperty interfaceParams;
    std::vector<std::string> interfaceStrVec;
};

//-*****************************************************************************
OMaterialSchema::OMaterialSchema(
    Alembic::AbcCoreAbstract::CompoundPropertyWriterPtr iParent,
    const std::string &iName,
    const Abc::Argument &iArg0,
    const Abc::Argument &iArg1,
    const Abc::Argument &iArg2,
    const Abc::Argument &iArg3 )
: Abc::OSchema<MaterialSchemaInfo>( iParent, iName, iArg0, iArg1, iArg2, iArg3 )
{
    init();
}

//-*****************************************************************************
OMaterialSchema::OMaterialSchema( Abc::OCompoundProperty iParent,
                                  const std::string &iName,
                                  const Abc::Argument &iArg0,
                                  const Abc::Argument &iArg1,
                                  const Abc::Argument &iArg2 )
: Abc::OSchema<MaterialSchemaInfo>( iParent.getPtr(), iName,
                                    GetErrorHandlerPolicy( iParent ),
                                    iArg0, iArg1, iArg2 )
{
    init();
}

//-*****************************************************************************
void OMaterialSchema::init()
{
    m_data = Alembic::Util::shared_ptr< Data >( new Data() );
    m_data->parent = this->getPtr();
}

//-*****************************************************************************
void OMaterialSchema::setShader(
        const std::string & iTarget,
        const std::string & iShaderType,
        const std::string & iShaderName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OMaterialSchema::setShader" );
    Util::validateName( iTarget, "target" );
    Util::validateName( iShaderType, "shaderType" );

    std::string propertyName = Util::buildTargetName(
            iTarget, iShaderType, "" );

    m_data->shaderNames[propertyName] = iShaderName;

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
Abc::OCompoundProperty OMaterialSchema::getShaderParameters(
        const std::string & iTarget,
        const std::string & iShaderType )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OMaterialSchema::getShaderParameters" );

    Util::validateName( iTarget, "target" );
    Util::validateName( iShaderType, "shaderType" );

    std::string propertyName = Util::buildTargetName(
        iTarget, iShaderType, "params" );

    Data::NodeMap::iterator i = m_data->nodes.find( propertyName );

    if ( i != m_data->nodes.end() )
    {
        return i->second.params;
    }

    Data::Node n;
    n.params = Abc::OCompoundProperty( this->getPtr(), propertyName );

    m_data->nodes[propertyName] = n;

    return n.params;
    ALEMBIC_ABC_SAFE_CALL_END();

    return Abc::OCompoundProperty();
}

//-*****************************************************************************
void OMaterialSchema::createNodeCompound()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OMaterialSchema::createNodeCompound" );

    if ( ! m_node.valid() )
    {
        m_node =  Abc::OCompoundProperty( this->getPtr(), ".nodes" );
    }

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OMaterialSchema::addNetworkNode(
        const std::string & iNodeName,
        const std::string & iTarget,
        const std::string & iNodeType )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OMaterialSchema::addNetworkNode" );
    Util::validateName( iNodeName, "nodeName" );
    Util::validateName( iTarget, "target" );

    std::string dstName = "nodes/" + iNodeName;

    ABCA_ASSERT( m_data->nodes.find( dstName ) == m_data->nodes.end(),
        "Node already added: " << iNodeName );

    createNodeCompound();

    Data::Node n;
    n.prop = Abc::OCompoundProperty( m_node.getPtr(), iNodeName );

    m_data->nodes[dstName] = n;

    Abc::OStringProperty( n.prop, "target" ).set( iTarget );
    Abc::OStringProperty( n.prop, "type" ).set( iNodeType );
    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OMaterialSchema::setNetworkNodeConnection(
        const std::string & iNodeName,
        const std::string & iInputName,
        const std::string & iConnectedNodeName,
        const std::string & iConnectedOutputName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OMaterialSchema::setNetworkNodeConnection" );
    std::string nodeDstName = "nodes/" + iNodeName;

    if ( m_data->nodes.find( nodeDstName ) == m_data->nodes.end() )
    {
        //make the empty node, this is legit for a child material
        //overriding something other than target or nodeType

        createNodeCompound();
        Data::Node n;
        n.prop = Abc::OCompoundProperty( m_node.getPtr(), iNodeName );
        m_data->nodes[nodeDstName] = n;
    }

    std::string connectionValue = iConnectedNodeName;
    if ( !iConnectedOutputName.empty() )
    {
        connectionValue += "." + iConnectedOutputName;
    }

    m_data->nodes[nodeDstName].connections[iInputName] = connectionValue;
    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
Abc::OCompoundProperty OMaterialSchema::getNetworkNodeParameters(
        const std::string & iNodeName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OMaterialSchema::getNetworkNodeParameters" );
    std::string nodeDstName = "nodes/" + iNodeName;

    Abc::OCompoundProperty params;

    Data::NodeMap::iterator i = m_data->nodes.find( nodeDstName );
    if ( i == m_data->nodes.end() )
    {
        //make the empty node, this is legit for a child material
        //overriding something other than target or nodeType
        createNodeCompound();
        Data::Node n;
        n.prop = Abc::OCompoundProperty( m_node.getPtr(), iNodeName );
        m_data->nodes[nodeDstName] = n;
    }

    if ( i->second.params.valid() )
    {
        return i->second.params;
    }

    i->second.params = Abc::OCompoundProperty( i->second.prop, "params" );

    return i->second.params;
    ALEMBIC_ABC_SAFE_CALL_END();

    return Abc::OCompoundProperty();
}


//-*****************************************************************************
void OMaterialSchema::setNetworkTerminal(
        const std::string & iTarget,
        const std::string & iShaderType,
        const std::string & iNodeName,
        const std::string & iOutputName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OMaterialSchema::setNetworkTerminal" );

    Util::validateName( iTarget, "target" );
    Util::validateName( iShaderType, "shaderType" );
    Util::validateName( iNodeName, "nodeName" );

    std::string connectionValue = iNodeName;
    if ( !iOutputName.empty() )
    {
        connectionValue += ".";
        connectionValue += iOutputName;
    }

    std::string terminalName = Util::buildTargetName( iTarget, iShaderType, "");
    m_data->terminals[terminalName] = connectionValue;

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void OMaterialSchema::setNetworkInterfaceParameterMapping(
        const std::string & iInterfaceParamName,
        const std::string & iMapToNodeName,
        const std::string & iMapToParamName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OMaterialSchema::setNetworkInterfaceParameterMapping" );

    //TODO, validate interface paramName?
    Util::validateName( iMapToNodeName, "mapToNodeName" );

    // order is important
    m_data->interfaceStrVec.push_back( iInterfaceParamName );
    m_data->interfaceStrVec.push_back( iMapToNodeName + "." + iMapToParamName );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
Abc::OCompoundProperty OMaterialSchema::getNetworkInterfaceParameters()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "OMaterialSchema::getNetworkInterfaceParameters" );

    if ( ! m_data->interfaceParams.valid() )
    {
        m_data->interfaceParams =
            Abc::OCompoundProperty( this->getPtr(), ".interfaceParams" );
    }

    return m_data->interfaceParams;

    ALEMBIC_ABC_SAFE_CALL_END();

    return Abc::OCompoundProperty();
}


} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcMaterial
} // End namespace Alembic

