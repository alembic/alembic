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

#include "Subd.h"
#include "MeshUtil.h"

namespace AlembicAbcExport {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// HELPER STUFF
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
bool SubdInterpBoundary( MDagPath &dp )
{
    MFnDependencyNode dn( dp.node() );

    MPlug attrPlug = dn.findPlug( ABC_MAYA_INTERP_BOUNDARY_ATTRIBUTE );
    if ( attrPlug.isNull() )
    {
        return true; // (default is to interpolate boundaries)
    }

    bool attrValue;
    attrPlug.getValue( attrValue );

    return attrValue;
}

//-*****************************************************************************
bool IsSubd( MDagPath &dp )
{
    MFnDependencyNode dn( dp.node() );

    MPlug attrPlug = dn.findPlug( ABC_MAYA_SUBD_ATTRIBUTE );
    if ( attrPlug.isNull() )
    {
        return false; // (default is to regular polys)
    }

    bool attrValue;
    attrPlug.getValue( attrValue );

    return attrValue;
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// SUBD
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************


//-*****************************************************************************
Subd::Subd( Exportable &parent,
            MDagPath &dpath,
            MObject &nde,
            const std::string &nme,
            const Abc::TimeSamplingInfo &tinfo,
            bool deforming )
  : BaseSimpleNull<Atg::OSimpleSubd>( parent, dpath, nde, nme, tinfo ),
    m_deforming( deforming && ( bool )tinfo )
{
    if ( m_alembicObject )
    {
        MFnMesh mesh( nde );
        size_t numUvs = mesh.numUVs();

        m_xformTrait = m_alembicObject->xform();
        m_meshTrait = m_alembicObject->mesh();
        m_subdTrait = m_alembicObject->subd();

        if ( numUvs > 0 )
        {
            m_alembicObject->uvs().addOptional();
            m_uvsTrait = m_alembicObject->uvs();
        }
    }

    // For now just assume that deformation is evarwhar
    if ( m_deforming )
    {
        // Only vertices animating.
        m_meshTrait.positions().makeAnimated( tinfo );
    }

    // Rest bounds.
    m_restBounds.makeEmpty();
}

//-*****************************************************************************
Abc::Box3d Subd::internalWriteSample( const Abc::Time &sampTime,
                                      const Abc::Box3d &childBounds )
{
    Abc::Box3d allBounds;
    if ( sampTime.isRestTime() )
    {
        Abc::M44d xform;
        xform.makeIdentity();
        m_xformTrait.set( xform );
        allBounds = WriteSubdRestSample( m_dagPath, m_node,
                                         m_meshTrait, m_subdTrait,
                                         m_uvsTrait );
        m_restBounds = allBounds;
    }
    else if ( m_deforming )
    {
        allBounds = WriteSubdAnimSample( m_dagPath, m_node,
                                         m_meshTrait,
                                         sampTime );
    }
    else
    {
        allBounds = m_restBounds;
    }

    allBounds.extendBy( childBounds );

    return BaseSimpleNull<Atg::OSimpleSubd>::
        internalWriteSample( sampTime,
                             allBounds );
}

} // End namespace AlembicAbcExport

