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

#ifndef _Alembic_AbcMaterial_OMaterial_h_
#define _Alembic_AbcMaterial_OMaterial_h_

#include <Alembic/Abc/All.h>

#include <Alembic/AbcMaterial/SchemaInfoDeclarations.h>

namespace Alembic {
namespace AbcMaterial {
namespace ALEMBIC_VERSION_NS {

//! Schema for writing shader definitions as either an object or
//! a compound property.
//! Only "monolithic" shader definitions (i.e. non network) are presently
//! supported in this implementation.
class OMaterialSchema : public Abc::OSchema<MaterialSchemaInfo>
{
public:

    //-------------------------------------------------------------------------
    //BOILERPLATE

    typedef OMaterialSchema this_type;

    OMaterialSchema() {}

    template <class CPROP_PTR>
    OMaterialSchema( CPROP_PTR iParent,
                     const std::string &iName,
                     const Abc::Argument &iArg0 = Abc::Argument(),
                     const Abc::Argument &iArg1 = Abc::Argument(),
                     const Abc::Argument &iArg2 = Abc::Argument() )
    : Abc::OSchema<MaterialSchemaInfo>( iParent, iName, iArg0, iArg1, iArg2 )
    {
        init();
    }

    template <class CPROP_PTR>
    explicit OMaterialSchema( CPROP_PTR iParent,
                              const Abc::Argument &iArg0 = Abc::Argument(),
                              const Abc::Argument &iArg1 = Abc::Argument(),
                              const Abc::Argument &iArg2 = Abc::Argument() )
    : Abc::OSchema<MaterialSchemaInfo>( iParent, iArg0, iArg1, iArg2 )
    {
        init();
    }

    //! Copy constructor.
    OMaterialSchema( const OMaterialSchema& iCopy )
    : Abc::OSchema<MaterialSchemaInfo>()
    {
        *this = iCopy;
    }

    //-------------------------------------------------------------------------
    //ACTUAL STUFF

    //! Declare shader for a given target and shaderType.
    //! "Target's" value is an agreed upon convention for a renderer
    //! or application (i.e. "prman")
    //! "ShaderType's" value is an agreed upon convention for shader terminals
    //! such as "surface," "displacement," "light", "coshader_somename."
    //! "ShaderName's" value is an identifier meaningful to the target
    //! application (i.e. "paintedplastic," "fancy_spot_light").
    //! It's only valid to call this once per target/shaderType combo.
    void setShader( const std::string & target,
                    const std::string & shaderType,
                    const std::string & shaderName );
    
    //! Declare and retrieve a container for storing properties representing
    //! parameters for the target and shaderType. You put them in there
    //! yourself since there are no restrictions on type or sampling other
    //! than what's appropriate and meaningful for the target application.
    //! Repeated calls will return the same object.
    Abc::OCompoundProperty getShaderParameters( const std::string & target,
                                                const std::string & shaderType);

    //add node by name, target, type, set node connection?, set node 
    //terminals? 

    void addNetworkNode( const std::string & nodeName,
                         const std::string & target,
                         const std::string & nodeType );

    void setNetworkNodeConnection( const std::string & nodeName,
                                   const std::string & inputName,
                                   const std::string & connectedNodeName,
                                   const std::string & connectedOutputName );

    Abc::OCompoundProperty
    getNetworkNodeParameters( const std::string & nodeName );

    void setNetworkTerminal( const std::string & target,
                             const std::string & shaderType,
                             const std::string & nodeName,
                             const std::string & outputName = "" );

    void
    setNetworkInterfaceParameterMapping( const std::string & interfaceParamName,
                                         const std::string & mapToNodeName,
                                         const std::string & mapToParamName );

    Abc::OCompoundProperty getNetworkInterfaceParameters();

protected:

    void init();

    // all the network nodes will be placed under this property
    Abc::OCompoundProperty m_node;
    void createNodeCompound();

    class Data;

    // shared and not scoped because we want this to survive a copy
    Util::shared_ptr< Data > m_data;
};

//! Object declaration
typedef Abc::OSchemaObject<OMaterialSchema> OMaterial;

typedef Util::shared_ptr< OMaterial > OMaterialPtr;

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcMaterial
} // End namespace Alembic


#endif

