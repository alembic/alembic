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
    if ( m_proxyObject )
    {
        return m_proxyObject->getHeader().getName();
    }

    return m_object->getHeader().getName();
}

//-*****************************************************************************
const std::string &IObject::getFullName() const
{
    if ( m_proxyObject )
    {
        return m_proxyFullName;
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
                               const std::string & iProxyTarget,
                               std::size_t iCurPos )
{
    std::size_t nextSlash = iProxyTarget.find( '/', iCurPos );
    std::string childName;
    if ( nextSlash == std::string::npos )
    {
        childName = iProxyTarget.substr( iCurPos );
    }
    else
    {
        childName = iProxyTarget.substr( iCurPos, nextSlash - iCurPos );
    }

    AbcA::ObjectReaderPtr child = iObj->getChild( childName );

    if ( child && nextSlash != std::string::npos )
    {
        return recurse( child, iProxyTarget, nextSlash + 1 );
    }

    // child not found, or we are on our last child
    return child;
}

static inline
AbcA::ObjectReaderPtr objectReaderByName( IArchive iArchive,
                                          const std::string & iProxyTarget)
{
    if ( iProxyTarget.empty() )
        return AbcA::ObjectReaderPtr();

    std::size_t curPos = 0;
    if ( iProxyTarget[0] == '/' )
    {
        curPos = 1;
    }

    AbcA::ObjectReaderPtr obj = iArchive.getTop().getPtr();
    return recurse( obj, iProxyTarget, curPos );
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

    if ( !m_proxyFullName.empty() )
    {
        AbcA::ObjectReaderPtr parentPtr;

        std::string parent = getParentFullName( m_proxyFullName );

        // true if this is the proxy object that references another hierarchy
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

        // If we're the proxy object, we don't pass the proxiness upward.
        if ( obj )
        {
            obj.setProxyFullName( parent );
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

        if ( !m_proxyFullName.empty() )
        {
            obj.setProxyFullName(
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

        if ( !m_proxyFullName.empty() )
        {
            obj.setProxyFullName(
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
    m_proxyObject.reset();
    m_proxyFullName.clear();

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
bool IObject::isProxy() const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::isProxy()" );

    if ( !m_proxyFullName.empty() )
    {
        return true;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return false;
}

//-*****************************************************************************
std::string IObject::proxyTargetPath()
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::proxyTargetPath()" );

    AbcA::CompoundPropertyReaderPtr propsPtr = m_object->getProperties();
    if ( m_proxyObject )
    {
        propsPtr = m_proxyObject->getProperties();
    }
    ICompoundProperty props =ICompoundProperty( propsPtr, kWrapExisting );

    if ( props == 0 )
        return std::string();

    if ( props.getPropertyHeader(".proxyTarget") == 0 )
        return std::string();

    IStringProperty proxyTargetProperty( props, ".proxyTarget" );
    if ( !proxyTargetProperty )
        return std::string();

    return proxyTargetProperty.getValue();

    ALEMBIC_ABC_SAFE_CALL_END();

    return std::string();
}

//-*****************************************************************************
void IObject::setProxyFullName( const std::string& parentPath ) const
{
    m_proxyFullName = parentPath;
}

//-*****************************************************************************
bool IObject::isChildAProxy( size_t iChildIndex ) const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::isChildAProxy(size_t iChildIndex)" );

    IObject child = getChild( iChildIndex );

    if ( child.valid() )
    {
        return child.isProxy();
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return false;
}

//-*****************************************************************************
bool IObject::isChildAProxy( const std::string &iChildName ) const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN(
        "IObject::isChildAProxy(const std::string &iChildName)" );

    IObject child = getChild( iChildName );

    if ( child.valid() )
    {
        return child.isProxy();
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
void IObject::initProxy()
{

    // not a proxy so m_proxyObject will stay empty
    if ( !m_object || m_object->getMetaData().get("proxy") != "1")
    {
        return;
    }

    AbcA::ObjectReaderPtr targetObject =
        objectReaderByName( getArchive(), proxyTargetPath() );

    m_proxyObject = m_object;
    m_object = targetObject;

    // initialize the full name to the proxy full name
    if ( m_proxyObject != 0 )
    {
        m_proxyFullName = m_proxyObject->getFullName();
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Abc
} // End namespace Alembic
