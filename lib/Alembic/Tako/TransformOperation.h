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


#ifndef _Alembic_Tako_TransformOperation_h_
#define _Alembic_Tako_TransformOperation_h_

#include <Alembic/Tako/Foundation.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {

/**
\brief A simple container for a double and whether the data is static.

This simple class is meant to deal with the question: "How do you tell the
difference between a static value and a sampled value?"  When reading and
writing sampled data we only deal with one frame at a time and so for a
particular point in time  it would be easy to confuse the static data with the
sampled data.  Since parts of a transform operation could be static or animated,
Value presents a simple way to differentiate it.
*/
class Value {
    public:

        Value()
        {
            mIsStatic = true;
            mValue = 0.0;
        };

        Value(double iVal) // NOLINT
        {
            mIsStatic = true;
            mValue = iVal;
        };

        Value& operator=(double iVal)
        {
            mValue = iVal;
            return *this;
        };

        bool operator==(double iVal)
        {
            return mValue == iVal;
        };

        bool   mIsStatic;
        double mValue;
};


/**
/brief A typedef to Imath Matrix44<double>.

Mat4x4 is a typedef for Imath's Matrix44&ltdouble&gt;, and is used for storing
the transformation matrix.
*/
typedef Imath::Matrix44 <double> Mat4x4;

/**
\brief Enum that indicates what type of transformational operation.

This enum is used when encoding and decoding the transform operation data.
*/
enum TransformOperationType
{
    cScaleOperation = 0,
    cTranslateOperation = 1,
    cRotateOperation = 2,
    cMatrixOperation = 3
};

/**
\brief Base class for all transformation operations.

TransformOperation is the base class for all transform operation classes.
These classes exist because different applications (Houdini or Maya for example)
store their transformation matrix, not as a simple 4x4, but as several
transformation operations with lot's of redundancy.  For example, Maya has
several translation operations for handling pivots.
*/
class TransformOperation
{
    public:
        /**
        \brief The default constructor.
        */
        TransformOperation();

        /**
        \brief The default constructor.

        The destructor is virtual because TransformOperation is a base class.
        */
        virtual ~TransformOperation();

        /**
        \brief Gets the 4x4 matrix of doubles associated with this operation.
        \return A 4x4 matrix of doubles representing this operation.

        This member function is virtual and must be overridden.
        This will return a 4x4 matrix.
        */
        virtual Mat4x4 get() const = 0;

        // currently only used by TransformWriter to encode only the Value
        // into ioStrm and to seperate the static from the animated data
        virtual void encode(std::iostream & ioStrm,
            std::vector<double> & ioVec) const = 0;

        // currently only used by TransformReader to update the animated Values
        virtual void updateSamples(uint16_t & ioIndex,
            const std::vector<double> & iSampledVec) = 0;
};

/**
\brief A class to encapsulate a scale transformation operation.
*/

class Scale : public TransformOperation
{
    public:

        /**
        \brief The Scale identifier type.

        Some 3d packages (like Maya) have multiple transformation operations
        that are mathmatically of the same type.  ScaleType is meant to
        help disambiguate these similiar mathmatical types when reading back
        into applications that natively support the type.
        */
        enum ScaleType
        {
            /** Default, regular scale. */
            cScale = 0
        };

        /**
        \brief Constructor which takes Value for each axis.
        \param iType The scale type.
        \param iX Const reference scale value for the X axis.
        \param iY Const reference scale value for the Y axis.
        \param iZ Const reference scale value for the Z axis.
        */
        Scale(ScaleType iType, const Value & iX, const Value & iY,
            const Value & iZ);

        /**
        \brief The destructor
        */
        ~Scale();

        /**
        \brief Gets the scale values for each of the axes.
        \param oX On return, the scale X value.
        \param oY On return, the scale Y value.
        \param oZ On return, the scale Z value.
        */
        void get(Value& oX, Value& oY, Value& oZ) const;

        /**
        \brief Gets the 4x4 matrix for the scale.
        \return The 4x4 matrix containing the scale values.
        */
        virtual Mat4x4 get() const;

        /**
        \brief Returns the scale type.
        \return The scale type.
        */
        ScaleType getType() const;

