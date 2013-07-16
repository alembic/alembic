//-*****************************************************************************
//
// Copyright (c) 2009-2012,
//  Sony Pictures Imageworks, Inc. and
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
// Industrial Light & Magic nor the names of their contributors may be used
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

#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

// We include some global mesh data to test with from an external source
// to keep this example code clean.
#include <Alembic/AbcGeom/Tests/CurvesData.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

using namespace std;
using namespace Alembic::AbcGeom; // Contains Abc, AbcCoreAbstract

//-*****************************************************************************
//-*****************************************************************************
// WRITING OUT AN ANIMATED Curve
//
// Here we'll create an "Archive", which is Alembic's term for the actual
// file on disk containing all of the scene geometry. The Archive will contain
// a single animated Transform with a single static PolyMesh as its child.
//-*****************************************************************************
//-*****************************************************************************

void doSample( OCurves &iCurves )
{

    OCurvesSchema &curves = iCurves.getSchema();

    OFloatGeomParam::Sample widthSample(
        FloatArraySample( (const float *)g_widths, g_numWidths ),
        kVertexScope );

    OV2fGeomParam::Sample uvSample(
        V2fArraySample( (const V2f *)g_uvs, g_totalVerts ),
        kVertexScope );


    std::cout << "creating sample " << curves.getNumSamples() << std::endl;
    OCurvesSchema::Sample curves_sample(
        V3fArraySample( ( const V3f * ) g_verts, g_totalVerts ),
        Int32ArraySample( g_numVerts, g_numCurves),
        kCubic,
        kNonPeriodic,
        widthSample,
        uvSample
                                       );

    std::cout << "setting sample" << std::endl;

    // Set the sample.
    curves.set( curves_sample );

    std::cout << "curves now have " << curves.getNumSamples() << " samples"
              << std::endl;

}

void Example1_CurvesOut()
{
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                      "curves1.abc" );

    OCurves myCurves( OObject( archive, kTop ),
                      "reallly_long_curves_name");

    for ( size_t i = 0 ; i < 5 ; ++i )
    {
        doSample( myCurves );
    }

    // Alembic objects close themselves automatically when they go out
    // of scope. So - we don't have to do anything to finish
    // them off!
    std::cout << "Writing: " << archive.getName() << std::endl;
}


void Example1_GeomBaseIn()
{
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(), "curves1.abc" );
    std::cout << "\nReading: " << archive.getName() << std::endl;

    std::cout <<"constructing curves" << std::endl;
    IObject myCurvesAsObj( IObject( archive, kTop) , "reallly_long_curves_name");
    ICurves myCurves( IObject( archive, kTop) , "reallly_long_curves_name");
    IGeomBaseObject geomBaseObject ( myCurvesAsObj, kWrapExisting);
    TESTING_ASSERT( IGeomBase::matches( myCurves.getMetaData() ) );

    std::cout << "getting schema" << std::endl;
    IGeomBase geomBase = geomBaseObject.getSchema();
    std::cout << "done getting schema" << std::endl;

    // get the samples from the curves
    IGeomBase::Sample geomBaseSample;
    geomBase.get( geomBaseSample );

    // test the bounding box
    TESTING_ASSERT( geomBaseSample.getSelfBounds().min == V3d( -1.0, -1.0, -1.0 ) );
    std::cout << "bounds max " << geomBaseSample.getSelfBounds().max << std::endl;
    TESTING_ASSERT( geomBaseSample.getSelfBounds().max == V3d( 1.0, 1.0, 1.0 ) );



}

