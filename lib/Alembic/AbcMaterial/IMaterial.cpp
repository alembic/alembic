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

void IMaterialSchema::getTargetNames( std::vector<std::string> & targetNames )
{
    std::set<std::string> uniqueNames;

    std::vector<std::string> tokens;

    for ( size_t i = 0; i < getNumProperties(); ++i )
    {
        const Abc::PropertyHeader &propHeader = getPropertyHeader( i );

        Util::split_tokens( propHeader.getName(), tokens );

        if ( tokens.size() == 3 )
        {
            if ( tokens[2] == "shader" || tokens[2] == "params" )
            {
                uniqueNames.insert( tokens[0] );
            }
        }
    }

    targetNames.clear();
    targetNames.reserve( uniqueNames.size() );
    targetNames.insert( targetNames.end(), uniqueNames.begin(),
                        uniqueNames.end() );
}


void IMaterialSchema::getShaderTypesForTarget( const std::string & targetName,
    std::vector<std::string> & shaderTypeNames)
{
    std::set<std::string> uniqueNames;

    std::vector<std::string> tokens;

    for ( size_t i = 0; i < getNumProperties(); ++i )
    {
        const Abc::PropertyHeader &propHeader = getPropertyHeader( i );

        Util::split_tokens( propHeader.getName(), tokens );

        if ( tokens.size() == 3 )
        {
            if ( tokens[0] == targetName &&
                ( tokens[2] == "shader" || tokens[2] == "params" ) )
            {
                uniqueNames.insert( tokens[1] );
            }
        }
    }

    shaderTypeNames.clear();
    shaderTypeNames.reserve( uniqueNames.size() );
    shaderTypeNames.insert( shaderTypeNames.end(),
            uniqueNames.begin(), uniqueNames.end() );
}



bool IMaterialSchema::getShader( const std::string & target,
                                 const std::string & shaderType,
                                 std::string & result )
{
    std::string propName = Util::buildTargetName( target, shaderType,
                                                  "shader" );

    if ( const Abc::PropertyHeader * header = getPropertyHeader( propName ) )
    {
        if ( header->isScalar() && Abc::IStringProperty::matches( *header ) )
        {
            Abc::IStringProperty prop( *this, propName );

            result = prop.getValue();
            return true;
        }
    }

    return false;
}


Abc::ICompoundProperty IMaterialSchema::getShaderParameters(
        const std::string & target,
        const std::string & shaderType )
{
    std::string propName = Util::buildTargetName( target, shaderType,
                                                  "params" );

    Abc::ICompoundProperty result;

    if (const Abc::PropertyHeader * header = getPropertyHeader( propName ) )
    {
        if ( header->isCompound() )
        {
            result = Abc::ICompoundProperty( *this, propName );
        }
    }

    return result;
}

Abc::ICompoundProperty IMaterialSchema::getInternalCompound(
    const std::string & name )
{
    CompoundPropertyMap::iterator i = m_compounds.find( name );

    if ( i != m_compounds.end() )
    {
        return ( *i ).second;
    }

    Abc::ICompoundProperty result;

    if ( const Abc::PropertyHeader * header = getPropertyHeader( name ) )
    {
        if ( header->isCompound() )
        {
            result = Abc::ICompoundProperty( *this, name );
        }
    }

    m_compounds[name] = result;

    return result;
}

size_t IMaterialSchema::getNumNetworkNodes()
{
    Abc::ICompoundProperty nodesCompound = getInternalCompound( "nodes" );
    
    if ( !nodesCompound.valid() )
    {
        return 0;
    }

    return nodesCompound.getNumProperties();
}

void IMaterialSchema::getNetworkNodeNames( std::vector<std::string> & names )
{
    names.clear();

    Abc::ICompoundProperty nodesCompound = getInternalCompound( "nodes" );

    if ( !nodesCompound.valid() )
    {
        return;
    }

    names.reserve( nodesCompound.getNumProperties() );

    for ( size_t i = 0, e = nodesCompound.getNumProperties(); i < e; ++i )
    {
        const Abc::PropertyHeader &header =
                nodesCompound.getPropertyHeader( i );

        if ( header.isCompound() )
        {
            names.push_back( header.getName() );
        }
    }
}

