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

#ifndef _AlembicAbcExport_Subd_h_
#define _AlembicAbcExport_Subd_h_

#include "Foundation.h"
#include "SimpleNull.h"

namespace AlembicAbcExport {

//-*****************************************************************************
#define ABC_MAYA_SUBD_ATTRIBUTE "alembicSubd"
#define ABC_MAYA_INTERP_BOUNDARY_ATTRIBUTE "alembicSubdInterpBoundary"

//-*****************************************************************************
bool SubdInterpBoundary( MDagPath &dp );

//-*****************************************************************************
bool IsSubd( MDagPath &dp );

//-*****************************************************************************
class Subd : public BaseSimpleNull<Atg::OSimpleSubd>
{
public:
    typedef Subd this_type;
    
    Subd( Exportable &parent,
          MDagPath &dagPath,
          MObject &nde,
          const std::string &nme,
          const Abc::TimeSamplingInfo &tinfo,
          bool deforming );

protected:
    virtual Abc::Box3d internalWriteSample( const Abc::Time &sampTime,
                                            const Abc::Box3d &childBounds );

    Atg::OXformLocalTrait m_xformTrait;
    Atg::OMeshTrait m_meshTrait;
    Atg::OSubdTrait m_subdTrait;
    Atg::OUvsTrait m_uvsTrait;

    // Deforming
    bool m_deforming;

    // Rest bounds
    Abc::Box3d m_restBounds;
};

} // End namespace AlembicAbcExport

#endif
