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


#include <Alembic/AbcMaterial/IMaterial.h>
#include "InternalUtil.h"

#include <set>

namespace Alembic {
namespace AbcMaterial {
namespace ALEMBIC_VERSION_NS {

void IMaterialSchema::init()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMaterialSchema::init()" );

    AbcCoreAbstract::CompoundPropertyReaderPtr _this = this->getPtr();

    if ( this->getPropertyHeader( ".nodes" ) != NULL )
    {
        m_node = Abc::ICompoundProperty( _this, ".nodes" );
    }

    if ( this->getPropertyHeader( ".interfaceParams" ) != NULL )
    {
        m_interfaceParams = Abc::ICompoundProperty( _this, ".interfaceParams" );
    }

    if ( this->getPropertyHeader( ".terminals" ) != NULL )
    {
        Abc::IStringArrayProperty termProp( _this, ".terminals" );
        Abc::StringArraySamplePtr samp;
        termProp.get( samp );

        size_t numTerms = samp->size() / 2;
        for( size_t i = 0; i < numTerms; ++i )
        {
            m_terminals[( *samp )[2 * i]] = ( *samp )[2 * i + 1];
        }
    }

    if ( this->getPropertyHeader( ".shaderNames" ) != NULL )
    {
        Abc::IStringArrayProperty shaderProp( _this, ".shaderNames" );
        Abc::StringArraySamplePtr samp;
        shaderProp.get( samp );

        size_t numShaders = samp->size() / 2;
        for( size_t i = 0; i < numShaders; ++i )
        {
            m_shaderNames[( *samp )[2 * i]] = ( *samp )[2 * i + 1];
        }
    }

    if ( this->getPropertyHeader( ".interface" ) != NULL )
    {
        Abc::IStringArrayProperty interfaceProp( _this, ".interface" );
        Abc::StringArraySamplePtr samp;
        interfaceProp.get( samp );

        size_t numInterface = samp->size() / 2;
        m_interface.reserve( numInterface );
        for( size_t i = 0; i < numInterface; ++i )
        {
            m_interfaceMap[( *samp )[2 * i]] = ( *samp )[2 * i + 1];
            m_interface.push_back( ( *samp )[2 * i] );
        }
    }
    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

void IMaterialSchema::getTargetNames( std::vector<std::string> & targetNames )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMaterial::getTargetNames" );
    std::set<std::string> uniqueNames;

    std::vector<std::string> tokens;

    std::map<std::string, std::string>::iterator i;
    for ( i = m_shaderNames.begin(); i != m_shaderNames.end(); ++i )
    {
        Util::split_tokens( i->first, tokens );

        // target.shaderType
        if ( tokens.size() == 2 )
        {
            uniqueNames.insert( tokens[0] );
        }
    }

    targetNames.clear();
    targetNames.reserve( uniqueNames.size() );
    targetNames.insert( targetNames.end(), uniqueNames.begin(),
                        uniqueNames.end() );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}


void IMaterialSchema::getShaderTypesForTarget( const std::string & targetName,
    std::vector<std::string> & shaderTypeNames )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMaterialSchema::getShaderTypesForTarget" );
    std::set<std::string> uniqueNames;

    std::vector<std::string> tokens;

    std::map<std::string, std::string>::iterator i;
    for ( i = m_shaderNames.begin(); i != m_shaderNames.end(); ++i )
    {
        Util::split_tokens( i->first, tokens );

        if ( tokens.size() == 2 )
        {
            if ( tokens[0] == targetName )
            {
                uniqueNames.insert( tokens[1] );
            }
        }
    }

    shaderTypeNames.clear();
    shaderTypeNames.reserve( uniqueNames.size() );
    shaderTypeNames.insert( shaderTypeNames.end(),
            uniqueNames.begin(), uniqueNames.end() );
    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

bool IMaterialSchema::getShader( const std::string & target,
                                 const std::string & shaderType,
                                 std::string & result )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMaterialSchema::getShader" );
    std::string propName = Util::buildTargetName( target, shaderType, "" );

    std::map<std::string, std::string>::iterator i =
        m_shaderNames.find( propName );

    if ( i != m_shaderNames.end() )
    {
        result = i->second;
        return true;
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
    return false;
}


Abc::ICompoundProperty IMaterialSchema::getShaderParameters(
        const std::string & target,
        const std::string & shaderType )
{
    Abc::ICompoundProperty result;

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMaterialSchema::getShaderParameters" );
    std::string propName = Util::buildTargetName( target, shaderType,
                                                  "params" );

    Abc::ICompoundProperty result;

    if (const AbcCoreAbstract::PropertyHeader * header =
        getPropertyHeader( propName ) )
    {
        if ( header->isCompound() )
        {
            result = Abc::ICompoundProperty( *this, propName );
        }
    }
    ALEMBIC_ABC_SAFE_CALL_END_RESET();

    return result;
}

size_t IMaterialSchema::getNumNetworkNodes()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMaterialSchema::getNumNetworkNodes" );
    if ( m_node.valid() )
    {
        return m_node.getNumProperties();
    }
    ALEMBIC_ABC_SAFE_CALL_END_RESET();

