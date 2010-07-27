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

#ifndef _GtoContainer_TypedProperty_h_
#define _GtoContainer_TypedProperty_h_

#include <GtoContainer/Property.h>
#include <string>
#include <string.h>
#include <algorithm>

namespace GtoContainer {

//-*****************************************************************************
// This is the kind of traits object that TypedProperty wants.
// You don't have to derive from it, though, you can just make a new one.
// The compiler goes MUCH faster when it doesn't need to fret about
// template specializations.
template <class T, Layout LYT, size_t WDTH>
struct SampleTypedPropertyTraits
{
    typedef std::vector<T> container_type;
    typedef T value_type;
    static inline Layout layout() { return LYT; }
    static inline size_t width() { return WDTH; }
    static inline std::string interpretation() { return GTO_INTERPRET_DEFAULT; }
    static inline T defaultValue()
    {
        T t;
        memset( ( void * )&t, sizeof( T ), 0 );
        return t;
    }
};

//-*****************************************************************************
// All typed properties have containers, and should be at least theoretically
// reservable. Default does nothing.
class ReservableProperty : public Property
{
public:
    ReservableProperty( const std::string &nme ) : Property( nme ) {}

    virtual void reserve( size_t num ) {}
};  

//-*****************************************************************************
//-*****************************************************************************
// We don't want to have a reference to the metaclass in the property,
// otherwise it's difficult to create a new instance of a property without
// having the metaclass.
//
// Therefore, information about the property travels from the property,
// which we allow users to create themselves, up to the MetaProperty.
template <class TRAITS>
class TypedProperty : public ReservableProperty
{
public:
    typedef TRAITS                                      traits_type;
    typedef typename TRAITS::value_type                 value_type;
    typedef value_type                                  T;
    typedef TypedProperty<TRAITS>                       this_type;

    typedef value_type*                                 value_pointer;
    typedef const value_type*                           const_value_pointer;

    typedef typename TRAITS::container_type             Container;
    typedef Container                                   container_type;
    
    typedef typename Container::iterator		iterator;
    typedef typename Container::const_iterator		const_iterator;
    typedef typename Container::reverse_iterator	reverse_iterator;
    typedef typename Container::const_reverse_iterator	const_reverse_iterator;
    typedef typename Container::reference		reference;
    typedef typename Container::const_reference		const_reference;

    TypedProperty( const std::string &nme )
      : ReservableProperty( nme ),
        m_container(),
        m_default( TRAITS::defaultValue() )
    {
        // Nothing
    }

    virtual ~TypedProperty();

    // Generic traits.
    virtual Layout layoutTrait() const;
    virtual size_t widthTrait() const;
    virtual std::string interpretationTrait() const;
    
    // Access
    reference operator[]( size_t i )
    {
        assert( i < m_container.size() );
        return m_container[i];
    }
    
    const_reference operator[]( size_t i ) const
    {
        assert( i < m_container.size() );
        return m_container[i];
    }
    
    reference front() { return m_container.front(); }
    const_reference front() const { return m_container.front(); }

    reference back() { return m_container.back(); }
    const_reference back() const { return m_container.back(); }
    
    // Iterators
    iterator		    begin() { return m_container.begin(); }
    const_iterator	    begin() const { return m_container.begin(); }
    iterator		    end() { return m_container.end(); }
    const_iterator	    end() const { return m_container.end(); }
    reverse_iterator	    rbegin() { return m_container.begin(); }
    const_reverse_iterator  rbegin() const { return m_container.begin(); }
    reverse_iterator	    rend() { return m_container.end(); }
    const_reverse_iterator  rend() const { return m_container.end(); }

    void erase( iterator i ) { m_container.erase(i); }
    
    // Adding/Removing elements
    void push_back( const_reference v ) 
    { m_container.push_back( v ); }

    void push_front( const_reference v )
    { m_container.insert( m_container.begin(), v ); }

    void pop_back() { m_container.pop_back(); }

    void pop_front() 
    { m_container.erase( m_container.begin() ); }
    
    // Reordering
    virtual void swap( size_t a, size_t b );
    
    // Size
    virtual size_t size() const;
    virtual bool empty() const;

    virtual size_t sizeofElement() const;

    virtual void resize( size_t s );

    virtual void erase( size_t start, size_t num );
    virtual void eraseUnsorted( size_t start, size_t num );
    
