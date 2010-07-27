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

#include "BaseTakoDrw.h"

namespace SimpleAbcViewer {

//-*****************************************************************************
BaseTakoDrw::BaseTakoDrw( HDFReaderNodePtr iNodePtr )
  : m_baseNodePtr( iNodePtr ),
    m_constant( false )
{
    if ( !m_baseNodePtr )
    {
        ABC_THROW( "Invalid base node ptr" );
    }

    m_baseNodePtr->getFrames( m_sampleFrames );

    if ( m_sampleFrames.count( FLT_MAX ) > 0 ||
         m_sampleFrames.empty() )
    {
        m_constant = true;
        m_minFrame = FLT_MAX;
        m_maxFrame = FLT_MAX;
    }
    else
    {
        m_constant = false;
        m_minFrame = (*(m_sampleFrames.begin()));
        m_maxFrame = (*(m_sampleFrames.rbegin()));
    }

#if 0
    HDFReaderNode::PropNameInfoMap propNamesAndInfo;
    m_baseNodePtr->getPropertyNamesAndInfo( propNamesAndInfo );

    std::cout << "\nPROPERTIES OF OBJECT: " << m_baseNodePtr->getName()
              << std::endl;
    for ( HDFReaderNode::PropNameInfoMap::const_iterator iter =
              propNamesAndInfo.begin();
          iter != propNamesAndInfo.end(); ++iter )
    {
        std::cout << "\t" << (*iter).first << std::endl;
    }
#endif
}

} // End namespace SimpleAbcViewer