        // currently only used by TransformWriter to encode only the Value
        // into ioStrm and to seperate the static from the animated data
        virtual void encode(std::iostream & ioStrm,
            std::vector<double> & ioVec) const;

        // currently only used by TransformReader to update the animated Values
        virtual void updateSamples(uint16_t & ioIndex,
            const std::vector<double> & iSampledVec);

    private:
        ScaleType mType;
        Value mX, mY, mZ;
};

/**
\brief A class to encapsulate a translation transformation operation.
*/

class Translate : public TransformOperation
{
    public:
        /**
        \brief The Translation identifier type.

        Some 3d packages (like Maya) have multiple transformation operations
        that are mathmatically of the same type.  TranslateType is meant to
        help disambiguate these similiar mathmatical types when reading back
        into applications that natively support the type.
        */
        enum TranslateType
        {
            /** Default, regular translation. */
            cTranslate = 0,

            /** Translation used for scaling around a pivot point.*/
            cScalePivotPoint = 1,

            /** Translation which is used to help preserve existing scale
            transformations when moving the pivot.*/
            cScalePivotTranslation = 2,

            /** Translation used for rotating around the pivot point. */
            cRotatePivotPoint = 3,

            /** Translation which is used to help preserve existing rotate
            transformations when moving the pivot. */
            cRotatePivotTranslation = 4
        };

        /**
        \brief Constructor which sets the translation value for each axis.
        \param iType The translation type.
        \param iX Const reference translation value for the X axis.
        \param iY Const reference translation value for the Y axis.
        \param iZ Const reference translation value for the Z axis.
        */
        Translate(TranslateType iType, const Value& iX, const Value& iY,
            const Value& iZ);

        /**
        \brief The destructor
        */
        ~Translate();

        /**
        \brief Gets the translation values for each of the axes.
        \param oX On return, the translation X value.
        \param oY On return, the translation Y value.
        \param oZ On return, the translation Z value.
        */
        void get(Value& oX, Value& oY, Value& oZ) const;

        /**
        \brief Gets the 4x4 matrix for the translation.

        \return The 4x4 matrix containing the translation values.
        */
        Mat4x4 get() const;

        /**
        \brief Returns the translation type.
        \return The translation type.
        */
        TranslateType getType() const;

        // currently only used by TransformWriter to encode only the Values
        // into ioStrm and to seperate the static from the animated data
        virtual void encode(std::iostream & ioStrm,
            std::vector<double> & ioVec) const;

        // currently only used by TransformReader to update the animated Values
        virtual void updateSamples(uint16_t & ioIndex,
            const std::vector<double> & iSampledVec);

    private:
        Value mX, mY, mZ;
        TranslateType mType;
};

/**
\brief A class to encapsulate a rotation transformation operation.
*/
class Rotate : public TransformOperation
{
    public:

        /**
        \brief The Rotate identifier type.

        Some 3d packages (like Maya) have multiple rotation operations
        that are mathmatically of the same type.  RotateType is meant to
        help disambiguate these similiar mathmatical types when reading back
        into applications that natively support the type.
        */
        enum RotateType
        {
            /** Default regular rotation */
            cRotate = 0,

            /** Rotation that goes along with the rotate pivot to help
            orient the local rotation space.*/
            cRotateOrientation = 1
        };

        /**
        \brief Constructor which sets the angular value and the axis for the
        rotation.
        \param iType The rotation type.
        \param iAngle Const reference to the angular rotation value in radians.
        \param iXAxis X component of the rotation axis.
        \param iYAxis Y component of the rotation axis.
        \param iZAxis Z component of the rotation axis.
        */
        Rotate(RotateType iType, const Value& iAngle, float iXAxis,
            float iYAxis, float iZAxis);

        /**
        \brief The destructor.
        */
        ~Rotate();

        /**
        \brief Gets the rotation angle and the rotation axis.
        \param oAngle On retrun, the angular rotation value in radians.
        \param oXAxis On return, the X component of the rotation axis.
        \param oYAxis On return, the Y component of the rotation axis.
        \param oZAxis On return, the Z component of the rotation axis.
        */
        void get(Value& oAngle, float& oXAxis, float& oYAxis, float& oZAxis)
            const;

