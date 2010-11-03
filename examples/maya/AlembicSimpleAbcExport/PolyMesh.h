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

#ifndef _AlembicSimpleAbcExport_PolyMesh_h_
#define _AlembicSimpleAbcExport_PolyMesh_h_

#include "Foundation.h"
#include "Exportable.h"

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
class PolyMesh : public Exportable
{
public:
    PolyMesh( Exportable &iExportableParent,
              MDagPath &iDagPath,
              MObject &iNode,
              const std::string &iName,
              const Abc::TimeSamplingType &iTsmpType,
              bool iDeforming );

    virtual bool valid();

    virtual Abc::Box3d writeSample( const Abc::OSampleSelector &iSS );

    virtual void close();

protected:
    // The poly mesh.
    Abc::OPolyMesh m_polyMesh;

    // The bounds property.
    Abc::OBox3dProperty m_boundsProperty;

    // Others.
    Abc::ON3fArrayProperty m_normals;
    Abc::OV2fArrayProperty m_sts;

    // Deforming
    bool m_deforming;

    // The bounds of the first sample, in case we're not deforming.
    Abc::Box3d m_firstSampleSelfBounds;
};

} // End namespace AlembicSimpleAbcExport

#endif
