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

#ifndef _AlembicSimpleAbcExport_Exportable_h_
#define _AlembicSimpleAbcExport_Exportable_h_

#include "Foundation.h"

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
// Handy forwards.
class Exportable;
typedef boost::shared_ptr<Exportable> ExportablePtr;
typedef std::vector<ExportablePtr> Exportables;

class Attribute;
typedef boost::shared_ptr<Attribute> AttributePtr;
typedef std::vector<AttributePtr> Attributes;

//-*****************************************************************************
class Exportable
{
public:
    Exportable( MDagPath &dpath,
                MObject &nde,
                const std::string &nme )
      : m_dagPath( dpath ),
        m_node( nde ),
        m_name( nme ) {}

    Exportable( const std::string &nme )
      : m_dagPath(),
        m_node(),
        m_name( nme ) {}
    
    virtual ~Exportable() {}

    // Extract basic info
    MDagPath &getDagPath() { return m_dagPath; }
    const MDagPath &getDagPath() const { return m_dagPath; }

    MObject &getNode() { return m_node; }
    const MObject &getNode() const { return m_node; }

    const std::string &getName() const { return m_name; }

    Abc::OObject &getAlembicObject() { return m_alembicObject; }
    const Abc::OObject &getAlembicObject() const { return m_alembicObject; }

    Abc::OCompoundProperty &getAlembicProperties()
    { return m_alembicProperties; }
    const Abc::OCompoundProperty &getAlembicProperties() const
    { return m_alembicProperties; }

    // Return whether it is valid.
    virtual bool valid();
    
    // Return whether or not this kind of node can export
    // a named attribute.
    virtual bool canExportAttributeNamed( const std::string &attrName );

    // ADD ATTRIBUTES
    void addAttribute( AttributePtr attr )
    {
        m_attributes.push_back( attr );
    }

    // ADD CHILDREN
    void addChild( ExportablePtr chld )
    {
        m_children.push_back( chld );
    }

    // Write a sample. Returns the bounding box of the object at
    // this sample time.
    virtual Abc::Box3d writeSample( const Abc::OSampleSelector &iSS );

    // Close.
    virtual void close();
    
protected:
    
    MDagPath m_dagPath;
    MObject m_node;
    std::string m_name;

    // The Abc::OObject that this exportable is exported into.
    Abc::OObject m_alembicObject;

    // The Abc::OCompoundProperties at the top of the object.
    // This is the schema, in the case of typed things.
    Abc::OCompoundProperty m_alembicProperties;

    // My attributes.
    Attributes m_attributes;

    // My children.
    Exportables m_children;
};

} // End namespace AlembicSimpleAbcExport

#endif
