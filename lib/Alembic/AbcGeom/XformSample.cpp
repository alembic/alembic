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

#include <Alembic/AbcGeom/XformSample.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
TranslateData::TranslateData(const Abc::V3d & iData)
    : XformData(kTranslateOperation), m_data(iData)
{
}

//-*****************************************************************************
TranslateData::TranslateData(XformDataPtr iXform)
    : XformData(kTranslateOperation)
{
    if (iXform && iXform->getType() == kTranslateOperation)
    {
        boost::shared_ptr < TranslateData > t =
            boost::static_pointer_cast < TranslateData > (iXform);

        m_data = t->get();
    }
    else
    {
        m_data = V3d(0, 0, 0);
    }
}

//-*****************************************************************************
Abc::M44d TranslateData::getMatrix() const
{
    return Abc::M44d().setTranslation(m_data);
}

//-*****************************************************************************
ScaleData::ScaleData(const Abc::V3d & iData)
    : XformData(kScaleOperation), m_data(iData)
{
}

//-*****************************************************************************
ScaleData::ScaleData(XformDataPtr iXform) : XformData(kScaleOperation)
{
    if (iXform && iXform->getType() == kScaleOperation)
    {
        boost::shared_ptr < ScaleData > s =
            boost::static_pointer_cast < ScaleData > (iXform);
        m_data = s->get();
    }
    else
    {
        m_data = V3d(1, 1, 1);
    }
}

//-*****************************************************************************
Abc::M44d ScaleData::getMatrix() const
{
    return Abc::M44d().setScale(m_data);
}

//-*****************************************************************************
RotateData::RotateData(const Abc::V3d & iAxis, double iAngle)
    : XformData(kRotateOperation), m_axis(iAxis), m_angle(iAngle)
{
}

//-*****************************************************************************
RotateData::RotateData(XformDataPtr iXform)
    : XformData(kRotateOperation)
{
    if (iXform && iXform->getType() == kRotateOperation)
    {
        boost::shared_ptr < RotateData > r =
            boost::static_pointer_cast < RotateData > (iXform);
        m_angle = r->getAngle();
        m_axis = r->getAxis();
    }
    else
    {
        m_angle = 0;
        m_axis = V3d(0, 0, 0);
    }
}

//-*****************************************************************************
Abc::M44d RotateData::getMatrix() const
{
    return Abc::M44d().setAxisAngle(m_axis, m_angle);
}

//-*****************************************************************************
MatrixData::MatrixData(const Abc::M44d & iMatrix)
    : XformData(kMatrixOperation), m_matrix(iMatrix)
{
}

//-*****************************************************************************
MatrixData::MatrixData(XformDataPtr iXform)
    : XformData(kMatrixOperation)
{
    if (iXform && iXform->getType() == kMatrixOperation)
    {
        boost::shared_ptr < MatrixData > m =
            boost::static_pointer_cast < MatrixData > (iXform);
        m_matrix = m->get();
    }
}

//-*****************************************************************************
Abc::M44d MatrixData::getMatrix() const
{
    return m_matrix;
}

//-*****************************************************************************
XformSample::XformSample()
{
}

//-*****************************************************************************
XformDataPtr XformSample::get(size_t iIndex)
{
    if (iIndex >= m_xforms.size())
    {
        return XformDataPtr();
    }

    return m_xforms[iIndex];
}

} // End namespace AbcGeom
} // End namespace Alembic