    return 0;
}

void IMaterialSchema::getNetworkNodeNames( std::vector<std::string> & names )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMateriaSchema::getNetworkNodeNames" );
    names.clear();

    if ( !m_node.valid() )
    {
        return;
    }

    names.reserve( m_node.getNumProperties() );

    for ( size_t i = 0, e = m_node.getNumProperties(); i < e; ++i )
    {
        const AbcCoreAbstract::PropertyHeader &header =
            m_node.getPropertyHeader( i );

        if ( header.isCompound() )
        {
            names.push_back( header.getName() );
        }
    }
    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

IMaterialSchema::NetworkNode IMaterialSchema::getNetworkNode( size_t index )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMateriaSchema::getNetworkNode" );
    if ( !m_node.valid() || index >= m_node.getNumProperties() )
    {
        return NetworkNode();
    }

    const AbcCoreAbstract::PropertyHeader &header =
        m_node.getPropertyHeader( index );

    if ( !header.isCompound() )
    {
        return NetworkNode();
    }

    return NetworkNode(
            Abc::ICompoundProperty( m_node, header.getName() ) );
    ALEMBIC_ABC_SAFE_CALL_END_RESET();

    return NetworkNode();
}


IMaterialSchema::NetworkNode IMaterialSchema::getNetworkNode(
    const std::string & nodeName )
{
    return NetworkNode( m_node, nodeName );
}


void IMaterialSchema::getNetworkTerminalTargetNames(
    std::vector<std::string> & targetNames )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IMateriaSchema::getNetworkTerminalTargetNames" );
    targetNames.clear();

    std::set<std::string> uniqueNames;

    std::vector<std::string> tokens;
    std::map<std::string, std::string>::iterator i;
    for ( i = m_terminals.begin(); i != m_terminals.end(); ++i )
    {
        Util::split_tokens( i->first, tokens );

        // target.shaderType
        if ( tokens.size() == 2 )
        {
            uniqueNames.insert( tokens[0] );
        }
    }

    targetNames.reserve( uniqueNames.size() );
    targetNames.insert( targetNames.end(),
            uniqueNames.begin(), uniqueNames.end() );
    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

void IMaterialSchema::getNetworkTerminalShaderTypesForTarget(
    const std::string & targetName,
    std::vector<std::string> & shaderTypeNames)
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IMateriaSchema::getNetworkTerminalShaderTypesForTarget" );

    shaderTypeNames.clear();

    std::set<std::string> uniqueNames;

    std::vector<std::string> tokens;
    std::map<std::string, std::string>::iterator i;
    for ( i = m_terminals.begin(); i != m_terminals.end(); ++i )
    {
        Util::split_tokens( i->first, tokens );

        if ( tokens.size() == 2 )
        {
            if ( tokens[0] == targetName )
            {
                uniqueNames.insert( tokens[1] );
            }
        }
    }

    shaderTypeNames.reserve( uniqueNames.size() );
    shaderTypeNames.insert( shaderTypeNames.end(),
        uniqueNames.begin(), uniqueNames.end() );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

bool IMaterialSchema::getNetworkTerminal(
        const std::string & target,
        const std::string & shaderType,
        std::string & nodeName,
        std::string & outputName)
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IMateriaSchema::getNetworkTerminal" );

    std::string propName = target + "." + shaderType;

    std::map<std::string, std::string>::iterator i =
        m_terminals.find( propName );

    if ( i == m_terminals.end() )
    {
        return false;
    }

    std::vector<std::string> tokens;
    Util::split_tokens( i->second, tokens, 1 );

    nodeName = tokens[0];
    outputName = tokens.size() > 1 ? tokens[1] : "";

    return true;
    ALEMBIC_ABC_SAFE_CALL_END_RESET();

    return false;
}

size_t IMaterialSchema::getNumNetworkInterfaceParameterMappings()
{
    return m_interface.size();
}


bool IMaterialSchema::getNetworkInterfaceParameterMapping( size_t index,
    std::string & interfaceParamName, std::string & mapToNodeName,
    std::string & mapToParamName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IMateriaSchema::getNetworkInterfaceParameterMapping(size_t,...)" );

    if ( index >= m_interface.size() )
    {
        return false;
    }

    interfaceParamName = m_interface[index];

    return getNetworkInterfaceParameterMapping( interfaceParamName,
                                                mapToNodeName,
                                                mapToParamName );
    ALEMBIC_ABC_SAFE_CALL_END_RESET();

    return false;
}




void IMaterialSchema::getNetworkInterfaceParameterMappingNames(
    std::vector<std::string> & names )
{
    names = m_interface;
}


