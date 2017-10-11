//-*****************************************************************************
//
// Copyright (c) 2009-2011,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#include <Alembic/Abc/OObject.h>
#include <Alembic/Abc/OArchive.h>
#include <Alembic/Abc/OCompoundProperty.h>
#include <Alembic/Abc/OTypedScalarProperty.h>

namespace Alembic {
namespace Abc {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
OObject::OObject( OObject iParent,
                  const std::string &iName,
                  const Argument &iArg0,
                  const Argument &iArg1,
                  const Argument &iArg2 )
{
    init( iParent.getPtr(),
          iName,
          GetErrorHandlerPolicy( iParent ),
          iArg0, iArg1, iArg2 );
}

//-*****************************************************************************
OObject::~OObject()
{
    // Nothing for now.
    // Mostly here in case we need to add reference-counting debug code.
    //std::cout << "OObject::~OObject() name: "
    //          << m_object->getName()
    //          << std::endl
    //          << "\tUse count of writer ptr: "
    //          << m_object.use_count() << std::endl;
}

namespace {

const AbcA::ObjectHeader g_ohd;

}

//-*****************************************************************************
const AbcA::ObjectHeader &OObject::getHeader() const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::getHeader()" );

    if ( m_object )
    {
        return m_object->getHeader();
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, so have a default behavior.
    return g_ohd;
};

//-*****************************************************************************
OArchive OObject::getArchive()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::getArchive()" );

    if ( m_object )
    {
        return OArchive( m_object->getArchive(),
                         kWrapExisting,
                         getErrorHandlerPolicy() );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw. Have a default.
    return OArchive();
}

//-*****************************************************************************
OObject OObject::getParent()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::getParent()" );

    if ( m_object )
    {
        return OObject( m_object->getParent(),
                        kWrapExisting,
                        getErrorHandlerPolicy() );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw. Have a default.
    return OObject();
}

//-*****************************************************************************
size_t OObject::getNumChildren()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::getNumChildren()" );

    if ( m_object )
    {
        return m_object->getNumChildren();
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return 0;
}

//-*****************************************************************************
const AbcA::ObjectHeader &OObject::getChildHeader( size_t iIdx )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::getChildHeader()" );

    if ( m_object )
    {
        return m_object->getChildHeader( iIdx );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return g_ohd;
}

//-*****************************************************************************
const AbcA::ObjectHeader *OObject::getChildHeader( const std::string &iName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::getChildHeader()" );

    if ( m_object )
    {
        return m_object->getChildHeader( iName );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return NULL;
}

//-*****************************************************************************
OObject OObject::getChild( size_t iIdx )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::getChild( idx )" );

    if ( m_object )
    {
        AbcA::ObjectWriterPtr child = m_object->getChild( iIdx );
        return OObject( child,
                        kWrapExisting,
                        getErrorHandlerPolicy() );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return OObject();
}

//-*****************************************************************************
OObject OObject::getChild( const std::string &iName )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::getChild( name )" );

    if ( m_object )
    {
        return OObject( m_object->getChild( iName ),
                        kWrapExisting,
                        getErrorHandlerPolicy() );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return OObject();
}

//-*****************************************************************************
OCompoundProperty OObject::getProperties()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::getProperties()" );

    if ( m_object )
    {
        return OCompoundProperty( m_object->getProperties(), kWrapExisting );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return OCompoundProperty();
}

//-*****************************************************************************
bool OObject::addChildInstance( OObject iTarget, const std::string& iName )
{
    if ( !iTarget || !m_object )
        return false;

    if ( iName.empty() )
        return false;

    // Instance and source target must be in the same archive.
    if ( getArchive().getName() != iTarget.getArchive().getName() )
        return false;

    // Cannot instance an instance, an instance created by normal means wouldn't
    // even be gettable, so this is a check for extraordinary circumstances.
    if ( iTarget.getMetaData().get("isInstance") == "1" )
        return false;

    // Check that the instance target is not an ancestor of this object.
    std::string targetFullName = iTarget.getFullName();
    std::string childFullName  = getFullName() + "/" + iName;

    std::string targetPath = targetFullName + "/";
    if ( childFullName.find(targetPath) == 0 )
        return false;

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::addChildInstance()" );

    AbcA::MetaData md;
    md.set("isInstance", "1");

    OObject instanceChild = OObject( *this, iName, md );

    OStringProperty instanceProp =
        OStringProperty( instanceChild.getProperties(), ".instanceSource" );
    instanceProp.set( targetFullName );

    return true;

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return false;
}

void OObject::init( OArchive & iArchive,
                    const Argument &iArg0,
                    const Argument &iArg1,
                    const Argument &iArg2 )
{
    // Set the error handling policy
    getErrorHandler().setPolicy(
        GetErrorHandlerPolicy( iArchive.getPtr(), iArg0, iArg1, iArg2 ) );

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::init( OArchive )" );

    m_object = iArchive.getPtr()->getTop();

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

//-*****************************************************************************
void OObject::init( AbcA::ObjectWriterPtr iParent,
                    const std::string &iName,

                    ErrorHandler::Policy iParentPolicy,
                    const Argument &iArg0,
                    const Argument &iArg1,
                    const Argument &iArg2 )
{
    Arguments args( iParentPolicy );
    iArg0.setInto( args );
    iArg1.setInto( args );
    iArg2.setInto( args );

    getErrorHandler().setPolicy( args.getErrorHandlerPolicy() );

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "OObject::init()" );

    AbcA::ObjectHeader ohdr( iName, args.getMetaData() );
    m_object = iParent->createChild( ohdr );

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Abc
} // End namespace Alembic
