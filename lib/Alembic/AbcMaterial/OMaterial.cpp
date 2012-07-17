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


void OMaterialSchema::setShader(
        const std::string & target,
        const std::string & shaderType,
        const std::string & shaderName )
{
    Util::validateName( target, "target" );
    Util::validateName( shaderType, "shaderType" );

    std::string propertyName = Util::buildTargetName(
            target, shaderType, "shader" );

    if ( m_shaderNames.find( propertyName ) != m_shaderNames.end() )
    {
        //TODO, throw if already set. This is not replaceable
        return;
    }

    Abc::OStringProperty prop( this->getPtr(), propertyName );

    prop.set( shaderName );

    m_shaderNames[propertyName] = prop;

}


Abc::OCompoundProperty OMaterialSchema::getShaderParameters(
        const std::string & target,
        const std::string & shaderType )
{
    Util::validateName( target, "target" );
    Util::validateName( shaderType, "shaderType" );

    std::string propertyName = Util::buildTargetName(
        target, shaderType, "params" );

    CompoundPropertyMap::iterator I = m_compounds.find( propertyName );

    if ( I != m_compounds.end() )
    {
        return (*I).second;
    }

    Abc::OCompoundProperty prop( this->getPtr(), propertyName );

    m_compounds[propertyName] = prop;

    return prop;
}

Abc::OCompoundProperty OMaterialSchema::getInternalCompound(
        const std::string & name )
{
    Abc::OCompoundProperty compound;
    CompoundPropertyMap::iterator I = m_compounds.find( name );
    if ( I != m_compounds.end() )
    {
        compound = (*I).second;
    }
    else
    {
        m_compounds[name] = compound = Abc::OCompoundProperty(
            this->getPtr(), name );
    }

    return compound;
}


void OMaterialSchema::addNetworkNode(
        const std::string & nodeName,
        const std::string & target,
        const std::string & nodeType )
{
    Util::validateName( nodeName, "nodeName" );
    Util::validateName( target, "target" );

    std::string dstName = "nodes/" + nodeName;

    if ( m_compounds.find( dstName ) != m_compounds.end() )
    {
        //TODO, throw if already set. This is not replaceable
        return;
    }

    Abc::OCompoundProperty nodesCompound = getInternalCompound( "nodes" );

    Abc::OCompoundProperty nodeCompound =
            Abc::OCompoundProperty( nodesCompound.getPtr(), nodeName );

    m_compounds[dstName] = nodeCompound;

    Abc::OStringProperty( nodeCompound.getPtr(), "target" ).set( target );
    Abc::OStringProperty( nodeCompound.getPtr(), "type" ).set( nodeType );
}



void OMaterialSchema::setNetworkNodeConnection(
        const std::string & nodeName,
        const std::string & inputName,
        const std::string & connectedNodeName,
        const std::string & connectedOutputName )
{
    std::string nodeDstName = "nodes/" + nodeName;

    if ( m_compounds.find(nodeDstName) == m_compounds.end() )
    {
        //make the empty node, this is legit for a child material
        //overriding something other than target or nodeType

        Abc::OCompoundProperty nodesCompound = getInternalCompound( "nodes" );
        Abc::OCompoundProperty nodeCompound =
                Abc::OCompoundProperty( nodesCompound.getPtr(), nodeName );
        m_compounds[nodeDstName] = nodeCompound;
    }

    Abc::OCompoundProperty connectionsCompound;

    std::string connectionsDstName = nodeDstName + "/connections";
    CompoundPropertyMap::iterator I = m_compounds.find( connectionsDstName );

    if ( I != m_compounds.end() )
    {
        connectionsCompound = (*I).second;
    }
    else
    {
        m_compounds[connectionsDstName] = connectionsCompound =
                Abc::OCompoundProperty( m_compounds[nodeDstName].getPtr(),
                         "connections" );
    }

    std::string connectionValue = connectedNodeName;
    if ( !connectedOutputName.empty() )
    {
        connectionValue += "." + connectedOutputName;
    }

    Abc::OStringProperty( connectionsCompound.getPtr(), inputName).set(
            connectionValue );
}



Abc::OCompoundProperty OMaterialSchema::getNetworkNodeParameters(
        const std::string & nodeName )
{
    std::string nodeDstName = "nodes/" + nodeName;
    std::string paramsDstName = nodeDstName + "/params";

    CompoundPropertyMap::iterator I = m_compounds.find( paramsDstName );
    if ( I != m_compounds.end() )
    {
        return (*I).second;
    }

    I = m_compounds.find( nodeDstName );
    if ( I == m_compounds.end() )
    {
        //make the empty node, this is legit for a child material
        //overriding something other than target or nodeType
        Abc::OCompoundProperty nodesCompound = getInternalCompound( "nodes" );
        Abc::OCompoundProperty nodeCompound =
                Abc::OCompoundProperty( nodesCompound.getPtr(), nodeName );
        m_compounds[nodeDstName] = nodeCompound;
    }

    Abc::OCompoundProperty result( (*I).second.getPtr(), "params" );
    m_compounds[paramsDstName] = result;

    return result;
}



void OMaterialSchema::setNetworkTerminal(
        const std::string & target,
        const std::string & shaderType,
        const std::string & nodeName,
        const std::string & outputName )
{
    Util::validateName( target, "target" );
    Util::validateName( shaderType, "shaderType" );
    Util::validateName( nodeName, "nodeName" );

    Abc::OCompoundProperty terminals = getInternalCompound( "terminals" );

    std::string connectionValue = nodeName;
    if ( !outputName.empty() )
    {
        connectionValue += ".";
        connectionValue += outputName;
    }

    Abc::OStringProperty( terminals.getPtr(),
            Util::buildTargetName( target, shaderType, "") ).set(
                connectionValue );
}

void OMaterialSchema::setNetworkInterfaceParameterMapping(
        const std::string & interfaceParamName,
        const std::string & mapToNodeName,
        const std::string & mapToParamName )
{
    //TODO, validate interface paramName?
    Util::validateName( mapToNodeName, "mapToNodeName" );

    Abc::OCompoundProperty interfaceCompound =
            getInternalCompound("interface");

    Abc::OStringProperty(interfaceCompound, interfaceParamName).set(
            mapToNodeName + "." + mapToParamName );
}

Abc::OCompoundProperty OMaterialSchema::getNetworkInterfaceParameters()
{
    return getInternalCompound("parameters");
}


} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcMaterial
} // End namespace Alembic

