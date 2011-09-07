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

#include "util.h"
#include "PointHelper.h"
#include "NodeIteratorVisitorHelper.h"

#include <maya/MString.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MVector.h>
#include <maya/MGlobal.h>
#include <maya/MVectorArray.h>
#include <maya/MFnParticleSystem.h>


MStatus read(double iFrame, const Alembic::AbcGeom::IPoints & iNode,
    MObject & iObject)
{
    MStatus status = MS::kSuccess;

    printWarning("Reading animated particle data not supported.");

    return status;
}

MStatus create(double iFrame, const Alembic::AbcGeom::IPoints & iNode,
    MObject & iParent, MObject & iObject)
{
    MStatus status = MS::kSuccess;
    Alembic::AbcGeom::IPointsSchema schema = iNode.getSchema();

    // object has no samples, bail early
    if (schema.getNumSamples() == 0)
    {
        return status;
    }

    Alembic::AbcGeom::IPointsSchema::Sample samp;
    Alembic::AbcCoreAbstract::index_t index =
        schema.getTimeSampling()->getNearIndex(
            iFrame, schema.getNumSamples()).first;

    schema.get(samp, index);

    size_t pSize = samp.getPositions()->size();

    // bail early if there's no particle data at this frame
    if (pSize == 0)
    {
        return status;
    }

    // convert the data to Maya format
    MFnParticleSystem fnParticle;
    iObject = fnParticle.create(iParent, &status);
    fnParticle.setObject(iObject);
    fnParticle.setName(iNode.getName().c_str());

    MPointArray pArray;
    Alembic::Abc::P3fArraySamplePtr v3ptr = samp.getPositions();
    MVectorArray vArray;

    for (unsigned int pId = 0; pId < pSize; pId++)
    {
        pArray.append((*v3ptr)[pId].x,
                      (*v3ptr)[pId].y,
                      (*v3ptr)[pId].z);
    }

    status = fnParticle.emit(pArray);
    status = fnParticle.saveInitialState();

    return status;
}
