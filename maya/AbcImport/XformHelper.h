//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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


#ifndef ABCIMPORT_TRANSFORMHELPER_H_
#define ABCIMPORT_TRANSFORMHELPER_H_

#include <maya/MObject.h>

#include <vector>
#include <string>

#include <Alembic/AbcGeom/IXform.h>
#include "NodeIteratorVisitorHelper.h"

MStatus connectToXform(const Alembic::AbcGeom::XformSample & iSamp,
    bool isConstant,
    MObject & iObject,
    std::vector<std::string> & oSampledTransOpNameList,
    std::vector<Prop> & iSampledPropList,
    std::size_t iFirstProp);

void readComplex(double iFrame, Alembic::AbcGeom::IXform & iNode,
    std::vector<double> & oSampleList);

void read(double iFrame, Alembic::AbcGeom::IXform & iNode,
    std::vector<double> & oSampleList, Alembic::AbcGeom::XformSample & oSamp);

// used during creation of the scene
// This function traverses the transform operation stack, and returns true if
// the transform stack can't be directly mapped to Maya's transform stack
bool isComplex(const Alembic::AbcGeom::XformSample & iSamp);

#endif  // ABCIMPORT_TRANSFORMHELPER_H_
