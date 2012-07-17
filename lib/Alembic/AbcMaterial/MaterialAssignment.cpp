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

bool getMaterialAssignmentPath( Abc::IObject object,
                                std::string & result,
                                const std::string & propName )
{
    if ( !object.valid() )
    {
        return false;
    }

    return getMaterialAssignmentPath( object.getProperties(), result,
                                      propName );
}

bool getMaterialAssignmentPath( Abc::ICompoundProperty prop,
                                std::string & result,
                                const std::string & propName )
{
    if ( !prop.valid() )
    {
        return false;
    }

    if ( const Abc::PropertyHeader * header =
            prop.getPropertyHeader(propName) )
    {

        if ( header->isScalar() && Abc::IStringProperty::matches(*header) )
        {
            Abc::IStringProperty sprop( prop, propName );
            result = sprop.getValue();
            return true;
        }
    }

    return false;
}

bool hasMaterial( Abc::IObject object,
                  IMaterialSchema & result,
                  const std::string & propName)
{
    //don't indicate has-a for matching Material objects
    if ( object.valid() && propName == MATERIAL_PROPNAME )
    {
        if ( IMaterial::matches( object.getHeader() ) )
        {
            return false;
        }
    }

    return hasMaterial( object.getProperties(), result, propName );
}

bool hasMaterial( Abc::ICompoundProperty compound,
                  IMaterialSchema & result,
                  const std::string & propName )
{
    if ( !compound.valid() )
    {
        return false;
    }
    
    if ( const Abc::PropertyHeader * header =
            compound.getPropertyHeader(propName) )
    {
        if ( IMaterialSchema::matches(*header) )
        {
            result = IMaterialSchema( compound, propName );
            return true;
        }
    }

    return false;
}

void addMaterialAssignment( Abc::OObject object,
                            const std::string & value,
                            const std::string & propName )
{
    addMaterialAssignment( object.getProperties(), value, propName );
}

void addMaterialAssignment( Abc::OCompoundProperty prop,
                            const std::string & value,
                            const std::string & propName )
{
    Abc::OStringProperty assignProp( prop, propName );
    assignProp.set( value );
}

OMaterialSchema addMaterial( Abc::OObject object,
                             const std::string & propName )
{
    return addMaterial( object.getProperties(), propName );
}

OMaterialSchema addMaterial( Abc::OCompoundProperty prop,
                             const std::string & propName )
{
    return OMaterialSchema( prop, propName );
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcMaterial
} // End namespace Alembic

