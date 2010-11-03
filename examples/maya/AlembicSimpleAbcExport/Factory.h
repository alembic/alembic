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

#ifndef _AlembicSimpleAbcExport_Factory_h_
#define _AlembicSimpleAbcExport_Factory_h_

#include "Foundation.h"
#include "Exportable.h"

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
// The factory class maps maya objects to their alembic counterparts,
// via the "exportable" class.
class Factory
{
public:
    struct Parameters
    {
        Parameters()
          : polysAsSubds( false ),
            deforming( false ),
            allUserAttributes( true ),
            allMayaAttributes( false ) {}
        
        bool polysAsSubds;
        bool deforming;
        bool allUserAttributes;
        bool allMayaAttributes;
    };
    
    Factory( const Parameters &iParams )
      : m_params( iParams ) {}

    void makeTree( Exportable &iExportableParent,
                   MDagPath &iDagPath,
                   int iRecursionLevel,
                   const Abc::TimeSamplingType &iTsmpType );

protected:
    void findAttributes( Exportable &iExportable,
                         const Abc::TimeSamplingType &iTsmpType );
    
    void findAttribute( Exportable &iExportable,
                        MObject &iNodeObj, 
                        MObject &iAttrObj,
                        MPlug &iPlug,
                        const std::string &iAttrName,
                        const Abc::TimeSamplingType &iTsmpType );
    
    static bool canExportAttribute( MPlug &iPlug, int iDepth = 0 );
    
    Parameters m_params;
    std::set<std::string> m_visitedPaths;
};

} // End namespace AlembicSimpleAbcExport

#endif
