//
//  Copyright (c) 2009, Tweak Software
//  All rights reserved.
// 
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
//     * Neither the name of the Tweak Software nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
// 
//  THIS SOFTWARE IS PROVIDED BY Tweak Software ''AS IS'' AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL Tweak Software BE LIABLE FOR
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
//  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//

#include "ObjectVector.h"
#include <algorithm>

namespace GtoContainer {

//-*****************************************************************************
void ObjectVector::deleteContents()
{
    for ( iterator iter = begin(); iter != end(); ++iter )
    {
        delete (*iter);
    }
    
    erase( begin(), end() );
}

//-*****************************************************************************
void ObjectVector::removeWithoutDeleting( const PropertyContainer *pc )
{
    iterator startOfFind = begin();
    while( 1 )
    {
        iterator found = std::find( startOfFind, end(), pc );
        if ( found == end() )
        {
            break;
        }

        startOfFind = found;
        ++startOfFind;

        erase( found );
    }
}

//-*****************************************************************************
//-*****************************************************************************
// PROTOCOL-BASED SEARCH FUNCTIONS
//-*****************************************************************************
//-*****************************************************************************
PropertyContainer *ObjectVector::findFirstOfProtocol( const Protocol &p )
{
    for ( iterator iter = begin(); iter != end(); ++iter )
    {
        if ( (*iter)->protocol() == p )
        {
            return (*iter);
        }
    }

    return NULL;
}

//-*****************************************************************************
const PropertyContainer *
ObjectVector::findFirstOfProtocol( const Protocol &p ) const
{
    for ( const_iterator iter = begin(); iter != end(); ++iter )
    {
        if ( (*iter)->protocol() == p )
        {
            return (*iter);
        }
    }

    return NULL;
}

//-*****************************************************************************
void ObjectVector::findAllOfProtocol( const Protocol &p,
                                      std::vector<PropertyContainer *> &into )
{
    for ( iterator iter = begin(); iter != end(); ++iter )
    {
        if ( (*iter)->protocol() == p )
        {
            into.push_back( (*iter) );
        }
    }
}


//-*****************************************************************************
void
ObjectVector::findAllOfProtocol(
    const Protocol &p,
    std::vector<const PropertyContainer *> &into ) const
{
    for ( const_iterator iter = begin(); iter != end(); ++iter )
    {
        if ( (*iter)->protocol() == p )
        {
            into.push_back( (*iter) );
        }
    }
}


//-*****************************************************************************
//-*****************************************************************************
// NAME-BASED SEARCH FUNCTIONS
//-*****************************************************************************
//-*****************************************************************************
PropertyContainer *ObjectVector::findFirstOfName( const std::string &nme )
{
    for ( iterator iter = begin(); iter != end(); ++iter )
    {
        if ( (*iter)->name() == nme )
        {
            return (*iter);
        }
    }

    return NULL;
}

//-*****************************************************************************
const PropertyContainer *
ObjectVector::findFirstOfName( const std::string &nme ) const
{
    for ( const_iterator iter = begin(); iter != end(); ++iter )
    {
        if ( (*iter)->name() == nme )
        {
            return (*iter);
        }
    }

    return NULL;
}

//-*****************************************************************************
void ObjectVector::findAllOfName( const std::string &nme,
                                  std::vector<PropertyContainer *> &into )
{
    for ( iterator iter = begin(); iter != end(); ++iter )
    {
        if ( (*iter)->name() == nme )
        {
            into.push_back( (*iter) );
        }
    }
}


//-*****************************************************************************
void
ObjectVector::findAllOfName(
    const std::string &nme,
    std::vector<const PropertyContainer *> &into ) const
{
    for ( const_iterator iter = begin(); iter != end(); ++iter )
    {
        if ( (*iter)->name() == nme )
        {
            into.push_back( (*iter) );
        }
    }
}

} // End namespace GtoContainer
