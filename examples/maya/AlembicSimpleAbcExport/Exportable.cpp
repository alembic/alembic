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

#include "Exportable.h"
#include "Attribute.h"

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
bool Exportable::valid()
{
    if ( m_node.isNull() ||
         !m_alembicObject.valid() ||
         !m_alembicProperties.valid() )
    {
        return false;
    }
    
    for ( Exportables::iterator citer = m_children.begin();
          citer != m_children.end(); ++citer )
    {
        ExportablePtr child = (*citer);
        if ( !child || !child->valid() )
        {
            return false;
        }
    }

    for ( Attributes::iterator aiter = m_attributes.begin();
          aiter != m_attributes.end(); ++aiter )
    {
        AttributePtr attr = (*aiter);
        if ( !attr || !attr->valid() )
        {
            return false;
        }
    }
    
    return true;
}   

//-*****************************************************************************
Abc::Box3d Exportable::writeSample( const Abc::OSampleSelector &iSS )
{
    // Write children.
    Abc::Box3d childrenBounds;
    childrenBounds.makeEmpty();
    for ( Exportables::iterator citer = m_children.begin();
          citer != m_children.end(); ++citer )
    {
        Abc::Box3d childBounds = (*citer)->writeSample( iSS );
        childrenBounds.extendBy( childBounds );
    }
    
    // Write attributes.
    for ( Attributes::iterator aiter = m_attributes.begin();
          aiter != m_attributes.end(); ++aiter )
    {
        if ( iSS.getIndex() == 0 || (*aiter)->isAnimated() )
        {
            (*aiter)->writeSample( iSS );    
        }
    }

    // Return bounds.
    return childrenBounds;
}

//-*****************************************************************************
void Exportable::close()
{
    // Close children.
    for ( Exportables::iterator citer = m_children.begin();
          citer != m_children.end(); ++citer )
    {
        (*citer)->close();
        (*citer).reset();
    }

    // Close attributes.
    for ( Attributes::iterator aiter = m_attributes.begin();
          aiter != m_attributes.end(); ++aiter )
    {
        (*aiter)->close();
        (*aiter).reset();
    }

    // Close self.
    m_alembicProperties.reset();
    m_alembicObject.reset();
}

//-*****************************************************************************
bool Exportable::canExportAttributeNamed( const std::string &attrName )
{
    // basically, just ignore the transform ones.
    // All of the transform attributes are handled as a special case
    
    if ( attrName == "translateX" ||
         attrName == "translateY" ||
         
         attrName == "translateX" ||
         attrName == "translateY" ||
         attrName == "translateZ" ||
    
         attrName == "rotateX" ||
         attrName == "rotateY" ||
         attrName == "rotateZ" ||
         attrName == "rotateOrder" ||
         attrName == "rotationInterpolation" ||
    
         attrName == "scaleX" ||
         attrName == "scaleY" ||
         attrName == "scaleZ" ||
    
         attrName == "shearXY" ||
         attrName == "shearXZ" ||
         attrName == "shearYZ" ||

         attrName == "rotatePivotX" ||
         attrName == "rotatePivotY" ||
         attrName == "rotatePivotZ" ||

         attrName == "rotatePivotTranslateX" ||
         attrName == "rotatePivotTranslateY" ||
         attrName == "rotatePivotTranslateZ" ||

         attrName == "scalePivotX" ||
         attrName == "scalePivotY" ||
         attrName == "scalePivotZ" ||

         attrName == "scalePivotTranslateX" ||
         attrName == "scalePivotTranslateY" ||
         attrName == "scalePivotTranslateZ" ||

         attrName == "rotateAxisX" ||
         attrName == "rotateAxisY" ||
         attrName == "rotateAxisZ" ||

         attrName == "transMinusRotatePivotX" ||
         attrName == "transMinusRotatePivotY" ||
         attrName == "transMinusRotatePivotZ" )
    {
        return false;
    }
    else
    {
        return true;
    }
}

} // End namespace AlembicSimpleAbcExport
