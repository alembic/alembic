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

#ifndef _Alembic_Util_Dimensions_h_
#define _Alembic_Util_Dimensions_h_

#include <Alembic/Util/Foundation.h>

namespace Alembic {
namespace Util {

//-*****************************************************************************
template <class T>
class BaseDimensions : public std::vector<T>
{
public:
    BaseDimensions() : std::vector<T>() {}
    explicit BaseDimensions( size_t n ) : std::vector<T>( n ) {}
    BaseDimensions( size_t n, const T &t ) : std::vector<T>( n, t ) {}

    BaseDimensions( const BaseDimensions &copy ) : std::vector<T>( copy ) {}

    template <class Y>
    BaseDimensions( const std::vector<Y> &copy )
      : std::vector<T>( copy.begin(), copy.end() ) {}

    BaseDimensions& operator=( const BaseDimensions &copy )
    {
        std::vector<T>::operator=( copy );
        return *this;
    }

    template <class Y>
    BaseDimensions& operator=( const std::vector<Y> &copy )
    {
        std::vector<T>::resize( 0 );
        std::vector<T>::reserve( copy.size() );
        std::vector<T>::insert( std::vector<T>::begin(),
                                copy.begin(), copy.end() );
        return *this;
    }

    size_t rank() const { return std::vector<T>::size(); }

    T *rootPtr() { return &( std::vector<T>::front() ); }
    const T *rootPtr() const
    { return &( std::vector<T>::front() ); }

    size_t numPoints() const
    {
        if ( std::vector<T>::size() == 0 ) { return 0; }
        else
        {
            size_t npoints = 1;
            for ( typename std::vector<T>::const_iterator diter =
                      std::vector<T>::begin();
                  diter != std::vector<T>::end(); ++diter )
            {
                npoints *= ( size_t )(*diter);
            }
            return npoints;
        }
    }
};

//-*****************************************************************************
template <class T, class Y>
bool operator==( const BaseDimensions<T> &a, const BaseDimensions<Y> &b )
{
    size_t aSize = a.size();
    size_t bSize = b.size();
    if ( aSize != bSize ) { return false; }

    for ( size_t d = 0; d < aSize; ++d )
    {
        if ( static_cast<size_t>( a[d] ) !=
             static_cast<size_t>( b[d] ) ) { return false; }
    }
    
    return true;
}

//-*****************************************************************************
template <class T, class Y>
inline bool operator!=( const BaseDimensions<T> &a, const BaseDimensions<Y> &b )
{
    return !( a == b );
}

//-*****************************************************************************
template <class T>
std::ostream &operator<<( std::ostream &ostr, const BaseDimensions<T> &a )
{
    typedef typename std::vector<T>::const_iterator ITER;
    ostr << "[";
    for ( ITER i = a.begin(); i != a.end(); ++i )
    {
        ostr << "[" << (*i) << "]";
    }
    ostr << "]";
    return ostr;
}

//-*****************************************************************************
typedef BaseDimensions<size_t> Dimensions;

} // End namespace Util
} // End namespace Alembic

#endif
