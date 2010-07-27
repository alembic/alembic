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

#include <Alembic/TakoAbstract/TransformOperation.h>

namespace Alembic {
namespace TakoAbstract {
namespace TAKO_ABSTRACT_LIB_VERSION_NS {

namespace {

//-*****************************************************************************
void encodeValue( const Value & iVal,
                  std::iostream& ioStrm,
                  std::vector<double> & ioVec )
{
    if ( iVal.mIsStatic )
    {
        ioStrm.write(reinterpret_cast<const char *>(&iVal.mValue),
                     sizeof(iVal.mValue));
    }
    else
    {
        uint16_t index = ioVec.size();
        ioStrm.write(reinterpret_cast<const char *>(&index), sizeof(index));
        ioVec.push_back(iVal.mValue);
    }
}

//-*****************************************************************************
void updateSample( uint16_t & ioIndex,
                   Value & ioVal,
                   const std::vector<double> & iSampledVec )
{
    // don't do anything
    if (ioVal.mIsStatic)
    {
        return;
    }

    if (ioIndex < iSampledVec.size())
    {
        ioVal.mValue = iSampledVec[ioIndex];
        ioVal.mIsStatic = false;
        ioIndex++;
    }
    else
    {
        throw std::runtime_error("Not enough sampled data!");
        // ioVal.mValue = 0.0;
        // ioIndex++;
    }
}

}  // End anonymous namespace

//-*****************************************************************************
TransformOperation::TransformOperation() {}

//-*****************************************************************************
TransformOperation::~TransformOperation() {}

//-*****************************************************************************
Scale::Scale( ScaleType iType,
              const Value& iX,
              const Value& iY,
              const Value& iZ )
{
    mType = iType;
    mX = iX;
    mY = iY;
    mZ = iZ;
}

//-*****************************************************************************
Scale::~Scale() {}

//-*****************************************************************************
void Scale::get( Value& oX, Value& oY, Value& oZ ) const
{
    oX = mX;
    oY = mY;
    oZ = mZ;
}

//-*****************************************************************************
Mat4x4 Scale::get() const
{
    Mat4x4 oMat;
    Imath::Vec3<double> scaleVec;
    scaleVec.x = mX.mValue;
    scaleVec.y = mY.mValue;
    scaleVec.z = mZ.mValue;
    return oMat.scale(scaleVec);
}

//-*****************************************************************************
Scale::ScaleType Scale::getType(void) const
{
    return mType;
}

//-*****************************************************************************
void Scale::encode( std::iostream & ioStrm,
                    std::vector<double> & ioVec) const
{
    static const char op = cScaleOperation;

    // first write the type of operation
    ioStrm.put(op);

    // put the type
    char type = mType;
    ioStrm.put(type);

    // determine what components are sampled for the sampled flag
    unsigned char xSamp = !mX.mIsStatic;
    unsigned char ySamp = !mY.mIsStatic;
    unsigned char zSamp = !mZ.mIsStatic;

    unsigned char sampFlag = (zSamp << 2) + (ySamp << 1) + xSamp;
    ioStrm.put(sampFlag);

    encodeValue(mX, ioStrm, ioVec);
    encodeValue(mY, ioStrm, ioVec);
    encodeValue(mZ, ioStrm, ioVec);
}

//-*****************************************************************************
void Scale::updateSamples( uint16_t & ioIndex,
                           const std::vector<double> & iSampledVec )
{
    updateSample(ioIndex, mX, iSampledVec);
    updateSample(ioIndex, mY, iSampledVec);
    updateSample(ioIndex, mZ, iSampledVec);
}

//-*****************************************************************************
Translate::Translate( TranslateType iType,
                      const Value& iX,
                      const Value& iY,
                      const Value& iZ )
{
    mType = iType;
    mX = iX;
    mY = iY;
    mZ = iZ;
}

//-*****************************************************************************
Translate::~Translate() {}

//-*****************************************************************************
void Translate::get( Value& oX, Value& oY, Value& oZ ) const
{
    oX = mX;
    oY = mY;
    oZ = mZ;
}

//-*****************************************************************************
Mat4x4 Translate::get() const
{
    Mat4x4 oMat;
    Imath::Vec3<double> transVec;
    transVec.x = mX.mValue;
    transVec.y = mY.mValue;
    transVec.z = mZ.mValue;

    return oMat.translate( transVec );
}

//-*****************************************************************************
Translate::TranslateType Translate::getType() const
{
    return mType;
}

//-*****************************************************************************
void Translate::encode( std::iostream & ioStrm,
                        std::vector<double> & ioVec ) const
{
    static const char op = cTranslateOperation;

    // first write the type of operation
    ioStrm.put(op);

    // put the type
    char type = mType;
    ioStrm.put(type);

    // determine what components are sampled for the sampled flag
    unsigned char xSamp = !mX.mIsStatic;
    unsigned char ySamp = !mY.mIsStatic;
    unsigned char zSamp = !mZ.mIsStatic;

    unsigned char sampFlag = (zSamp << 2) + (ySamp << 1) + xSamp;
    ioStrm.put(sampFlag);

    encodeValue(mX, ioStrm, ioVec);
    encodeValue(mY, ioStrm, ioVec);
    encodeValue(mZ, ioStrm, ioVec);
}

//-*****************************************************************************
void Translate::updateSamples( uint16_t & ioIndex,
                               const std::vector<double> & iSampledVec )
{
    updateSample(ioIndex, mX, iSampledVec);
    updateSample(ioIndex, mY, iSampledVec);
    updateSample(ioIndex, mZ, iSampledVec);
}

//-*****************************************************************************
Rotate::Rotate( RotateType iType,
                const Value & iAngle,
                float iXAxis, float iYAxis, float iZAxis )
{
    mType = iType;
    mAngle = iAngle;
    mXAxis = iXAxis;
    mYAxis = iYAxis;
    mZAxis = iZAxis;
}

//-*****************************************************************************
Rotate::~Rotate() {}

//-*****************************************************************************
void Rotate::get( Value& oAngle, float& oXAxis, float& oYAxis,
                  float &oZAxis ) const
{
    oAngle = mAngle;
    oXAxis = mXAxis;
    oYAxis = mYAxis;
    oZAxis = mZAxis;
}

//-*****************************************************************************
Mat4x4 Rotate::get() const
{
    Mat4x4 oMat;
    Imath::Vec3<double> rotVec;

    rotVec.x = mXAxis;
    rotVec.y = mYAxis;
    rotVec.z = mZAxis;
    return oMat.setAxisAngle( rotVec, mAngle.mValue );
}

//-*****************************************************************************
Rotate::RotateType Rotate::getType() const
{
    return mType;
}

//-*****************************************************************************
void Rotate:: encode( std::iostream & ioStrm,
                      std::vector<double> & ioVec ) const
{
    static const char op = cRotateOperation;

    // first write the type of operation
    ioStrm.put(op);

    // put the type
    char type = mType;
    ioStrm.put(type);

    // determine if the angle is sampled for the flag
    unsigned char sampFlag = !mAngle.mIsStatic;
    ioStrm.put(sampFlag);

    encodeValue(mAngle, ioStrm, ioVec);

    // write out the axis values
    ioStrm.write(reinterpret_cast<const char *>(&mXAxis), sizeof(mXAxis));
    ioStrm.write(reinterpret_cast<const char *>(&mYAxis), sizeof(mYAxis));
    ioStrm.write(reinterpret_cast<const char *>(&mZAxis), sizeof(mZAxis));
}

//-*****************************************************************************
void Rotate::updateSamples( uint16_t & ioIndex,
                            const std::vector<double> & iSampledVec )
{
    updateSample( ioIndex, mAngle, iSampledVec );
}

//-*****************************************************************************
Matrix4x4::Matrix4x4( Matrix4x4::Matrix4x4Type iType,
                      const Value & iM11, const Value & iM12,
                      const Value & iM13, const Value & iM14,
                      const Value & iM21, const Value & iM22,
                      const Value & iM23, const Value & iM24,
                      const Value & iM31, const Value & iM32,
                      const Value & iM33, const Value & iM34,
                      const Value & iM41, const Value & iM42,
                      const Value & iM43, const Value & iM44 )
{
    mType = iType;
    mM[0] = iM11;
    mM[1] = iM12;
    mM[2] = iM13;
    mM[3] = iM14;

    mM[4] = iM21;
    mM[5] = iM22;
    mM[6] = iM23;
    mM[7] = iM24;

    mM[8] = iM31;
    mM[9] = iM32;
    mM[10] = iM33;
    mM[11] = iM34;

    mM[12] = iM41;
    mM[13] = iM42;
    mM[14] = iM43;
    mM[15] = iM44;
}

//-*****************************************************************************
Matrix4x4::~Matrix4x4() {}

//-*****************************************************************************
void Matrix4x4::get(
    Value & oM11, Value & oM12, Value & oM13, Value & oM14,
    Value & oM21, Value & oM22, Value & oM23, Value & oM24,
    Value & oM31, Value & oM32, Value & oM33, Value & oM34,
    Value & oM41, Value & oM42, Value & oM43, Value & oM44 ) const
{
    oM11 = mM[0];
    oM12 = mM[1];
    oM13 = mM[2];
    oM14 = mM[3];

    oM21 = mM[4];
    oM22 = mM[5];
    oM23 = mM[6];
    oM24 = mM[7];

    oM31 = mM[8];
    oM32 = mM[9];
    oM33 = mM[10];
    oM34 = mM[11];

    oM41 = mM[12];
    oM42 = mM[13];
    oM43 = mM[14];
    oM44 = mM[15];
}

//-*****************************************************************************
Mat4x4 Matrix4x4::get() const
{
    Mat4x4 oMat(
        mM[ 0].mValue, mM[ 1].mValue, mM[ 2].mValue, mM[ 3].mValue,
        mM[ 4].mValue, mM[ 5].mValue, mM[ 6].mValue, mM[ 7].mValue,
        mM[ 8].mValue, mM[ 9].mValue, mM[10].mValue, mM[11].mValue,
        mM[12].mValue, mM[13].mValue, mM[14].mValue, mM[15].mValue);
    return oMat;
}

//-*****************************************************************************
Matrix4x4::Matrix4x4Type Matrix4x4::getType() const
{
    return mType;
}

//-*****************************************************************************
void Matrix4x4::encode( std::iostream & ioStrm,
                        std::vector<double> & ioVec) const
{

    static const char op = cMatrixOperation;

    // first write the type of operation
    ioStrm.put(op);

    // put the sub-type
    char type = mType;
    ioStrm.put(type);

    // determine what components are sampled for the sampled flag
    uint16_t sampFlag = 0;
    unsigned int i;

    // unlike the other transformation operations, Matrix4x4 has up to
    // 16 channels that could be sampled over time, so we need a 2 byte value
    // to store the sampled or not sampled flags instead of just a 1 byte value
    for (i = 0; i < 16; i++)
    {
        uint16_t flag = !mM[i].mIsStatic;
        sampFlag +=  flag << i;
    }

    // write out the 2 byte flag
    ioStrm.write(reinterpret_cast<const char *> (&sampFlag), sizeof(sampFlag));

    for (i = 0; i < 16; i++)
    {
        encodeValue(mM[i], ioStrm, ioVec);
    }
}

//-*****************************************************************************
void Matrix4x4::updateSamples( uint16_t & ioIndex,
                               const std::vector<double> & iSampledVec )
{
    for (unsigned int i = 0; i < 16; i++)
    {
        updateSample(ioIndex, mM[i], iSampledVec);
    }
}

} // End namespace TAKO_ABSTRACT_LIB_VERSION_NS
} // End namespace TakoAbstract
} // End namespace Alembic
