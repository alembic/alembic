//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <Alembic/Abc/IObject.h>
#include <Alembic/Abc/IArchive.h>
#include <Alembic/Abc/ICompoundProperty.h>
#include <Alembic/Abc/ITypedScalarProperty.h>

namespace Alembic {
namespace Abc {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
// Nothing at the moment, this is just here as a debug entry point for
// tracking down problems with reference counting.
IObject::~IObject()
{
    // Nothing for now.
    // Mostly here in case we need to add reference-counting debug code.
    //std::cout << "IObject::~IObject() name: "
    //          << m_object->getName()
    //          << std::endl
    //          << "\tUse count of writer ptr: "
    //          << m_object.use_count() << std::endl;
}


//-*****************************************************************************
const AbcA::ObjectHeader &IObject::getHeader() const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getHeader()" );

    if ( m_object )
    {
        return m_object->getHeader();
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, so have a default behavior.
    static const AbcA::ObjectHeader ohd;
    return ohd;
};

//-*****************************************************************************
const std::string &IObject::getName() const
{
    // Get the name of the original object
    if ( m_instanceObject )
    {
        return m_instanceObject->getHeader().getName();
    }

    return m_object->getHeader().getName();
}

//-*****************************************************************************
const std::string &IObject::getFullName() const
{
    if ( m_instanceObject )
    {
        return m_instancedFullName;
    }

    return getHeader().getFullName();
}

//-*****************************************************************************
IArchive IObject::getArchive() const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getArchive()" );

    // proxies and targets are currently required to be in the
    // same archive. Just use the m_object archive.
    if ( m_object )
    {
        return IArchive( m_object->getArchive(),
                         kWrapExisting,
                         getErrorHandlerPolicy() );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw. Have a default.
    return IArchive();
}

namespace { // anonymous

static inline
AbcA::ObjectReaderPtr recurse( AbcA::ObjectReaderPtr iObj,
                               const std::string & iInstanceSource,
                               std::size_t iCurPos )
{
    std::size_t nextSlash = iInstanceSource.find( '/', iCurPos );
    std::string childName;
    if ( nextSlash == std::string::npos )
    {
        childName = iInstanceSource.substr( iCurPos );
    }
    else
    {
        childName = iInstanceSource.substr( iCurPos, nextSlash - iCurPos );
    }

    AbcA::ObjectReaderPtr child = iObj->getChild( childName );

    if ( child && nextSlash != std::string::npos )
    {
        return recurse( child, iInstanceSource, nextSlash + 1 );
    }

    // child not found, or we are on our last child
    return child;
}

static inline
AbcA::ObjectReaderPtr objectReaderByName( IArchive iArchive,
                                          const std::string & iInstanceSource )
{
    if ( iInstanceSource.empty() )
        return AbcA::ObjectReaderPtr();

    std::size_t curPos = 0;
    if ( iInstanceSource[0] == '/' )
    {
        curPos = 1;
    }

    AbcA::ObjectReaderPtr obj = iArchive.getTop().getPtr();
    return recurse( obj, iInstanceSource, curPos );
}

//-*****************************************************************************
static inline
std::string getParentFullName( const std::string& iChildFullName )
{
    size_t pos = iChildFullName.rfind('/');

    if ( pos == std::string::npos || pos == 0 )
    {
        return std::string();
    }

    return iChildFullName.substr(0, pos);
}

}  // end anonymous namespace

//-*****************************************************************************
IObject IObject::getParent() const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getParent()" );

    if ( !m_instancedFullName.empty() )
    {
        AbcA::ObjectReaderPtr parentPtr;

        std::string parent = getParentFullName( m_instancedFullName );

        // true if this is the instance object that references another hierarchy
        if ( parent.empty() )
        {
            parentPtr = m_object->getParent();
        }
        else
        {
            parentPtr = objectReaderByName( getArchive(), parent );
        }

        IObject obj( parentPtr,
                     kWrapExisting,
                     getErrorHandlerPolicy() );

        // If we're the instance object, we don't pass the instance upward.
        if ( obj )
        {
            obj.setInstancedFullName( parent );
        }

        return obj;
    }
    else if ( m_object )
    {
        return IObject( m_object->getParent(),
                        kWrapExisting,
                        getErrorHandlerPolicy() );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw. Have a default.
    return IObject();
}

//-*****************************************************************************
size_t IObject::getNumChildren() const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getNumChildren()" );

    if ( m_object )
    {
        return m_object->getNumChildren();
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return 0;
}

//-*****************************************************************************
const AbcA::ObjectHeader &IObject::getChildHeader( size_t iIdx ) const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getChildHeader()" );

