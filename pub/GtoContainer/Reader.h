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

#ifndef _GtoContainer_Reader_h_
#define _GtoContainer_Reader_h_

#include <GtoContainer/PropertyContainer.h>
#include <GtoContainer/ObjectVector.h>

#ifdef None
#define __NONE_REP__ None
#undef None
#endif

#include <Gto/Reader.h>

#ifdef __NONE_REP__
#define None __NONE_REP__
#undef __NONE_REP__
#endif

#include <string>
#include <vector>

namespace GtoContainer {

//-*****************************************************************************
class Reader : protected Gto::Reader
{
public:
    //-*************************************************************************
    // Types
    //-*************************************************************************
    typedef Reader::Request                 Request;
    
    // Constructors
    Reader();
    
    virtual ~Reader();
    
    // There's only one call to this class that does anything
    // interesting. read will throw one of the GtoContainer exceptions if
    // something goes wrong.
    // If readIntoExistingObjects is true, the class will only read stuff
    // that's there to begin with.
    void read( const std::string &filename,
               ObjectVector &objects,
               bool readIntoExistingObjects = false );

    //-*************************************************************************
    //-*************************************************************************
    // INTERNAL STUFF
    //-*************************************************************************
    // Override this function if you want to do non-default
    // container creation based on the protocol.
    virtual PropertyContainer* newContainer( const Protocol &protocol );
    
    // Override this function if you want to make a specific type of
    // property given the name of a property, a protocol, and
    // component name.
    //
    // If this function returns NULL, the reader will use the default
    // property type.
    //
    // This mechanism allows for total override control, however it's
    // a heavy tool. The more regular thing would be add new
    // metaproperties below.
    virtual Property *newProperty( const std::string &name,
                                   const PropertyInfo & );

    virtual Request object( const std::string &name,
                            const std::string &protocol,
                            unsigned int protocolVersion,
                            const ObjectInfo &header );

    virtual Request component( const std::string &name,
                               const std::string &interp,
                               const ComponentInfo &header );

    virtual Request property( const std::string &name,
                              const std::string &interp,
                              const PropertyInfo &header);

    virtual void *data( const PropertyInfo &, size_t bytes );
    virtual void dataRead( const PropertyInfo & );
    // To accomodate some GTO base library implementations which have changed
    // the dataRead signature.
    virtual void dataRead( const PropertyInfo &p, char * ) { dataRead( p ); }

    // The reader uses reverse ordering with the meta properties.
    // The last ones are consulted first.
    // This behavior can be changed by overriding findMetaProperty
    void appendMetaProperty( MetaProperty *mp )
    { m_metaProperties.push_back( mp ); }

    virtual const MetaProperty *findMetaProperty( Layout lyt,
                                                  size_t width,
                                                  const std::string &nt ) const;

protected:
    MetaProperties      m_metaProperties;

private:
    bool                m_useExisting;
    ObjectVector*       m_objects;
    std::vector<int>    m_tempstrings;
};

} // End namespace GtoContainer

#endif
