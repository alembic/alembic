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

void IMaterialSchema::getTargetNames( std::vector<std::string> & oTargetNames )
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

    oTargetNames.clear();
    oTargetNames.reserve( uniqueNames.size() );
    oTargetNames.insert( oTargetNames.end(), uniqueNames.begin(),
                         uniqueNames.end() );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}


void IMaterialSchema::getShaderTypesForTarget( const std::string & iTargetName,
    std::vector<std::string> & oShaderTypeNames )
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
            if ( tokens[0] == iTargetName )
            {
                uniqueNames.insert( tokens[1] );
            }
        }
    }

    oShaderTypeNames.clear();
    oShaderTypeNames.reserve( uniqueNames.size() );
    oShaderTypeNames.insert( oShaderTypeNames.end(),
            uniqueNames.begin(), uniqueNames.end() );
    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

bool IMaterialSchema::getShader( const std::string & iTarget,
                                 const std::string & iShaderType,
                                 std::string & oResult )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMaterialSchema::getShader" );
    std::string propName = Util::buildTargetName( iTarget, iShaderType, "" );

    std::map<std::string, std::string>::iterator i =
        m_shaderNames.find( propName );

    if ( i != m_shaderNames.end() )
    {
        oResult = i->second;
        return true;
    }

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
    return false;
}


Abc::ICompoundProperty IMaterialSchema::getShaderParameters(
        const std::string & iTarget,
        const std::string & iShaderType )
{
    Abc::ICompoundProperty result;

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMaterialSchema::getShaderParameters" );
    std::string propName = Util::buildTargetName( iTarget, iShaderType,
                                                  "params" );

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

void IMaterialSchema::getNetworkNodeNames( std::vector<std::string> & oNames )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMateriaSchema::getNetworkNodeNames" );
    oNames.clear();

    if ( !m_node.valid() )
    {
        return;
    }

    oNames.reserve( m_node.getNumProperties() );

    for ( size_t i = 0, e = m_node.getNumProperties(); i < e; ++i )
    {
        const AbcCoreAbstract::PropertyHeader &header =
            m_node.getPropertyHeader( i );

        if ( header.isCompound() )
        {
            oNames.push_back( header.getName() );
        }
    }
    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

IMaterialSchema::NetworkNode IMaterialSchema::getNetworkNode( size_t iIndex )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IMateriaSchema::getNetworkNode" );
    if ( !m_node.valid() || iIndex >= m_node.getNumProperties() )
    {
        return NetworkNode();
    }

    const AbcCoreAbstract::PropertyHeader &header =
        m_node.getPropertyHeader( iIndex );

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
    const std::string & iNodeName )
{
    return NetworkNode( m_node, iNodeName );
}


void IMaterialSchema::getNetworkTerminalTargetNames(
    std::vector<std::string> & oTargetNames )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IMateriaSchema::getNetworkTerminalTargetNames" );
    oTargetNames.clear();

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

    oTargetNames.reserve( uniqueNames.size() );
    oTargetNames.insert( oTargetNames.end(),
            uniqueNames.begin(), uniqueNames.end() );
    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

void IMaterialSchema::getNetworkTerminalShaderTypesForTarget(
    const std::string & iTargetName,
    std::vector<std::string> & oShaderTypeNames)
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IMateriaSchema::getNetworkTerminalShaderTypesForTarget" );

    oShaderTypeNames.clear();

    std::set<std::string> uniqueNames;

    std::vector<std::string> tokens;
    std::map<std::string, std::string>::iterator i;
    for ( i = m_terminals.begin(); i != m_terminals.end(); ++i )
    {
        Util::split_tokens( i->first, tokens );

        if ( tokens.size() == 2 )
        {
            if ( tokens[0] == iTargetName )
            {
                uniqueNames.insert( tokens[1] );
            }
        }
    }

    oShaderTypeNames.reserve( uniqueNames.size() );
    oShaderTypeNames.insert( oShaderTypeNames.end(),
        uniqueNames.begin(), uniqueNames.end() );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

