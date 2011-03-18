//-*****************************************************************************
//
// Copyright (c) 2009-2011,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#ifndef _Alembic_AbcGeom_XformSample_h_
#define _Alembic_AbcGeom_XformSample_h_

#include <Alembic/AbcGeom/Foundation.h>

namespace Alembic {
namespace AbcGeom {

// forwards
class XformOp;
class boost::uuids::uuid;

//-*****************************************************************************
class XformSample
{
public:
    XformSample();

    // odd translate or scale op
    void addOp( const XformOp iOp, const Abc::V3d &iVal );

    // add rotate op
    void addOp( const XformOp iOp, const Abc::V3d &iAxis,
                const double iAngle );

    // add matrix op
    void addOp( const XformOp iOp, const Abc::M44d &iMatrix );

    const XformOp &getOp( std::size_t iIndex ) const;

    const std::vector<Alembic::Util::uint8_t> &getOpsArray() const;

    std::size_t getNumOps() const;
    std::size_t getNumOpChannels() const;

    void setIsToWorld( bool iIsToWorld );
    bool getIsToWorld() const;

    void setChildBounds( const Abc::Box3d &iBnds );
    const Abc::Box3d &getChildBounds() const;

    // non-op-based methods; the getters will compute their return values
    // from the ops under the hood.
    void setTranslation( const Abc::V3d &iTrans );
    const Abc::V3d &getTranslation() const;

    void setRotation( const Abc::V3d &iAxis, const double iAngle );
    const Abc::V3d &getAxis() const;
    const double getAngle() const;

    void setScale( const Abc::V3d &iScale );
    const Abc::V3d &getScale() const;

    void setMatrix( const Abc::M44d iMatrix );
    const Abc::M44d &getMatrix() const;

private:
    friend class OXform;
    friend class IXform;
    void setHasBeenRead( bool iHasBeenRead );
    const boost::uuids::uuid &getID() const;

private:
    // 0 is unset; 1 is set via addOp; 2 is set via non-op-based methods
    int m_setWithOpStack;
    boost::uuids::uuid m_id;

    // This will be populated by the addOp() methods or setFoo() methods
    // in the case of the sample being used to write data, and by the
    // IXform in the case of the sample being used to read data.
    std::vector<Alembic::Util::uint8_t> m_opsArray;

    bool m_isToWorld;
    Abc::Box3d m_childBounds;

    // This starts out false, but is set to true by the OXform and controls
    // whether or not addOp() changes the topology of the Sample, in the form
    // of the layout of the m_opsArray.
    bool m_hasBeenRead;

}


} // End namespace AbcGeom
} // End namespace Alembic

#endif
