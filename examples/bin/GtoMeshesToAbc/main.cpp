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

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include <GtoContainer/PropertyContainer.h>
#include <GtoContainer/ObjectVector.h>
#include <GtoContainer/StdProperties.h>
#include <GtoContainer/Reader.h>

#include <boost/progress.hpp>

#include <iostream>
#include <stdlib.h>

//-*****************************************************************************
namespace Abc = Alembic::AbcGeom;
namespace Gtc = GtoContainer;

//-*****************************************************************************
// Copy indices and whatnots.
template <class SAMPLE>
void copyElementsIndices( const Gtc::PropertyContainer &pc,
                          SAMPLE &psamp )
{
    static std::vector<Abc::int32_t> intVec;
    
    const Gtc::UshortProperty *sizes =
        pc.property<Gtc::UshortProperty>( "elements", "size" );
    const Gtc::IntProperty *indices =
        pc.property<Gtc::IntProperty>( "indices", "vertex" );

    ABCA_ASSERT( (( bool )sizes) && (sizes->size() > 0),
                 "Missing elements.size property in object: " << pc.name() );
    ABCA_ASSERT( (( bool )indices) && (indices->size() > 0),
                 "Missing indices.vertex property in object: " << pc.name() );

    // Convert sizes.
    size_t numSizes = sizes->size();
    intVec.resize( numSizes );
    for ( size_t s = 0; s < numSizes; ++s )
    {
        intVec[s] = ( Abc::int32_t )((*sizes)[s]);
    }

    psamp.setCounts( Abc::Int32ArraySample( intVec ) );

    // Set indices.
    psamp.setIndices( Abc::Int32ArraySample(
                          ( const Abc::int32_t * )&((*indices)[0]),
                          indices->size() ) );
}

//-*****************************************************************************
// Copy matrices over.
Abc::M44d copyGlobalMatrix( const Gtc::PropertyContainer &pc,
                            Abc::OSimpleXform iXform )
{
    Abc::M44d ret;
    
    const Gtc::Double16Property *matD =
        pc.property<Gtc::Double16Property>( "object", "globalMatrix" );
    if ( matD )
    {
        ABCA_ASSERT( matD->size() > 0,
                     "Degenerate object.globalPosition property in object: "
                     << pc.name() );
        const Abc::M44d *vals =
            ( const Abc::M44d * )&((*matD)[0]);
        ret = (*vals).transposed();
        Abc::SimpleXformSample sampOut( ret );
        iXform.getSchema().set( sampOut );
        return ret;
    }

    const Gtc::Float16Property *matF =
        pc.property<Gtc::Float16Property>( "object", "globalMatrix" );
    if ( matF )
    {
        ABCA_ASSERT( matF->size() > 0,
                     "Degenerate object.globalPosition property in object: "
                     << pc.name() );
        const Abc::M44f *vals =
            ( const Abc::M44f * )&((*matF)[0]);
        ret.setTheMatrix( (*vals).transposed() );
        Abc::SimpleXformSample sampOut( ret );
        iXform.getSchema().set( sampOut );
        return ret;
    }

    ret.makeIdentity();
    Abc::SimpleXformSample sampOut( ret );
    iXform.getSchema().set( sampOut );
    return ret;
}

