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

#ifndef _AlembicAsset_Body_DataTypeTuple_h_
#define _AlembicAsset_Body_DataTypeTuple_h_

#include <AlembicAsset/Base/DataType.h>
#include <AlembicAsset/Body/FoundationBody.h>
#include <boost/utility.hpp>
#include <boost/smart_ptr.hpp>
#include <map>

namespace AlembicAsset {

//-*****************************************************************************
// You should never need to call these directly, use the tuple below.
// Still, they're kinda handy.
SharedH5T GetNativeH5T( const DataType &adt );
SharedH5T GetFileH5T( const DataType &adt );
DataType InterpretH5T( const H5T &dt );

//-*****************************************************************************
// Bind of Alembic DataType with HDF5 File and Native Datatypes.
class DataTypeTuple
{
public:
    // Generic
    DataTypeTuple() {}
    
    // This will use the same shared h5t
    DataTypeTuple( SharedH5T fH5T );
    
    // This will make a new shared h5t, without ownership of the underlying
    // HDF5 id. (for built-in types, mostly)
    DataTypeTuple( const H5T &fH5T );
    DataTypeTuple( const DataType &dtype );

    DataTypeTuple( const DataTypeTuple &copy )
      : m_dataType( copy.m_dataType ),
        m_fileH5T( copy.m_fileH5T ),
        m_nativeH5T( copy.m_nativeH5T ) {}

    DataTypeTuple &operator=( const DataTypeTuple &copy )
    {
        m_dataType = copy.m_dataType;
        m_fileH5T = copy.m_fileH5T;
        m_nativeH5T = copy.m_nativeH5T;
        return *this;
    }
    
    const DataType &dataType() const { return m_dataType; }
    const H5T &fileH5T() const { return *m_fileH5T; }
    const H5T &nativeH5T() const { return *m_nativeH5T; }

protected:
    DataType m_dataType;
    SharedH5T m_fileH5T;
    SharedH5T m_nativeH5T;
};

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// DATA TYPE TUPLE MAP
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
class DataTypeTupleMap
{
public:
    DataTypeTupleMap() {}

    bool exists( const H5T &fH5T ) const;
    bool exists( const DataType &dtype ) const;

    const DataTypeTuple &find( SharedH5T fH5T );
    const DataTypeTuple &find( const H5T &fH5T );
    const DataTypeTuple &find( const DataType &dtype );

protected:
    typedef std::map<DataType,DataTypeTuple> InternalMap;
    InternalMap m_map;
};

} // End namespace AlembicAsset

#endif
