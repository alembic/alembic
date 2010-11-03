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

#include "Collider.h"

namespace AlembicAbcExport {

//-*****************************************************************************
Collider::ShapeType GetColliderShapeType( MDagPath &dp )
{
    MFnDependencyNode dn( dp.node() );

    MPlug attrPlug = dn.findPlug( "ALEMBIC_FLUID_SIM_DEMO_TYPE" );
    if ( attrPlug.isNull() )
    {
        return Collider::kUnknown;
    }

    MString attrValue;
    attrPlug.getValue( attrValue );

    std::string PRIM_TYPE = attrValue.asChar();
    if ( PRIM_TYPE == "CollisionPlane" )
    {
        return Collider::kPlane;
    }
    else if ( PRIM_TYPE == "SphereCollision" )
    {
        return Collider::kSphere;
    }
    else if ( PRIM_TYPE == "CubeCollision" )
    {
        return Collider::kCube;
    }
    else if ( PRIM_TYPE == "CylinderCollision" )
    {
        return Collider::kCylinder;
    }
    else if ( PRIM_TYPE == "ConeCollision" )
    {
        return Collider::kCone;
    }
    else
    {
        return Collider::kUnknown;
    }
}

//-*****************************************************************************
Abc::Box3d WriteColliderRestSample( MDagPath &dagPath,
                                    MObject &node,
                                    
                                    Abc::OCharProperty &abcShapeProp,
                                    Collider::ShapeType shapeType )
{
    std::cout << "About to write Collider Rest Sample" << std::endl;

    //-*************************************************************************
    // UNIFORM STUFF (Just Subd Stuff for now)
    //-*************************************************************************
    if ( abcShapeProp )
    {
        abcShapeProp.set( ( char )shapeType );
    }
    
    // All done.
    return Abc::Box3d( Abc::V3d( -1, -1, -1 ),
                       Abc::V3d( 1, 1, 1 ) );
}


//-*****************************************************************************
Collider::Collider( Exportable &parent,
                    MDagPath &dpath,
                    MObject &nde,
                    const std::string &nme,
                    const Abc::TimeSamplingType &iTst,
                    ShapeType iShape )
  : BaseSimpleNull<Atg::OSimpleNull>( parent, dpath, nde, nme, iTst ),
    m_shapeType( iShape )
{
    if ( m_alembicObject )
    {
        m_shapeTypeProp = Abc::OCharProperty( m_alembicObject,
                                              "ALEMBIC_FLUID_SIM_DEMO_TYPE" );
    }
}

//-*****************************************************************************
Abc::Box3d Collider::internalWriteSample( const Abc::Box3d &childBounds,
                                          Abc::index_t iSampleIndex,
                                          Abc::chrono_t iSampleTime )
{
    if ( iSampleIndex == 0 )
    {
        m_restBounds = WriteColliderRestSample( m_dagPath, m_node,
                                                m_shapeTypeProp,
                                                m_shapeType );
    }

    Abc::Box3d allBounds = m_restBounds;

    allBounds.extendBy( childBounds );
    
    return BaseSimpleNull<Abc::OObject>::
        internalWriteSample( allBounds, iSampleIndex, iSampleTime );
}

} // End namespace AlembicAbcExport
