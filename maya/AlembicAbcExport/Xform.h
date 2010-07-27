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

#ifndef _AlembicAbcExport_Xform_h_
#define _AlembicAbcExport_Xform_h_

#include "Foundation.h"
#include "SimpleNull.h"

namespace AlembicAbcExport {

//-*****************************************************************************
// An Xform is an object that has a transform attribute, as well as possibly
// children.
template <class ABC_OBJECT>
class BaseXform : public BaseSimpleNull<ABC_OBJECT>
{
public:
    typedef BaseXform<ABC_OBJECT> this_type;
    
    BaseXform( Exportable &parent,
               MDagPath &dpath,
               MObject &nde,
               const std::string &nme,
               const Abc::TimeSamplingInfo &tinfo );

protected:
    virtual Abc::Box3d internalWriteSample( const Abc::Time &sampTime,
                                            const Abc::Box3d &childBounds );

    Atg::OXformLocalTrait m_xformTrait;
};

//-*****************************************************************************
typedef BaseXform<Atg::OSimpleXform> Xform;

//-*****************************************************************************
//-*****************************************************************************
template <class ABC_OBJECT>
inline BaseXform<ABC_OBJECT>::BaseXform
(
    Exportable &parent,
    MDagPath &dpath,
    MObject &nde,
    const std::string &nme,
    const Abc::TimeSamplingInfo &tinfo
)
  : BaseSimpleNull<ABC_OBJECT>( parent, dpath, nde, nme, tinfo )
{
    if ( this->m_alembicObject )
    {
        m_xformTrait = this->m_alembicObject->xform();
    }

    // We'll just crudely assume the transform is animated for everything.
    // This could be improved.
    if ( tinfo )
    {
        m_xformTrait.makeAnimated( tinfo );
    }
}

//-*****************************************************************************
template <class ABC_OBJECT>
Abc::Box3d BaseXform<ABC_OBJECT>::internalWriteSample
(
    const Abc::Time &sampTime,
    const Abc::Box3d &childBounds
)
{
    Abc::M44d matAbc;
    matAbc.makeIdentity();
    if ( this->m_node.hasFn( MFn::kTransform ) )
    {
        MStatus status;
        MFnTransform xformObject( this->m_node, &status );
        CHECK_MAYA_STATUS;
        
        MTransformationMatrix xformMat =
            xformObject.transformationMatrix( &status );
        CHECK_MAYA_STATUS;

        MMatrix mat = xformMat.asMatrix();
        for ( int row = 0; row < 4; ++row )
        {
            for ( int col = 0; col < 4; ++col )
            {
                matAbc[row][col] = mat[row][col];
            }
        }
    }

    // Set the matrix AND the bounds, since we didn't inherit
    // the SimpleNull method.
    if ( sampTime.isRestTime() )
    {
        // std::cout << "Writing BaseXform rest pose" << std::endl;
        m_xformTrait.set( matAbc );
    }
    else
    {
        if ( m_xformTrait.isAnimated() )
        {
            // std::cout << "Writing BaseXform anim pose at time: " << sampTime
            //           << std::endl;
            m_xformTrait.setAnim( sampTime, matAbc );
        }
    }

    // Call inherited version, with bounds transformed.
    return BaseSimpleNull<ABC_OBJECT>::
        internalWriteSample( sampTime,
                             Imath::transform( childBounds,
                                               matAbc ) );
}

} // End namespace AlembicAbcExport

#endif

