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

#ifndef _GtoContainer_Exception_h_
#define _GtoContainer_Exception_h_

#include <iostream>
#include <sstream>
#include <exception>
#include <string>

namespace GtoContainer {

//-*****************************************************************************
class Exception : protected std::string, public std::exception
{
public:
    Exception() throw() : std::string( "" ) {}
    Exception( const char *str ) throw() : std::string( str ) {}
    Exception( const Exception &exc ) throw() : std::string( exc.c_str() ) {}

    virtual ~Exception() throw() {}

    virtual const char *what() const throw() { return c_str(); }
};

//-*****************************************************************************
#define GTC_THROW( TEXT )                                 \
do                                                          \
{                                                           \
    std::stringstream sstr;                                 \
    sstr << TEXT;                                           \
    GtoContainer::Exception exc( sstr.str().c_str() );      \
    throw exc;                                              \
}                                                           \
while( 0 )

//-*****************************************************************************
#define GTC_EXC_DECLARE( EXC_TYPE, BASE_TYPE, TAG )	        \
class EXC_TYPE : public BASE_TYPE				\
{								\
public:								\
    EXC_TYPE() : BASE_TYPE( TAG ) {}				\
    EXC_TYPE( const char *str ) : BASE_TYPE( TAG )		\
    { this->append( str ); }					\
};


//-*****************************************************************************
GTC_EXC_DECLARE( ReadFailedExc, Exception, "read failed" );
GTC_EXC_DECLARE( BadPropertyTypeMatchExc, Exception, "bad property match");
GTC_EXC_DECLARE( NoPropertyExc, Exception, "no such property" );
GTC_EXC_DECLARE( UnexpectedExc, Exception, "unexpected program state" );
GTC_EXC_DECLARE( TypeMismatchExc, Exception,
                 "PropertyContainer type mismatch" );
GTC_EXC_DECLARE( InvalidDerefExc, Exception,
                 "invalid iterator dereference" );
    

} // End namespace GtoContainer

#endif
