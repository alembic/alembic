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

#include "Attribute.h"

namespace AlembicAbcExport {

//-*****************************************************************************
Attribute::Attribute( Exportable &parent,
                      MObject &nodeObj,
                      MObject &attrObj,
                      MPlug &attrPlug,
                      const std::string &attrName,
                      const Abc::TimeSamplingInfo &tinfo )
  : m_nodeHandle( nodeObj ),
    m_attrHandle( attrObj ),
    m_plug( attrPlug ),
    m_name( attrName ),
    m_animated( ( bool )tinfo )
{
    // Nothing
}

//-*****************************************************************************
template <>
void
SingularPODTypedAttribute<Abc::OCharProperty>::
writeSample( const Abc::Time &time )
{
    MStatus status;

    if ( !m_nodeHandle.isValid() ||
         !m_attrHandle.isValid() ||
         m_plug.isNull() )
    {
        MGlobal::displayError( "ERROR: Invalid node or attr handle in "
                               "SingularPODTypedAttribute<T>::writeSample()" );
        return;
    }

    // MObject nodeObj = m_nodeHandle.objectRef();
    // MObject attrObj = m_attrHandle.objectRef();
    // MPlug plug( nodeObj, attrObj, &status );
    // CHECK_MAYA_STATUS;
    
    // value_type value;
    char value;
    status = m_plug.getValue( value );
    CHECK_MAYA_STATUS;

    m_property.setAnim( time, ( Abc::int8_t )value );
}


}  //  End namespace AlembicAbcExport
