//-*****************************************************************************
//
// Copyright (c) 2009-2015,
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

#ifndef Alembic_AbcMaterial_IMaterial_h
#define Alembic_AbcMaterial_IMaterial_h

#include <Alembic/Abc/All.h>
#include <Alembic/Util/Export.h>
#include <Alembic/AbcMaterial/SchemaInfoDeclarations.h>

namespace Alembic {
namespace AbcMaterial {
namespace ALEMBIC_VERSION_NS {

//! Schema for reading and querying shader definitions from either an object or
//! compound property.
//! Only "monolithic" shader definitions (i.e. non network) are presently
//! supported in this implementation.
class ALEMBIC_EXPORT IMaterialSchema
    : public Abc::ISchema<MaterialSchemaInfo>
{
public:

    //-------------------------------------------------------------------------
    //BOILERPLATE

    typedef IMaterialSchema this_type;

    IMaterialSchema() {}

    //! This constructor creates a new material reader.
    //! The first argument is the parent ICompoundProperty, from which the
    //! error handler policy for is derived.  The second argument is the name
    //! of the ICompoundProperty that contains this schemas properties.  The
    //! remaining optional arguments can be used to override the
    //! ErrorHandlerPolicy and to specify schema interpretation matching.
    IMaterialSchema( const ICompoundProperty &iParent,
                     const std::string &iName,
                     const Abc::Argument &iArg0 = Abc::Argument(),
                     const Abc::Argument &iArg1 = Abc::Argument() )

      : Abc::ISchema<MaterialSchemaInfo>( iParent, iName, iArg0, iArg1 )
    {
        init();
    }

    //! This constructor wraps an existing ICompoundProperty as the material
    //! reader, and the error handler policy is derived from it.
    //! The  remaining optional arguments can be used to override the
    //! ErrorHandlerPolicy and to specify schema interpretation matching.
    IMaterialSchema( const ICompoundProperty &iProp,
                     const Abc::Argument &iArg0 = Abc::Argument(),
                     const Abc::Argument &iArg1 = Abc::Argument() )
      : Abc::ISchema<MaterialSchemaInfo>( iProp, iArg0, iArg1 )
    {
        init();
    }

    //-------------------------------------------------------------------------
    //ACTUAL STUFF

    //! Fills a list of target name strings for which either shader or
    //! parameter definitions are locally present.
    //! Target name values match an upon convention for a renderer
    //! or application (i.e. "prman")
    //! This gets the target names for the monolithic shaders
    void getTargetNames( std::vector<std::string> & iTargetNames );

    //! Fills a list of shader type strings for which either shader or
    //! parameter definitions are locally present for a given target.
    //! Shader type names match an agreed upon convention for shader terminals
    //! such as "surface," "displacement," "light", "coshader_somename."
    void getShaderTypesForTarget( const std::string & iTargetName,
                                  std::vector<std::string> & oShaderTypeNames );

    //! Returns true and fills result with the shader name for a given
    //! target and shaderType if locally defined
    bool getShader( const std::string & iTarget,
                    const std::string & iShaderType,
                    std::string & oResult );

    //! Returns the enclosing compound property for the given target and
    //! shader type. Call .valid() on the result to determine whether it's
    //! locally defined or not.
    Abc::ICompoundProperty getShaderParameters(
        const std::string & iTarget, const std::string & iShaderType );

    //-------------------------------------------------------------------------

    class ALEMBIC_EXPORT NetworkNode
    {
    public:

        NetworkNode();
        NetworkNode( Abc::ICompoundProperty iCompound );
        NetworkNode( Abc::ICompoundProperty iParent,
                     const std::string & iNodeName );

        bool valid();

        std::string getName();

        bool getTarget( std::string & oResult );
        bool getNodeType( std::string & oResult );

        Abc::ICompoundProperty getParameters();

        size_t getNumConnections();
        bool getConnection( size_t iIndex,
                            std::string & oInputName,
                            std::string & oConnectedNodeName,
                            std::string & oConnectedOutputName );

        bool getConnection( const std::string & iInputName,
                            std::string & oConnectedNodeName,
                            std::string & oConnectedOutputName );

    private:
        Abc::ICompoundProperty m_compound;

        bool m_connectionsChecked;
        std::vector< std::string > m_connections;
        std::map< std::string, std::string > m_connectionsMap;

        void splitConnectionValue( const std::string & v,
                                   std::string & a,
                                   std::string & b );
    };

    size_t getNumNetworkNodes();
    void getNetworkNodeNames( std::vector<std::string> & oNames );

    NetworkNode getNetworkNode( size_t iIndex );
    NetworkNode getNetworkNode( const std::string & iNodeName );

    void getNetworkTerminalTargetNames(
        std::vector<std::string> & oTargetNames );

    void getNetworkTerminalShaderTypesForTarget(
        const std::string & iTargetName,
        std::vector<std::string> & oShaderTypeNames );

    bool getNetworkTerminal( const std::string & iTarget,
                             const std::string & iShaderType,
                             std::string & oNodeName,
                             std::string & oOutputName );

    size_t getNumNetworkInterfaceParameterMappings();
    bool getNetworkInterfaceParameterMapping( size_t iIndex,
                                              std::string & oInterfaceParamName,
                                              std::string & oMapToNodeName,
                                              std::string & oMapToParamName );

    void getNetworkInterfaceParameterMappingNames(
        std::vector<std::string> & oNames );

    bool getNetworkInterfaceParameterMapping(
        const std::string & iInterfaceParamName,
        std::string & oMapToNodeName,
        std::string & oMapToParamName );

    Abc::ICompoundProperty getNetworkInterfaceParameters();

protected:

private:

    void init();

    std::map<std::string, std::string> m_shaderNames;
    std::map<std::string, std::string> m_terminals;
    std::map<std::string, std::string> m_interfaceMap;
    std::vector<std::string> m_interface;

    Abc::ICompoundProperty m_interfaceParams;
    Abc::ICompoundProperty m_node;
};

//! Object declaration
typedef Abc::ISchemaObject<IMaterialSchema> IMaterial;

typedef Util::shared_ptr< IMaterial > IMaterialPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcMaterial
} // End namespace Alembic

#endif
