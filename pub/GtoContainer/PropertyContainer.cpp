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

#include <GtoContainer/PropertyContainer.h>
#include <GtoContainer/Utilities.h>
#include <GtoContainer/Exception.h>
#include <iostream>
#include <algorithm>

//-*****************************************************************************
using namespace std;

//-*****************************************************************************
namespace GtoContainer {

//-*****************************************************************************
PropertyContainer::PropertyContainer()
  : m_name( "" ),
    m_protocol()
{
    // Nothing
}

//-*****************************************************************************
PropertyContainer::PropertyContainer( const std::string &nme,
                                      const Protocol &p )
  : m_name( nme ),
    m_protocol( p )
{
    // Nothing
}

//-*****************************************************************************
PropertyContainer::~PropertyContainer() 
{
    delete_contents( m_components );
    m_components.clear();
}

//-*****************************************************************************
std::string PropertyContainer::name() const
{
    return m_name;
}

//-*****************************************************************************
void PropertyContainer::setName( const std::string &nme )
{
    m_name = nme;
}

//-*****************************************************************************
Protocol PropertyContainer::protocol() const
{
    return m_protocol;
}

//-*****************************************************************************
void PropertyContainer::setProtocol( const Protocol &prot )
{
    m_protocol = prot;
}

//-*****************************************************************************
PropertyContainer*
PropertyContainer::copy() const
{
    PropertyContainer *c = emptyContainer();

    for ( int i = 0; i < m_components.size(); ++i )
    {
        if ( Component *o = c->component( m_components[i]->name() ) )
        {
            c->remove( o );
            delete o;
        }

        c->add( m_components[i]->copy() );
    }

    c->setName( name() );
    c->setProtocol( protocol() );
    
    return c;
}

//-*****************************************************************************
void
PropertyContainer::copy( const PropertyContainer *container )
{
    const Components &components = container->components();

    for ( int i = 0; i < components.size(); ++i )
    {
        const Component* other = components[i];
        
        if ( Component* c = component( other->name() ) )
        {
            c->copy( other );
        }
    }

    setName( container->name() );
    setProtocol( container->protocol() );
}

//-*****************************************************************************
PropertyContainer*
PropertyContainer::emptyContainer() const
{
    return new PropertyContainer;
}

//-*****************************************************************************
void
PropertyContainer::add( Component *c )
{
    for ( int i = 0; i < m_components.size(); ++i )
    {
	if ( m_components[i]->name() == c->name() )
	{
	    throw UnexpectedExc(
                ", component with same name already exists" );
	}
    }

    m_components.push_back(c);
}

//-*****************************************************************************
void
PropertyContainer::remove( Component *c )
{
    Components::iterator i = std::find( m_components.begin(),
                                        m_components.end(), c );

    if ( i != m_components.end() )
    {
        m_components.erase( i );
    }
}

//-*****************************************************************************
Component*
PropertyContainer::createComponent( const std::string &name, 
                                    bool synchronized )
{
    if ( Component *c = component( name ) )
    {
        return c;
    }
    else
    {
        Component *cc = new Component( name, synchronized );
        add( cc );
        return cc;
    }
}

//-*****************************************************************************
Component*
PropertyContainer::component( const std::string &name )
{
    for ( int i = 0; i < m_components.size(); ++i )
    {
	if ( m_components[i]->name() == name )
	{
	    return m_components[i];
	}
    }

    return 0;
}

//-*****************************************************************************
const Component*
PropertyContainer::component( const std::string &name ) const
{
    for ( int i = 0; i < m_components.size(); ++i )
    {
	if ( m_components[i]->name() == name )
	{
	    return m_components[i];
	}
    }

    return 0;
}

//-*****************************************************************************
void
PropertyContainer::synchronize( const PropertyContainer* container )
{
    const Components& components = container->components();

    for ( int i = 0; i < components.size(); ++i )
    {
        const Component *oc = components[i];
        Component *c = createComponent( oc->name() );

        const Component::Container &properties = oc->properties();

        for ( int q = 0; q < properties.size(); ++q )
        {
            const Property *op = properties[q];

            if ( !c->find( op->name() ) )
            {
                c->add( op->copyNoData() );
            }
        }
    }
}

//-*****************************************************************************
void
PropertyContainer::concatenate( const PropertyContainer* pc )
{
    for ( int i = 0; i < m_components.size(); ++i )
    {
        Component *c = m_components[i];

        if ( const Component *oc = pc->component( c->name() ) )
        {
            c->concatenate( oc );
        }
    }
}

//-*****************************************************************************
Property*
PropertyContainer::find( const std::string &comp,
                         const std::string &name )
{
    if ( Component* c = component( comp ) )
    {
        return c->find( name );
    }
	
    return 0;
}

//-*****************************************************************************
const Property*
PropertyContainer::find( const std::string &comp,
                         const std::string &name ) const
{
    if ( const Component* c = component( comp ) )
    {
        return c->find( name );
    }
	
    return 0;
}

//-*****************************************************************************
Component*
PropertyContainer::componentOf( const Property* p )
{
    for ( int i = 0; i < m_components.size(); ++i )
    {
        Component *c = m_components[i];

        if ( Property* cp = c->find( p->name() ) )
        {
            if ( cp == p )
            {
                return c;
            }
        }
    }

    return 0;
}

//-*****************************************************************************
const Component*
PropertyContainer::componentOf( const Property* p ) const
{
    for ( int i = 0; i < m_components.size(); ++i )
    {
        Component *c = m_components[i];

        if ( const Property* cp = c->find( p->name() ) )
        {
            if ( cp == p )
            {
                return c;
            }
        }
    }

    return 0;
}

//-*****************************************************************************
void
PropertyContainer::removeProperty( Property* p )
{
    if ( Component* c = componentOf( p ) )
    {
        c->remove( p );
        return ;
    }
}

//-*****************************************************************************
bool
PropertyContainer::hasComponent( Component* c ) const
{
    for ( int i = 0; i < m_components.size(); ++i )
    {
	if ( m_components[i] == c )
        {
            return true;
        }
    }

    return false;
}

//-*****************************************************************************
bool
PropertyContainer::isPersistent() const
{
    for ( int i = 0; i < m_components.size(); ++i )
    {
	if ( m_components[i]->isPersistent() )
        {
            return true;
        }
    }

    return false;
}

} // End namespace GtoContainer