bool IMaterialSchema::getNetworkTerminal(
        const std::string & iTarget,
        const std::string & iShaderType,
        std::string & oNodeName,
        std::string & oOutputName)
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IMateriaSchema::getNetworkTerminal" );

    std::string propName = iTarget + "." + iShaderType;

    std::map<std::string, std::string>::iterator i =
        m_terminals.find( propName );

    if ( i == m_terminals.end() )
    {
        return false;
    }

    std::vector<std::string> tokens;
    Util::split_tokens( i->second, tokens, 1 );

    oNodeName = tokens[0];
    oOutputName = tokens.size() > 1 ? tokens[1] : "";

    return true;
    ALEMBIC_ABC_SAFE_CALL_END_RESET();

    return false;
}

size_t IMaterialSchema::getNumNetworkInterfaceParameterMappings()
{
    return m_interface.size();
}


bool IMaterialSchema::getNetworkInterfaceParameterMapping( size_t iIndex,
    std::string & oInterfaceParamName, std::string & oMapToNodeName,
    std::string & oMapToParamName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IMateriaSchema::getNetworkInterfaceParameterMapping(size_t,...)" );

    if ( iIndex >= m_interface.size() )
    {
        return false;
    }

    oInterfaceParamName = m_interface[iIndex];

    return getNetworkInterfaceParameterMapping( oInterfaceParamName,
                                                oMapToNodeName,
                                                oMapToParamName );
    ALEMBIC_ABC_SAFE_CALL_END_RESET();

    return false;
}




void IMaterialSchema::getNetworkInterfaceParameterMappingNames(
    std::vector<std::string> & oNames )
{
    oNames = m_interface;
}


bool IMaterialSchema::getNetworkInterfaceParameterMapping(
    const std::string & iInterfaceParamName,
    std::string & oMapToNodeName,
    std::string & oMapToParamName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IMateriaSchema::getNetworkInterfaceParameterMapping" );

    std::map<std::string, std::string>::iterator i =
        m_interfaceMap.find( iInterfaceParamName );

    if ( i == m_interfaceMap.end() )
    {
        return false;
    }

    std::vector<std::string> tokens;

    Util::split_tokens( i->second, tokens, 1 );

    oMapToNodeName = tokens[0];
    oMapToParamName = tokens.size() > 1 ? tokens[1] : "";

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


IMaterialSchema::NetworkNode::NetworkNode( Abc::ICompoundProperty iCompound )
: m_compound( iCompound )
, m_connectionsChecked( false )
{
}


IMaterialSchema::NetworkNode::NetworkNode( Abc::ICompoundProperty iParent,
                                           const std::string & iNodeName )
: m_connectionsChecked( false )
{
    if ( iParent.valid() )
    {
        if ( const AbcCoreAbstract::PropertyHeader * header =
            iParent.getPropertyHeader( iNodeName ) )
        {
            if ( header->isCompound() )
            {
                m_compound =  Abc::ICompoundProperty( iParent, iNodeName );
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


bool IMaterialSchema::NetworkNode::getTarget( std::string & oResult )
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

            oResult = prop.getValue();
            return true;
        }
    }

    return false;
}

bool IMaterialSchema::NetworkNode::getNodeType( std::string & oResult )
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

            oResult = prop.getValue();
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
    size_t iIndex,
    std::string & oInputName,
    std::string & oConnectedNodeName,
    std::string & oConnectedOutputName )
{
    if ( iIndex >= getNumConnections() )
    {
        return false;
    }

    oInputName = m_connections[iIndex];
    return getConnection( oInputName, oConnectedNodeName,
        oConnectedOutputName );
}

bool IMaterialSchema::NetworkNode::getConnection(
    const std::string & iInputName,
    std::string & oConnectedNodeName,
    std::string & oConnectedOutputName )
{
    // loads the connections if it hasn't already
    getNumConnections();

    std::map< std::string, std::string >::iterator i =
        m_connectionsMap.find( iInputName );

    if ( i == m_connectionsMap.end() )
    {
        return false;
    }

    std::string value = i->second;
    splitConnectionValue( value, oConnectedNodeName, oConnectedOutputName );

    return true;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcMaterial
} // End namespace Alembic

