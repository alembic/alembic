//-*****************************************************************************
//
// Copyright (c) 2009-2012,
//  Sony Pictures Imageworks Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic, nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
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

#ifndef PyAlembic_PyIPropertyUtil_h
#define PyAlembic_PyIPropertyUtil_h

#include <Foundation.h>

//-*****************************************************************************
enum ReturnTypeEnum
{
    kReturnAll,
    kReturnScalar,
    kReturnArray
};

//-*****************************************************************************
template <class PROP>
boost::python::object
getValue( PROP &p, const Abc::ISampleSelector &iSS,
          const ReturnTypeEnum iReturnType = kReturnAll );

//-*****************************************************************************
//
// SampleList and SampleIterator
//
// For iterating over the ISampleList of a Scalar or Array property.
//
template<class PROP>
class SampleIterator
{
public:
    SampleIterator( PROP &p, const ReturnTypeEnum iReturnType )
        : _p( p ), _iter( 0 ), _end( p.getNumSamples() ),
          _returnType( iReturnType ) {}

    boost::python::object next()
    {
        if ( _iter >= _end )
            boost::python::objects::stop_iteration_error();

        return getValue<PROP>( _p, _iter++, _returnType );
    }
private:
    PROP          _p;
    AbcA::index_t _iter;
    AbcA::index_t _end;
    ReturnTypeEnum _returnType;
};

//-*****************************************************************************
template<class PROP>
class SampleList
{
public:
    SampleList( PROP &p, const ReturnTypeEnum iReturnType )
    : _p(p), _returnType( iReturnType ) {}

    Py_ssize_t len()
    {
        return (Py_ssize_t)_p.getNumSamples();
    }

    boost::python::object getItem( Py_ssize_t index )
    {
        return getValue<PROP>( _p, index, _returnType );
    }

    SampleIterator<PROP>* getIterator()
    {
        return new SampleIterator<PROP>( _p, _returnType );
    }
private:
    PROP           _p;
    ReturnTypeEnum _returnType;
};

//-*****************************************************************************
template<class PROP>
SampleList<PROP>
getSampleList( PROP &p, const ReturnTypeEnum iReturnType = kReturnAll )
{
    return SampleList<PROP>( p, iReturnType );
}

#endif

