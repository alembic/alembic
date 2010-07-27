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

#include <AlembicTraitsGeom/AlembicTraitsGeom.h>
#include <AlembicAsset/AlembicAsset.h>

#include <GtoContainer/PropertyContainer.h>
#include <GtoContainer/ObjectVector.h>
#include <GtoContainer/StdProperties.h>
#include <GtoContainer/Reader.h>

#include <boost/progress.hpp>

#include <iostream>
#include <stdlib.h>

namespace Abc = AlembicAsset;
namespace Atg = AlembicTraitsGeom;
namespace Gtc = GtoContainer;

//-*****************************************************************************
// Copy indices and whatnots.
template <class MESHABLE>
void copyElementsIndices( const Gtc::PropertyContainer &pc,
                          MESHABLE &mesh )
{
    static std::vector<Abc::int32_t> intVec;
    
    const Gtc::UshortProperty *sizes =
        pc.property<Gtc::UshortProperty>( "elements", "size" );
    const Gtc::IntProperty *indices =
        pc.property<Gtc::IntProperty>( "indices", "vertex" );

    AAH5_ASSERT( (( bool )sizes) && (sizes->size() > 0),
                 "Missing elements.size property in object: " << pc.name() );
    AAH5_ASSERT( (( bool )indices) && (indices->size() > 0),
                 "Missing indices.vertex property in object: " << pc.name() );

    // Convert sizes.
    size_t numSizes = sizes->size();
    intVec.resize( numSizes );
    for ( size_t s = 0; s < numSizes; ++s )
    {
        intVec[s] = ( Abc::int32_t )((*sizes)[s]);
    }
    mesh->mesh().faceCounts().set( &intVec.front(), numSizes );

    // Set indices.
    mesh->mesh().vertexIndices().set( &((*indices)[0]), indices->size() );
}

//-*****************************************************************************
// Copy matrices over.
template <class MESHABLE>
Abc::M44d copyGlobalMatrix( const Gtc::PropertyContainer &pc,
                            MESHABLE &mesh )
{
    Abc::M44d ret;
    
    const Gtc::Double16Property *matD =
        pc.property<Gtc::Double16Property>( "object", "globalMatrix" );
    if ( matD )
    {
        AAH5_ASSERT( matD->size() > 0,
                     "Degenerate object.globalPosition property in object: "
                     << pc.name() );
        const Abc::M44d *vals =
            ( const Abc::M44d * )&((*matD)[0]);
        ret = (*vals).transposed();
        mesh->xform().set( ret );
        return ret;
    }

    const Gtc::Float16Property *matF =
        pc.property<Gtc::Float16Property>( "object", "globalMatrix" );
    if ( matF )
    {
        AAH5_ASSERT( matF->size() > 0,
                     "Degenerate object.globalPosition property in object: "
                     << pc.name() );
        const Abc::M44f *vals =
            ( const Abc::M44f * )&((*matF)[0]);
        ret.setTheMatrix( (*vals).transposed() );
        mesh->xform().set( ret );
        return ret;
    }

    ret.makeIdentity();
    mesh->xform().set( ret );
    return ret;
}

//-*****************************************************************************
// Copy positions over.
template <class MESHABLE>
Abc::Box3d copyPositions( const Abc::M44d &localMtx,
                          const Gtc::PropertyContainer &pc,
                          MESHABLE &mesh )
{
    Abc::Box3d bnds;
    static std::vector<Abc::V3d> dvals;
    
    const Gtc::Double3Property *pointsD =
        pc.property<Gtc::Double3Property>( "points", "position" );
    if ( pointsD )
    {
        size_t numPoints = pointsD->size();
        AAH5_ASSERT( numPoints > 0,
                     "Degenerate points.position property in object: "
                     << pc.name() );
        
        const Abc::V3d *vals =
            ( const Abc::V3d * )&((*pointsD)[0]);
        
        bnds.makeEmpty();
        for ( size_t i = 0; i < numPoints; ++i )
        {
            bnds.extendBy( vals[i] * localMtx );
        }

        mesh->mesh().positions().set( vals, numPoints );
        return bnds;
    }

    const Gtc::Float3Property *pointsF =
        pc.property<Gtc::Float3Property>( "points", "position" );
    AAH5_ASSERT( ( bool )pointsF,
                 "Can't find float or double points.position property in: "
                 << pc.name() );
    {
        size_t numPoints = pointsF->size();
        AAH5_ASSERT( numPoints > 0,
                     "Degenerate points.position property in object: "
                     << pc.name() );

        const Abc::V3f *fvals =
            ( const Abc::V3f * )&((*pointsF)[0]);
        
        dvals.resize( numPoints );
        bnds.makeEmpty();
        for ( size_t i = 0; i < numPoints; ++i )
        {
            Abc::V3d &dv = dvals[i];
            dv.setValue( fvals[i] );
            bnds.extendBy( dv * localMtx );
        }

        mesh->mesh().positions().set( ( const Abc::V3d * )(&dvals.front()),
                                      numPoints );
        return bnds;
    }
}