    if ( m_object )
    {
        return m_object->getChildHeader( iIdx );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    static const AbcA::ObjectHeader hd;
    return hd;
}

//-*****************************************************************************
const AbcA::ObjectHeader *
IObject::getChildHeader( const std::string &iName ) const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getChildHeader( name )" );

    if ( m_object )
    {
        return m_object->getChildHeader( iName );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return NULL;
}

//-*****************************************************************************
IObject IObject::getChild( size_t iChildIndex ) const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getChild()" );

    if ( m_object )
    {
        IObject obj( m_object->getChild( iChildIndex ),
                     kWrapExisting,
                     getErrorHandlerPolicy() );

        if ( !m_instancedFullName.empty() )
        {
            obj.setInstancedFullName(
                getFullName() + std::string("/") + obj.getName() );
        }

        return obj;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, return something in case.
    return IObject();
}

//-*****************************************************************************
IObject IObject::getChild( const std::string &iChildName ) const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getChild()" );

    if ( m_object )
    {
        IObject obj( m_object->getChild( iChildName ),
                     kWrapExisting,
                     getErrorHandlerPolicy() );

        if ( !m_instancedFullName.empty() )
        {
            obj.setInstancedFullName(
                getFullName() + std::string("/") + obj.getName() );
        }

        return obj;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, return something in case.
    return IObject();
}

//-*****************************************************************************
void IObject::reset()
{
    m_instanceObject.reset();
    m_instancedFullName.clear();

    m_object.reset();
    Base::reset();
}

//-*****************************************************************************
ICompoundProperty IObject::getProperties() const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getProperties()" );

    if ( m_object )
    {
        return ICompoundProperty( m_object->getProperties(), kWrapExisting );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return ICompoundProperty();
}

//-*****************************************************************************
bool IObject::getPropertiesHash( Util::Digest & oDigest )
{
   ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getPropertiesHash()" );

    if ( m_object )
    {
        return m_object->getPropertiesHash( oDigest );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return false;
}


//-*****************************************************************************
bool IObject::getChildrenHash( Util::Digest & oDigest )
{
   ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getChildrenHash()" );

    if ( m_object )
    {
        return m_object->getChildrenHash( oDigest );
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, have a default.
    return false;
}

//-*****************************************************************************
bool IObject::isInstance() const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::isInstance()" );

    if ( !m_instancedFullName.empty() )
    {
        return true;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return false;
}

//-*****************************************************************************
std::string IObject::instanceSourcePath()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::instanceSourcePath()" );

    AbcA::CompoundPropertyReaderPtr propsPtr = m_object->getProperties();
    if ( m_instanceObject )
    {
        propsPtr = m_instanceObject->getProperties();
    }
    ICompoundProperty props =ICompoundProperty( propsPtr, kWrapExisting );

    if ( props == 0 )
        return std::string();

    if ( props.getPropertyHeader(".instanceSource") == 0 )
        return std::string();

    IStringProperty instanceSourceProp( props, ".instanceSource" );
    if ( !instanceSourceProp )
        return std::string();

    return instanceSourceProp.getValue();

    ALEMBIC_ABC_SAFE_CALL_END();

    return std::string();
}

//-*****************************************************************************
void IObject::setInstancedFullName( const std::string& parentPath ) const
{
    m_instancedFullName = parentPath;
}

//-*****************************************************************************
bool IObject::isChildInstance( size_t iChildIndex ) const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IObject::isChildInstanced(size_t iChildIndex)" );

    IObject child = getChild( iChildIndex );

    if ( child.valid() )
    {
        return child.isInstance();
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return false;
}

//-*****************************************************************************
bool IObject::isChildInstance( const std::string &iChildName ) const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IObject::isChildInstance(const std::string &iChildName)" );

    IObject child = getChild( iChildName );

    if ( child.valid() )
    {
        return child.isInstance();
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return false;
}

//-*****************************************************************************
void IObject::init( AbcA::ObjectReaderPtr iParent,
                    const std::string &iName,
                    ErrorHandler::Policy iPolicy )
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::init()" );

    getErrorHandler().setPolicy( iPolicy );

    m_object = iParent->getChild( iName );

    ALEMBIC_ABC_SAFE_CALL_END();
}

//-*****************************************************************************
void IObject::initInstance()
{

    // not an instance so m_instanceObject will stay empty
    if ( !m_object || m_object->getMetaData().get("isInstance") != "1")
    {
        return;
    }

    AbcA::ObjectReaderPtr targetObject =
        objectReaderByName( getArchive(), instanceSourcePath() );

    m_instanceObject = m_object;
    m_object = targetObject;

    // initialize the full name to the instance full name
    if ( m_instanceObject != 0 )
    {
        m_instancedFullName = m_instanceObject->getFullName();
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Abc
} // End namespace Alembic
