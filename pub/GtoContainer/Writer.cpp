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

#include <GtoContainer/Writer.h>
#include <GtoContainer/PropertyContainer.h>
#include <GtoContainer/StdProperties.h>
#include <GtoContainer/ObjectVector.h>
#include <iostream>

namespace GtoContainer {

//-*****************************************************************************
Writer::Writer( const char *stamp )
  : m_writer()
{
    if ( stamp ) 
    {
        m_stamp = stamp;
    }   
}

//-*****************************************************************************
Writer::~Writer()
{
    // Nothing
}

//-*****************************************************************************
void
Writer::writeProperty( bool header, const Property *property )
{
    if ( !property->isPersistent() )
    {
        return;
    }

    if ( property->layoutTrait() == CompoundLayout )
    {
        std::cerr << "WARNING: Writer does not understand "
                  << "type of property "
                  << property->name()
                  << std::endl;
        return;
    }

    Gto::DataType type = layoutToGtoType( property->layoutTrait() );
    std::string interp = property->interpretationTrait();
    if ( interp == GTO_INTERPRET_DEFAULT )
    {
        interp = "";
    }
    int width = property->widthTrait();

    //-*************************************************************************
    // Handle String properties differently, because Strings have to be
    // interred.
    if ( const StringProperty *sp =
         dynamic_cast<const StringProperty*>( property ) )
    {
	if ( header )
	{
	    for ( int i = 0; i < sp->size(); ++i )
	    {
		const std::string &s = (*sp)[i];
		m_writer.intern( s );
	    }

            m_writer.property( property->name().c_str(),
                               type,
                               property->size(),
                               width,
                               interp.c_str() );
	}
	else
	{
	    int *data = new int[ sp->size() ];

	    for ( int i = 0; i < sp->size(); ++i )
	    {
		const std::string &s = (*sp)[i];
		data[i] = m_writer.lookup( s );
	    }

	    m_writer.propertyData( data );
	}
    }
    else
    {
        if ( header )
        {
            m_writer.property( property->name().c_str(),
                               type,
                               property->size(),
                               width,
                               interp.c_str() );
        }
        else
        {
            if ( !property->empty() )
            {
                m_writer.propertyDataRaw( property->rawData() );
            }
            else
            {
                m_writer.propertyDataRaw( 0 );
            }
        }
    }
}

//-*****************************************************************************
void
Writer::writeComponent( bool header, const Component *component )
{
    if ( component->isPersistent() )
    {
        const Component::Container &props = component->properties();
    
        unsigned int flags = component->isTransposable() ? Gto::Matrix : 0;
        if ( header )
        {
            m_writer.beginComponent( component->name().c_str(), flags );
        }

        for ( int i = 0; i < props.size(); ++i )
        {
            const Property *p = props[i];
            writeProperty( header, p );
        }

        if ( header )
        {
            m_writer.endComponent();
        }
    }
}

//-*****************************************************************************
bool
Writer::write( const char *filename,
               const ObjectVector &objects,
               FileType type )
{
    if ( !m_writer.open( filename, type ) )
    {
        return false;
    }

    // Intern the stamp.
    if ( m_stamp != "" )
    {
        m_writer.intern( m_stamp );
    }

    // Intern useful build info.
    m_writer.intern( "(GtoContainer::Writer compiled " __DATE__
                     " " __TIME__ " )" );

    for ( int i = 0; i < objects.size(); ++i )
    {
	const PropertyContainer *container = objects[i];
        std::string name = container->name();
        Protocol protocol = container->protocol();

        if ( !container->isPersistent() ) { continue; }

        if ( name == "" ) { name = "NO_NAME"; }

        if ( protocol.protocol == "" )
        {
            protocol.protocol = "NO_PROTOCOL";
            protocol.version = 0;
        }

	m_writer.beginObject( name.c_str(), 
                              protocol.protocol.c_str(),
                              protocol.version );

	const PropertyContainer *g = container;
	const PropertyContainer::Components &comps = g->components();

	for ( int q = 0; q < comps.size(); ++q )
	{
	    writeComponent( true, comps[q] );
	}

	m_writer.endObject();
    }

#ifdef GTO_ORDERED_STRING_TABLE
    std::string orderedStrings = "";
    m_writer.beginData( &orderedStrings, 1 );
#else
    m_writer.beginData();
#endif
    
    for ( int i = 0; i < objects.size(); ++i )
    {
	const PropertyContainer *g = objects[i];
	const PropertyContainer::Components &comps = g->components();

	for ( int q = 0; q < comps.size(); ++q )
	{
	    writeComponent( false, comps[q] );
	}
    }

    m_writer.endData();
    m_writer.close();
    return true;
}

} // End namespace GtoContainer




