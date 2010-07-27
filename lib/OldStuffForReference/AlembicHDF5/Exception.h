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

#ifndef _AlembicHDF5_Exception_h_
#define _AlembicHDF5_Exception_h_

#include <AlembicHDF5/Foundation.h>
#include <AlembicExc/AlembicExc.h>

namespace AlembicHDF5 {

//-*****************************************************************************
class Exception : public AlembicExc::Exception
{
public:
    Exception() throw()
      : AlembicExc::Exception( "AlembicHDF5 EXCEPTION: " ) {}
    Exception( const std::string &str ) throw()
      : AlembicExc::Exception( "AlembicHDF5 EXCEPTION: " )
    {
        this->append( str );
    }
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
class DuplicateNameExc : public Exception
{
public:
    DuplicateNameExc( const std::string &parentName,
                      const std::string &dupChildName ) throw()
      : Exception( "Duplicate Name: Parent Name: " )
    {
        this->append( parentName );
        this->append( ", Duplicate Child Name: " );
        this->append( dupChildName );
    }
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
class NonexistentObjectExc : public Exception
{
public:
    NonexistentObjectExc() throw() : Exception() {}
    NonexistentObjectExc( const std::string &str ) throw() : Exception( str ) {}
};  

//-*****************************************************************************
class NonexistentFileExc : public NonexistentObjectExc
{
public:
    NonexistentFileExc( const std::string &fileName ) throw()
      : NonexistentObjectExc( "Nonexistent or Non-HDF5 File: " )
    {
        this->append( fileName );
    }
};


//-*****************************************************************************
class NonexistentAttributeExc : public NonexistentObjectExc
{
public:
    NonexistentAttributeExc( const std::string &parentName,
                             const std::string &attrName ) throw()
      : NonexistentObjectExc( "Nonexistent Attribute: " )
    {
        this->append( "Parent Object Name: " );
        this->append( parentName );
        this->append( ", Attribute Name: " );
        this->append( attrName );
    }
};

//-*****************************************************************************
class NonexistentGroupExc : public NonexistentObjectExc
{
public:
    NonexistentGroupExc( const std::string &parentName,
                         const std::string &attrName ) throw()
      : NonexistentObjectExc( "Nonexistent Group: " )
    {
        this->append( "Parent Object Name: " );
        this->append( parentName );
        this->append( ", Group Name: " );
        this->append( attrName );
    }
};

//-*****************************************************************************
class NonexistentDatasetExc : public NonexistentObjectExc
{
public:
    NonexistentDatasetExc( const std::string &parentName,
                           const std::string &attrName ) throw()
      : NonexistentObjectExc( "Nonexistent Dataset: " )
    {
        this->append( "Parent Object Name: " );
        this->append( parentName );
        this->append( ", Dataset Name: " );
        this->append( attrName );
    }
};

//-*****************************************************************************
#define ABCH5_THROW( TEXT )                     \
do                                              \
{                                               \
    std::stringstream sstr;                     \
    sstr << TEXT;                               \
    AlembicHDF5::Exception exc( sstr.str() );   \
    throw exc;                                  \
}                                               \
while( 0 )

//-*****************************************************************************
#define ALEMBICHDF5_THROW( TEXT ) ABCH5_THROW( TEXT )

} // End namespace AlembicHDF5

#endif
