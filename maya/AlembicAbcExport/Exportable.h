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

#ifndef _AlembicAbcExport_Exportable_h_
#define _AlembicAbcExport_Exportable_h_

#include "Foundation.h"

namespace AlembicAbcExport {

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

    MDagPath &dagPath() { return m_dagPath; }
    const MDagPath &dagPath() const { return m_dagPath; }

    MObject &node() { return m_node; }
    const MObject &node() const { return m_node; }

    const std::string &name() const { return m_name; }


private:
    friend class Attribute;
    virtual const Abc::OObject &alembicObject() = 0;

public:
    // Return an alembic parent object, for children
    // to be born from.
    virtual const Abc::OParentObject &alembicParentObject()
    {
        return alembicObject();
    }

    // Return whether it is valid.
    virtual bool valid() const = 0;

    // Return whether or not this kind of node can export
    // a named attribute.
    virtual bool canExportAttributeNamed( const std::string &attrName );
    
    void addAttribute( AttributePtr attr )
    {
        m_attributes.push_back( attr );
    }

    void addChild( ExportablePtr chld )
    {
        m_children.push_back( chld );
    }

    // Write a sample. Returns the bounding box of the object at
    // this sample time.
    Abc::Box3d writeSample( const Abc::Time &sampTime );

    // Close.
    void close();

    // Just close self, worry not about attrs or children.
    virtual void internalClose() = 0;

protected:
    MDagPath m_dagPath;
    MObject m_node;
    std::string m_name;

    //-*************************************************************************
    // OVERRIDE ME!
    // Return the bounds of yourself (after any transformations have been
    // applied), given the bounds of your children in their local spaces.
    //-*************************************************************************
    virtual Abc::Box3d
    internalWriteSample( const Abc::Time &sampTime,
                         const Abc::Box3d &childrenBounds ) = 0;

    // My attributes.
    Attributes m_attributes;

    // My children.
    Exportables m_children;
};

} // End namespace AlembicAbcExport

#endif
