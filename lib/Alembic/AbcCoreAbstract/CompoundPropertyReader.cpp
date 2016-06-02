//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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

#include <Alembic/AbcCoreAbstract/CompoundPropertyReader.h>
#include <Alembic/AbcCoreAbstract/ScalarPropertyReader.h>
#include <Alembic/AbcCoreAbstract/ArrayPropertyReader.h>

namespace Alembic {
namespace AbcCoreAbstract {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
CompoundPropertyReader::~CompoundPropertyReader()
{
    // Nothing
}

//-*****************************************************************************
size_t
CompoundPropertyReader::getNumProperties()
{
   return m_propertyReaders.size();
}

//-*****************************************************************************
const PropertyHeader &
CompoundPropertyReader::getPropertyHeader( size_t i )
{
   CompoundPropertyReaderMap::const_iterator propertyHeadersItr = m_propertyReaders.begin();
   std::advance(propertyHeadersItr, i);
   return (*propertyHeadersItr).second->getPropertyHeaderImpl( i );
}

//-*****************************************************************************
const PropertyHeader *
CompoundPropertyReader::getPropertyHeader( const std::string &iName )
{
   CompoundPropertyReaderMap::const_iterator findResult = m_propertyReaders.find(iName);
   if( findResult != m_propertyReaders.end() )
   {
       return (*findResult).second->getPropertyHeaderImpl( iName );
   }

   return 0;
}

//-*****************************************************************************
ScalarPropertyReaderPtr
CompoundPropertyReader::getScalarProperty( const std::string &iName )
{
   CompoundPropertyReaderMap::const_iterator findResult = m_propertyReaders.find(iName);
   if( findResult != m_propertyReaders.end() )
   {
       return (*findResult).second->getScalarPropertyImpl( iName );
   }

   return ScalarPropertyReaderPtr();
}

//-*****************************************************************************
ArrayPropertyReaderPtr
CompoundPropertyReader::getArrayProperty( const std::string &iName )
{
   CompoundPropertyReaderMap::const_iterator findResult = m_propertyReaders.find(iName);
   if( findResult != m_propertyReaders.end() )
   {
       return (*findResult).second->getArrayPropertyImpl( iName );
   }

   return ArrayPropertyReaderPtr();
}

//-*****************************************************************************
CompoundPropertyReaderPtr
CompoundPropertyReader::getCompoundProperty( const std::string &iName )
{
   CompoundPropertyReaderMap::const_iterator findResult = m_propertyReaders.find(iName);
   if( findResult != m_propertyReaders.end() )
   {
       return (*findResult).second->getCompoundPropertyImpl( iName );
   }

   return CompoundPropertyReaderPtr();
}

//-*****************************************************************************
BasePropertyReaderPtr
CompoundPropertyReader::getProperty( const std::string &iName )
{
    const PropertyHeader *header = getPropertyHeader( iName );
    if ( !header )
    {
        return BasePropertyReaderPtr();
    }
    else
    {
        switch ( header->getPropertyType() )
        {
        default:
        case kScalarProperty:
            return getScalarProperty( header->getName() );
        case kArrayProperty:
            return getArrayProperty( header->getName() );
        case kCompoundProperty:
            return getCompoundProperty( header->getName() );
        }
    }        
}

//-*****************************************************************************
ScalarPropertyReaderPtr
CompoundPropertyReader::getScalarProperty( size_t i )
{
    // This will throw if bad index.
    const PropertyHeader &header = getPropertyHeader( i );

    if ( header.getPropertyType() != kScalarProperty )
    {
        return ScalarPropertyReaderPtr();
    }
    else
    {
        return getScalarProperty( header.getName() );
    }
}

//-*****************************************************************************
ArrayPropertyReaderPtr
CompoundPropertyReader::getArrayProperty( size_t i )
{
    // This will throw if bad index.
    const PropertyHeader &header = getPropertyHeader( i );

    if ( header.getPropertyType() != kArrayProperty )
    {
        return ArrayPropertyReaderPtr();
    }
    else
    {
        return getArrayProperty( header.getName() );
    }
}

//-*****************************************************************************
CompoundPropertyReaderPtr
CompoundPropertyReader::getCompoundProperty( size_t i )
{
    // This will throw if bad index.
    const PropertyHeader &header = getPropertyHeader( i );

    if ( header.getPropertyType() != kCompoundProperty )
    {
        return CompoundPropertyReaderPtr();
    }
    else
    {
        return getCompoundProperty( header.getName() );
    }
}

//-*****************************************************************************
BasePropertyReaderPtr
CompoundPropertyReader::getProperty( size_t i )
{
    // This will throw if bad index.
    const PropertyHeader &header = getPropertyHeader( i );
    
    switch ( header.getPropertyType() )
    {
    default:
    case kScalarProperty:
        return getScalarProperty( header.getName() );
    case kArrayProperty:
        return getArrayProperty( header.getName() );
    case kCompoundProperty:
        return getCompoundProperty( header.getName() );
    }
}

//-*****************************************************************************
void CompoundPropertyReader::initializePropertyMaps(CompoundPropertyReaderPtr _sharedthis)
{
   const size_t numProperties = _sharedthis->getNumPropertiesImpl();

   for( size_t i = 0; i< numProperties; i++ )
   {
       const PropertyHeader &propertyHeader = _sharedthis->getPropertyHeaderImpl( i );
       m_propertyReaders[propertyHeader.getName()] = _sharedthis;
   }
}

//-*****************************************************************************
void CompoundPropertyReader::setPropertyReader(const std::string &iName, CompoundPropertyReaderPtr iReader)
{
   m_propertyReaders[iName] = iReader;
}

//-*****************************************************************************
void CompoundPropertyReader::layerProperties( CompoundPropertyReaderPtr layer )
{
   const size_t numLayerProperties = layer->getNumProperties();

   for( size_t i = 0; i< numLayerProperties; i++ )
   {
       const PropertyHeader &header = layer->getPropertyHeader( i );
       const std::string &name = header.getName();

       switch ( header.getPropertyType() )
       {
       case kCompoundProperty:
           {
               // Add in sub-properties of this compound property
               CompoundPropertyReaderMap::iterator itr = m_propertyReaders.find( name );
               if( itr != m_propertyReaders.end())
               {
                   CompoundPropertyReaderPtr baseSubCpr = (*itr).second->getCompoundProperty( name );
                   CompoundPropertyReaderPtr layerSubCpr =layer->getCompoundProperty( name );
                   baseSubCpr->layerProperties( layerSubCpr );
                   return;
               }

               //If this compound property doesn't exist in the base, just fall through and grab everything
               //from the layer's compound property
           }
       default:
           {
               this->setPropertyReader( header.getName(), layer );
           }
       }
   }

}

} // End namespace ALEMBIC_VERSION_NS
} // End namespace AbcCoreAbstract
} // End namespace Alembic
