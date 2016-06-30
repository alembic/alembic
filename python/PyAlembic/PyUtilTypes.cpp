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

#include <Foundation.h>

#include <PyImathFixedArray.h>

using namespace boost::python;

//-*****************************************************************************
void register_utiltypes()
{
    // Dimensions
    //
    struct DimensionsOverloads
    {
        static AbcU::uint64_t getItemConst(AbcU::Dimensions &iDim,
                                                 Py_ssize_t index )
        {
            return iDim[index];
        }

        static void setItemScalar( AbcU::Dimensions &iDim,
                                   PyObject *index, const size_t &data)
        {
            PyImath::FixedArray<AbcU::uint64_t> fixedArray( iDim.rootPtr(),
                                                            iDim.rank() );
            fixedArray.setitem_scalar( index, data );
        }
    };

    class_<AbcU::Dimensions>(
         "Dimensions",
         "The Dimensions class describes the dimensional layout of an array "
         " sample in Array Property",
         init<>( "Create a rank-0 Dimensions" ) )
        .def( init<size_t>(
                  ( arg( "size" ) ),
                  "Create a rank-1 Dimensions with the given size" ) )
        .def( init<AbcU::Dimensions&>(
                  ( arg( "dimensions" ) ),
                  "Create a Dimensions as a copy of the given Dimensions" ) )
        .def( "rank",
              &AbcU::Dimensions::rank,
              "Return the rank of this Dimensions" )
        .def( "setRank",
              &AbcU::Dimensions::setRank,
              ( arg( "rank" ) ),
              "Set Dimensions to have the given rank" )
        .def( "numPoints",
              &AbcU::Dimensions::numPoints,
              "Return the number of total points in a dimensional container "
              "described by this Dimensions" )
        .def( "__getitem__",
              DimensionsOverloads::getItemConst,
              "Return the dimension size of the given rank" )
        .def( "__setitem__",
              DimensionsOverloads::setItemScalar,
              "Set a dimension size of the given rank" )
        .def( self_ns::str( self_ns::self ) )
        .def( self == self )
        .def( self != self )
        ;

    // PlainOldDataType Enum
    //
    enum_<AbcU::PlainOldDataType>(
         "POD",
         "Plain Old Data Types which are suppored in a Simple Property")
        .value( "kBooleanPOD",  AbcU::kBooleanPOD )
        .value( "kUint8POD",    AbcU::kUint8POD )
        .value( "kInt8POD",     AbcU::kInt8POD )
        .value( "kUint16POD",   AbcU::kUint16POD )
        .value( "kInt16POD",    AbcU::kInt16POD )
        .value( "kUint32POD",   AbcU::kUint32POD )
        .value( "kInt32POD",    AbcU::kInt32POD )
        .value( "kUint64POD",   AbcU::kUint64POD )
        .value( "kInt64POD",    AbcU::kInt64POD )
        .value( "kFloat16POD",  AbcU::kFloat16POD )
        .value( "kFloat32POD",  AbcU::kFloat32POD )
        .value( "kFloat64POD",  AbcU::kFloat64POD )
        .value( "kStringPOD",   AbcU::kStringPOD )
        .value( "kWstringPOD",  AbcU::kWstringPOD )
        .value( "kNumPlainOldDataTypes", AbcU::kNumPlainOldDataTypes )
        .value( "kUnknownPOD",  AbcU::kUnknownPOD )
        ;
}
