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

#ifndef _AlembicAbcExport_MeshUtil_h_
#define _AlembicAbcExport_MeshUtil_h_

#include "Foundation.h"

namespace AlembicAbcExport {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MESH REST SAMPLE
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
Abc::Box3d WriteMeshFirstSample( MDagPath &dagPath,
                                 MObject &node,
                                 
                                 Atg::OMeshTrait &abcMeshTrait,
                                 Atg::OSubdTrait &abcSubdTrait,
                                 Atg::ONormalsTrait &abcNormalsTrait,
                                 Atg::OUvsTrait &abcUvsTrait );

//-*****************************************************************************
inline
Abc::Box3d WritePolyMeshFirstSample( MDagPath &dagPath,
                                     MObject &node,
                                     
                                     Atg::OMeshTrait &abcMeshTrait,
                                     Atg::ONormalsTrait &abcNormalsTrait,
                                     Atg::OUvsTrait &abcUvsTrait )
{
    Atg::OSubdTrait dummy;
    Abc::Box3d re = WriteMeshRestSample( dagPath, node, abcMeshTrait,
                                         dummy, abcNormalsTrait, abcUvsTrait );
}

//-*****************************************************************************
inline
Abc::Box3d WriteSubdRestSample( MDagPath &dagPath,
                                MObject &node,

                                Atg::OMeshTrait &abcMeshTrait,
                                Atg::OSubdTrait &abcSubdTrait,
                                Atg::OUvsTrait &abcUvsTrait )
{
    Atg::ONormalsTrait dummy;
    return WriteMeshRestSample( dagPath, node, abcMeshTrait,
                                abcSubdTrait, dummy, abcUvsTrait );
}                               

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MESH ANIM SAMPLE
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
Abc::Box3d WriteMeshAnimSample( MDagPath &dagPath,
                                MObject &node,
                                
                                Atg::OMeshTrait &abcMeshTrait,
                                Atg::ONormalsTrait &abcNormalsTrait,

                                const Abc::Time &time );

//-*****************************************************************************
inline
Abc::Box3d WritePolyMeshAnimSample( MDagPath &dagPath,
                                    MObject &node,

                                    Atg::OMeshTrait &abcMeshTrait,
                                    Atg::ONormalsTrait &abcNormalsTrait,

                                    const Abc::Time &time )
{
    return WriteMeshAnimSample( dagPath, node, abcMeshTrait, abcNormalsTrait,
                                time );
}

//-*****************************************************************************
inline
Abc::Box3d WriteSubdAnimSample( MDagPath &dagPath,
                                MObject &node,
                                
                                Atg::OMeshTrait &abcMeshTrait,
                                
                                const Abc::Time &time )
{
    Atg::ONormalsTrait dummy;
    return WriteMeshAnimSample( dagPath, node, abcMeshTrait, dummy, time );
}

} // End namespace AlembicAbcExport

#endif
