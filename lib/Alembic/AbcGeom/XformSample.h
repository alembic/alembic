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

#ifndef _Alembic_AbcGeom_XformSample_h_
#define _Alembic_AbcGeom_XformSample_h_

#include <Alembic/AbcGeom/Foundation.h>

namespace Alembic {
namespace AbcGeom {

//! Base class for holding transform operations, and the data which defines
//! them.  This is useful for applications which only care about the operation
//! type and the data that goes along with it, and not which channels are
//! static or animated.
class XformData
{
public:
    XformData(XformOperationType iType) : m_type(iType) {}
    XformOperationType getType() const {return m_type;}
private:
    XformOperationType m_type;
};

typedef boost::shared_ptr < XformData > XformDataPtr;

//! Holds a translate vector.
class TranslateData : public XformData
{
public:

    TranslateData(const Abc::V3d & iData);

    //! Copies (and casts) the data from iXform.
    //! If iXform is NULL, or the type is not kTranslateOperation then
    //! the translate vector defaults to (0, 0, 0)
    TranslateData(XformDataPtr iXform);

    Abc::V3d get() const {return m_data;}

    Abc::M44d getMatrix() const;

private:
    Abc::V3d m_data;
};

//! Holds a scale vector.
class ScaleData : public XformData
{
public:
    ScaleData(const Abc::V3d & iData);

    //! Copies (and casts) the data from iXform.
    //! If iXform is NULL, or the type is not kScaleOperation then
    //! the scale vector defaults to (1, 1, 1)
    ScaleData(XformDataPtr iXform);

    Abc::M44d getMatrix() const;

    Abc::V3d get() const {return m_data;}

private:
    Abc::V3d m_data;
};

//! Holds the rotate vector and the rotate angle
class RotateData : public XformData
{
public:
    RotateData(const Abc::V3d & iAxis, double iAngle);

    //! Copies (and casts) the data from iXform.
    //! If iXform is NULL, or the type is not kRotateOperation then
    //! the rotation axis defaults to (0, 0, 0) and the rotation angle
    //! defaults to 0.
    RotateData(XformDataPtr iXform);

    Abc::M44d getMatrix() const;

    Abc::V3d getAxis() const {return m_axis;}
    double getAngle() const {return m_angle;}

private:
    Abc::V3d m_axis;
    double m_angle;
};

//! Holds the 4x4 matrix
class MatrixData : public XformData
{
public:
    MatrixData(const Abc::M44d & iMatrix);

    //! Copies (and casts) the data from iXform.
    //! If iXform is NULL, or the type is not kMatrixOperation then
    //! the matrix defaults to identity.
    MatrixData(XformDataPtr iXform);

    Abc::M44d getMatrix() const;

    Abc::M44d get() const {return m_matrix;}

private:
    Abc::M44d m_matrix;
};

//! Class which holds the data for multiple transform operations.
class XformSample
{
public:
    XformSample();

    //! Returns the total number of transform operations
    size_t getNum() {return m_xforms.size();}

    //! Returns a specific transform operation at iIndex.  If iIndex exceeds
    //! the total number of transform operations an empty XformDataPtr is
    //! returned.
    XformDataPtr get(size_t iIndex);

    void push(XformDataPtr iXform) {m_xforms.push_back(iXform);};
    void clear() {m_xforms.clear();};
private:
    std::vector < XformDataPtr > m_xforms;
};

} // End namespace AbcGeom
} // End namespace Alembic

#endif
