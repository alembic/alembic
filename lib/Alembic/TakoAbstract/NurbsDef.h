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

#ifndef _Alembic_TakoAbstract_NurbsDef_h_
#define _Alembic_TakoAbstract_NurbsDef_h_

#include <Alembic/TakoAbstract/Foundation.h>

namespace Alembic {

namespace TakoAbstract {

namespace TAKO_ABSTRACT_LIB_VERSION_NS {

//-*****************************************************************************
struct NurbsSurfaceMiscInfo
{
    unsigned int    spanU;
    unsigned int    spanV;
    unsigned char   degreeU;
    unsigned char   degreeV;
    unsigned char   formU;
    unsigned char   formV;

    float   minU;
    float   maxU;
    float   minV;
    float   maxV;
};

//-*****************************************************************************
struct NurbsCurveMiscInfo
{   
    unsigned int    numCV;
    unsigned int    span;
    unsigned char   degree;
    unsigned char   form;

    float   min;
    float   max;
};

//-*****************************************************************************
// for Nurbs Surface, each instance of NurbsCurveGrp is a trim curve
class NurbsCurveGrp
{
public:
    NurbsCurveGrp()
      : numCurve(0),
        constantWidth(0.0) {}
    
    bool isValid() const;
    
    unsigned int getNumCV() const
    {
        return cv.size()/4;
    }

    unsigned int getNumCV(unsigned int index) const
    {
        return miscInfo[index].numCV;
    }

    unsigned int getSpan(unsigned int index) const
    {
        return miscInfo[index].span;
    }

    unsigned int getDegree(unsigned int index) const
    {
        return miscInfo[index].degree;
    }

    unsigned char getForm(unsigned int index) const
    {
        return miscInfo[index].form;
    }

    float getMin(unsigned int index) const
    {
        return miscInfo[index].min;
    }

    float getMax(unsigned int index) const
    {
        return miscInfo[index].max;
    }

    unsigned int numCurve;
    std::vector<NurbsCurveMiscInfo> miscInfo;
    std::vector<float> cv;
    std::vector<float> knot;
    
    // optional
    std::vector<float> width;
    float constantWidth;
};

} // End namespace TAKO_ABSTRACT_LIB_VERSION_NS

} // End namespace TakoAbstract

} // End namespace Alembic

#endif  // _Alembic_TakoAbstract_NurbsDef_h_
