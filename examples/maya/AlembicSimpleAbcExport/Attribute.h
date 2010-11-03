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

#ifndef _AlembicSimpleAbcExport_Attribute_h_
#define _AlembicSimpleAbcExport_Attribute_h_

#include "Foundation.h"
#include "Exportable.h"

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
class Attribute
{
public:
    Attribute( Exportable &iExportable,
               MObject &iNodeObj,
               MObject &iAttrObj,
               MPlug &iAttrPlug,
               const std::string &iAttrName,
               const Abc::TimeSamplingType &iTstType,
               bool iAnimated );

    virtual ~Attribute() {}

    const std::string &name() const { return m_name; }
    bool isAnimated() const { return m_animated; }

    virtual void writeSample( const Abc::OSampleSelector &iSS ) = 0;
    virtual bool valid() = 0;
    virtual void close() = 0;

protected:

    // These are the handles to the maya objects we care about.
    MObjectHandle m_nodeHandle;
    MObjectHandle m_attrHandle;
    MPlug m_plug;

    std::string m_name;

    bool m_animated;
};

//-*****************************************************************************
//-*****************************************************************************
// Typed implementation, for singular pod types values
//-*****************************************************************************
//-*****************************************************************************
template <class ABC_PROPERTY>
class SingularPODTypedAttribute : public Attribute
{
public:
    typedef typename ABC_PROPERTY::value_type value_type;
    typedef SingularPODTypedAttribute<ABC_PROPERTY> this_type;

    SingularPODTypedAttribute( Exportable &iExportable,
                               MObject &iNodeObj,
                               MObject &iAttrObj,
                               MPlug &iAttrPlug,
                               const std::string &iAttrName,
                               const Abc::TimeSamplingType &iTsmpType,
                               bool iAnimated );

    // This will assume that the maya scene has been set to the right scene
    // time.
    virtual void writeSample( const Abc::OSampleSelector &iSS );
    virtual bool valid();
    virtual void close();

protected:
    ABC_PROPERTY m_property;
};

//-*****************************************************************************
//-*****************************************************************************
// Typedefs
//-*****************************************************************************
//-*****************************************************************************

typedef SingularPODTypedAttribute<Abc::OUcharProperty> UcharAttribute;
typedef SingularPODTypedAttribute<Abc::OCharProperty> CharAttribute;

typedef SingularPODTypedAttribute<Abc::OUInt16Property> UshortAttribute;
typedef SingularPODTypedAttribute<Abc::OInt16Property> ShortAttribute;

typedef SingularPODTypedAttribute<Abc::OUInt32Property> UintAttribute;
typedef SingularPODTypedAttribute<Abc::OInt32Property> IntAttribute;

typedef SingularPODTypedAttribute<Abc::OUInt64Property> UlongAttribute;
typedef SingularPODTypedAttribute<Abc::OInt64Property> LongAttribute;

typedef SingularPODTypedAttribute<Abc::OFloatProperty> FloatAttribute;
typedef SingularPODTypedAttribute<Abc::ODoubleProperty> DoubleAttribute;

//-*****************************************************************************
//-*****************************************************************************
// TEMPLATE IMPLEMENTATION
//-*****************************************************************************
//-*****************************************************************************
template <class ABC_PROPERTY>
SingularPODTypedAttribute<ABC_PROPERTY>::SingularPODTypedAttribute
(
    Exportable &iParent,
    MObject &iNodeObj,
    MObject &iAttrObj,
    MPlug &iAttrPlug,
    const std::string &iName,
    const Abc::TimeSamplingType &iTsmpType,
    bool iAnimated )
  : Attribute( iParent, iNodeObj, iAttrObj, iAttrPlug, iName, iTsmpType,
               iAnimated ),
    m_property( iParent.getAlembicProperties(),
                iName, iTsmpType )
{
    // Nothing
}

//-*****************************************************************************
template <class ABC_PROPERTY>
void
SingularPODTypedAttribute<ABC_PROPERTY>::writeSample
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

    value_type value;
    status = m_plug.getValue( value );
    CHECK_MAYA_STATUS;

    if ( iSS.getIndex() == 0 || m_animated )
    {
        m_property.set( value, iSS );
    }
}

//-*****************************************************************************
template <>
void
SingularPODTypedAttribute<Abc::OCharProperty>::
writeSample( const Abc::OSampleSelector &iSS );

//-*****************************************************************************
template <class ABC_PROPERTY>
bool SingularPODTypedAttribute<ABC_PROPERTY>::valid()
{
    return m_property.valid();
}

//-*****************************************************************************
template <class ABC_PROPERTY>
void SingularPODTypedAttribute<ABC_PROPERTY>::close()
{
    // std::cout << "\n\nTrying to close attribute: " << m_name << std::endl;
    m_property.reset();
    // std::cout << "\t******Closed attribute: " << m_name << std::endl;
}

} // End namespace AlembicSimpleAbcExport

#endif
