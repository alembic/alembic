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

#ifndef _GtoContainer_ObjectVector_h_
#define _GtoContainer_ObjectVector_h_

//-*****************************************************************************
// The ObjectVector is just a big list of PropertyContainers. This is what you
// pass to GTOWriter, and what you receive from GTOReader.
//
// If you have a fancy scene class, you can extract the pointers out of this
// class for your scene, and similarly, just iterate over your scene to
// create an ObjectVector for writing.
//-*****************************************************************************

#include <GtoContainer/PropertyContainer.h>
#include <vector>
#include <algorithm>

namespace GtoContainer {

//-*****************************************************************************
// Note that deleting the ObjectVector does NOT delete the contents.
// There is a deleteContents operator for exactly that purpose.
//
// This is just derived from the STL vector, so you can use all of those
// member functions to traverse. The primary functions will be extracted here.
class ObjectVector : public std::vector<PropertyContainer *>
{
public:
    ObjectVector() : std::vector<PropertyContainer *>() {}

    // Destruction does NOT delete the contents.
    ~ObjectVector() {}

    // Delete the contents.
    void deleteContents();

    // Remove a PropertyContainer without deleting.
    // Removes all copies of it from this list.
    void removeWithoutDeleting( const PropertyContainer *pc );

    //-*************************************************************************
    // Find the first PropertyContainer that matches a particular type.
    template <class DerivedType>
    DerivedType *findFirstOfType();

    template <class DerivedType>
    const DerivedType *findFirstOfType() const;

    // Find all the PropertyContainers that match a particular type.
    // Appends them to the list.
    template <class DerivedType>
    void findAllOfType( std::vector<DerivedType *> &into );

    template <class DerivedType>
    void findAllOfType( std::vector<const DerivedType *> &into ) const;

    //-*************************************************************************
    // Find the first PropertyContainer that matches a particular protocol
    // Returns NULL if none found.
    // Protocol must match in name AND version. If you want to find all the
    // protocols that match for a range of versions, you'll have to parse the
    // collection yourself.
    PropertyContainer *findFirstOfProtocol( const Protocol &p );
    const PropertyContainer *findFirstOfProtocol( const Protocol &p ) const;

    // Find all property containers that match a particular protocol.
    void findAllOfProtocol( const Protocol &p,
                            std::vector<PropertyContainer *> &into );
    void findAllOfProtocol( const Protocol &p,
                            std::vector<const PropertyContainer *> &i ) const;

    //-*************************************************************************
    // Find the first PropertyContainer that matches a particular name
    PropertyContainer *findFirstOfName( const std::string &n );
    const PropertyContainer *findFirstOfName( const std::string &n ) const;

    // Find all property containers that match a particular name
    void findAllOfName( const std::string &p,
                        std::vector<PropertyContainer *> &into );
    void findAllOfName( const std::string &p,
                        std::vector<const PropertyContainer *> &i ) const;
    
    
};

//-*****************************************************************************
// TEMPLATE AND INLINE FUNCTIONS
//-*****************************************************************************

//-*****************************************************************************
template <class DTYPE>
DTYPE *ObjectVector::findFirstOfType()
{
    for ( iterator iter = begin(); iter != end(); ++iter )
    {
        DTYPE *dt = dynamic_cast<DTYPE *>( (*iter) );
        if ( dt != NULL )
        {
            return dt;
        }
    }
    return NULL;
}

//-*****************************************************************************
template <class DTYPE>
const DTYPE *ObjectVector::findFirstOfType() const
{
    for ( const_iterator iter = begin(); iter != end(); ++iter )
    {
        const DTYPE *dt = dynamic_cast<const DTYPE *>( (*iter) );
        if ( dt != NULL )
        {
            return dt;
        }
    }
    return NULL;
}

//-*****************************************************************************
template <class DTYPE>
void ObjectVector::findAllOfType( std::vector<DTYPE *> &into )
{
    for ( iterator iter = begin(); iter != end(); ++iter )
    {
        DTYPE *dt = dynamic_cast<DTYPE *>( (*iter) );
        if ( dt != NULL )
        {
            into.push_back( dt );
        }
    }
}

//-*****************************************************************************
template <class DTYPE>
void ObjectVector::findAllOfType( std::vector<const DTYPE *> &into ) const
{
    for ( const_iterator iter = begin(); iter != end(); ++iter )
    {
        const DTYPE *dt = dynamic_cast<const DTYPE *>( (*iter) );
        if ( dt != NULL )
        {
            into.push_back( dt );
        }
    }
}

} // End namespace GtoContainer

#endif