    // copy() copies the property completely. copyNoData() copies the
    // type and name, but without any data. copy() with an argument
    // will copy the contents of the argument to this.
    virtual Property *copy( const char *newName = NULL ) const;
    virtual Property *copyNoData() const;
    virtual void copy( const Property * );
    virtual void copyRange( const Property *, size_t begin, size_t end );
    
    // concat
    virtual void concatenate( const Property * );
    void concatenateWithOffset( const this_type *, const value_type &offset );
    
    // Default value handling
    virtual void insertDefaultValue( size_t, size_t len = 1 );
    virtual void clearToDefaultValue();
    
    // Raw data pointer
    const_value_pointer data() const;
    value_pointer data();

    // Return container directly
    const Container &container() const { return m_container; }
    Container &container() { return m_container; }

    virtual void *rawData();
    virtual const void *rawData() const;

protected:
    Container m_container;
    value_type m_default;
};

//-*****************************************************************************
template <class TPROPERTY>
class TypedMetaProperty : public MetaProperty
{
public:
    typedef TPROPERTY property_type;
    typedef typename TPROPERTY::value_type value_type;

    TypedMetaProperty()
    {
        TPROPERTY dummy( "dummy" );
        m_layout = dummy.layoutTrait();
        m_width = dummy.widthTrait();
        m_interpretation = dummy.interpretationTrait();
    }

    // C++ compiler can de-inline these.
    virtual Layout layout() const { return m_layout; }
    virtual size_t width() const { return m_width; }
    virtual std::string interpretation() const
    { return m_interpretation; }

    virtual int32 canHandle( Layout lyt, size_t wdth,
                             const std::string &interp ) const;

    virtual Property *create( const std::string &name ) const;

