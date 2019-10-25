//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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
#include <Alembic/AbcGeom/GeometryScope.h>

namespace AbcGeom = Alembic::AbcGeom;

MayaPointPrimitiveWriter::MayaPointPrimitiveWriter(
    double iFrame, MDagPath & iDag, Alembic::AbcGeom::OObject & iParent,
    Alembic::Util::uint32_t iTimeIndex,
    const JobArgs & iArgs) :
    mIsAnimated(false), mDagPath(iDag)
{
    MFnParticleSystem particle(mDagPath);
    MString name = particle.name();

    name = util::stripNamespaces(name, iArgs.stripNamespace);

    Alembic::AbcGeom::OPoints obj(iParent, name.asChar(),
        iTimeIndex);
    mSchema = obj.getSchema();

    Alembic::Abc::OCompoundProperty cp;
    Alembic::Abc::OCompoundProperty up;
    if (AttributesWriter::hasAnyAttr(particle, iArgs))
    {
        cp = mSchema.getArbGeomParams();
        up = mSchema.getUserProperties();
    }

    mAttrs = AttributesWriterPtr(new AttributesWriter(cp, up, obj, particle,
        iTimeIndex, iArgs, true));

    MObject object = iDag.node();
    if (iTimeIndex != 0 && util::isAnimated(object))
    {
        mIsAnimated = true;
    }

    if (!mIsAnimated || iArgs.setFirstAnimShape)
    {
        write(iFrame);
    }
}

void MayaPointPrimitiveWriter::write(double iFrame)
{
    MStatus status;
    std::vector<float> position;
    std::vector<float> velocity;
    std::vector< Alembic::Util::uint64_t > particleIds;
    std::vector<float> width;

    bool runupFromStart = false;
    MTime to(iFrame, MTime::kSeconds);

    // need to force re-evaluation
    MFnParticleSystem particle(mDagPath);
    particle.evaluateDynamics(to, runupFromStart);

    unsigned int size = particle.count();

    Alembic::AbcGeom::OPointsSchema::Sample samp;

    if (size == 0)
    {
        samp.setPositions(Alembic::Abc::V3fArraySample(NULL, 0));
        samp.setVelocities(Alembic::Abc::V3fArraySample(NULL, 0));
        samp.setIds(Alembic::Abc::UInt64ArraySample(NULL, 0));

        mSchema.set(samp);
        return;
    }

    position.reserve(size*3);
    velocity.reserve(size*3);
    particleIds.reserve(size);
    width.reserve(size);

    // get particle position
    MVectorArray posArray;
    particle.position(posArray);
    for (unsigned int i = 0; i < size; i++)
    {
        MVector vec = posArray[i];
        position.push_back(static_cast<float>(vec.x));
        position.push_back(static_cast<float>(vec.y));
        position.push_back(static_cast<float>(vec.z));
    }
    samp.setPositions(
        Alembic::Abc::P3fArraySample((const Imath::V3f *) &position.front(),
            position.size() / 3) );

    // get particle velocity
    MVectorArray vecArray;
    particle.velocity(vecArray);
    for (unsigned int i = 0; i < size; i++)
    {
        MVector vec = vecArray[i];
        velocity.push_back(static_cast<float>(vec.x));
        velocity.push_back(static_cast<float>(vec.y));
        velocity.push_back(static_cast<float>(vec.z));
    }
    if (!velocity.empty())
    {
        samp.setVelocities(
            Alembic::Abc::V3fArraySample((const Imath::V3f *) &velocity.front(),
                velocity.size() / 3) );
    }

    // get particleIds
    MIntArray idArray;
    particle.particleIds(idArray);
    for (unsigned int i = 0; i < size; i++)
    {
        particleIds.push_back(idArray[i]);
    }
    samp.setIds(
        Alembic::Abc::UInt64ArraySample(&(particleIds.front()),
            particleIds.size()) );

    // assume radius is width
    MDoubleArray radiusArray;
    MPlug radius = particle.findPlug("radiusPP", true, &status);
    AbcGeom::GeometryScope widthScope = AbcGeom::kUnknownScope;
    if ( status == MS::kSuccess)
    {
        // RadiusPP exists, get all particles value
        widthScope = AbcGeom::kVaryingScope;
        particle.radius(radiusArray);
        for (unsigned int i = 0; i < size; i++)
        {
            float radius = static_cast<float>(radiusArray[i]);
            width.push_back(radius);
        }
    }
    else
    {
        // Get the value of the radius attribute
        widthScope = AbcGeom::kUniformScope;
        width.push_back( particle.findPlug("radius", true).asDouble() );
    }


    if (!width.empty())
    {
        Alembic::AbcGeom::OFloatGeomParam::Sample widthSamp;
        widthSamp.setVals(width);
        widthSamp.setScope(widthScope);
        samp.setWidths( widthSamp );
    }

    mSchema.set(samp);
}

unsigned int MayaPointPrimitiveWriter::getNumCVs()
{
    MFnParticleSystem particle(mDagPath);
    return particle.count();
}

bool MayaPointPrimitiveWriter::isAnimated() const
{
    return  mIsAnimated;
}
