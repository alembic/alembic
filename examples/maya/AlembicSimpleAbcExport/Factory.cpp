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

//-*****************************************************************************
// Some portions of this file are:
//
//  Copyright (c) 2009, Tweak Software
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
//     * Neither the name of the Tweak Software nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
//
//  THIS SOFTWARE IS PROVIDED BY Tweak Software ''AS IS'' AND ANY EXPRESS
//  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL Tweak Software BE LIABLE FOR
//  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
//  OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
//  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//
//-*****************************************************************************

#include "Factory.h"
#include "Attribute.h"
#include "GenericNode.h"
#include "Transform.h"
#include "PolyMesh.h"

namespace AlembicSimpleAbcExport {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// EXPORTABLES STUFF
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
void Factory::makeTree( Exportable &iExportableParent,
                        MDagPath &iDagPath,
                        int iRecursionLevel,
                        const Abc::TimeSamplingType &iTsmpType )
{
    MStatus status;

    // Don't recurse beyond a point!
    if ( iRecursionLevel < 0 ) { return; }

    // Full path names are unique.
    // Don't write them more than once.
    std::string fullPathName = iDagPath.fullPathName().asChar();
    if ( m_visitedPaths.count( fullPathName ) > 0 ) { return; }

    // Haven't visited this one yet, so mark it as visited.
    m_visitedPaths.insert( fullPathName );

    // Get the MObject, leave if we don't need to export
    // this for whatever reason.
    MObject object = iDagPath.node();
    std::string objectName;
    if ( object.hasFn( MFn::kDependencyNode ) )
    {
        MFnDependencyNode dn( object );

        // Find an acceptable name.
        MString name = iDagPath.partialPathName( &status );
        CHECK_MAYA_STATUS;

        std::string n( name.asChar() );
        size_t p = n.rfind( '|' );
        if ( p != std::string::npos )
        {
            n = n.substr( p + 1 );
        }
        objectName = n;

        // See whether it's non-exportable for some reason.
        if ( object.hasFn( MFn::kDagNode ) )
        {
            MFnDagNode dn( object );
            MPlug iPlug = dn.findPlug( "intermediateObject" );
            bool intermediate;
            iPlug.getValue( intermediate );
            if ( intermediate )
            {
                return;
            }
        }
    }

    // Here is a thingy ptr that we will set the value of.
    ExportablePtr exportable;

    if ( object.apiType() == MFn::kTransform )
    {
        exportable.reset( new Transform( iExportableParent,
                                         iDagPath,
                                         object,
                                         objectName,
                                         iTsmpType ) );
        std::cout << "Created new Transform named: "
                  << objectName << std::endl;
    }
    else if ( object.hasFn( MFn::kMesh ) )
    {
#if 0
        if ( m_params.polysAsSubds || IsSubd( iDagPath ) )
        {
            exportable.reset( new SubD( iExportableParent,
                                        iDagPath,
                                        object,
                                        objectName,
                                        iTsmpType,
                                        m_params.deforming ) );

            std::cout << "Created new SubD named: "
                      << objectName << std::endl;
        }
        else
#endif
        {
            exportable.reset( new PolyMesh( iExportableParent,
                                            iDagPath,
                                            object,
                                            objectName,
                                            iTsmpType,
                                            m_params.deforming ) );

            std::cout << "Created new PolyMesh named: "
                      << objectName << std::endl;
        }
    }
    else
    {
        // CJH: Ignore for now.
        if ( iDagPath.childCount() > 0 )
        {
            exportable.reset( new GenericNode( iExportableParent,
                                               iDagPath,
                                               object,
                                               objectName ) );
        }

        std::cout << "Created new GenericNode named: "
                  << objectName << std::endl;
    }

    // Get out if there are errors.
    if ( !exportable || !exportable->valid() )
    {
        std::cout << "But I'm leaving child: " << objectName
                  << " because it isn't valid." << std::endl;
        if ( exportable )
        {
            std::cout << "...exportable exists but isn't valid."
                      << std::endl;
        }

        return;
    }

    //-*************************************************************************
    // ADD SELF AS CHILD OF PARENT
    //-*************************************************************************
    iExportableParent.addChild( exportable );

    //-*************************************************************************
    // GET THE ATTRIBUTES, PUT THEM ON THE THINGY
    //-*************************************************************************
#if 0
    if ( m_params.allUserAttributes || m_params.allMayaAttributes )
    {
        findAttributes( *exportable, tst );
    }
#endif

    //-*************************************************************************
    // GET THE CHILDREN, PUT THEM ON THE THINGY
    //-*************************************************************************
    for ( size_t i = 0; i < iDagPath.childCount(); ++i )
    {
        MObject child = iDagPath.child( i );
        if ( child.hasFn( MFn::kDagNode ) )
        {
            MFnDagNode dn( child );
            MDagPath childDp;
            dn.getPath( childDp );

            makeTree( *exportable,
                      childDp,
                      iRecursionLevel - 1,
                      iTsmpType );
        }
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// ATTRIBUTE STUFF
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
void Factory::findAttributes( Exportable &iExportable,
                              const Abc::TimeSamplingType &iTsmpType )
{
    // #define CONT(x)                                              \
    // {                                                            \
    //     std::cerr << plug.info() << ": " << x << std::endl;      \
    //     continue;                                                \
    // }
#define CONT(x) { continue; }

    MStatus status;

    MDagPath &dagPath = iExportable.getDagPath();
    MObject nodeObj = dagPath.node();

    MFnDependencyNode dn( dagPath.node(), &status );
    CHECK_MAYA_STATUS;

    for ( int i = 0; i < dn.attributeCount(); ++i )
    {
        MObject attrObj = dn.attribute( i );
        MPlug plug( nodeObj, attrObj );

        // Get the attribute's name.
        std::string attrName =
            plug.partialName( false, false, false,
                              false, false, true ).asChar();

        if ( plug.isNull() )
        {
            CONT( "NULL Plug" );
        }
        else if ( !canExportAttribute( plug ) )
        {
            CONT( "Can't export this kind of attribute" );
        }
        else if ( !iExportable.canExportAttributeNamed( attrName ) )
        {
            CONT( "Exportable doesn't like this attrName" );
        }
        else if ( !plug.isDynamic() && !m_params.allMayaAttributes )
        {
            CONT( "Not dynamic, and all attrs not requested." );
        }

        findAttribute( iExportable,
                       nodeObj,
                       attrObj,
                       plug,
                       attrName,
                       iTsmpType );
    }

#undef CONT
}

//-*****************************************************************************
void Factory::findAttribute( Exportable &iExportable,
                             MObject &iNodeObj,
                             MObject &iAttrObj,
                             MPlug &iPlug,
                             const std::string &iAttrName,
                             const Abc::TimeSamplingType &iTsmpType )
{
    MStatus status;
    MFnAttribute attr( iAttrObj, &status );
    CHECK_MAYA_STATUS;

    // Figure out if the attribute is animated.
    // Default constructor creates an "i am not animated" time sampling.
    Abc::TimeSamplingType attrTsmpType;
    bool isAnim = false;
    if ( MAnimUtil::isAnimated( iPlug ) )
    {
        // If we're animated, set it to the tst passed in!
        attrTsmpType = iTsmpType;
        isAnim = true;
    }

    const MFn::Type &attributeType = iAttrObj.apiType();

    AttributePtr newAttr;

    if ( attr.accepts( MFnData::kString ) )
    {
        // Just return without creating an Alembic property.
        // We don't support strings yet.
        // std::cerr << "We don't support string attributes yet"
        //           << std::endl;
        return;
    }
    else if ( attributeType == MFn::kDoubleLinearAttribute ||
              attributeType == MFn::kDoubleAngleAttribute ||
              attributeType == MFn::kTimeAttribute )
    {
        // Create a double attribute.
        newAttr.reset( new DoubleAttribute( iExportable,
                                            iNodeObj,
                                            iAttrObj,
                                            iPlug,
                                            iAttrName,
                                            attrTsmpType,
                                            isAnim ) );
    }
    else if ( attributeType == MFn::kFloatLinearAttribute ||
              attributeType == MFn::kFloatAngleAttribute )
    {
        // Create a float attribute.
        newAttr.reset( new FloatAttribute( iExportable,
                                           iNodeObj,
                                           iAttrObj,
                                           iPlug,
                                           iAttrName,
                                           attrTsmpType,
                                           isAnim ) );
    }
    else if ( attributeType == MFn::kEnumAttribute )
    {
        // Just return without creating an Alembic property.
        // We don't support enums yet.
        // std::cerr << "We don't support enum attributes yet."
        //           << std::endl;
        return;
    }
    else if ( attr.accepts( MFnData::kNumeric ) )
    {
        MFnNumericAttribute numAttr( iAttrObj, &status );
        CHECK_MAYA_STATUS;

        MFnNumericData::Type type = numAttr.unitType( &status );
        CHECK_MAYA_STATUS;

        switch( type )
        {
        case MFnNumericData::kFloat:
            newAttr.reset( new FloatAttribute( iExportable,
                                               iNodeObj,
                                               iAttrObj,
                                               iPlug,
                                               iAttrName,
                                               attrTsmpType,
                                               isAnim ) );
            break;
        case MFnNumericData::kDouble:
            newAttr.reset( new DoubleAttribute( iExportable,
                                                iNodeObj,
                                                iAttrObj,
                                                iPlug,
                                                iAttrName,
                                                attrTsmpType,
                                                isAnim ) );
            break;
        case MFnNumericData::kShort:
            newAttr.reset( new ShortAttribute( iExportable,
                                               iNodeObj,
                                               iAttrObj,
                                               iPlug,
                                               iAttrName,
                                               attrTsmpType,
                                               isAnim ) );
            break;
        case MFnNumericData::kInt:
            newAttr.reset( new IntAttribute( iExportable,
                                             iNodeObj,
                                             iAttrObj,
                                             iPlug,
                                             iAttrName,
                                             attrTsmpType,
                                             isAnim ) );
            break;
        case MFnNumericData::kByte:
        case MFnNumericData::kChar:
        case MFnNumericData::kBoolean:
            newAttr.reset( new CharAttribute( iExportable,
                                              iNodeObj,
                                              iAttrObj,
                                              iPlug,
                                              iAttrName,
                                              attrTsmpType,
                                              isAnim ) );
            break;
        default:
            // Nothing doin'
            // ERROR: unknown data type.
            return;
        }
    }

    if ( newAttr )
    {
        //std::cout << "AlembicSimpleAbcExport: Adding new attribute named: "
        //          << newAttr->name() << std::endl;
        iExportable.addAttribute( newAttr );
    }
}

//-*****************************************************************************
// static!
bool Factory::canExportAttribute( MPlug &iPlug, int iDepth )
{
    //#define CANTEXPORT(x)
    //do { std::cerr << plug.info() << ": "
    //            << x << std::endl;
    //             return false; } while( 0 )

#define CANTEXPORT(x) { return false; }

    if ( iPlug.isNull() )                                 CANTEXPORT( "isNull" );

    if ( iPlug.isArray() )                                CANTEXPORT( "isArray" );
    if ( iPlug.isElement() )                              CANTEXPORT( "isElement" );
    if ( iPlug.isProcedural() )                           CANTEXPORT( "isProcedural" );
    if ( iDepth == 0 && iPlug.isCompound() )              CANTEXPORT( "isCompound" );

    if ( iPlug.isChild() )
    {
        MPlug parent( iPlug.parent() );
        return Factory::canExportAttribute( parent, iDepth+1 );
    }

    MFnAttribute attr( iPlug.attribute() );

    if ( ! attr.isStorable() )                           CANTEXPORT( "!isStorable" );
    if ( attr.isIndeterminant() )                        CANTEXPORT( "isIndeterminant" );
    if ( attr.isHidden() )                               CANTEXPORT( "isHidden" );

    if ( attr.accepts( MFnData::kPlugin ) )              CANTEXPORT( "kPlugin" );
    if ( attr.accepts( MFnData::kPluginGeometry ) )      CANTEXPORT( "kPluginGeometry" );
    if ( attr.accepts( MFnData::kMatrix ) )              CANTEXPORT( "kMatrix" );
    if ( attr.accepts( MFnData::kMesh ) )                CANTEXPORT( "kMesh" );
    if ( attr.accepts( MFnData::kLattice ) )             CANTEXPORT( "kLattice" );
    if ( attr.accepts( MFnData::kNurbsCurve ) )          CANTEXPORT( "kNurbsCurve" );
    if ( attr.accepts( MFnData::kNurbsSurface ) )        CANTEXPORT( "kNurbsSurface" );
    if ( attr.accepts( MFnData::kSphere ) )              CANTEXPORT( "kSphere" );
    if ( attr.accepts( MFnData::kDynSweptGeometry ) )    CANTEXPORT( "kDynSweptGeometry" );
    if ( attr.accepts( MFnData::kSubdSurface ) )         CANTEXPORT( "kSubdSurface" );

    if ( attr.accepts( MFnData::kStringArray ) )         CANTEXPORT( "kIntArray" );
    if ( attr.accepts( MFnData::kIntArray ) )            CANTEXPORT( "kIntArray" );
    if ( attr.accepts( MFnData::kDoubleArray ) )         CANTEXPORT( "kSubdSurface" );
    if ( attr.accepts( MFnData::kPointArray ) )          CANTEXPORT( "kSubdSurface" );
    if ( attr.accepts( MFnData::kVectorArray ) )         CANTEXPORT( "kSubdSurface" );
    if ( attr.accepts( MFnData::kComponentList ) )       CANTEXPORT( "kSubdSurface" );

    return true;

#undef CANTEXPORT
}

} // End namespace AlembicSimpleAbcExport


