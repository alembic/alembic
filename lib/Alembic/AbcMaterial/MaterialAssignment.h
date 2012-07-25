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


#ifndef _Alembic_AbcMaterial_MaterialAssignment_h_
#define _Alembic_AbcMaterial_MaterialAssignment_h_

#include <Alembic/AbcMaterial/IMaterial.h>
#include <Alembic/AbcMaterial/OMaterial.h>

namespace Alembic {
namespace AbcMaterial {
namespace ALEMBIC_VERSION_NS {

#define MATERIALASSIGN_PROPNAME ".material.assign"
#define MATERIAL_PROPNAME ".material"

//! Adds a material assignment to an object. The value should be
//! the absolute path to an Material object.
//! (The default interpretation is that this path is to an object within
//! the same archive.)
//!
//! The default value for "propName" is officially supported convention
//! of the library. You may provide an alternative value for custom
//! solutions that want to make use of this convenience.
void addMaterialAssignment(
        Abc::OObject iObject,
        const std::string & iValue,
        const std::string & iPropName = MATERIALASSIGN_PROPNAME );

//! Adds a material assignment within a compound property. The conventions
//! and concerns are identical as with applying to an object. This exists
//! primarily for custom solutions which want to make use of the convenience
//! within nested compounds.
void addMaterialAssignment(
        Abc::OCompoundProperty iProp,
        const std::string & iValue,
        const std::string & iPropName = MATERIALASSIGN_PROPNAME );

//! Adds a local material schema to any object. This is intended for
//! "has-a" cases such as "this polymesh has a material locally defined."
//!
//! The default value for "propName" is officially supported convention
//! of the library. You may provide an alternative value for custom
//! solutions that want to make use of this convenience.
OMaterialSchema addMaterial(
    Abc::OObject iObject,
    const std::string & iPropName = MATERIAL_PROPNAME );

//! Adds a local material schema within any compound. This is intended
//! for the "has-a" cases for nested compound properties.
OMaterialSchema addMaterial(
    Abc::OCompoundProperty iProp,
    const std::string & iPropName = MATERIAL_PROPNAME );


//! Returns true and fills result with the value of previously defined
//! material assignment path. The default interpretation of the value
//! is as a full path to an IMaterial object within the same archive.
//!
//! The default value for "propName" is officially supported convention
//! of the library. You may provide an alternative value for custom
//! solutions that want to make use of this convenience.
bool getMaterialAssignmentPath(
    Abc::IObject iObject,
    std::string & oResult,
    const std::string & iPropName = MATERIALASSIGN_PROPNAME );

//! Returns true and fills result with the value of previously defined
//! material assignment path within a given compound property.
//!
//! The common case is at the object level but this is here as a convenince
//! for custom solutions
bool getMaterialAssignmentPath(
    Abc::ICompoundProperty iProp,
    std::string & oResult,
    const std::string & iPropName = MATERIALASSIGN_PROPNAME );

//! Returns true and fills result a previously defined local material
//! on this object following the "has a" pattern.
//!
//! The default value for "propName" is officially supported convention
//! of the library. You may provide an alternative value for custom
//! solutions that want to make use of this convenience.
bool hasMaterial(
        Abc::IObject iObject, 
        IMaterialSchema & oResult,
        const std::string & iPropName = MATERIAL_PROPNAME );

//! Returns true and fills result a previously defined local material
//! within the given compound property. This is useful for solutions
//! in which another schema wants to contain a material definition
//! but not advertise it as such at the object level
bool hasMaterial(
        Abc::ICompoundProperty iCompound,
        IMaterialSchema & oResult,
        const std::string & iPropName = MATERIAL_PROPNAME );


} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcMaterial
} // End namespace Alembic

#endif
