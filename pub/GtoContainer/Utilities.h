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

#ifndef _GtoContainer_Utilities_h_
#define _GtoContainer_Utilities_h_

#include <algorithm>
#include <functional>

namespace GtoContainer {

//-*****************************************************************************
// Function remove
//
// This removes an element from a Container by erasing it. It does a
// linear search to find the element to remove.
template <class Container>
void 
remove( Container &container,
        typename Container::value_type value )
{
    typename Container::iterator i = std::find( container.begin(),
                                                container.end(),
                                                value );

    if ( i != container.end() )
    {
        container.erase( i );
    }
}

//-*****************************************************************************
// Function remove_unsorted
//
// This removes an element from a Container by copying the last
// element in the container on top of the element to be removed. It
// then erases the last element.
template <class Container>
void 
remove_unsorted( Container &container,
                 typename Container::value_type value )
{
    typename Container::iterator i = std::find( container.begin(),
                                                container.end(),
                                                value );

    if ( i != container.end() )
    {
        *i = container.back();
        container.erase( container.begin() + container.size() - 1 );
    }
}

//-*****************************************************************************
// Function delete_contents
//
// Deletes the contents of a container. Useful when the container
// holds pointers.
template <class Container>
inline void
delete_contents( Container &container )
{
    for ( typename Container::iterator i = container.begin(); 
          i != container.end(); 
          ++i )
    {
	delete *i;
    }
}

//-*****************************************************************************
// Function exists
//
// Returns true if the element exists inside the container false otherwise.
template <class Container>
inline bool
exists( const Container &container, 
        typename Container::value_type value )
{
    return std::find( container.begin(), 
		      container.end(), 
                      value ) != container.end();
}

//-*****************************************************************************
// Cast Adapters for std::transform. So transform can be used to copy
// from a container of one type to another.
template <class From, class To>
struct StaticPointerCast
{
    To* operator() (From* x) const { return static_cast<To*>(x); }
};

template <class From, class To>
struct DynamicPointerCast
{
    To* operator() (From* x) const { return dynamic_cast<To*>(x); }
};

template <class From, class To>
struct ReinterpertCast
{
    To operator() (From x) const { return reinterpret_cast<To>(x); }
};

template <class From, class To>
struct StaticCast
{
    To operator() (From x) const { return static_cast<To>(x); }
};

template <class From, class To>
struct DynamicCast
{
    To operator() (From x) const { return dynamic_cast<To>(x); }
};

//-*****************************************************************************
//
//  Predicates. (All end in _p)
//
template <class From, class To>
struct IsA_p
{
    bool operator() (From* s) const { return dynamic_cast<To*>(s) == 0; }
};

//
//  Full container versions of std algorithms
//

template <class Container, class UnaryFunction>
inline typename Container::iterator 
for_each (Container& c, UnaryFunction f)
{
    return std::for_each(c.begin(), c.end(), f);
}

template <class Container>
inline typename Container::iterator 
find (Container& c, const typename Container::value_type& v)
{
    return std::find(c.begin(), c.end(), v);
}

template <class Container>
inline void
sort (Container& c)
{
    std::sort(c.begin(), c.end());
}

template <class Container, class OutputIterator>
inline OutputIterator 
copy (Container& c, OutputIterator i)
{
    return std::copy(c.begin(), c.end(), i);
}

} // End namespace GtoContainer

#endif
