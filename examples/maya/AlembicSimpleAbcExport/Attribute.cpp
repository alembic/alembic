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

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
Attribute::Attribute( Exportable &iParent,
                      MObject &iNodeObj,
                      MObject &iAttrObj,
                      MPlug &iAttrPlug,
                      const std::string &iAttrName,
                      const Abc::TimeSamplingType &iTsmpType,
                      bool iAnimated )
  : m_nodeHandle( iNodeObj )
  , m_attrHandle( iAttrObj )
  , m_plug( iAttrPlug )
  , m_name( iAttrName )
  , m_animated( iAnimated )
{
    // Nothing
}

//-*****************************************************************************
template <>
void
SingularPODTypedAttribute<Abc::OCharProperty>::
writeSample( const Abc::OSampleSelector &iSS )
{
    MStatus status;

    if ( !this->m_nodeHandle.isValid() ||
        !this->m_attrHandle.isValid() ||
        this->m_plug.isNull() )
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
    status = this->m_plug.getValue( value );
    CHECK_MAYA_STATUS;

    m_property.set( ( Abc::int8_t )value, iSS );
}

//-*****************************************************************************
template <>
void
SingularPODTypedAttribute<Abc::OInt32Property>::writeSample
( const Abc::OSampleSelector &iSS )
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
    int value;
    status = m_plug.getValue( value );
    CHECK_MAYA_STATUS;

    if ( iSS.getIndex() == 0 || m_animated )
    {
        m_property.set( value, iSS );
    }
}

}  //  End namespace AlembicSimpleAbcExport