IMaterialSchema::NetworkNode IMaterialSchema::getNetworkNode( size_t index )
{
    Abc::ICompoundProperty nodesCompound = getInternalCompound( "nodes" );
    
    if ( !nodesCompound.valid() || index >= nodesCompound.getNumProperties() )
    {
        return NetworkNode();
    }

    const Abc::PropertyHeader &header =
            nodesCompound.getPropertyHeader( index );

    if ( !header.isCompound() )
    {
        return NetworkNode();
    }

    return NetworkNode(
            Abc::ICompoundProperty( nodesCompound, header.getName() ) );
}


IMaterialSchema::NetworkNode IMaterialSchema::getNetworkNode(
    const std::string & nodeName )
{
    return NetworkNode( getInternalCompound( "nodes" ), nodeName );
}


void IMaterialSchema::getNetworkTerminalTargetNames(
    std::vector<std::string> & targetNames )
{
    targetNames.clear();

    Abc::ICompoundProperty terminalsCompound =
        getInternalCompound( "terminals" );

    if ( !terminalsCompound.valid() )
    {
        return;
    }

    std::set<std::string> uniqueNames;

    std::vector<std::string> tokens;
    for ( size_t i = 0; i < terminalsCompound.getNumProperties(); ++i )
    {
        const Abc::PropertyHeader & header =
                terminalsCompound.getPropertyHeader( i );

        if ( header.isScalar() && Abc::IStringProperty::matches( header ) )
        {
            Util::split_tokens( header.getName(), tokens );

            if ( tokens.size() == 2 )
            {
                uniqueNames.insert( tokens[0] );
            }
        }
    }

    targetNames.reserve( uniqueNames.size() );
    targetNames.insert( targetNames.end(),
            uniqueNames.begin(), uniqueNames.end() );
}

void IMaterialSchema::getNetworkTerminalShaderTypesForTarget(
    const std::string & targetName,
    std::vector<std::string> & shaderTypeNames)
{
    shaderTypeNames.clear();

    Abc::ICompoundProperty terminalsCompound =
        getInternalCompound( "terminals" );

    if ( !terminalsCompound.valid() )
    {
        return;
    }

    std::set<std::string> uniqueNames;

    std::vector<std::string> tokens;
    for ( size_t i = 0; i < terminalsCompound.getNumProperties(); ++i )
    {
        const Abc::PropertyHeader & header =
            terminalsCompound.getPropertyHeader( i );

        if ( header.isScalar() && Abc::IStringProperty::matches( header ) )
        {
            Util::split_tokens( header.getName(), tokens );

            if ( tokens.size() == 2 && tokens[0] == targetName )
            {
                uniqueNames.insert( tokens[1] );
            }
        }
    }

    shaderTypeNames.reserve( uniqueNames.size() );
    shaderTypeNames.insert( shaderTypeNames.end(),
        uniqueNames.begin(), uniqueNames.end() );
}




bool IMaterialSchema::getNetworkTerminal(
        const std::string & target,
        const std::string & shaderType,
        std::string & nodeName,
        std::string & outputName)
{
    Abc::ICompoundProperty terminalsCompound =
        getInternalCompound( "terminals" );

    if ( !terminalsCompound.valid() )
    {
        return false;
    }

    std::string propName = target + "." + shaderType;

    const Abc::PropertyHeader * header =
            terminalsCompound.getPropertyHeader( propName );

    if ( !header || !header->isScalar() ||
         !Abc::IStringProperty::matches( *header ) )
    {
        return false;
    }
    
    Abc::IStringProperty prop( terminalsCompound, propName );
    std::vector<std::string> tokens;

    Util::split_tokens( prop.getValue(), tokens, 1 );

    nodeName = tokens[0];
    outputName = tokens.size() > 1 ? tokens[1] : "";

    return true;
}

size_t IMaterialSchema::getNumNetworkInterfaceParameterMappings()
{
    Abc::ICompoundProperty interfaceCompound =
        getInternalCompound( "interface" );

    if ( !interfaceCompound.valid() )
    {
        return 0;
    }

    return interfaceCompound.getNumProperties();
}


bool IMaterialSchema::getNetworkInterfaceParameterMapping( size_t index,
    std::string & interfaceParamName, std::string & mapToNodeName,
    std::string & mapToParamName )
{
    Abc::ICompoundProperty interfaceCompound =
        getInternalCompound( "interface" );

    if ( !interfaceCompound.valid() )
    {
        return false;
    }

    if ( index >= interfaceCompound.getNumProperties() )
    {
        return false;
    }

    interfaceParamName = interfaceCompound.getPropertyHeader( index ).getName();

    return getNetworkInterfaceParameterMapping( interfaceParamName,
                                                mapToNodeName,
                                                mapToParamName );
}