void IndexexedGeomParamTest()
{
    {
        std::vector< std::string > vals( 4 );
        vals[0] = "a";
        vals[1] = "b";
        vals[2] = "c";
        vals[3] = "d";

        std::vector< Alembic::Util::uint32_t > indices( 4 );
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 3;

        StringArraySample valsSamp( vals );
        UInt32ArraySample indicesSamp( indices );

        OStringGeomParam::Sample samp( valsSamp, indicesSamp,
                                       Alembic::AbcGeom::kConstantScope );

        OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                          "indexedGeomParam.abc" );
        OCompoundProperty prop = archive.getTop().getProperties();
        TimeSamplingPtr ts( new TimeSampling( 1.0 / 24.0, 0.0 ) );

        // constant values, constant indices
        OStringGeomParam cvci( prop, "cvci", true,
                               Alembic::AbcGeom::kConstantScope, 1, ts );
        cvci.set( samp );
        cvci.set( samp );

        // animated values, constant indices
        OStringGeomParam avci( prop, "avci", true,
                               Alembic::AbcGeom::kConstantScope, 1, ts );
        avci.set( samp );
        vals[3] = "e";
        avci.set( samp );
        vals[3] = "d";

        // constant values, animated indices
        OStringGeomParam cvai( prop, "cvai", true,
                               Alembic::AbcGeom::kConstantScope, 1, ts );
        cvai.set( samp );
        indices[3] = 0;
        cvai.set( samp );
        indices[3] = 3;

        // animated values, animated indices
        OStringGeomParam avai( prop, "avai", true,
                               Alembic::AbcGeom::kConstantScope, 1, ts );
        avai.set( samp );
        vals[0] = "aa";
        indices[3] = 0;
        avai.set( samp );

    }

    {
        IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                          "indexedGeomParam.abc" );
        ICompoundProperty prop = archive.getTop().getProperties();
        IStringGeomParam::Sample samp;

        IStringGeomParam cvci( prop, "cvci" );
        cvci.getExpanded(samp, ISampleSelector( 0.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );
        cvci.getExpanded(samp, ISampleSelector( 1.0/24.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );

        cvci.getIndexed(samp, ISampleSelector( 0.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );
        TESTING_ASSERT(
            samp.getIndices()->get()[0] == 0 &&
            samp.getIndices()->get()[1] == 1 &&
            samp.getIndices()->get()[2] == 2 &&
            samp.getIndices()->get()[3] == 3 );
        cvci.getIndexed(samp, ISampleSelector( 1.0/24.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );
        TESTING_ASSERT(
            samp.getIndices()->get()[0] == 0 &&
            samp.getIndices()->get()[1] == 1 &&
            samp.getIndices()->get()[2] == 2 &&
            samp.getIndices()->get()[3] == 3 );

        IStringGeomParam avci( prop, "avci" );
        avci.getExpanded(samp, ISampleSelector( 0.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );
        avci.getExpanded(samp, ISampleSelector( 1.0/24.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "e" );

        avci.getIndexed(samp, ISampleSelector( 0.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );
        TESTING_ASSERT(
            samp.getIndices()->get()[0] == 0 &&
            samp.getIndices()->get()[1] == 1 &&
            samp.getIndices()->get()[2] == 2 &&
            samp.getIndices()->get()[3] == 3 );
        avci.getIndexed(samp, ISampleSelector( 1.0/24.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "e" );
        TESTING_ASSERT(
            samp.getIndices()->get()[0] == 0 &&
            samp.getIndices()->get()[1] == 1 &&
            samp.getIndices()->get()[2] == 2 &&
            samp.getIndices()->get()[3] == 3 );

        IStringGeomParam cvai( prop, "cvai" );
        cvai.getExpanded(samp, ISampleSelector( 0.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );
        cvai.getExpanded(samp, ISampleSelector( 1.0/24.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "a" );

        cvai.getIndexed(samp, ISampleSelector( 0.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );
        TESTING_ASSERT(
            samp.getIndices()->get()[0] == 0 &&
            samp.getIndices()->get()[1] == 1 &&
            samp.getIndices()->get()[2] == 2 &&
            samp.getIndices()->get()[3] == 3 );
        cvai.getIndexed(samp, ISampleSelector( 1.0/24.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );
        TESTING_ASSERT(
            samp.getIndices()->get()[0] == 0 &&
            samp.getIndices()->get()[1] == 1 &&
            samp.getIndices()->get()[2] == 2 &&
            samp.getIndices()->get()[3] == 0 );

        IStringGeomParam avai( prop, "avai" );
        avai.getExpanded(samp, ISampleSelector( 0.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );
        avai.getExpanded(samp, ISampleSelector( 1.0/24.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "aa" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "aa" );

        avai.getIndexed(samp, ISampleSelector( 0.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "a" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );
        TESTING_ASSERT(
            samp.getIndices()->get()[0] == 0 &&
            samp.getIndices()->get()[1] == 1 &&
            samp.getIndices()->get()[2] == 2 &&
            samp.getIndices()->get()[3] == 3 );
        avai.getIndexed(samp, ISampleSelector( 1.0/24.0 ) );
        TESTING_ASSERT(
            samp.getVals()->get()[0] == "aa" &&
            samp.getVals()->get()[1] == "b" &&
            samp.getVals()->get()[2] == "c" &&
            samp.getVals()->get()[3] == "d" );
        TESTING_ASSERT(
            samp.getIndices()->get()[0] == 0 &&
            samp.getIndices()->get()[1] == 1 &&
            samp.getIndices()->get()[2] == 2 &&
            samp.getIndices()->get()[3] == 0 );
    }
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MAIN FUNCTION!
// I'm not going to bother with exceptions, since I have no actions I
// could do to deal with them. If something goes wrong, it will cheerfully
// crash and print the exception information.
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
int main( int argc, char *argv[] )
{

    std::cout << "writing curves" << std::endl;

    // Curves Out
    Example1_CurvesOut();

    std::cout << "wrote curves" << std::endl;

    Example1_GeomBaseIn();

    IndexexedGeomParamTest();
    return 0;
}
