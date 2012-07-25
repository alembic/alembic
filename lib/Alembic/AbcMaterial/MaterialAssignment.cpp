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

#include <Alembic/AbcMaterial/MaterialAssignment.h>

namespace Alembic {
namespace AbcMaterial {
namespace ALEMBIC_VERSION_NS {

bool getMaterialAssignmentPath( Abc::IObject iObject,
                                std::string & oResult,
                                const std::string & iPropName )
{
    if ( !iObject.valid() )
    {
        return false;
    }

    return getMaterialAssignmentPath( iObject.getProperties(), oResult,
                                      iPropName );
}

bool getMaterialAssignmentPath( Abc::ICompoundProperty iProp,
                                std::string & oResult,
                                const std::string & iPropName )
{
    if ( !iProp.valid() )
    {
        return false;
    }

    if ( const AbcCoreAbstract::PropertyHeader * header =
            iProp.getPropertyHeader( iPropName ) )
    {

        if ( header->isScalar() && Abc::IStringProperty::matches(*header) )
        {
            Abc::IStringProperty sprop( iProp, iPropName );
            oResult = sprop.getValue();
            return true;
        }
    }

    return false;
}

bool hasMaterial( Abc::IObject iObject,
                  IMaterialSchema & oResult,
                  const std::string & iPropName)
{
    //don't indicate has-a for matching Material objects
    if ( iObject.valid() && iPropName == MATERIAL_PROPNAME )
    {
        if ( IMaterial::matches( iObject.getHeader() ) )
        {
            return false;
        }
    }

    return hasMaterial( iObject.getProperties(), oResult, iPropName );
}

bool hasMaterial( Abc::ICompoundProperty iCompound,
                  IMaterialSchema & oResult,
                  const std::string & iPropName )
{
    if ( !iCompound.valid() )
    {
        return false;
    }
    
    if ( const AbcCoreAbstract::PropertyHeader * header =
            iCompound.getPropertyHeader( iPropName ) )
    {
        if ( IMaterialSchema::matches( *header ) )
        {
            oResult = IMaterialSchema( iCompound, iPropName );
            return true;
        }
    }

    return false;
}

void addMaterialAssignment( Abc::OObject iObject,
                            const std::string & iValue,
                            const std::string & iPropName )
{
    addMaterialAssignment( iObject.getProperties(), iValue, iPropName );
}

void addMaterialAssignment( Abc::OCompoundProperty iProp,
                            const std::string & iValue,
                            const std::string & iPropName )
{
    Abc::OStringProperty assignProp( iProp, iPropName );
    assignProp.set( iValue );
}

OMaterialSchema addMaterial( Abc::OObject iObject,
                             const std::string & iPropName )
{
    return addMaterial( iObject.getProperties(), iPropName );
}

OMaterialSchema addMaterial( Abc::OCompoundProperty iProp,
                             const std::string & iPropName )
{
    return OMaterialSchema( iProp, iPropName );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcMaterial
} // End namespace Alembic