void IMaterialSchema::getNetworkInterfaceParameterMappingNames(
    std::vector<std::string> & names )
{
    names.clear();

    Abc::ICompoundProperty interfaceCompound =
        getInternalCompound( "interface" );

    if ( !interfaceCompound.valid() )
    {
        return;
    }

    names.reserve( interfaceCompound.getNumProperties() );

    for ( size_t i = 0; i < interfaceCompound.getNumProperties(); ++i )
    {
        const Abc::PropertyHeader & header =
            interfaceCompound.getPropertyHeader( i );

        //TODO, worth checking this type or just assuming it's ok because
        //we're not exposing this to anyone.
        //if (header.isScalar() && Abc::IStringProperty::matches(header))

        names.push_back( header.getName() );
    }
}


bool IMaterialSchema::getNetworkInterfaceParameterMapping(
    const std::string & interfaceParamName, std::string & mapToNodeName,
    std::string & mapToParamName)
{
    Abc::ICompoundProperty interfaceCompound =
        getInternalCompound( "interface" );

    if ( !interfaceCompound.valid() )
    {
        return false;
    }

    const Abc::PropertyHeader * header =
        interfaceCompound.getPropertyHeader( interfaceParamName );

    if ( !header || !header->isScalar() ||
         !Abc::IStringProperty::matches( *header ) )
    {
        return false;
    }

    Abc::IStringProperty prop( interfaceCompound, interfaceParamName );
    std::vector<std::string> tokens;

    Util::split_tokens( prop.getValue(), tokens, 1 );

    mapToNodeName = tokens[0];
    mapToParamName = tokens.size() > 1 ? tokens[1] : "";

    return true;
}

Abc::ICompoundProperty IMaterialSchema::getNetworkInterfaceParameters()
{
    return getInternalCompound( "parameters" );
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
        if ( const Abc::PropertyHeader * header =
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

    if ( const Abc::PropertyHeader * header =
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

    if ( const Abc::PropertyHeader * header =
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

    if (const Abc::PropertyHeader * header =
        m_compound.getPropertyHeader( "params" ) )
    {
        if ( header->isCompound() )
        {
            result = Abc::ICompoundProperty( m_compound, "params" );
        }
    }

    return result;
}


Abc::ICompoundProperty IMaterialSchema::NetworkNode::getConnectionsCompound()
{
    if ( !m_connectionsChecked )
    {
        m_connectionsChecked = true;
        if ( valid() )
        {
            if ( const Abc::PropertyHeader * header =
                m_compound.getPropertyHeader( "connections" ) )
            {
                if ( header->isCompound() )
                {
                    m_connections =
                        Abc::ICompoundProperty( m_compound, "connections" );
                }
            }
        }
    }

    return m_connections;
}


size_t IMaterialSchema::NetworkNode::getNumConnections()
{
    Abc::ICompoundProperty connections = getConnectionsCompound();
    if ( !connections.valid() )
    {
        return false;
    }

    return connections.getNumProperties();

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
    Abc::ICompoundProperty connections = getConnectionsCompound();
    if ( !connections.valid() )
    {
        return false;
    }

    if ( index >= connections.getNumProperties() )
    {
        return false;
    }

    const Abc::PropertyHeader & header =
        connections.getPropertyHeader( index );

    if ( !header.isScalar() || !Abc::IStringProperty::matches( header ) )
    {
        return false;
    }

    Abc::IStringProperty prop( connections, header.getName() );

    inputName = header.getName();
    splitConnectionValue( prop.getValue(),
                          connectedNodeName,
                          connectedOutputName );

    return true;
}


bool IMaterialSchema::NetworkNode::getConnection(
    const std::string & inputName,
    std::string & connectedNodeName,
    std::string & connectedOutputName )
{
    Abc::ICompoundProperty connections = getConnectionsCompound();
    if ( !connections.valid() )
    {
        return false;
    }

    if ( const Abc::PropertyHeader * header =
        connections.getPropertyHeader( inputName ) )
    {
        if ( !header->isScalar() || !Abc::IStringProperty::matches( *header ) )
        {
            return false;
        }

        Abc::IStringProperty prop( connections, header->getName() );
        splitConnectionValue( prop.getValue(),
                              connectedNodeName,
                              connectedOutputName );
        return true;
    }

    return false;
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcMaterial
} // End namespace Alembic

