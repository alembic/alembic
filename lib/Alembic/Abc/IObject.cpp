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

namespace {

const AbcA::ObjectHeader g_ohd;

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
    return g_ohd;
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
    if ( !m_instancedFullName.empty() )
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
std::string readInstanceSource( AbcA::CompoundPropertyReaderPtr iProp )
{
    if ( !iProp || !iProp->getPropertyHeader(".instanceSource") )
    {
        return std::string();
    }

    IStringProperty instanceSourceProp( iProp, ".instanceSource" );
    if ( !instanceSourceProp )
        return std::string();

    return instanceSourceProp.getValue();
}

static inline
AbcA::ObjectReaderPtr objectReaderByName( AbcA::ObjectReaderPtr iObj,
                                          const std::string & iInstanceSource );

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
        // we hit an instance so we have to evaluate down to the correct spot
        if ( child->getMetaData().get("isInstance") == "1" )
        {
            // get and recursively walk down this other path
            AbcA::CompoundPropertyReaderPtr prop = child->getProperties();
            std::string instanceSource = readInstanceSource( prop );
            child = objectReaderByName( child, instanceSource);
        }
        return recurse( child, iInstanceSource, nextSlash + 1 );
    }

    // child not found, or we are on our last child
    return child;
}

//-*****************************************************************************
static inline
AbcA::ObjectReaderPtr objectReaderByName( AbcA::ObjectReaderPtr iObj,
                                          const std::string & iInstanceSource )
{
    if ( iInstanceSource.empty() || ! iObj )
        return AbcA::ObjectReaderPtr();

    std::size_t curPos = 0;
    if ( iInstanceSource[0] == '/' )
    {
        curPos = 1;
    }

    AbcA::ObjectReaderPtr obj = iObj->getArchive()->getTop();
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
        std::string parentFullName = getParentFullName( m_instancedFullName );

        AbcA::ObjectReaderPtr parentPtr = m_object->getParent();
        bool setFullName = false;

        // if the instanced full name doesn't match the parents full name
        // then we have an instanced situation where we need to carefully
        // walk the hierarchy to make sure we end up with the correct parent

        // If the names do match, then the parent isn't a part of the instance
        // and so we don't need to set that full name
        if ( parentPtr && !parentFullName.empty() &&
             parentFullName != parentPtr->getFullName() )
        {
            parentPtr = objectReaderByName( parentPtr, parentFullName );
            setFullName = true;
        }

        IObject obj( parentPtr,
                     kWrapExisting,
                     getErrorHandlerPolicy() );

        if ( setFullName )
        {
            obj.setInstancedFullName( parentFullName );
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
                m_instancedFullName + std::string("/") + obj.getName() );
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
                m_instancedFullName + std::string("/") + obj.getName() );
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
bool IObject::isInstanceRoot() const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::isInstanceRoot()" );

    if ( m_instanceObject )
    {
        return true;
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return false;
}

//-*****************************************************************************
bool IObject::isInstanceDescendant() const
{
    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::isInstanceDescendant()" );

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

    if ( !m_instanceObject )
    {
        return std::string();
    }

    AbcA::CompoundPropertyReaderPtr props = m_instanceObject->getProperties();
    return readInstanceSource( props );

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
        return child.isInstanceRoot();
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
        return child.isInstanceRoot();
    }

    ALEMBIC_ABC_SAFE_CALL_END();

    return false;
}

//-*****************************************************************************
void IObject::init( IArchive & iArchive, const Argument &iArg0 )
{
    // Set the error handling policy
    getErrorHandler().setPolicy(
        GetErrorHandlerPolicy( iArchive, iArg0 ) );

    ALEMBIC_ABC_SAFE_CALL_BEGIN( "IObject::init( IArchive )" );

    m_object = iArchive.getTop().getPtr();

    ALEMBIC_ABC_SAFE_CALL_END_RESET();
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

    AbcA::CompoundPropertyReaderPtr propsPtr = m_object->getProperties();
    std::string instanceSource = readInstanceSource( propsPtr );
    AbcA::ObjectReaderPtr targetObject =
        objectReaderByName( m_object, instanceSource );

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
