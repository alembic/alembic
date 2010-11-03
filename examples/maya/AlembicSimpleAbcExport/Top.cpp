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

#include "Top.h"

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
Top::Top( const std::string &iFileName,
          const Abc::TimeSamplingType &iTsmpType,
          Abc::chrono_t iFramesPerSecond )
  : Exportable( iFileName ),
    m_minTime( 0.0 ),
    m_maxTime( 0.0 )
{
    m_archive = Abc::OArchive( Alembic::AbcCoreHDF5::WriteArchive(),
                               iFileName, Abc::ErrorHandler::kThrowPolicy );
    m_alembicObject = m_archive.getTop();
    ABCA_ASSERT( m_alembicObject.valid(),
                 "Invalid top object in Alembic archive: " << iFileName );

    // Add the bounds property.
    m_alembicProperties = m_alembicObject.getProperties();
    m_boundsProperty = Abc::OBox3dProperty( m_alembicProperties, "bounds",
                                            iTsmpType );

    // Immediately write frames per second.
    Abc::ODoubleProperty( m_alembicProperties, "framesPerSecond" ).set(
        ( double )iFramesPerSecond );
}

//-*****************************************************************************
bool Top::valid()
{
    return m_archive.valid() && m_boundsProperty.valid() &&
        Exportable::valid();
}

//-*****************************************************************************
Abc::Box3d Top::writeSample( const Abc::OSampleSelector &iSS )
{
    Abc::chrono_t time = iSS.getTime();
    if ( iSS.getIndex() == 0 )
    {
        m_minTime = time;
        m_maxTime = time;
    }
    else
    {
        m_minTime = std::min( m_minTime, time );
        m_maxTime = std::max( m_maxTime, time );
    }

    Abc::Box3d bounds = Exportable::writeSample( iSS );
    m_boundsProperty.set( bounds, iSS );

    m_totalBounds.extendBy( bounds );

    return bounds;
}

//-*****************************************************************************
void Top::close()
{
    // Export.
    std::string fileName = this->getName();

    Abc::ODoubleProperty( m_alembicProperties, "minSampleTime" )
        .set( m_minTime );
    Abc::ODoubleProperty( m_alembicProperties, "maxSampleTime" )
        .set( m_maxTime );
    Abc::OBox3dProperty( m_alembicProperties, "totalBounds" )
        .set( m_totalBounds );

    m_boundsProperty.reset();
    Exportable::close();

    m_archive.reset();

    std::cout << "Closed archive: " << fileName << std::endl;
}

} // End namespace AlembicSimpleAbcExport
