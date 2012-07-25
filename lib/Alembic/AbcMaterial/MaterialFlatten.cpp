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

#include <Alembic/AbcMaterial/MaterialFlatten.h>
#include <Alembic/AbcMaterial/MaterialAssignment.h>

#include <set>

namespace Alembic {
namespace AbcMaterial {
namespace ALEMBIC_VERSION_NS {

MaterialFlatten::MaterialFlatten()
: m_networkFlattened( false )
{
}

MaterialFlatten::MaterialFlatten( IMaterialSchema iMaterialSchema )
: m_networkFlattened( false )
{
    m_schemas.push_back( iMaterialSchema );
}

MaterialFlatten::MaterialFlatten( IMaterial iMaterialObject )
: m_networkFlattened( false )
{
    append( iMaterialObject );
}

MaterialFlatten::MaterialFlatten( Abc::IObject iObject,
        Abc::IArchive iAlternateSearchArchive )
: m_networkFlattened( false )
{
    //first apply a local material
    IMaterialSchema localMaterial;

    if ( hasMaterial( iObject, localMaterial ) )
    {
        append( localMaterial );
    }

    //then apply the inheritance chain of an assigned material
    std::string assignedPath;
    if ( getMaterialAssignmentPath( iObject, assignedPath ) )
    {
        //now walk to that object, confirm it's a material,
        //and then append it.

        //For now, walk from root and then back down
        //eventually, support relative paths

        Abc::IObject parent;
        if ( iAlternateSearchArchive.valid() &&
             iAlternateSearchArchive.getTop().valid() )
        {
            parent = iAlternateSearchArchive.getTop();
        }
        else
        {
            parent = iObject.getArchive().getTop();
        }


        size_t lastPos = 0;
        bool isDone = false;

        while ( ! isDone )
        {
            size_t curPos = assignedPath.find( '/', lastPos );
            size_t length = 0;

            if ( curPos == std::string::npos )
            {
                isDone = true;
                length = std::string::npos;
            }
            // no other characters between / (starting / or multiple / in a row)
            else if ( lastPos == curPos )
            {
                lastPos = curPos + 1;
                if ( lastPos == assignedPath.size() )
                {
                    isDone = true;
                }
                continue;
            }
            else
            {
                length = curPos - lastPos;
            }

            std::string childName = assignedPath.substr( lastPos, length );
            lastPos = curPos + 1;

            if ( parent.getChildHeader( childName ) )
            {
                parent = parent.getChild( childName );
            }
            else
            {
                parent = Abc::IObject();
                break;
            }
        }

        if ( parent.valid() && IMaterial::matches( parent.getHeader() ) )
        {
            append( IMaterial( parent, Abc::kWrapExisting ) );
        }
    }
}

void MaterialFlatten::append( IMaterialSchema iMaterialSchema )
{
    m_schemas.push_back( iMaterialSchema );

    m_networkFlattened = false;
}


void MaterialFlatten::append( IMaterial iMaterialObject )
{
    //append the schema objects
    m_schemas.push_back( iMaterialObject.getSchema() );

    Abc::IObject parent = iMaterialObject.getParent();

    while ( parent.valid() )
    {
        if ( IMaterial::matches( parent.getHeader() ) )
        {
            m_schemas.push_back(
                IMaterial( parent, Abc::kWrapExisting ).getSchema() );
        }

        parent = parent.getParent();
    }

    m_networkFlattened = false;
}




bool MaterialFlatten::empty()
{
    return m_schemas.empty();
}

void MaterialFlatten::getTargetNames( std::vector<std::string> & oTargetNames )
{
    std::set<std::string> uniqueNames;

    for ( SchemaVector::iterator i = m_schemas.begin(); i != m_schemas.end();
          ++i )
    {
        std::vector<std::string> names;
        ( *i ).getTargetNames( names );
        uniqueNames.insert( names.begin(), names.end() );
    }

    oTargetNames.clear();

    oTargetNames.reserve( uniqueNames.size() );
    oTargetNames.insert( oTargetNames.end(), uniqueNames.begin(),
                         uniqueNames.end() );
}

void MaterialFlatten::getShaderTypesForTarget( const std::string & iTargetName,
    std::vector<std::string> & oShaderTypeNames )
{
    std::set<std::string> uniqueNames;

    for ( SchemaVector::iterator i = m_schemas.begin(); i != m_schemas.end();
          ++i )
    {
        std::vector<std::string> names;
        ( *i ).getShaderTypesForTarget( iTargetName, names );
        uniqueNames.insert( names.begin(), names.end() );
    }

    oShaderTypeNames.clear();
    oShaderTypeNames.reserve( uniqueNames.size() );
    oShaderTypeNames.insert( oShaderTypeNames.end(), uniqueNames.begin(),
                            uniqueNames.end() );
}

bool MaterialFlatten::getShader( const std::string & iTarget,
                                 const std::string & iShaderType,
                                 std::string & oResult)
{
    for ( SchemaVector::iterator i = m_schemas.begin(); i != m_schemas.end();
          ++i )
    {
        if ( ( *i ).getShader( iTarget, iShaderType, oResult ) )
        {
            return true;
        }
    }

    return false;
}

void MaterialFlatten::getShaderParameters( const std::string & iTarget,
                                           const std::string & iShaderType,
                                           ParameterEntryVector & oResult )
{
    oResult.clear();
    std::set<std::string> uniqueNames;

    for ( SchemaVector::iterator i = m_schemas.begin(); i != m_schemas.end();
          ++i )
    {
        Abc::ICompoundProperty params = ( *i ).getShaderParameters(
            iTarget, iShaderType );

        if (!params.valid())
        {
            continue;
        }

        oResult.reserve(params.getNumProperties());

        for ( size_t j = 0; j < params.getNumProperties(); ++j )
        {
            const AbcCoreAbstract::PropertyHeader &propHeader =
                params.getPropertyHeader( j );

            if ( uniqueNames.find( propHeader.getName() ) != uniqueNames.end() )
            {
                continue;
            }

            uniqueNames.insert( propHeader.getName() );

            oResult.push_back( ParameterEntry( propHeader.getName(),
                                               params, &propHeader ) );
        }
    }
}

void MaterialFlatten::getNetworkTerminalTargetNames(
    std::vector<std::string> & oTargetNames )
{
    std::set<std::string> uniqueNames;

    for ( SchemaVector::iterator i = m_schemas.begin(); i != m_schemas.end();
          ++i)
    {
        std::vector<std::string> names;
        ( *i ).getNetworkTerminalTargetNames( names );
        uniqueNames.insert( names.begin(), names.end() );
    }

    oTargetNames.clear();
    oTargetNames.reserve( uniqueNames.size() );
    oTargetNames.insert( oTargetNames.end(), uniqueNames.begin(),
                         uniqueNames.end() );
}

void MaterialFlatten::getNetworkTerminalShaderTypesForTarget(
    const std::string & iTargetName,
    std::vector<std::string> & oShaderTypeNames )
{
    std::set<std::string> uniqueNames;

    for ( SchemaVector::iterator i = m_schemas.begin(); i != m_schemas.end();
          ++i )
    {
        std::vector<std::string> names;
        ( *i ).getNetworkTerminalShaderTypesForTarget(
            iTargetName, names );

        uniqueNames.insert( names.begin(), names.end() );
    }

    oShaderTypeNames.clear();
    oShaderTypeNames.reserve( uniqueNames.size() );
    oShaderTypeNames.insert( oShaderTypeNames.end(), uniqueNames.begin(),
                             uniqueNames.end() );
}

bool MaterialFlatten::getNetworkTerminal( const std::string & iTarget,
                                          const std::string & iShaderType,
                                          std::string & oNodeName,
                                          std::string & oOutputName )
{
    for ( SchemaVector::iterator i = m_schemas.begin(); i != m_schemas.end();
          ++i)
    {
        if ( ( *i ).getNetworkTerminal( iTarget, iShaderType, oNodeName,
                                        oOutputName ) )
        {
            return true;
        }
    }
    return false;
}




void MaterialFlatten::flattenNetwork()
{
    if ( m_networkFlattened )
    {
        return;
    }

    m_networkFlattened = true;

    m_nodeNames.clear();
    m_nodesToInterfaceMappings.clear();

    std::set<std::string> foundNodes;
    std::vector<std::string> workingNames;
    std::string interfaceParamName, mapToNodeName, mapToParamName;

    for ( SchemaVector::iterator i = m_schemas.begin(); i != m_schemas.end();
          ++i)
    {
        IMaterialSchema & schema = ( *i );

        //find the unique node names
        schema.getNetworkNodeNames( workingNames );
        for ( std::vector<std::string>::iterator j = workingNames.begin();
              j != workingNames.end(); ++j )
        {
            const std::string & name = ( *j );

            if ( foundNodes.find( name ) != foundNodes.end() )
            {
                foundNodes.insert( name );
                m_nodeNames.push_back( name );
            }
        }

        //build the public interface mappings
        for ( size_t j = 0,
              e = schema.getNumNetworkInterfaceParameterMappings();
              j < e; ++j)
        {
            if ( !schema.getNetworkInterfaceParameterMapping( j,
                interfaceParamName, mapToNodeName, mapToParamName ) )
            {
                continue;
            }

            StringMapPtr nodeParameterMappings;
            StringMapMap::iterator nI = m_nodesToInterfaceMappings.find(
                mapToNodeName );

            if ( nI != m_nodesToInterfaceMappings.end() )
            {
                nodeParameterMappings = ( *nI ).second;
            }
            else
            {
                nodeParameterMappings = StringMapPtr( new StringMap );
                m_nodesToInterfaceMappings[mapToNodeName] =
                    nodeParameterMappings;
            }

            if ( nodeParameterMappings->find( mapToParamName ) ==
                 nodeParameterMappings->end() )
            {
                ( *nodeParameterMappings )[mapToParamName] = interfaceParamName;
            }
        }
    }
}

size_t MaterialFlatten::getNumNetworkNodes()
{
    flattenNetwork();
    return m_nodeNames.size();
}

MaterialFlatten::NetworkNode MaterialFlatten::getNetworkNode( size_t iIndex )
{
    flattenNetwork();

    if ( iIndex >= m_nodeNames.size() )
    {
        return NetworkNode();
    }

    return getNetworkNode( m_nodeNames[iIndex] );
}

MaterialFlatten::NetworkNode MaterialFlatten::getNetworkNode(
    const std::string & iNodeName)
{
    flattenNetwork();

    StringMapPtr interfaceMappings;
    StringMapMap::iterator i = m_nodesToInterfaceMappings.find( iNodeName );
    if ( i != m_nodesToInterfaceMappings.end() )
    {
        interfaceMappings = ( *i ).second;
    }

   return NetworkNode( iNodeName, m_schemas, interfaceMappings );
}

///////////////////////////////////////////////////////////////////////////////

MaterialFlatten::NetworkNode::NetworkNode()
{
}


MaterialFlatten::NetworkNode::NetworkNode( const std::string & iName,
    SchemaVector & iSchemas, StringMapPtr iInterfaceMappings )
: m_name( iName )
, m_interfaceMappings( iInterfaceMappings )
{
    m_nodes.reserve( iSchemas.size() );
    m_networkParameters.reserve( iSchemas.size() );

    for ( SchemaVector::iterator i = iSchemas.begin(); i != iSchemas.end();
          ++i )
    {
        IMaterialSchema & schema = ( *i );

        IMaterialSchema::NetworkNode node = schema.getNetworkNode( iName );
        if ( node.valid() )
        {
            m_nodes.push_back( node );
        }

        Abc::ICompoundProperty networkParams =
            schema.getNetworkInterfaceParameters();

        if ( networkParams.valid() )
        {
            m_networkParameters.push_back( networkParams );
        }

    }
}






bool MaterialFlatten::NetworkNode::valid()
{
    //TODO! confirm IMaterialSchema node validity for cases in which type,
    // etc are not specified locally
    return !m_nodes.empty();
}


std::string MaterialFlatten::NetworkNode::getName()
{
    return m_name;
}

bool MaterialFlatten::NetworkNode::getTarget( std::string & oResult )
{
    for ( std::vector<IMaterialSchema::NetworkNode>::iterator i =
          m_nodes.begin(); i != m_nodes.end(); ++i)
    {
        if ( ( *i ).getTarget( oResult ) && !oResult.empty() )
        {
            return true;
        }
    }

    return false;
}


bool MaterialFlatten::NetworkNode::getNodeType( std::string & oResult )
{
    for ( std::vector<IMaterialSchema::NetworkNode>::iterator i =
          m_nodes.begin(); i != m_nodes.end(); ++i )
    {
        if ( ( *i ).getNodeType( oResult ) && !oResult.empty() )
        {
            return true;
        }
    }

    return false;
}

void
MaterialFlatten::NetworkNode::getParameters( ParameterEntryVector & oResult )
{
    oResult.clear();
    std::set<std::string> uniqueNames;

    if ( m_interfaceMappings )
    {
        for ( StringMap::iterator i = m_interfaceMappings->begin();
              i != m_interfaceMappings->end(); ++i )
        {
            const std::string & nodeParamName = (*i).first;
            const std::string & networkParamName = (*i).second;

            if ( uniqueNames.find( nodeParamName ) != uniqueNames.end() )
            {
                continue;
            }

            std::vector<Abc::ICompoundProperty>::iterator j;
            for ( j = m_networkParameters.begin();
                  j != m_networkParameters.end(); ++j )
            {
                Abc::ICompoundProperty & networkParams = ( *j );

                if ( const AbcCoreAbstract::PropertyHeader * header =
                     networkParams.getPropertyHeader( networkParamName ) )
                {
                    oResult.push_back( ParameterEntry( nodeParamName,
                                                       networkParams,
                                                       header ) );
                    uniqueNames.insert( nodeParamName );
                    break;
                }
            }
        }
    }

    for ( std::vector<IMaterialSchema::NetworkNode>::iterator i =
          m_nodes.begin(); i != m_nodes.end(); ++i )
    {
        IMaterialSchema::NetworkNode & node = ( *i );

        Abc::ICompoundProperty nodeParameters = node.getParameters();

        if (!nodeParameters.valid())
        {
            continue;
        }

        for ( size_t j = 0; j < nodeParameters.getNumProperties(); ++j)
        {
            const AbcCoreAbstract::PropertyHeader &propHeader =
                nodeParameters.getPropertyHeader( j );

            if ( uniqueNames.find(propHeader.getName()) != uniqueNames.end() )
            {
                continue;
            }

            uniqueNames.insert(propHeader.getName());

            oResult.push_back( ParameterEntry( propHeader.getName(),
                                               nodeParameters, &propHeader ) );
        }
    }
}


void MaterialFlatten::NetworkNode::getConnections( ConnectionVector & oResult )
{
    oResult.clear();

    std::set<std::string> uniqueNames;

    std::string inputName;
    std::string connectedNodeName;
    std::string connectedOutputName;

    for ( std::vector<IMaterialSchema::NetworkNode>::iterator i =
          m_nodes.begin(); i != m_nodes.end(); ++i )
    {
        IMaterialSchema::NetworkNode & node = ( *i );

        for (size_t j = 0, e = node.getNumConnections(); j < e; ++j)
        {
            if ( node.getConnection(j, inputName, connectedNodeName,
                                    connectedOutputName ) )
            {
                if ( uniqueNames.find( inputName ) != uniqueNames.end() )
                {
                    continue;
                }

                uniqueNames.insert( inputName );
                oResult.push_back( NetworkNode::Connection( inputName,
                    connectedNodeName, connectedOutputName ) );
            }
        }
    }

}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcMaterial
} // End namespace Alembic
