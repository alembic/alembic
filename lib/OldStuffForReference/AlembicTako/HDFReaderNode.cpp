//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#include <AlembicTako/HDFReaderNode.h>

namespace Abc = AlembicAsset;

namespace AlembicTako {

//-*****************************************************************************
void HDFReaderNode::init( const Abc::IObject &obj )
{
    m_alembic = obj;

    // Get properties and make ioSamples.
    m_ioSamples.insert( FLT_MAX );
    size_t nProps = m_alembic.numProperties();
    for ( size_t p = 0; p < nProps; ++p )
    {
        Abc::PropertyInfo pinfo = m_alembic.propertyInfo( p );
        PropertyPtr ptr;
        if ( pinfo->ptype == Abc::kSingularProperty )
        {
            ptr.reset(
                new Abc::ISingularProperty( m_alembic,
                                            pinfo ) );
        }
        else
        {
            ptr.reset(
                new Abc::IMultiProperty( m_alembic,
                                         pinfo ) );
        }

        if ( ptr && ptr->valid() )
        {
            m_properties.push_back( ptr );

            if ( ptr->isAnimated() )
            {
                const Abc::ITimeSampling *tsmp = ptr->timeSampling();
                if ( tsmp )
                {
                    size_t numSamples = tsmp->numSamples();
                    for ( size_t samp = 0;
                          samp < numSamples; ++samp )
                    {
                        Abc::seconds_t secs =
                            tsmp->sampleSeconds( samp );
                        m_ioSamples.insert( ( float )secs );
                    }
                }
            }
        }
    }
}


} // End namespace AlembicTako
