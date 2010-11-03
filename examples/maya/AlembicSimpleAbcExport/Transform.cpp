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

#include "Transform.h"

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
Transform::Transform( Exportable &iExportableParent,
                      MDagPath &iDagPath,
                      MObject &iNode,
                      const std::string &iName,
                      const Abc::TimeSamplingType &iTsmpType )
  : Exportable( iDagPath, iNode, iName )
{
    m_simpleXform = Abc::OSimpleXform( iExportableParent.getAlembicObject(),
                                       iName,
                                       iTsmpType );
    m_alembicObject = m_simpleXform;
    m_alembicProperties = m_simpleXform.getSchema();

    // Make bounds with the same time sampling as the transform.
    // m_boundsProperty = Abc::OBox3dProperty( m_simpleXform,
    //                                        "bounds",
    //                                        iTsmpType );
}

//-*****************************************************************************
bool Transform::valid()
{
    return /*m_boundsProperty.valid() &&*/ m_simpleXform.valid()
        && Exportable::valid();
}

//-*****************************************************************************
Abc::Box3d Transform::writeSample( const Abc::OSampleSelector &iSS )
{
    // First, call base class sample write, which will return bounds
    // of any children.
    Abc::index_t sampleIndex = iSS.getIndex();
    Abc::Box3d bounds = Exportable::writeSample( iSS );

    // Make a transform
    MStatus status;
    MFnTransform xform( m_dagPath, &status );
    CHECK_MAYA_STATUS;
    
    // Get Maya Transformation Matrix
    MTransformationMatrix xformMat = xform.transformationMatrix( &status );
    CHECK_MAYA_STATUS;

    // Convert it to an Abc::M44d
    Abc::M44d matAbc;
    MMatrix mat = xformMat.asMatrix();
    for ( int row = 0; row < 4; ++row )
    {
        for ( int col = 0; col < 4; ++col )
        {
            matAbc[row][col] = mat[row][col];
        }
    }

    // Turn it into a SimpleTransformSample.
    Abc::SimpleXformSample xformSamp;
    xformSamp.setMatrix( matAbc );

    // Set the sample.
    m_simpleXform.getSchema().set( xformSamp, iSS );

    // Transform the bounds.
    if ( !bounds.isEmpty() )
    {
        bounds = Imath::transform( bounds, matAbc );
    }
    // m_boundsProperty.set( bounds, iSS );

    return bounds;
}

//-*****************************************************************************
void Transform::close()
{
    // m_boundsProperty.reset();
    m_simpleXform.reset();
    Exportable::close();
}

} // End namespace AlembicSimpleAbcExport