//-*****************************************************************************
void doMesh( const Gtc::PropertyContainer &pc,
             Abc::OAsset &parent,
             bool forceSubd )
{
    // Get properties out of the pc, put them into the obj.
    // What properties?
    if ( !forceSubd &&
         ( pc.protocol() == Gtc::Protocol( "alembic_polymesh", 1 ) ||
           pc.protocol() == Gtc::Protocol( "polygon", 2 ) ) )
    {
        Atg::OSimplePolyMesh pmesh( parent, pc.name(),
                                    Abc::kThrowException );
        Abc::M44d localMtx = copyGlobalMatrix( pc, pmesh );
        copyElementsIndices( pc, pmesh );
        Abc::Box3d localBnds = copyPositions( localMtx, pc, pmesh );

        pmesh->bounds().set( localBnds );
    }
    else if ( pc.protocol() == Gtc::Protocol( "catmull-clark", 2 ) ||
              ( forceSubd &&
                ( pc.protocol() == Gtc::Protocol( "alembic_polymesh", 1 ) ||
                  pc.protocol() == Gtc::Protocol( "polygon", 2 ) ) ) )
    {
        Atg::OSimpleSubd psubd( parent, pc.name(),
                                Abc::kThrowException );
        Abc::M44d localMtx = copyGlobalMatrix( pc, psubd );
        copyElementsIndices( pc, psubd );
        Abc::Box3d localBnds = copyPositions( localMtx, pc, psubd );

        psubd->bounds().set( localBnds );
        
        psubd->subd().scheme().set( ( char )
                                    Atg::kCatmullClarkSubd );
        psubd->subd().boundary().set( ( char )
                                      Atg::kEdgesCornersBoundaryInterp );
    }
}

//-*****************************************************************************
void doIt( const std::string &gtoFileName,
           const std::string &alembicFileName,
           bool forceSubd )
{
#ifdef DEBUG
    Abc::Init( true, false );
#else
    Abc::Init();
#endif

    std::cout << "Beginning to read GTO file: " << gtoFileName << std::endl;
    Gtc::ObjectVector objs;
    {
        Gtc::Reader reader;
        reader.read( gtoFileName, objs );
    }
    std::cout << "Finished reading GTO file: " << gtoFileName << std::endl;

    // Scope
    {
        Abc::OAsset asset( alembicFileName, Abc::kThrowException );
        asset.setCompressionLevel( 5 );
        std::cout << "Opened asset: " << alembicFileName << std::endl;
        
        std::cout << "Converting..." << std::endl;
        boost::progress_display prog( objs.size() );
        
        for ( Gtc::ObjectVector::iterator oiter = objs.begin();
              oiter != objs.end(); ++oiter, ++prog )
        {
            Gtc::PropertyContainer *pc = (*oiter);
            doMesh( *pc, asset, forceSubd );
        }
    }
    std::cout << "Wrote Alembic Asset: " << alembicFileName << std::endl;
        
    objs.deleteContents();
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    if ( argc != 3 && argc != 4 )
    {
        std::cerr << "USAGE: " << argv[0]
                  << " <gtoFile> <abcFile> <forceSubd:Optional>"
                  << std::endl;
        exit( -1 );
    }

    bool forceSubd = false;
    if ( argc > 3 )
    {
        forceSubd = ( bool )atoi( argv[3] );
    }

    doIt( argv[1], argv[2], forceSubd );
}


