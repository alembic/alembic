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

    if ( m_proxyObject )
    {
        return m_proxyObject->getHeader();
    }
    else if ( m_object )
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
    // Whether or not this IObject is a proxy object, we want to use the
    // name from the original object.
    return m_object->getHeader().getName();
}

//-*****************************************************************************
const std::string &IObject::getFullName() const
{
    if ( m_isProxy )
        return m_proxyFullName;

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

//-*****************************************************************************
static inline
std::string getParentFullName(const std::string& childFullName)
{
    size_t pos = childFullName.rfind('/');
    if (pos == std::string::npos || pos == 0)
        return "";

    return childFullName.substr(0, pos);
}

static inline
std::vector<std::string> splitPath(std::string path)
{
    std::vector<std::string> split;
    std::stringstream ss(path);

    std::string item;
    while (std::getline(ss, item, '/'))
    {
        if (item.empty())
            continue;

        split.push_back(item);
    }

    return split;
}

static inline
AbcA::ObjectReaderPtr recurse(Alembic::Abc::IObject iobject,
                              std::vector<std::string> path)
{
    if (path.size() == 0)
        return iobject.m_object;

    for (std::size_t i=0; i<iobject.getNumChildren(); ++i)
    {
        Alembic::Abc::IObject ichild = iobject.getChild(i);
        if (ichild.getName() == path[0])
        {
            path.erase( path.begin() );
            return recurse(ichild, path);
        }
    }

    return AbcA::ObjectReaderPtr();
}

static inline
AbcA::ObjectReaderPtr objectReaderByName(IArchive archive, const std::string& proxyTarget)
{
    // split the path up into individual names
    std::vector<std::string> path = splitPath(proxyTarget);
    if (path.size() == 0)
        return AbcA::ObjectReaderPtr();

    for (std::size_t i=0; i<archive.getTop().getNumChildren(); ++i)
    {
        Alembic::Abc::IObject iobject = archive.getTop().getChild(i);
        if (iobject.getName() == path[0])
        {
            path.erase( path.begin() );
            return recurse(iobject, path);
        }
    }

    return AbcA::ObjectReaderPtr();
}

//-*****************************************************************************
IObject IObject::getParent() const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getParent()" );

    if ( m_isProxy && !isProxyObject() )
    {
        AbcA::ObjectReaderPtr parentPtr;

        std::string parent = getParentFullName( getFullName() );

        // true if this is the proxy object that references another hierarchy
        if ( parent.empty() )
        {
            parentPtr = m_object->getParent();
        }
        else
        {
            parentPtr = objectReaderByName( getArchive(), parent );
        }

        IObject iobj( parentPtr,
                      kWrapExisting,
                      getErrorHandlerPolicy() );

        //
        // If we're the proxy object, we don't pass the proxiness upward.
        if ( iobj )
        {
            iobj.setIsProxy( true );
            iobj.setProxyFullName( parent );
        }

        return iobj;
    }
    else if ( m_object )
    {
        IObject iobj( m_object->getParent(),
                      kWrapExisting,
                      getErrorHandlerPolicy() );

        if ( m_isProxy && !isProxyObject() )
        {
            iobj.setIsProxy( true );
            iobj.setProxyFullName( getParentFullName( getFullName() ) );
        }

        return iobj;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw. Have a default.
    return IObject();
}

//-*****************************************************************************
size_t IObject::getNumChildren() const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getNumChildren()" );

    if ( m_proxyObject )
    {
        return m_proxyObject->getNumChildren();
    }
    else if ( m_object )
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

    if ( m_proxyObject )
    {
        return m_proxyObject->getChildHeader( iIdx );
    }
    else if ( m_object )
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

    if ( m_proxyObject )
    {
        return m_object->getChildHeader( iName );
    }
    else if ( m_object )
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

    if ( m_proxyObject )
    {
        IObject iobj( m_proxyObject->getChild( iChildIndex ),
                      kWrapExisting,
                      getErrorHandlerPolicy() );

        iobj.setIsProxy( true );
        iobj.setProxyFullName( getFullName() + std::string("/") + iobj.getName() );

        return iobj;
    }
    else if ( m_object )
    {
        IObject iobj( m_object->getChild( iChildIndex ),
                      kWrapExisting,
                      getErrorHandlerPolicy() );

        if ( m_isProxy )
        {
            iobj.setIsProxy( true );
            iobj.setProxyFullName( getFullName() + std::string("/") + iobj.getName() );
        }

        return iobj;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, return something in case.
    return IObject();
}

//-*****************************************************************************
IObject IObject::getChild( const std::string &iChildName ) const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getChild()" );

    if ( m_proxyObject )
    {
        IObject iobj( m_proxyObject->getChild( iChildName ),
                      kWrapExisting,
                      getErrorHandlerPolicy() );

        iobj.setIsProxy( true );
        iobj.setProxyFullName( getFullName() + std::string("/") + iobj.getName() );

        return iobj;
    }
    else if ( m_object )
    {
        IObject iobj( m_object->getChild( iChildName ),
                      kWrapExisting,
                      getErrorHandlerPolicy() );

        if ( m_isProxy )
        {
            iobj.setIsProxy( true );
            iobj.setProxyFullName( getFullName() + std::string("/") + iobj.getName() );
        }

        return iobj;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    // Not all error handlers throw, return something in case.
    return IObject();
}

//-*****************************************************************************
AbcA::ObjectReaderPtr IObject::getPtr() const
{
    if ( m_proxyObject )
        return m_proxyObject;

    return m_object;
}

//-*****************************************************************************
void IObject::reset()
{
    m_isProxy = false;
    m_proxyObject.reset();
    m_proxyFullName.clear();

    m_object.reset();
    Base::reset();
}

//-*****************************************************************************
ICompoundProperty IObject::getProperties() const
{

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::getProperties()" );

    if ( m_proxyObject )
    {
        return ICompoundProperty( m_proxyObject->getProperties(), kWrapExisting );
    }
    else if ( m_object )
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
std::string IObject::proxyTargetPath()
{
    //
    // Don't want to use getProperties(), that will just return the proxy's properties.
    ICompoundProperty props = ICompoundProperty( m_object->getProperties(), kWrapExisting );
    if (props == 0)
        return "";

    if (props.getPropertyHeader(".proxyTarget") == 0)
        return "";

    IStringProperty proxyTargetProperty(props, ".proxyTarget");
    if (!proxyTargetProperty)
        return "";

    ISampleSelector iss( (index_t)0 );
    return proxyTargetProperty.getValue(iss);
}

//-*****************************************************************************
void IObject::setProxyFullName(const std::string& parentPath) const
{
    m_proxyFullName = parentPath;
}

bool IObject::isProxyObject() const
{
    Alembic::Abc::ICompoundProperty props = ICompoundProperty( m_object->getProperties(),
                                                               kWrapExisting );
    if (props == 0)
        return false;

    return ( props.getPropertyHeader(".proxyTarget") != 0 );
}

//-*****************************************************************************
bool IObject::isChildAProxy(size_t iChildIndex) const
{
    IObject child = getChild(iChildIndex);
    if (child)
        return child.isProxy();

    return false;
}

//-*****************************************************************************
bool IObject::isChildAProxy(const std::string &iChildName) const
{
    IObject child = getChild(iChildName);
    if (child)
        return child.isProxy();

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
    if (m_proxyObject != 0)
        return;

    m_isProxy = false;

    ICompoundProperty props = getProperties();
    if (props && props.getPropertyHeader(".proxyTarget") != 0)
        m_proxyObject = objectReaderByName( getArchive(), proxyTargetPath() );

    //
    // An IObject with its own proxyObject is definitely a proxy.
    if (m_proxyObject != 0)
    {
        m_isProxy = true;
        m_proxyFullName = m_object->getFullName();
    }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace Abc
} // End namespace Alembic
