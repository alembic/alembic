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

#ifndef _AlembicHDF5_Datatype_h_
#define _AlembicHDF5_Datatype_h_

#include <AlembicHDF5/BaseObject.h>
#include <boost/cstdint.hpp>

namespace AlembicHDF5 {

//-*****************************************************************************
class Attribute;
class Dataset;

//-*****************************************************************************
class Datatype : public BaseObject
{
public:
    typedef boost::uint32_t flag_t;
    
    static const flag_t kInteger =      1U << 0;
    static const flag_t kFloat =        1U << 1;
    static const flag_t kBytes1 =       1U << 2;
    static const flag_t kBytes2 =       1U << 3;
    static const flag_t kBytes4 =       1U << 4;
    static const flag_t kBytes8 =       1U << 5;
    static const flag_t kSigned =       1U << 6;
    static const flag_t kUnsigned =     1U << 7;
    static const flag_t kLittle =       1U << 8;
    static const flag_t kBig =          1U << 9;
    static const flag_t kRefObj =       1U << 10;
    static const flag_t kRefDsetreg =   1U << 11;
    
    static const flag_t kF32BE = kFloat | kBytes4 | kBig;
    static const flag_t kF32LE = kFloat | kBytes4 | kLittle;
    static const flag_t kF64BE = kFloat | kBytes8 | kBig;
    static const flag_t kF64LE = kFloat | kBytes8 | kLittle;

    static const flag_t kI8BE = kInteger | kSigned | kBytes1 | kBig;
    static const flag_t kI8LE = kInteger | kSigned | kBytes1 | kLittle;
    static const flag_t kI16BE = kInteger | kSigned | kBytes2 | kBig;
    static const flag_t kI16LE = kInteger | kSigned | kBytes2 | kLittle;
    static const flag_t kI32BE = kInteger | kSigned | kBytes4 | kBig;
    static const flag_t kI32LE = kInteger | kSigned | kBytes4 | kLittle;
    static const flag_t kI64BE = kInteger | kSigned | kBytes8 | kBig;
    static const flag_t kI64LE = kInteger | kSigned | kBytes8 | kLittle;
        
    static const flag_t kU8BE = kInteger | kUnsigned | kBytes1 | kBig;
    static const flag_t kU8LE = kInteger | kUnsigned | kBytes1 | kLittle;
    static const flag_t kU16BE = kInteger | kUnsigned | kBytes2 | kBig;
    static const flag_t kU16LE = kInteger | kUnsigned | kBytes2 | kLittle;
    static const flag_t kU32BE = kInteger | kUnsigned | kBytes4 | kBig;
    static const flag_t kU32LE = kInteger | kUnsigned | kBytes4 | kLittle;
    static const flag_t kU64BE = kInteger | kUnsigned | kBytes8 | kBig;
    static const flag_t kU64LE = kInteger | kUnsigned | kBytes8 | kLittle;

    static const flag_t kREF_OBJ = kRefObj;
    static const flag_t kREF_DSETREG = kRefDsetreg;

    static const char *predefinedName( flag_t pd )
    {
        switch( pd )
        {
        case kF32BE: return "H5T_IEEE_F32BE";
        case kF32LE: return "H5T_IEEE_F32LE";
        case kF64BE: return "H5T_IEEE_F64BE";
        case kF64LE: return "H5T_IEEE_F64LE";

        case kI8BE: return "H5T_STD_I8BE";
        case kI8LE: return "H5T_STD_I8LE";
        case kI16BE: return "H5T_STD_I16BE";
        case kI16LE: return "H5T_STD_I16LE";
        case kI32BE: return "H5T_STD_I32BE";
        case kI32LE: return "H5T_STD_I32LE";
        case kI64BE: return "H5T_STD_I64BE";
        case kI64LE: return "H5T_STD_I64LE";
            
        case kU8BE: return "H5T_STD_U8BE";
        case kU8LE: return "H5T_STD_U8LE";
        case kU16BE: return "H5T_STD_U16BE";
        case kU16LE: return "H5T_STD_U16LE";
        case kU32BE: return "H5T_STD_U32BE";
        case kU32LE: return "H5T_STD_U32LE";
        case kU64BE: return "H5T_STD_U64BE";
        case kU64LE: return "H5T_STD_U64LE";

        case kREF_OBJ: return "H5T_REF_OBJ";
        case kREF_DSETREG: return "H5T_REF_DSETREG";
        default: return "UNKNOWN";
        };
    }
    
    Datatype() : BaseObject() {}
    Datatype( hid_t builtIn ) : BaseObject( builtIn, false ) {}
    
    // Array creation
    void arrayCreate( const Datatype &baseType,
                      const Dimensions &dims );
    void arrayCreate( const Datatype &baseType, size_t rank0dim )
    {
        Dimensions dims( 1 );
        dims[0] = rank0dim;
        arrayCreate( baseType, dims );
    }

    // Open from attributes and datasets.
    void open( const Attribute &attr );
    void open( const Dataset &dset );

    // Array creation constructors.
    Datatype( const Datatype &baseType,
              const Dimensions &dims )
      : BaseObject()
    {
        arrayCreate( baseType, dims );
    }
    
    Datatype( const Datatype &baseType, size_t rank0dim )
      : BaseObject()
    {
        arrayCreate( baseType, rank0dim );
    }

    // Acquiring the Datatype of Attributes and Datasets.
    Datatype( const Attribute &attr ) : BaseObject() { open( attr ); }
    Datatype( const Dataset &dset ) : BaseObject() { open( dset ); }
    
    Datatype &operator=( hid_t propId );

    virtual ~Datatype();
    virtual void close();


    // Explicit copy functions.
    void copyFrom( const Datatype &other );

    //-*************************************************************************
    // Datatype functions

    // General
    bool equal( const Datatype &other ) const;

    // Equivalent actually compares the guts of the types
    // and sees if they're functionally the same, even if they're different
    // objects.
    bool equivalent( const Datatype &other ) const;
    
    void lock();
    H5T_class_t getClass() const;

    bool isIntegerClass() const
    { return ( getClass() == H5T_INTEGER ); }
    bool isFloatClass() const
    { return ( getClass() == H5T_FLOAT ); }
    bool isStringClass() const
    { return ( getClass() == H5T_STRING ); }
    bool isReferenceClass() const
    { return ( getClass() == H5T_REFERENCE ); }
    bool isVariableLengthClass() const
    { return ( getClass() == H5T_VLEN ); }
    bool isArrayClass() const
    { return ( getClass() == H5T_ARRAY ); }

    // Have to return base type here, because it's not an
    // allocated object.
    hid_t getSuper() const;

    // Atomic
    size_t getSize() const;

    // For strings, mostly.
    void setSize( size_t s );

    H5T_order_t getOrder() const;
    bool isBigEndian() const
    { return ( getOrder() == H5T_ORDER_BE ); }
    bool isLittleEndian() const
    { return ( getOrder() == H5T_ORDER_LE ); }

    H5T_sign_t getSign() const;
    bool isSigned() const
    { return ( getSign() == H5T_SGN_2 ); }
    bool isUnsigned() const
    { return ( getSign() == H5T_SGN_NONE ); }

    // Array stuff
    int getArrayNdims() const;
    int getRank() const { return getArrayNdims(); }
    void getArrayDims( Dimensions &dims ) const;
    size_t getRank0Dim() const;
};

} // End namespace AlembicHDF5

#endif
