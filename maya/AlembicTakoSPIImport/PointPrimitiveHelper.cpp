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

#include <AlembicTakoSPIImport/PointPrimitiveHelper.h>
#include <AlembicTakoSPIImport/Util.h>
#include <AlembicTakoSPIImport/NodeIteratorVisitorHelper.h>

namespace Alembic {

MStatus read(float iFrame, const TakoSPI::PointPrimitiveReaderPtr & iNode,
    MObject & iObject)
{
    MStatus status = MS::kSuccess;

    printWarning("Reading animated particle data not supported.");

    return status;
}

MStatus create(float iFrame, const TakoSPI::PointPrimitiveReaderPtr & iNode,
    MObject & iParent, MObject & iObject,
    std::vector<std::string> & iSampledPropNameList,
    bool iSwap)
{
    MStatus status = MS::kSuccess;

    iNode->read(iFrame);

    unsigned int sizePoint = iNode->getParticleIds().size();
    unsigned int vSize = iNode->getVelocity().size()/3;
    unsigned int pSize = iNode->getPosition().size()/3;
    // bail early if there's no particle data at this frame
    if (sizePoint == 0 || vSize == 0)
    {
        return status;
    }
    else if (sizePoint != vSize || vSize != pSize)
    {
        MString theError("Size incompatible: particle count ");
        theError += pSize;
        theError += ", velocity array size ";
        theError += vSize;
        theError += ", particle Id array size ";
        theError += sizePoint;
        printError(theError);
        return status;
    }

    // convert the data to Maya format
    MFnParticleSystem fnParticle;
    iObject = fnParticle.create(iParent, &status);
    fnParticle.setObject(iObject);
    fnParticle.setName(iNode->getName().c_str());

    MPointArray pArray;
    MVectorArray vArray;
    unsigned int vId = 0;
    const std::vector < float > & position = iNode->getPosition();
    const std::vector < float > & velocity = iNode->getVelocity();
    for (unsigned int pId = 0; pId < sizePoint; pId++)
    {
        pArray.append(position[vId], position[vId+1], position[vId+2]);
        if (vSize>0)
        {
            MVector velocity(velocity[vId], velocity[vId+1], velocity[vId+2]);
            vArray.append(velocity);
        }
        vId += 3;
    }

    addProperties(iFrame, *iNode, iObject, iSampledPropNameList);

    status = fnParticle.emit(pArray, vArray);
    status = fnParticle.saveInitialState();

    return status;
}

} // End namespace Alembic