bool IMaterialSchema::getNetworkInterfaceParameterMapping(
    const std::string & interfaceParamName,
    std::string & mapToNodeName,
    std::string & mapToParamName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IMateriaSchema::getNetworkInterfaceParameterMapping" );

    std::map<std::string, std::string>::iterator i =
        m_interfaceMap.find( interfaceParamName );

    if ( i == m_interfaceMap.end() )
    {
        return false;
    }

    std::vector<std::string> tokens;

    Util::split_tokens( i->second, tokens, 1 );

    mapToNodeName = tokens[0];
    mapToParamName = tokens.size() > 1 ? tokens[1] : "";

    return true;
    ALEMBIC_ABC_SAFE_CALL_END_RESET();

    return false;
}

Abc::ICompoundProperty IMaterialSchema::getNetworkInterfaceParameters()
{
    return m_interfaceParams;
}


///////////////////////////////////////////////////////////////////////////////

IMaterialSchema::NetworkNode::NetworkNode()
: m_connectionsChecked( false )
{
}


IMaterialSchema::NetworkNode::NetworkNode( Abc::ICompoundProperty compound )
: m_compound( compound )
, m_connectionsChecked( false )
{
}


IMaterialSchema::NetworkNode::NetworkNode( Abc::ICompoundProperty parent,
                                           const std::string & nodeName )
: m_connectionsChecked( false )
{
    if ( parent.valid() )
    {
        if ( const AbcCoreAbstract::PropertyHeader * header =
            parent.getPropertyHeader( nodeName ) )
        {
            if ( header->isCompound() )
            {
                m_compound =  Abc::ICompoundProperty( parent, nodeName );
            }
        }
    }
}

bool IMaterialSchema::NetworkNode::valid()
{
    return m_compound.valid();
}


std::string IMaterialSchema::NetworkNode::getName()
{
    if ( valid() )
    {
        return m_compound.getName();
    }

    return "";
}


bool IMaterialSchema::NetworkNode::getTarget( std::string & result )
{
    if ( !valid() )
    {
        return false;
    }

    if ( const AbcCoreAbstract::PropertyHeader * header =
        m_compound.getPropertyHeader( "target" ) )
    {
        if ( header->isScalar() && Abc::IStringProperty::matches( *header) )
        {
            Abc::IStringProperty prop( m_compound, header->getName() );

            result = prop.getValue();
            return true;
        }
    }

    return false;
}

bool IMaterialSchema::NetworkNode::getNodeType( std::string & result )
{
    if ( !valid() )
    {
        return false;
    }

    if ( const AbcCoreAbstract::PropertyHeader * header =
        m_compound.getPropertyHeader( "type" ) )
    {
        if ( header->isScalar() && Abc::IStringProperty::matches( *header ) )
        {
            Abc::IStringProperty prop( m_compound, header->getName() );

            result = prop.getValue();
            return true;
        }
    }

    return false;
}

Abc::ICompoundProperty IMaterialSchema::NetworkNode::getParameters()
{
    Abc::ICompoundProperty result;
    if ( !valid() )
    {
        return result;
    }

    if (const AbcCoreAbstract::PropertyHeader * header =
        m_compound.getPropertyHeader( "params" ) )
    {
        if ( header->isCompound() )
        {
            result = Abc::ICompoundProperty( m_compound, "params" );
        }
    }

    return result;
}

size_t IMaterialSchema::NetworkNode::getNumConnections()
{
    if ( ! m_connectionsChecked )
    {
        if ( m_compound.getPropertyHeader( ".connections" ) != NULL )
        {
            Abc::IStringArrayProperty connectProp( m_compound, ".connections" );
            Abc::StringArraySamplePtr samp;
            connectProp.get( samp );

            size_t numConnect = samp->size() / 2;
            m_connections.reserve( numConnect );
            for( size_t i = 0; i < numConnect; ++i )
            {
                m_connectionsMap[( *samp )[2 * i]] = ( *samp )[2 * i + 1];
                m_connections.push_back( ( *samp )[2 * i] );
            }
        }
        m_connectionsChecked = true;
    }

    return m_connections.size();

}

void IMaterialSchema::NetworkNode::splitConnectionValue( const std::string & v,
    std::string & a, std::string & b )
{
    std::vector<std::string> tokens;

    Util::split_tokens(v, tokens, 1);

    a = tokens[0];
    b = tokens.size() > 1 ? tokens[1] : "";
}

bool IMaterialSchema::NetworkNode::getConnection(
    size_t index,
    std::string & inputName,
    std::string & connectedNodeName,
    std::string & connectedOutputName )
{
    if ( index >= getNumConnections() )
    {
        return false;
    }

    inputName = m_connections[index];
    return getConnection( inputName, connectedNodeName, connectedOutputName );
}

bool IMaterialSchema::NetworkNode::getConnection(
    const std::string & inputName,
    std::string & connectedNodeName,
    std::string & connectedOutputName )
{
    // loads the connections if it hasn't already
    getNumConnections();

    std::map< std::string, std::string >::iterator i =
        m_connectionsMap.find( inputName );

    if ( i == m_connectionsMap.end() )
    {
        return false;
    }

    std::string value = i->second;
    splitConnectionValue( value, connectedNodeName, connectedOutputName );

    return true;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcMaterial
} // End namespace Alembic

