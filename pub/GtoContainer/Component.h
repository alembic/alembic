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

#ifndef _GtoContainer_Component_h_
#define _GtoContainer_Component_h_

#include <string>
#include <vector>

namespace GtoContainer {

//-*****************************************************************************
class Property;

//-*****************************************************************************
//  class Component
//
//  A Component is a group of Properties. An example would be "points"
//  which might contain the properties "position", "velocity", "mass",
//  etc. Typically, a component will have properties whose sizes are
//  the same. However, there is no enforcement of this.
//-*****************************************************************************

class Component
{
public:
    typedef std::vector<Property*> Container;
    typedef Container Properties;
    
    // Transposable components should have all properties of the same
    // size.
    explicit Component( const std::string &name, bool transposable = false );
    ~Component();
    
    // Returns the values passed into the constructor.
    bool isTransposable() const { return m_transposable; }
    const std::string &name() const { return m_name; }
    
    // Property management
    const Container& properties() const  { return m_properties; }
    Container & properties() { return m_properties; }

    void add( Property *p );
    void remove( Property *p );
    void remove( const std::string & );

    const Property *find( const std::string & ) const;
    Property *find( const std::string & );

    template <class T>
    T *property( const std::string &name );

    template <class T>
    const T *property( const std::string &name ) const;
    
    // resizing all properties in a component
    void resize( size_t s );
    
    // range deletion
    void erase( size_t start, size_t num );
    void eraseUnsorted( size_t start, size_t num );
    
    // resize to the first non-zero property size
    void resizeNonZero();
    
    // Copy everthing
    Component *copy() const;
    
    // Copy other to this
    void copy( const Component * );
    
    // Concatenate. Presumably the passed in component has all the
    // same properties as this one. However, its not an error to pass
    // in something that doesn't have all the same props.
    void concatenate( const Component * );
    
    // Archive
    bool isPersistent() const;
 
private:
    std::string		m_name;
    Container		m_properties;
    bool		m_transposable;
};

//-*****************************************************************************
// TEMPLATE AND INLINE FUNCTIONS
//-*****************************************************************************
template <class T>
inline T *Component::property( const std::string &name )
{
    return dynamic_cast<T*>( find( name ) );
}

//-*****************************************************************************
template<class T>
inline const T *Component::property( const std::string &name ) const
{
    return dynamic_cast<const T*>( find( name ) );
}

} // End namespace GtoContainer

#endif

