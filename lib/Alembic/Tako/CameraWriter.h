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

#ifndef _Alembic_Tako_CameraWriter_h_
#define _Alembic_Tako_CameraWriter_h_

#include <Alembic/Tako/Foundation.h>
#include <Alembic/Tako/HDFWriterNode.h>
#include <Alembic/Tako/CameraDef.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {

class TransformWriter;
class CameraWriterPrivate;

/**
\brief CameraWriter is used to write camera data to the HDF file.
*/
class CameraWriter : public HDFWriterNode
{
    public:
        /**
        \brief Constructor requires a node name and a parent node.
        \param iName The name of the node to write.
        \param iParent The parent transform node.
        */
        CameraWriter(const char * iName, TransformWriter & iParent);

        /**
        \brief Destructor
        */
        ~CameraWriter();

        /**
        \brief Writes the camera data for a specified frame.
        \param iFrame The frame at which to write the camera data.
        \param iCam The camera data to write out.
        */
        void write(float iFrame, const CameraData & iCam) const;

    private:
        // disallow copying
        CameraWriter(const CameraWriter &);
        void operator=(const CameraWriter &);

        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}

using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_CameraWriter_h_
