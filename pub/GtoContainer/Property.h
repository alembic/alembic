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

#ifndef _GtoContainer_Property_h_
#define _GtoContainer_Property_h_

#include <GtoContainer/Exception.h>
#include <GtoContainer/Foundation.h>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

namespace GtoContainer {

//-*****************************************************************************
// Forward declaration
class Property;

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// The MetaProperty is how the reader knows how to make properties. All
// properties are related to one. If you want to create new types, you simply
// add new MetaProperties to the reader.
class MetaProperty
{
public:
    MetaProperty() {}
    virtual ~MetaProperty() {}

    // Key of the MetaProperty.
    virtual Layout layout() const = 0;
    virtual size_t width() const = 0;
    virtual std::string interpretation() const = 0;

    // Return whether we can handle a particular
    // layout, width, size & interpretation.
    // 0 means can't handle
    // Any other value means it can be handled, with higher values
    // meaning it can be handled 'better'. This allows readers to sort
    // qualifying MetaProperties.
    virtual int32 canHandle( Layout lyt,
                             size_t wdth,
                             const std::string &interp ) const = 0;

    // And finally, create a property
    virtual Property *create( const std::string &nme ) const = 0;

    // This tests if Property can upcase to the type that this
    // metaProperty creates.
    virtual bool validUpcast( const Property *prop ) const = 0;
};

//-*****************************************************************************
typedef std::vector<MetaProperty *> MetaProperties;

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// class Property
//
// Property is an array of some type with a name. The base class
// holds nothing but the name and a reference to the MetaProperty.
class Property
{
public:
    //-*************************************************************************
    // Constructors
    Property( const std::string &nme );
    virtual ~Property();
    
    // Member access
    const std::string &name() const { return m_name; }
    
    // Generic Traits
    virtual Layout          layoutTrait() const = 0;
    virtual size_t          widthTrait() const = 0;
    virtual std::string     interpretationTrait() const = 0;
    
    bool                    isCompoundLayout() 
    { return layoutTrait() == CompoundLayout; }

    // User can set whether or not this property is persistent
    void setPersistence( bool tf ) { m_isPersistent = tf; }
    bool isPersistent() const { return m_isPersistent; }
    
    // Reordering elements
    virtual void            swap( size_t a, size_t b ) = 0;
    
    // Size
    virtual size_t	    size() const = 0;
    virtual bool	    empty() const = 0;
    
    // Resize
    virtual void	    resize( size_t ) = 0;
    
    // Delete range
    virtual void            erase( size_t start, size_t num ) = 0;
    virtual void            eraseUnsorted( size_t start, size_t num ) = 0;
    
    // This returns the size of an element as calculated by the
    // sizeof() operator.
    virtual size_t	    sizeofElement() const = 0;
    
    // Inserting a default value
    virtual void	    insertDefaultValue( size_t index,
                                                size_t len = 1 ) = 0;

    void		    appendDefaultValue( size_t len = 1 ) 
    { insertDefaultValue( size(), len ); }

    virtual void            clearToDefaultValue() = 0;
    
    // copy() copies the property completely. copyNoData() copies the
    // type and name, but without any data. copy() with an argument
    // will copy the contents of the argument to this.
    virtual Property*	    copy( const char *newName = NULL ) const = 0;
    virtual Property*	    copyNoData() const = 0;
    virtual void            copy( const Property * ) = 0;
    virtual void            copyRange( const Property *,
                                       size_t begin, size_t end ) = 0;
    
    // Concatenate data
    virtual void            concatenate( const Property * ) = 0;
    
    // Continuguous anonymous raw data
    virtual void*           rawData() = 0;
    virtual const void*     rawData() const = 0;
    
private:
    std::string		    m_name;
    bool                    m_isPersistent;
};

} // End namespace GtoContainer

#endif
