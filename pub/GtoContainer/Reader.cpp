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

#include <GtoContainer/Reader.h>
#include <GtoContainer/Exception.h>
#include <GtoContainer/StdProperties.h>
#include <iostream>
#include <algorithm>

namespace GtoContainer {

//-*****************************************************************************
Reader::Reader() 
  : Gto::Reader( 0 ),
    m_useExisting( false ),
    m_objects( NULL )
{
    // Add the standard meta propreties.
    AppendStdMetaProperties( m_metaProperties );
}

//-*****************************************************************************
Reader::~Reader()
{
    for ( MetaProperties::iterator iter = m_metaProperties.begin();
          iter != m_metaProperties.end(); ++iter )
    {
        delete (*iter);
    }
}

//-*****************************************************************************
void
Reader::read( const std::string &filename,
              ObjectVector &objects,
              bool readIntoExisting )
{
    m_useExisting = readIntoExisting;
    m_objects = &objects;
    
    if ( !open( filename.c_str() ) )
    {
        std::string msg = ", opening .gto file \"";
        msg += filename;
        msg += "\", " + why();
        throw ReadFailedExc( msg.c_str() );
    }

    // Reset.
    m_useExisting = false;
    m_objects = NULL;
}

//-*****************************************************************************
PropertyContainer *
Reader::newContainer( const Protocol &protocol )
{
    return new PropertyContainer;
}

//-*****************************************************************************
Property * 
Reader::newProperty( const std::string &name, const PropertyInfo & )
{
    return NULL;
}

//-*****************************************************************************
Reader::Request
Reader::object( const std::string &name, 
                const std::string &prot, 
                unsigned int protVersion,
                const ObjectInfo &info ) 
{
    if ( m_objects == NULL )
    {
        GTC_THROW( "Reader reading without objects" );
    }
    
    PropertyContainer *g = NULL;
    Protocol protocol( prot, protVersion );
    bool found = false;

    if ( m_useExisting )
    {
        for ( int i = 0; i < m_objects->size(); ++i )
        {
            PropertyContainer *pc = (*m_objects)[i];

            if ( pc->name() == name &&
                 pc->protocol() == protocol )
            {
                found = true;
                g = pc;
            }
        }
    }
    
    if ( !g )
    {
        g = newContainer( protocol );
        g->setName( name );
        g->setProtocol( protocol );
    }

    if ( !found ) { m_objects->push_back( g ); }

    return Request( true, g );
}

//-*****************************************************************************
Reader::Request
Reader::component( const std::string &name, 
                      const std::string &ininterp,
                      const ComponentInfo &info ) 
{
    std::string interp = ininterp;
    if ( fileHeader().version < 3 ) { interp = GTO_INTERPRET_DEFAULT; }
    
    if ( m_objects == NULL )
    {
        GTC_THROW( "Reader reading without objects" );
    }
    
    PropertyContainer *g  = 
        reinterpret_cast<PropertyContainer*>( info.object->objectData );
    Component *c = g->component( name );
    
    if ( !c )
    {
        // adds it too.
        c = g->createComponent( name, info.flags & Gto::Matrix );
    }

    return Request( true, c );
}

//-*****************************************************************************
Reader::Request
Reader::property( const std::string &name, 
                  const std::string &pre_ininterp,
                  const PropertyInfo &info ) 
{
    // Fix interp for versions prior to 3.
    std::string ininterp = pre_ininterp;
    if ( fileHeader().version < 3 )
    {
        ininterp = GTO_INTERPRET_DEFAULT;
    }
    
    if ( m_objects == NULL )
    {
        GTC_THROW( "Reader reading without objects" );
    }
    
    std::string interp;

    // In case you need the property container, here it is.
    // PropertyContainer *pc  = 
    //    reinterpret_cast<PropertyContainer*>(
    //         info.component->object->objectData );
    
    Component *c =
        reinterpret_cast<Component*>( info.component->componentData );
    Property *p  = c->find( name );
    Property *np = NULL;

    // Only use the part of the interpretation up to the first semicolon.
    size_t sq = ininterp.find( ';' );

    if ( sq != std::string::npos )
    {
        interp = ininterp.substr( 0, sq );
    }
    else
    {
        interp = ininterp;
    }

    // If there's no interpretation, change the interpretation to
    // GTO_INTERPRET_DEFAULT
    if ( interp == "" )
    {
        interp = GTO_INTERPRET_DEFAULT;
    }   

    // If the property doesn't exist, try the virtual 'newProperty' function
    // which returns NULL by default. If it makes a property, awesome! Use
    // that. If not, continue below.
    if ( p == NULL )
    {
        if ( np = newProperty( name, info ) )
        {
            c->add( np );
            np->resize( info.size );
            return Request( true, np );
        }
    }
    
    // From the layout, width & interpretation, build us a new
    // property, OR, verify that the old property is the right upcastable
    // type.
    Layout layout = gtoTypeToLayout( ( Gto::DataType )( info.type ) );
    const MetaProperty *metaProp = findMetaProperty( layout,
                                                     info.width,
                                                     interp );
    if ( metaProp == NULL )
    {
        std::cerr << "GtoContainer::Reader WARNING: "
                  << "Ignoring property \"" << name << "\""
                  << std::endl;
        return Request( false );
    }

    Property *newProp = NULL;
    if ( p != NULL )
    {
        if ( !metaProp->validUpcast( p ) )
        {
            throw TypeMismatchExc();
        }

        newProp = p;
    }
    else
    {
        newProp = metaProp->create( name );
        c->add( newProp );
    }

    // If we get here, we've got a new property, ready to receive data.
    // Resize it and send out the Request.
    assert( newProp != NULL );
    newProp->resize( info.size );
    return Request( true, newProp );
}

//-*****************************************************************************
void *
Reader::data( const PropertyInfo &info, size_t bytes )
{
    Property *p = reinterpret_cast<Property*>( info.propertyData );
    p->resize( info.size );

    // no dereferencing empty array's
    if ( p->size() == 0 )
    {
        return NULL;
    }

    if ( StringProperty *sp = dynamic_cast<StringProperty*>( p ) )
    {
        m_tempstrings.resize( sp->size() );
        return &m_tempstrings.front();
    }

    return p->rawData();
}

//-*****************************************************************************
void
Reader::dataRead( const PropertyInfo &info )
{
    Property *p  = reinterpret_cast<Property*>( info.propertyData );

    if ( StringProperty *sp = dynamic_cast<StringProperty*>( p ) )
    {
        for ( int i = 0; i < m_tempstrings.size(); ++i )
        {
            (*sp)[i] = stringFromId( m_tempstrings[i] );
        }

        m_tempstrings.clear();
    }
}

//-*****************************************************************************
// The algorithm is to search for the highest numbered handler, prioritizing
// metaproperties later in the list.
const MetaProperty *
Reader::findMetaProperty( Layout lyt, size_t wdth,
                          const std::string &interp ) const
{
    const MetaProperty *found = NULL;
    int foundScore = 0;
    
    for ( MetaProperties::const_reverse_iterator iter =
              m_metaProperties.rbegin();
          iter != m_metaProperties.rend(); ++iter )
    {
        const MetaProperty *mp = (*iter);
        int score = mp->canHandle( lyt, wdth, interp );
        if ( score > 0 )
        {
            if ( score > foundScore )
            {
                found = mp;
                foundScore = score;
            }
        }
    }

    return found;
}

} // End namespace GtoContainer
