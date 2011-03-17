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

#ifndef _Alembic_AbcGeom_SimpleXformSample_h_
#define _Alembic_AbcGeom_SimpleXformSample_h_

#include <Alembic/AbcGeom/Foundation.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
class SimpleXformSample
{
protected:
    void resetSHRT( const Abc::M44d &iMat )
    {
        Imath::Eulerd euler;

        Imath::extractSHRT( iMat,
                            m_scale,
                            m_shear,
                            euler,
                            m_translation,
                            false );

        m_xyzRotation = euler.toXYZVector();
    }

public:
    SimpleXformSample() { makeIdentity(); }
    SimpleXformSample( const Abc::M44d &iMat ) { resetSHRT( iMat ); }

    const Abc::V3d &getScale() const { return m_scale; }
    void setScale( const Abc::V3d &iScale ) { m_scale = iScale; }

    const Abc::V3d &getShear() const { return m_shear; }
    void setShear( const Abc::V3d &iShear ) { m_shear = iShear; }

    const Abc::V3d &getXYZRotation() const { return m_xyzRotation; }
    void setXYZRotation( const Abc::V3d &iRot ) { m_xyzRotation = iRot; }

    const Abc::V3d &getTranslation() const { return m_translation; }
    void setTranslation( const Abc::V3d &iTrn ) { m_translation = iTrn; }

    const Abc::Box3d &getChildBounds() const { return m_childBounds; }
    void setChildBounds( const Abc::Box3d &iBnds )
    { m_childBounds = iBnds; }

    Abc::M44d getMatrix() const
    {
        Imath::Eulerd euler;
        euler.setXYZVector( m_xyzRotation );

        return Abc::M44d().setScale( m_scale ) *
            Abc::M44d().setShear( m_shear ) *
            euler.toMatrix44() *
            Abc::M44d().setTranslation( m_translation );
    }

    void setMatrix( const Abc::M44d &iMat )
    {
        resetSHRT( iMat );
    }

    void makeIdentity()
    {
        m_scale = Abc::V3d( 1.0 );
        m_shear = Abc::V3d( 0.0 );
        m_xyzRotation = Abc::V3d( 0.0 );
        m_translation = Abc::V3d( 0.0 );
    }

    void reset()
    {
        this->makeIdentity();
        m_childBounds.makeEmpty();
    }

protected:
    Abc::V3d m_scale;
    Abc::V3d m_shear;
    Abc::V3d m_xyzRotation;
    Abc::V3d m_translation;
    Abc::Box3d m_childBounds;
};

//-*****************************************************************************
inline std::ostream& operator<<( std::ostream &ostr,
                                 const SimpleXformSample &iSmp )
{
    ostr << "\tscale = " << iSmp.getScale() << std::endl
         << "\tshear = " << iSmp.getShear() << std::endl
         << "\txyz rotation = " << iSmp.getXYZRotation() << std::endl
         << "\ttranslation = " << iSmp.getTranslation() << std::endl;
    return ostr;
}

} // End namespace AbcGeom
} // End namespace Alembic

#endif