//-*****************************************************************************
// Copy positions over.
template <class SAMPLE>
Abc::Box3d copyPositions( const Gtc::PropertyContainer &pc,
                          SAMPLE &meshSamp )
{
    Abc::Box3d bnds;
    static std::vector<Abc::V3f> fvals;
    
    const Gtc::Double3Property *pointsD =
        pc.property<Gtc::Double3Property>( "points", "position" );
    if ( pointsD )
    {
        size_t numPoints = pointsD->size();
        ABCA_ASSERT( numPoints > 0,
                     "Degenerate points.position property in object: "
                     << pc.name() );
        
        const Abc::V3d *dvals =
            ( const Abc::V3d * )&((*pointsD)[0]);
        
        bnds.makeEmpty();
        fvals.resize( numPoints );
        for ( size_t i = 0; i < numPoints; ++i )
        {
            Abc::V3f &fv = fvals[i];
            fv.setValue( dvals[i] );
            bnds.extendBy( dvals[i] );
        }

        meshSamp.setPositions( Abc::V3fArraySample( fvals ) );
        return bnds;
    }

    const Gtc::Float3Property *pointsF =
        pc.property<Gtc::Float3Property>( "points", "position" );
    ABCA_ASSERT( ( bool )pointsF,
                 "Can't find float or double points.position property in: "
                 << pc.name() );
    {
        size_t numPoints = pointsF->size();
        ABCA_ASSERT( numPoints > 0,
                     "Degenerate points.position property in object: "
                     << pc.name() );

        const Abc::V3f *fvalsPC =
            ( const Abc::V3f * )&((*pointsF)[0]);
        
        bnds.makeEmpty();
        for ( size_t i = 0; i < numPoints; ++i )
        {
            Abc::V3d dv;
            dv.setValue( fvalsPC[i] );
            bnds.extendBy( dv );
        }

        meshSamp.setPositions( Abc::V3fArraySample( fvalsPC, numPoints ) );
        return bnds;
    }
}

//-*****************************************************************************
void doMesh( const Gtc::PropertyContainer &iPC,
             Abc::OArchive &iParent,
             bool iForceSubd )
{
    // TopObject
    Abc::OObject topObject( iParent, Abc::kTop );
    
    // Make the transform
    Abc::OSimpleXform oMeshXform( topObject, iPC.name() );
    copyGlobalMatrix( iPC, oMeshXform );
    
    // Get properties out of the pc, put them into the obj.
    // What properties?
    if ( !iForceSubd &&
         ( iPC.protocol() == Gtc::Protocol( "alembic_polymesh", 1 ) ||
           iPC.protocol() == Gtc::Protocol( "polygon", 2 ) ) )
    {
        // Make the poly mesh.
        Abc::OPolyMesh pmesh( oMeshXform, iPC.name() + "Shape" );
        Abc::OPolyMeshSchema::Sample psamp;
        copyElementsIndices( iPC, psamp );
        Abc::Box3d localBnds = copyPositions( iPC, psamp );
        pmesh.getSchema().set( psamp );
    }
    else if ( iPC.protocol() == Gtc::Protocol( "catmull-clark", 2 ) ||
              ( iForceSubd &&
                ( iPC.protocol() == Gtc::Protocol( "alembic_polymesh", 1 ) ||
                  iPC.protocol() == Gtc::Protocol( "polygon", 2 ) ) ) )
    {
        //Atg::OSimpleSubd psubd( parent, pc.name(),
        //                        Abc::kThrowException );
        Abc::OPolyMesh pmesh( oMeshXform, iPC.name() + "Shape" );
        Abc::OPolyMeshSchema::Sample psamp;
        copyElementsIndices( iPC, psamp );
        Abc::Box3d localBnds = copyPositions( iPC, psamp );
        pmesh.getSchema().set( psamp );
    }
}

//-*****************************************************************************
void doIt( const std::string &gtoFileName,
           const std::string &alembicFileName,
           bool forceSubd )
{
    std::cout << "Beginning to read GTO file: " << gtoFileName << std::endl;
    Gtc::ObjectVector objs;
    {
        Gtc::Reader reader;
        reader.read( gtoFileName, objs );
    }
    std::cout << "Finished reading GTO file: " << gtoFileName << std::endl;

    // Scope
    {
        Abc::OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                               alembicFileName );
        std::cout << "Opened Alembic Archive: "
                  << archive.getName() << std::endl;
        
        std::cout << "Converting..." << std::endl;
        boost::progress_display prog( objs.size() );
        
        for ( Gtc::ObjectVector::iterator oiter = objs.begin();
              oiter != objs.end(); ++oiter, ++prog )
        {
            Gtc::PropertyContainer *pc = (*oiter);
            doMesh( *pc, archive, forceSubd );
        }
    }
    std::cout << "Wrote Alembic Asset: "
              << alembicFileName << std::endl;
        
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


