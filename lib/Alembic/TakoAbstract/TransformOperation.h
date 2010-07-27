//-*****************************************************************************
//
// Copyright (c) 2009-2010,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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


#ifndef _Alembic_TakoAbstract_TransformOperation_h_
#define _Alembic_TakoAbstract_TransformOperation_h_

#include <Alembic/TakoAbstract/Foundation.h>

namespace Alembic {

namespace TakoAbstract {

namespace TAKO_ABSTRACT_LIB_VERSION_NS {

//-*****************************************************************************
class Value
{
public:
    Value()
      : mIsStatic( true ),
        mValue( 0.0 ) {}
    
    Value( double iVal ) // NOLINT
      : mIsStatic( true ),
        mValue( iVal ) {}

    Value( const Value& copy )
      : mIsStatic( copy.mIsStatic ),
        mValue( copy.mValue ) {}
    
    Value& operator=( double iVal )
    {
        mValue = iVal;
        return *this;
    }

    Value& operator=( const Value& copy )
    {
        mIsStatic = copy.mIsStatic;
        mValue = copy.mValue;
        return *this;
    }
    
    bool operator==( double iVal )
    {
        return mValue == iVal;
    };

    operator double() const { return mValue; }
    
    bool   mIsStatic;
    double mValue;
};

//-*****************************************************************************
typedef Imath::Matrix44 <double> Mat4x4;

//-*****************************************************************************
enum TransformOperationType
{
    cScaleOperation = 0,
    cTranslateOperation = 1,
    cRotateOperation = 2,
    cMatrixOperation = 3
};

//-*****************************************************************************
class TransformOperation
{
public:
    TransformOperation();

    virtual ~TransformOperation();

    virtual Mat4x4 get() const = 0;
    
    virtual void encode( std::iostream & ioStrm,
                         std::vector<double> & ioVec) const = 0;
    
    virtual void updateSamples( uint16_t & ioIndex,
                                const std::vector<double> & iSampledVec) = 0;
};

//-*****************************************************************************
class Scale : public TransformOperation
{
public:
    
    enum ScaleType
    {
        cScale = 0
    };

    Scale( ScaleType iType,
           const Value & iX,
           const Value & iY,
           const Value & iZ );

    ~Scale();
    
    void get( Value& oX, Value& oY, Value& oZ ) const;

    virtual Mat4x4 get() const;

    ScaleType getType() const;

    virtual void encode( std::iostream & ioStrm,
                         std::vector<double> & ioVec) const;

    virtual void updateSamples(uint16_t & ioIndex,
                               const std::vector<double> & iSampledVec);

private:
    ScaleType mType;
    Value mX, mY, mZ;
};

//-*****************************************************************************
class Translate : public TransformOperation
{
public:
    enum TranslateType
    {
        cTranslate = 0,
        cScalePivotPoint = 1,
        cScalePivotTranslation = 2,
        cRotatePivotPoint = 3,
        cRotatePivotTranslation = 4
    };
    
    Translate( TranslateType iType,
               const Value& iX,
               const Value& iY,
               const Value& iZ );
    
    ~Translate();
    
    void get( Value& oX, Value& oY, Value& oZ ) const;

    Mat4x4 get() const;
    
    TranslateType getType() const;
    
    virtual void encode(std::iostream & ioStrm,
                        std::vector<double> & ioVec) const;
    
    virtual void updateSamples(uint16_t & ioIndex,
                               const std::vector<double> & iSampledVec);

private:
    Value mX, mY, mZ;
    TranslateType mType;
};

//-*****************************************************************************
class Rotate : public TransformOperation
{
public:
    enum RotateType
    {
        cRotate = 0,
        cRotateOrientation = 1
    };
    
    Rotate( RotateType iType,
            const Value& iAngle,
            float iXAxis,
            float iYAxis,
            float iZAxis );

    ~Rotate();

    void get( Value& oAngle,
              float& oXAxis,
              float& oYAxis,
              float& oZAxis ) const;
    
    Mat4x4 get() const;
    
    RotateType getType() const;
    
    virtual void encode(std::iostream & ioStrm,
                        std::vector<double> & ioVec) const;
    
    // currently only used by TransformReader to update an animated angle.
    virtual void updateSamples(uint16_t & ioIndex,
                               const std::vector<double> & iSampledVec);

private:
    float mXAxis, mYAxis, mZAxis;
    Value mAngle;
    RotateType mType;
};

//-*****************************************************************************
class Matrix4x4 : public TransformOperation
{
public:
    enum Matrix4x4Type
    {
        cMatrix = 0,
        cMayaShear = 1
    };
    
    Matrix4x4( const Matrix4x4Type iType,
               const Value & iM11,
               const Value & iM12,
               const Value & iM13,
               const Value & iM14,
               const Value & iM21,
               const Value & iM22,
               const Value & iM23,
               const Value & iM24,
               const Value & iM31,
               const Value & iM32,
               const Value & iM33,
               const Value & iM34,
               const Value & iM41,
               const Value & iM42,
               const Value & iM43,
               const Value & iM44 );

    ~Matrix4x4();
    
    void get( Value & oM11, Value & oM12, Value & oM13, Value & oM14,
              Value & oM21, Value & oM22, Value & oM23, Value & oM24,
              Value & oM31, Value & oM32, Value & oM33, Value & oM34,
              Value & oM41, Value & oM42, Value & oM43, Value & oM44 ) const;

    Mat4x4 get() const;
    
    Matrix4x4Type getType() const;
    
    virtual void encode( std::iostream & ioStrm,
                         std::vector<double> & ioVec ) const;
    
    virtual void updateSamples( uint16_t & ioIndex,
                                const std::vector<double> & iSampledVec );
    
private:
    Matrix4x4Type mType;
    Value mM[16];
};

//-*****************************************************************************
typedef boost::variant< Translate,
                        Scale,
                        Rotate,
                        Matrix4x4 > TransformOperationVariant;

} // End namespace TAKO_ABSTRACT_LIB_VERSION_NS

} // End namespace TakoAbstract

} // End namespace Alembic

#endif  // _Alembic_TakoAbstract_TransformOperation_h_
