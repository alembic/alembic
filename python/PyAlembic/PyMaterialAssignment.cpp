//-*****************************************************************************
//
// Copyright (c) 2012,
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

#include <Foundation.h>
#include <PyISchema.h>
#include <PyISchemaObject.h>
#include <PyTypeBindingUtil.h>

using namespace boost::python;

//-*****************************************************************************
void register_materialassignment()
{
    // addMaterialAssignment
    //
    void ( *addMaterialAssignmentToObject )( 
            Abc::OObject iObject, 
            const std::string & iValue,
            const std::string & iPropName
            ) = &AbcM::addMaterialAssignment;
    void ( *addMaterialAssignmentToProperty )( 
            Abc::OCompoundProperty iProp, 
            const std::string & iValue,
            const std::string & iPropName
            ) = &AbcM::addMaterialAssignment;

    def( "addMaterialAssignment", 
         addMaterialAssignmentToObject,
         ( arg( "iObject" ), 
           arg( "iValue" ), 
           arg( "iPropName" ) = MATERIALASSIGN_PROPNAME )
       );
    def( "addMaterialAssignment", 
         addMaterialAssignmentToProperty,
         ( arg( "iProp" ), 
           arg( "iValue" ), 
           arg( "iPropName" ) = MATERIALASSIGN_PROPNAME )
       );

    // addMaterial
    //
    AbcM::OMaterialSchema ( *addMaterialToObject )( 
            Abc::OObject iObject, 
            const std::string & iPropName
            ) = &AbcM::addMaterial;
    AbcM::OMaterialSchema ( *addMaterialToProperty )( 
            Abc::OCompoundProperty iProp, 
            const std::string & iPropName
            ) = &AbcM::addMaterial;

    def( "addMaterial", 
         addMaterialToObject,
         ( arg( "iObject" ), 
           arg( "iPropName" ) = MATERIAL_PROPNAME )
       );
    def( "addMaterial", 
         addMaterialToProperty,
         ( arg( "iProp" ), 
           arg( "iPropName" ) = MATERIAL_PROPNAME )
       );

    // getMaterialAssignmentPath
    //
    struct GetOverloads
    {
        static std::string getMaterialAssignmentPathFromObject (
                Abc::IObject iObject,
                const std::string iPropName
                ) 
        {
            std::string oResult;
            AbcM::getMaterialAssignmentPath(
                    iObject, oResult, iPropName
                    );
            return oResult;
        };

        static std::string getMaterialAssignmentPathFromProperty (
                Abc::ICompoundProperty iProp,
                const std::string iPropName
                ) 
        {
            std::string oResult;
            AbcM::getMaterialAssignmentPath(
                    iProp, oResult, iPropName
                    );
            return oResult;
        };

    };
    
    def( "getMaterialAssignmentPath", 
         &GetOverloads::getMaterialAssignmentPathFromObject,
         ( arg( "iObject" ), 
           arg( "iPropName" ) = MATERIALASSIGN_PROPNAME )
       );
    def( "getMaterialAssignmentPath", 
         &GetOverloads::getMaterialAssignmentPathFromProperty,
         ( arg( "iProp" ), 
           arg( "iPropName" ) = MATERIALASSIGN_PROPNAME )
       );

    // hasMaterial
    //
    struct HasOverloads
    {
        static AbcM::IMaterialSchema hasMaterialOnObject (
                Abc::IObject iObject,
                const std::string iPropName
                ) 
        {
            AbcM::IMaterialSchema oResult;
            if (AbcM::hasMaterial( iObject, oResult, iPropName ))
            {
                return oResult;
            };
            return AbcM::IMaterialSchema();
        };

        static AbcM::IMaterialSchema hasMaterialOnProperty (
                Abc::ICompoundProperty iProp,
                const std::string iPropName
                ) 
        {
            AbcM::IMaterialSchema oResult;
            if (AbcM::hasMaterial( iProp, oResult, iPropName ))
            {
                return oResult;
            };
            return AbcM::IMaterialSchema();
        };

    };
    
    def( "hasMaterial", 
         &HasOverloads::hasMaterialOnObject,
         ( arg( "iObject" ), 
           arg( "iPropName" ) = MATERIAL_PROPNAME )
       );
    def( "hasMaterial", 
         &HasOverloads::hasMaterialOnProperty,
         ( arg( "iProp" ), 
           arg( "iPropName" ) = MATERIAL_PROPNAME )
       );

}