        /**
        \brief Gets the 4x4 matrix for the rotation.

        \return The 4x4 matrix containing the rotation values.
        */
        Mat4x4 get() const;

        /**
        \brief Returns the rotation type.
        \return The rotation type.
        */
        RotateType getType() const;

        // currently only used by TransformWriter to encode only the angle Value
        // into ioStrm and to seperate the static from the animated data
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

/**
\brief A class to encapsulate a transformation operation.

This class is used to represent all other transformations that can't be
represented by a translation, rotation, or scale.
*/
class Matrix4x4 : public TransformOperation
{
    public:

        /**
        \brief The Matrix4x4 identifier type.

        Some 3d packages (like Maya) may have certain transformation operations
        that aren't supported in other packages.  Matrix4x4Type is meant to
        help with reading back into applications that natively support the type.
        */
        enum Matrix4x4Type
        {
            /** Regular 4x4 Matrix. */
            cMatrix = 0,

            /** Matrix represents Maya's version of Shear */
            cMayaShear = 1
        };

        /**
        \brief Constructor which sets each element of the matrix.
        \param iType The matrix type.
        \param iM11 first row, first column element
        \param iM12 first row, second column element
        \param iM13 first row, third column element
        \param iM14 first row, fourth column element
        \param iM21 second row, first column element
        \param iM22 second row, second column element
        \param iM23 second row, third column element
        \param iM24 second row, fourth column element
        \param iM31 third row, first column element
        \param iM32 third row, second column element
        \param iM33 third row, third column element
        \param iM34 third row, fourth column element
        \param iM41 fourth row, first column element
        \param iM42 fourth row, second column element
        \param iM43 fourth row, third column element
        \param iM44 fourth row, fourth column element

        Sets each element  of the matrix in row major order.
        */
        Matrix4x4(const Matrix4x4Type iType,
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
            const Value & iM44);

        /**
        \brief The destructor.
        */
        ~Matrix4x4();

        /**
        \brief Gets the matrix Values.
        \param oM11 On return the first row, first column element
        \param oM12 On return the first row, second column element
        \param oM13 On return the first row, third column element
        \param oM14 On return the first row, fourth column element
        \param oM21 On return the second row, first column element
        \param oM22 On return the second row, second column element
        \param oM23 On return the second row, third column element
        \param oM24 On return the second row, fourth column element
        \param oM31 On return the third row, first column element
        \param oM32 On return the third row, second column element
        \param oM33 On return the third row, third column element
        \param oM34 On return the third row, fourth column element
        \param oM41 On return the fourth row, first column element
        \param oM42 On return the fourth row, second column element
        \param oM43 On return the fourth row, third column element
        \param oM44 On return the fourth row, fourth column element

        Sets each element  of the matrix in row major order.
        */
        void get(Value & oM11, Value & oM12, Value & oM13, Value & oM14,
            Value & oM21, Value & oM22, Value & oM23, Value & oM24,
            Value & oM31, Value & oM32, Value & oM33, Value & oM34,
            Value & oM41, Value & oM42, Value & oM43, Value & oM44) const;

        /**
        \brief Gets the 4x4 matrix.

        \return The 4x4 matrix.
        */
        Mat4x4 get() const;

        /**
        \brief Returns the matrix type.
        \return The matrix type.
        */
        Matrix4x4Type getType() const;

        // currently only used by TransformWriter to encode the matrix Values
        // into ioStrm and to seperate the static from the animated data
        virtual void encode(std::iostream & ioStrm,
            std::vector<double> & ioVec) const;

        // currently only used by TransformReader to update the animated Values
        virtual void updateSamples(uint16_t & ioIndex,
            const std::vector<double> & iSampledVec);

    private:
        Matrix4x4Type mType;
        Value mM[16];
};

/**
\brief A variant that can contain a Translate, Scale, Rotate, or
Matrix4x4

*/
typedef boost::variant<Translate, Scale, Rotate, Matrix4x4>
    TransformOperationVariant;
}

using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_TransformOperation_h_
