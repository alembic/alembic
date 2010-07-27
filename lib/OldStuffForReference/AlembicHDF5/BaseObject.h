//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#ifndef _AlembicHDF5_BaseObject_h_
#define _AlembicHDF5_BaseObject_h_

#include <AlembicHDF5/Foundation.h>
#include <AlembicHDF5/Exception.h>
#include <AlembicHDF5/Assert.h>
#include <AlembicHDF5/Init.h>
#include <AlembicUtil/Dimensions.h>
#include <boost/utility.hpp>
#include <vector>

namespace AlembicHDF5 {

//-*****************************************************************************
typedef AlembicUtil::BaseDimensions<size_t> Dimensions;
typedef AlembicUtil::BaseDimensions<hsize_t> HDimensions;

//-*****************************************************************************
// Typed construction flags
enum OpenConstructionFlag { kOpen = 0 };
enum CreateConstructionFlag { kCreate = 1 };

//-*****************************************************************************
// For all the classes that we have here, we only expose the functions
// that the Alembic libraries make use of in HDF5. This acts like a 'using'
// statement.
// All HDF5 Objects are noncopyable.
class BaseObject : public boost::noncopyable
{
protected:
    BaseObject() : m_id( -1 ), m_owned( false )
    {
        // Alembic Init Check
        ABCH5_CHECK_INIT;
    }

    // It is illegal to wrap a base object with an invalid id.
    BaseObject( hid_t i, bool own )
      : m_id( i ),
        m_owned( own )
    {
        // Alembic Init Check
        ABCH5_CHECK_INIT;
        ABCH5_ASSERT( m_id >= 0,
                      "Invalid hid_t passed into BaseObject constructor" );
    }

    // Settable only by derived classes.
    // This amounts to resource leaking, so it should be done with care.
    void setId( hid_t i ) { m_id = i; }
    void setOwned( bool own ) { m_owned = own; }

    // Validity check of id
    void checkValid( const std::string &err ) const
    {
        ABCH5_ASSERT( m_id >= 0, err );
    }
    
public:
    virtual ~BaseObject() { m_id = -1; m_owned = false; }

    hid_t id() const { return m_id; }
    bool owned() const { return m_owned; }
    bool valid() const { return ( m_id >= 0 ); }

    // All BaseObjects have the ability to be closed.
    virtual void close() = 0;

    // Comment control
    std::string comment() const;
    void setComment( const std::string &c );

private:
    hid_t m_id;
    bool m_owned;
};

//-*****************************************************************************
class NamedObject : public BaseObject
{
protected:
    NamedObject() : BaseObject(), m_name( "UNKNOWN" ) {}
    NamedObject( hid_t i, bool own, const std::string &nme )
      : BaseObject( i, own ), m_name( nme ) {}

public:
    const std::string &name() const { return m_name; }

protected:
    void setName( const std::string &nme ) { m_name = nme; }

private:
    std::string m_name;
};

//-*****************************************************************************
#define ABCH5_CHECK_VALID( OBJ , TEXT ) \
    ABCH5_ASSERT( ( OBJ ).valid() , TEXT )

} // End namespace AlembicHDF5

#endif
