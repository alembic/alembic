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
#include "SimpleNull.h"
#include "Xform.h"
#include "PolyMesh.h"
#include "Subd.h"
#include "Collider.h"

namespace AlembicAbcExport {


//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// EXPORTABLES STUFF
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
void Factory::makeTree( Exportable &parent,
                        MDagPath &dagPath,
                        int recursionLevel,
                        const Abc::TimeSamplingInfo &tinfo )
{
    MStatus status;

    // Don't recurse beyond a point!
    if ( recursionLevel < 0 )
    {
        return;
    }

    // Full path names are unique.
    std::string fullPathName = dagPath.fullPathName().asChar();
    if ( m_visitedPaths.count( fullPathName ) > 0 )
    {
        // Already wrote this one!
        return;
    }

    // Haven't visited this one yet, so mark it as visited.
    m_visitedPaths.insert( fullPathName );

    // Get the MObject, leave if we don't need to export
    // this for whatever reason.
    MObject object = dagPath.node();
    std::string objectName;
    if ( object.hasFn( MFn::kDependencyNode ) )
    {
        MFnDependencyNode dn( object );

        // Find an acceptable name.
        MString name = dagPath.partialPathName( &status );
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

    Collider::ShapeType colliderShape = GetColliderShapeType( dagPath );
    if ( colliderShape != Collider::kUnknown )
    {
        std::cout << "MAKING COLLIDER NAMED: " << objectName
                  << " OF TYPE: " << ( int )colliderShape << std::endl;
        exportable.reset( new Collider( parent,
                                        dagPath,
                                        object,
                                        objectName,
                                        tinfo,
                                        colliderShape ) );
    }
    else if ( object.apiType() == MFn::kTransform )
    {
        exportable.reset( new Xform( parent,
                                     dagPath,
                                     object,
                                     objectName,
                                     tinfo ) );
        // std::cout << "Created new Xform named: "
        //          << objectName << std::endl;
    }
    else if ( object.hasFn( MFn::kMesh ) )
    {
        if ( m_params.polysAsSubds || IsSubd( dagPath ) )
        {
            exportable.reset( new Subd( parent,
                                        dagPath,
                                        object,
                                        objectName,
                                        tinfo,
                                        m_params.deforming ) );

            // std::cout << "Created new Subd named: "
            //           << objectName << std::endl;
        }
        else
        {
            exportable.reset( new PolyMesh( parent,
                                            dagPath,
                                            object,
                                            objectName,
                                            tinfo,
                                            m_params.deforming ) );

            // std::cout << "Created new PolyMesh named: "
            //           << objectName << std::endl;
        }
    }
    else
    {
        //exportable.reset( new SimpleNull( parent,
        //                                  dagPath,
        //                                  object,
        //                                  objectName,
        //                                  tinfo ) );

        exportable.reset( new Xform( parent,
                                     dagPath,
                                     object,
                                     objectName,
                                     tinfo ) );

        // std::cout << "Created new XFORM NULL named: "
        //           << objectName << std::endl;
    }

    // Get out if there are errors.
    if ( !exportable || !exportable->valid() )
    {
        return;
    }

    //-*************************************************************************
    // ADD SELF AS CHILD OF PARENT
    //-*************************************************************************
    parent.addChild( exportable );

    //-*************************************************************************
    // GET THE ATTRIBUTES, PUT THEM ON THE THINGY
    //-*************************************************************************
#if 0
    if ( m_params.allUserAttributes || m_params.allMayaAttributes )
    {
        findAttributes( *exportable, tinfo );
    }
#endif

    //-*************************************************************************
    // GET THE CHILDREN, PUT THEM ON THE THINGY
    //-*************************************************************************
    for ( size_t i = 0; i < dagPath.childCount(); i++ )
    {
        MObject child = dagPath.child( i );
        if ( child.hasFn( MFn::kDagNode ) )
        {
            MFnDagNode dn( child );
            MDagPath childDp;
            dn.getPath( childDp );

            makeTree( *exportable,
                      childDp,
                      recursionLevel - 1,
                      tinfo );
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
void Factory::findAttributes( Exportable &exportable,
                              const Abc::TimeSamplingInfo &tinfo )
{
    // #define CONT(x)                                              \
    // {                                                            \
    //     std::cerr << plug.info() << ": " << x << std::endl;      \
    //     continue;                                                \
    // }
#define CONT(x) { continue; }

    MStatus status;

    MDagPath &dagPath = exportable.dagPath();
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
        else if ( !exportable.canExportAttributeNamed( attrName ) )
        {
            CONT( "Exportable doesn't like this attrName" );
        }
        else if ( !plug.isDynamic() && !m_params.allMayaAttributes )
        {
            CONT( "Not dynamic, and all attrs not requested." );
        }

        findAttribute( exportable,
                       nodeObj,
                       attrObj,
                       plug,
                       attrName,
                       tinfo );
    }

#undef CONT
}

//-*****************************************************************************
void Factory::findAttribute( Exportable &exportable,
                             MObject &nodeObj,
                             MObject &attrObj,
                             MPlug &plug,
                             const std::string &attrName,
                             const Abc::TimeSamplingInfo &tinfo )
{
    MStatus status;
    MFnAttribute attr( attrObj, &status );
    CHECK_MAYA_STATUS;

    // Figure out if the attribute is animated.
    // Default constructor creates an "i am not animated" time sampling.
    Abc::TimeSamplingInfo attrTinfo;
    if ( MAnimUtil::isAnimated( plug ) )
    {
        // If we're animated, set it to the tinfo passed in!
        attrTinfo = tinfo;
    }

    const MFn::Type &attributeType = attrObj.apiType();

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
        newAttr.reset( new DoubleAttribute( exportable,
                                            nodeObj,
                                            attrObj,
                                            plug,
                                            attrName,
                                            attrTinfo ) );
    }
    else if ( attributeType == MFn::kFloatLinearAttribute ||
              attributeType == MFn::kFloatAngleAttribute )
    {
        // Create a float attribute.
        newAttr.reset( new FloatAttribute( exportable,
                                           nodeObj,
                                           attrObj,
                                           plug,
                                           attrName,
                                           attrTinfo ) );
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
        MFnNumericAttribute numAttr( attrObj, &status );
        CHECK_MAYA_STATUS;

        MFnNumericData::Type type = numAttr.unitType( &status );
        CHECK_MAYA_STATUS;

        switch( type )
        {
        case MFnNumericData::kFloat:
            newAttr.reset( new FloatAttribute( exportable,
                                               nodeObj,
                                               attrObj,
                                               plug,
                                               attrName,
                                               attrTinfo ) );
            break;
        case MFnNumericData::kDouble:
            newAttr.reset( new DoubleAttribute( exportable,
                                                nodeObj,
                                                attrObj,
                                                plug,
                                                attrName,
                                                attrTinfo ) );
            break;
        case MFnNumericData::kShort:
            newAttr.reset( new ShortAttribute( exportable,
                                               nodeObj,
                                               attrObj,
                                               plug,
                                               attrName,
                                               attrTinfo ) );
            break;
        case MFnNumericData::kInt:
            newAttr.reset( new IntAttribute( exportable,
                                             nodeObj,
                                             attrObj,
                                             plug,
                                             attrName,
                                             attrTinfo ) );
            break;
        case MFnNumericData::kByte:
        case MFnNumericData::kChar:
        case MFnNumericData::kBoolean:
            newAttr.reset( new CharAttribute( exportable,
                                              nodeObj,
                                              attrObj,
                                              plug,
                                              attrName,
                                              attrTinfo ) );
            break;
        default:
            // Nothing doin'
            // ERROR: unknown data type.
            return;
        }
    }

    if ( newAttr )
    {
        // std::cout << "AlembicAbcExport: Adding new attribute named: "
        //           << newAttr->name() << std::endl;
        exportable.addAttribute( newAttr );
    }
}

//-*****************************************************************************
// static!
bool Factory::canExportAttribute( MPlug &plug, int depth )
{
    //#define CANTEXPORT(x)
    //do { std::cerr << plug.info() << ": "
    //            << x << std::endl;
    //             return false; } while( 0 )

#define CANTEXPORT(x) { return false; }

    if ( plug.isNull() )                                 CANTEXPORT( "isNull" );

    if ( plug.isArray() )                                CANTEXPORT( "isArray" );
    if ( plug.isElement() )                              CANTEXPORT( "isElement" );
    if ( plug.isProcedural() )                           CANTEXPORT( "isProcedural" );
    if ( depth == 0 && plug.isCompound() )               CANTEXPORT( "isCompound" );

    if ( plug.isChild() )
    {
        MPlug parent( plug.parent() );
        return Factory::canExportAttribute( parent, depth+1 );
    }

    MFnAttribute attr( plug.attribute() );

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

} // End namespace AlembicAbcExport


