//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

#include "MayaPointPrimitiveWriter.h"
#include "MayaUtility.h"

MayaPointPrimitiveWriter::MayaPointPrimitiveWriter(
    double iFrame, MDagPath & iDag, Alembic::AbcGeom::OObject & iParent,
    Alembic::AbcCoreAbstract::v1::TimeSamplingType & iTimeType,
    bool iWriteVisibility) :
    mIsAnimated(false), mDagPath(iDag), mCurIndex(0)
{
    MFnParticleSystem particle(mDagPath);
    Alembic::AbcGeom::OPoints obj(iParent, particle.name().asChar(), iTimeType);
    mSchema = obj.getSchema();

    mAttrs = AttributesWriterPtr(new AttributesWriter(iFrame, obj,
        particle, iTimeType, iWriteVisibility));

    MObject object = iDag.node();
    if (util::isAnimated(object))
        mIsAnimated = true;

    write(iFrame);
}

void MayaPointPrimitiveWriter::write(double iFrame)
{
    std::vector<float> position;
    std::vector<float> velocity;
    std::vector< uint64_t > particleIds;
    std::vector<float> width;
    float constantwidth = -1.0;

    bool runupFromStart = false;
    MTime to(iFrame);

    // need to force re-evaluation
    MFnParticleSystem particle(mDagPath);
    particle.evaluateDynamics(to, runupFromStart);

    unsigned int size = particle.count();

    Alembic::AbcGeom::OPointsSchema::Sample samp;

    if (size == 0)
    {
        mSchema.set(samp,
            Alembic::Abc::OSampleSelector(mCurIndex++, iFrame));
        return;
    }

    position.reserve(size*3);
    velocity.reserve(size*3);
    particleIds.reserve(size);
    width.reserve(size);

    // get particle position
    MVectorArray posArray;
    particle.position(posArray);
    for (size_t i = 0; i < size; i++)
    {
        MVector vec = posArray[i];
        position.push_back(vec.x);
        position.push_back(vec.y);
        position.push_back(vec.z);
    }
    samp.setPositions(
        Alembic::Abc::V3fArraySample((const Imath::V3f *) &position.front(),
            position.size()) );

    // get particle velocity
    MVectorArray vecArray;
    particle.velocity(vecArray);
    for (size_t i = 0; i < size; i++)
    {
        MVector vec = vecArray[i];
        velocity.push_back(vec.x);
        velocity.push_back(vec.y);
        velocity.push_back(vec.z);
    }

    // get particleIds
    MIntArray idArray;
    particle.particleIds(idArray);
    for (size_t i = 0; i < size; i++)
        particleIds.push_back(idArray[i]);

    // assume radius is width
    MDoubleArray radiusArray;
    particle.radius(radiusArray);
    constantwidth = radiusArray[0];
    bool isConstantWidth = true;
    for (size_t i = 0; i < size; i++)
    {
        float radius = radiusArray[i];
        width.push_back(radius);
        if (fabs(constantwidth-radius) > 1e-4)
            isConstantWidth = false;
    }

    // ignoring width and the velocity vectors for now
    mSchema.set(samp, Alembic::Abc::OSampleSelector(mCurIndex++, iFrame));
    mAttrs->write(iFrame);
}

unsigned int MayaPointPrimitiveWriter::getNumCVs()
{
    MFnParticleSystem particle(mDagPath);
    return particle.count();
}

bool MayaPointPrimitiveWriter::isAnimated() const
{
    return  mIsAnimated || (mAttrs != NULL && mAttrs->isAnimated());
}
