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

#ifndef _Alembic_AbcGeom_CameraSample_h_
#define _Alembic_AbcGeom_CameraSample_h_

#include <Alembic/AbcGeom/Foundation.h>

namespace Alembic {
namespace AbcGeom {

//-*****************************************************************************
class CameraSample
{
public:

    //! Creates a default sample with a bunch of defaults set
    CameraSample() { reset() };

    //! Create a default sample and set the defaults so that they
    //! calculate the provided screen window.
    CameraSample( double iTop, double iBottom, double iLeft, double iRight );

    void getScreenWindow( double & iTop, double & iBottom, double & iLeft,
        double & iRight );

    double getFocalLength() { return m_focalLength; }
    void  setFocalLength( double iVal )
    { m_focalLength = iVal; }

    double getHorizontalAperture() { return m_horizontalAperture; }
    void setHorizontalAperture( double iVal )
    { m_horizontalAperture = iVal; }

    double getHorizontalFilmOffset() { return m_horizontalFilmOffset; }
    void setHorizontalFilmOffset( double iVal )
    { m_horizontalFilmOffset = iVal; }

    double getVerticalAperture() { return m_verticalAperture; }
    void setVerticalAperture( double iVal )
    { m_verticalAperture = iVal; }

    double getVerticalFilmOffset() { return m_verticalFilmOffset; }
    void setVerticalFilmOffset( double iVal )
    { m_verticalFilmOffset = iVal; }

    double getLensSqueezeRatio() { return m_lensSqueezeRatio; }
    void setLensSqueezeRatio( double iVal )
    { m_lensSqueezeRatio = iVal; }

    double getOverScanLeft() { return m_overscanLeft; }
    void setOverScanLeft( double iVal )
    { m_overscanLeft = iVal; }

    double getOverScanRight() { return m_overscanRight; }
    void setOverScanRight( double iVal )
    { m_overscanRight = iVal; }

    double getOverScanTop() { return m_overscanTop; }
    void setOverScanTop( double iVal )
    { m_overscanTop = iVal; }

    double getOverScanBottom() { return m_overscanBottom; }
    void setOverScanBottom( double iVal )
    { m_overscanBottom = iVal; }

    double getFStop() { return m_fStop; }
    void setFStop( double iVal )
    { m_fStop = iVal; }

    double getFocusDistance() { return m_focusDistance; }
    void setFocusDistance( double iVal )
    { m_focusDistance = iVal; }

    double getShutterOpen() { return m_shutterOpen; }
    void setShutterOpen( double iVal )
    { m_shutterOpen = iVal; }

    double getShutterClose() { return m_shutterClose; }
    void setShutterClose( double iVal )
    { m_shutterClose = iVal; }

    double getNearClippingPlane() { return m_nearClippingPlane; }
    void setNearClippingPlane( double iVal )
    { m_nearClippingPlane = iVal; }

    double getFarClippingPlane() { return m_farClippingPlane; }
    void setFarClippingPlane( double iVal )
    { m_farClippingPlane = iVal; }

    Abc::Box3d getChildBounds() { return m_childBounds; }
    void setChildBounds( const Abc::Box3d & iBounds )
    { m_childBounds = iBounds; }

    void reset()
    {
        // in mm
        m_focalLength = 35.0;

        // in cm
        m_horizontalAperture = 3.6;
        m_horizontalFilmOffset = 0.0;
        m_verticleAperture = 2.4;
        m_verticalFilmOffset = 0.0;

        // width/height lens aspect ratio
        m_lensSqueezeRatio = 1.0;

        m_overscanLeft = 1.0;
        m_overscanRight = 1.0;
        m_overscanTop = 1.0;
        m_overscanBottom = 1.0;

        // optical property of the lens, focal length divided by 
        // "effective" lens diameter
        m_fStop = 5.6;

        m_focusDistance = 5.0;
        m_shutterOpen = 0.0;
        m_shutterClose = 1.0;

        m_nearClippingPlane = 0.1;
        m_farClippingPlane = 100000.0;

        m_childBounds.makeEmpty();
    }

protected:
    double m_focalLength;
    double m_horizontalAperture;
    double m_horizontalFilmOffset;
    double m_verticleAperture;
    double m_verticalFilmOffset;
    double m_lensSqueezeRatio;

    double m_overscanLeft;
    double m_overscanRight;
    double m_overscanTop;
    double m_overscanBottom;

    double m_fStop;
    double m_focusDistance;
    double m_shutterOpen;
    double m_shutterClose;

    double m_nearClippingPlane;
    double m_farClippingPlane;

    Abc::Box3d m_childBounds;
};

} // End namespace AbcGeom
} // End namespace Alembic

#endif
