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

#ifndef _Alembic_AbcMaterial_MaterialFlatten_h_
#define _Alembic_AbcMaterial_MaterialFlatten_h_

#include <Alembic/AbcMaterial/IMaterial.h>

namespace Alembic {
namespace AbcMaterial {
namespace ALEMBIC_VERSION_NS {

//! Utility class for querying against flattened inheritance hierarchies
//! or definitions.
class MaterialFlatten
{
public:

    //! Create empty. Use append to add material schema manually
    MaterialFlatten();

    //! Create with a single materialSchema
    MaterialFlatten( IMaterialSchema materialSchema );

    //! Create from a material object. The schemas of matching parent
    //! material objects are automatically append
    MaterialFlatten( IMaterial materialObject );

    //! Create from an IObject. This will interpret values for hasMaterial
    //! and getMaterialAssignmentPath to flatten the full assignment
    //! and inheritance chain. A locally present material is first in the
    //! inheritance path. A material assignment is then appended with
    //! IMaterial rules.
    //!
    //! An alternate archive can be optionally specified. If provided,
    //! assigned material paths will be traversed within that archive
    //! instead of the archive of the object itself.
    MaterialFlatten( Abc::IObject object,
                     Abc::IArchive alternateSearchArchive=Abc::IArchive() );
    //TODO: need an append equivalent!

    //! Manually append a schema to the inheritance hierarchy
    void append( IMaterialSchema materialSchema );
    
    //! Append the schemas of matching parent material objects
    void append( IMaterial materialObject );
    
    
    //! Returns true is there are no schema in the inheritance path
    bool empty();
    
    //! Fill the list with a union of target names defined within
    //! the inheritance hierarchy
    void getTargetNames( std::vector<std::string> & targetNames );
    
    //! Fill the list with a union of shader types define for the specified
    //! target within the inheritance hierarchy
    void getShaderTypesForTarget( const std::string & targetName,
                                  std::vector<std::string> & shaderTypeNames );
    
    //! Returns true and fills result with the shader name of first defined
    //! for the target and shaderType within the inheritance hierarchy. False
    //! if not defined.
    bool getShader( const std::string & target,
                    const std::string & shaderType,
                    std::string & result );

    struct ParameterEntry
    {
        ParameterEntry()
        : header(0)
        {
        }

        ParameterEntry( const std::string & iName,
                        Abc::ICompoundProperty iParent,
                        const AbcCoreAbstract::PropertyHeader * iHeader )
        : name(iName)
        , parent(iParent)
        , header(iHeader)
        {
        }

        std::string name;
        Abc::ICompoundProperty parent;
        const AbcCoreAbstract::PropertyHeader * header;
    };

    typedef std::vector<ParameterEntry> ParameterEntryVector;

    //! Fills result with the parent compound and property header for 
    //! shader parameters defined for the target and shader type within
    //! the inheritance hierarchy. Shallower definitions mask deeper ones
    //! (i.e. you'll only get one entry for a given name)
    void getShaderParameters( const std::string & target,
                              const std::string & shaderType,
                              ParameterEntryVector & result );

    ///////////////////////////////////////////////////////////////////////////
    /// network stuff

    void getNetworkTerminalTargetNames(std::vector<std::string> & targetNames);
    void getNetworkTerminalShaderTypesForTarget( const std::string & targetName,
        std::vector<std::string> & shaderTypeNames );

    bool getNetworkTerminal( const std::string & target,
                             const std::string & shaderType,
                             std::string & nodeName,
                             std::string & outputName );

    typedef std::map<std::string, std::string> StringMap;
    typedef Alembic::Util::shared_ptr<StringMap> StringMapPtr;
    typedef std::vector<IMaterialSchema> SchemaVector;

    class NetworkNode
    {
    public:

        NetworkNode();

        bool valid();

        std::string getName();
        bool getTarget( std::string & result );
        bool getNodeType( std::string & result );
        void getParameters( ParameterEntryVector & result );
        
        struct Connection
        {
            Connection( const std::string & iInputName,
                        const std::string & iConnectedNodeName,
                        const std::string & iConnectedOutputName )
            : inputName( iInputName )
            , connectedNodeName( iConnectedNodeName )
            , connectedOutputName( iConnectedOutputName )
            {
            }

            std::string inputName;
            std::string connectedNodeName;
            std::string connectedOutputName;
        };

        typedef std::vector<Connection> ConnectionVector;

        void getConnections( ConnectionVector & result );

    private:

        friend class MaterialFlatten;

        NetworkNode( const std::string & name,
                     SchemaVector & schemas,
                     StringMapPtr interfaceMappings);

        std::string m_name;
        std::vector<IMaterialSchema::NetworkNode> m_nodes;
        std::vector<Abc::ICompoundProperty> m_networkParameters;
        StringMapPtr m_interfaceMappings;
    };

    size_t getNumNetworkNodes();
    NetworkNode getNetworkNode( size_t index );
    NetworkNode getNetworkNode( const std::string & nodeName );

    // TODO: no method to get the node names?

private:

    SchemaVector m_schemas;

    void flattenNetwork();

    bool m_networkFlattened;

    std::vector<std::string> m_nodeNames;
    typedef std::map<std::string, StringMapPtr> StringMapMap;
    StringMapMap m_nodesToInterfaceMappings;

};

}

using namespace ALEMBIC_VERSION_NS;

}
}

#endif
