//-*****************************************************************************
//
// Copyright (c) 2009-2013,
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

#include <Alembic/AbcCoreAbstract/ObjectReader.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
ObjectReader::~ObjectReader()
{
    // Nothing
}

//-*****************************************************************************
bool ObjectReader::getPropertiesHash( Util::Digest & oDigest )
{
    return false;
}

//-*****************************************************************************
bool ObjectReader::getChildrenHash( Util::Digest & oDigest )
{
    return false;
}

//-*****************************************************************************
void ObjectReader::initChildMap()
{
    const size_t numChildren = this->getNumChildrenImpl();

    for( size_t i = 0; i < numChildren; i++ )
    {
        ObjectReaderPtr child = this->getChildImpl( i );
        const std::string name = child->getName();
        m_children[name] = m_objectReaderPtrs.size();
        m_objectReaderPtrs.push_back( child );

        ObjectHeaderPtr headerPtr = ObjectHeaderPtr( new ObjectHeader( this->getChildHeaderImpl( i ) ) );
        m_objectHeaderPtrs.push_back( headerPtr );
    }
}

//-*****************************************************************************
size_t ObjectReader::getNumChildren()
{
   return m_children.size();
}

//-*****************************************************************************
const ObjectHeader & ObjectReader::getChildHeader( size_t i )
{
    return *(m_objectHeaderPtrs[ i ]);
}

//-*****************************************************************************
const ObjectHeader * ObjectReader::getChildHeader( const std::string &iName )
{
    IObjectChildMap::iterator findChild = m_children.find( iName );
   if( findChild != m_children.end() )
   {
       return m_objectHeaderPtrs[ findChild->second ].get();
   }

   return 0;
}

//-*****************************************************************************
ObjectReaderPtr ObjectReader::getChild( const std::string &iName )
{
    IObjectChildMap::iterator findChild = m_children.find( iName );
    if( findChild != m_children.end() )
    {
        return m_objectReaderPtrs[ findChild->second ];
    }

    return ObjectReaderPtr();
}

//-*****************************************************************************
ObjectReaderPtr ObjectReader::getChild( size_t i )
{
    return m_objectReaderPtrs[ i ];
}

//-*****************************************************************************
void ObjectReader::addChild( const ObjectHeader& iHeader, ObjectReaderPtr iReader )
{
   if( iReader )
   {
       if(iReader->getNumChildren() == 0)
           return;

       const std::string name = iReader->getName();
       ObjectHeaderPtr headerPtr = ObjectHeaderPtr( new ObjectHeader( iHeader ) );

       IObjectChildMap::iterator findChild = m_children.find( name );
       if( findChild == m_children.end() )
       {
           m_children[name] = m_objectReaderPtrs.size();
           m_objectReaderPtrs.push_back( iReader );
           m_objectHeaderPtrs.push_back( headerPtr );
       }
       else
       {
           m_objectReaderPtrs[findChild->second] = iReader;
           m_objectHeaderPtrs[findChild->second] = headerPtr;
       }
   }
}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreAbstract
} // End namespace Alembic