    virtual bool validUpcast( const Property *prop ) const;

protected:
    Layout m_layout;
    size_t m_width;
    std::string m_interpretation;
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// TYPED PROPERTY IMPLEMENTATION
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
template <class TRAITS>
TypedProperty<TRAITS>::~TypedProperty() 
{
    // nothing
}

//-*****************************************************************************
template <class TRAITS>
Layout TypedProperty<TRAITS>::layoutTrait() const
{
    return TRAITS::layout();
}

//-*****************************************************************************
template <class TRAITS>
size_t TypedProperty<TRAITS>::widthTrait() const
{
    return TRAITS::width();
}

//-*****************************************************************************
template <class TRAITS>
std::string TypedProperty<TRAITS>::interpretationTrait() const
{
    return TRAITS::interpretation();
}

//-*****************************************************************************
template <class TRAITS>
size_t
TypedProperty<TRAITS>::size() const
{
    return m_container.size();
}

//-*****************************************************************************
template <class TRAITS>
bool
TypedProperty<TRAITS>::empty() const
{
    return m_container.empty();
}

//-*****************************************************************************
template <class TRAITS>
size_t
TypedProperty<TRAITS>::sizeofElement() const
{
    return sizeof( value_type );
}

//-*****************************************************************************
template <class TRAITS>
void
TypedProperty<TRAITS>::insertDefaultValue( size_t index, size_t len )
{
    m_container.insert( m_container.begin() + index, len, m_default );
}

//-*****************************************************************************
template <class TRAITS>
void
TypedProperty<TRAITS>::clearToDefaultValue()
{
    std::fill( m_container.begin(), m_container.end(), m_default );
}

//-*****************************************************************************
template <class TRAITS>
void
TypedProperty<TRAITS>::swap( size_t a, size_t b )
{
    std::swap( m_container[a], m_container[b] );
}

//-*****************************************************************************
template <class TRAITS>
void
TypedProperty<TRAITS>::resize( size_t s )
{ 
    size_t osize = m_container.size();

    if ( s < osize )
    {
	m_container.resize( s ); 
    }
    else if ( s > osize )
    {
	insertDefaultValue( m_container.size(),
                            s - m_container.size() );
    }
}

//-*****************************************************************************
template <class TRAITS>
void
TypedProperty<TRAITS>::erase( size_t s, size_t n )
{ 
    if ( m_container.size() )
    {
        m_container.erase( m_container.begin() + s, 
                           m_container.begin() + (s + n) );
    }
}

//-*****************************************************************************
template <class TRAITS>
void
TypedProperty<TRAITS>::eraseUnsorted( size_t s,
                                 size_t n )
{ 
    if ( m_container.size() )
    {
        if ( n == 1 )
        {
            *( m_container.begin() + s ) = m_container.back();
        }
        else
        {
            std::copy( m_container.begin() + (m_container.size() - n - 1),
                       m_container.end(),
                       m_container.begin() + s );
        }
        
        m_container.resize( m_container.size() - n );
    }
}

//-*****************************************************************************
template <class TRAITS>
typename TypedProperty<TRAITS>::const_value_pointer
TypedProperty<TRAITS>::data() const
{
    return &( m_container.front() );
}

//-*****************************************************************************
template <class TRAITS>
typename TypedProperty<TRAITS>::value_pointer
TypedProperty<TRAITS>::data()
{
    return &( m_container.front() );
}

//-*****************************************************************************
template <class TRAITS>
Property*
TypedProperty<TRAITS>::copyNoData() const
{
    return new this_type( name() );
}

//-*****************************************************************************
template <class TRAITS>
Property*
TypedProperty<TRAITS>::copy( const char *newName ) const
{
    this_type *p = new this_type( newName ? std::string( newName ) : name() );
    p->resize( size() );
    std::copy( begin(), end(), p->begin() );
    p->setPersistence( isPersistent() );
    return p;
}

//-*****************************************************************************
template <class TRAITS>
void
TypedProperty<TRAITS>::copy( const Property *p )
{
    if ( const this_type *tp = dynamic_cast<const this_type *>( p ) )
    {
        m_container.resize( tp->size() );
        std::copy( tp->begin(), tp->end(), begin() );
        setPersistence( p->isPersistent() );
    }
    else
    {
        throw TypeMismatchExc();
    }
}

//-*****************************************************************************
template <class TRAITS>
void
TypedProperty<TRAITS>::copyRange( const Property *p,
                             size_t i0,
                             size_t i1 )
{
    if ( const this_type *tp = dynamic_cast<const this_type *>( p ) )
    {
        m_container.resize( i1 - i0 );
        std::copy( tp->begin() + i0, tp->begin() + i1, begin() );
    }
    else
    {
        throw TypeMismatchExc();
    }
}

//-*****************************************************************************
template <class TRAITS>
void
TypedProperty<TRAITS>::concatenate( const Property *p )
{
    if ( const this_type *tp = dynamic_cast<const this_type *>( p ) )
    {
        for ( int i = 0; i < tp->size(); ++i )
        {
            push_back( (*tp)[i] );
        }
    }
    else
    {
        throw TypeMismatchExc();
    }
}

//-*****************************************************************************
template <class TRAITS>
void
TypedProperty<TRAITS>::concatenateWithOffset( const this_type *p, const T &v )
{
    for ( int i = 0, s = p->size(); i < s; ++i )
    {
        push_back( (*p)[i] + v );
    }
}

//-*****************************************************************************
template <class TRAITS>
void *
TypedProperty<TRAITS>::rawData() 
{ 
    return data(); 
}

//-*****************************************************************************
template <class TRAITS>
const void *
TypedProperty<TRAITS>::rawData() const
{ 
    return data(); 
}


//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// META TYPED PROPERTY IMPLEMENTATION
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
template <class TPROPERTY>
int32 TypedMetaProperty<TPROPERTY>::canHandle( Layout lyt,
                                               size_t wdth,
                                               const std::string &interp ) const
{
    if ( lyt == m_layout &&
         wdth == m_width )
    {
        // If we have default interpretation, it means we like handing
        // stuff, just not as much as with a specific interpretation,
        // but more than with a mismatched interpretation.
        if ( m_interpretation == "" ||
             m_interpretation == GTO_INTERPRET_DEFAULT )
        {
            return 2;
        }
        else if ( interp.find( m_interpretation ) != std::string::npos )
        {
            return 2 + m_interpretation.size();
        }
        else
        {
            // We didn't find our interpretation, it's not a default
            // interpretation, but we CAN store this guy, if need be.
            // return 1.
            return 1;
        }
    }

    // Can't handle this guy.
    return 0;
}

//-*****************************************************************************
template <class TPROPERTY>
Property *TypedMetaProperty<TPROPERTY>::create( const std::string &name ) const
{
    return new TPROPERTY( name );
}

//-*****************************************************************************
template <class TPROPERTY>
bool TypedMetaProperty<TPROPERTY>::validUpcast( const Property *prop ) const
{
    return ( dynamic_cast<const TPROPERTY *>( prop ) != NULL );
}

} // End namespace GtoContainer

#endif
