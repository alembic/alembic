//-*****************************************************************************
//
// Copyright (c) 2009-2014,
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
#include <Alembic/AbcCoreOgawa/All.h>

// We include some global mesh data to test with from an external source
// to keep this example code clean.
#include <Alembic/AbcGeom/Tests/CurvesData.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

using namespace std;
using namespace Alembic::AbcGeom; // Contains Abc, AbcCoreAbstract

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
    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(),
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


void Example1_CurvesIn()
{
    IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), "curves1.abc" );
    std::cout << "Reading: " << archive.getName() << std::endl;

    std::cout <<"constructing curves" << std::endl;
    ICurves myCurves( IObject( archive, kTop) , "reallly_long_curves_name");
    std::cout << "getting schema" << std::endl;
    ICurvesSchema &curves = myCurves.getSchema();
    std::cout << "done getting schema" << std::endl;

    // get the samples from the curves
    ICurvesSchema::Sample curvesSample;
    curves.get( curvesSample );

    // test the bounding box
    TESTING_ASSERT( curvesSample.getSelfBounds().min == V3d( -1.0, -1.0, -1.0 ) );
    std::cout << "bounds max " << curvesSample.getSelfBounds().max << std::endl;
    TESTING_ASSERT( curvesSample.getSelfBounds().max == V3d( 1.0, 1.0, 1.0 ) );

    // test other attributes
    TESTING_ASSERT( curvesSample.getPositions() -> size() == 12);

    IFloatGeomParam::Sample widthSample;
    curves.getWidthsParam().getExpanded( widthSample );
    TESTING_ASSERT( widthSample.getVals()->size() == 12);
    TESTING_ASSERT( curves.getWidthsParam().valid() );
    TESTING_ASSERT( widthSample.getScope() == kVertexScope );


}

void Example2_CurvesOut()
{
    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(),
                      "curves2.abc" );

    OCurves myCurves( OObject( archive, kTop ), "nurbsCurve" );

    OCurvesSchema &curves = myCurves.getSchema();

    OFloatGeomParam::Sample widthSample(
        FloatArraySample( (const float *)g_widths, g_numWidths ),
        kVertexScope );

    OV2fGeomParam::Sample uvSample(
        V2fArraySample( (const V2f *)g_uvs, g_totalVerts ),
        kVertexScope );

    OCurvesSchema::Sample curves_sample(
        V3fArraySample( ( const V3f * ) g_verts, g_totalVerts ),
        Int32ArraySample( g_numVerts, g_numCurves),
        kVariableOrder,
        kNonPeriodic,
        widthSample,
        uvSample,
        ON3fGeomParam::Sample(),
        kBezierBasis,
        FloatArraySample( g_weights, g_totalVerts ),
        UcharArraySample( g_orders, g_numCurves ),
        FloatArraySample( g_knots, 18 )
                                       );
    // Set the sample.
    curves.set( curves_sample );

    OCurvesSchema::Sample curves_sample2(
        V3fArraySample( ( const V3f * ) g_verts, g_totalVerts ),
        Int32ArraySample( g_numVerts, g_numCurves),
        kVariableOrder,
        kNonPeriodic,
        widthSample,
        uvSample,
        ON3fGeomParam::Sample(),
        kBezierBasis);
    curves.set( curves_sample2 );
}

void Example2_CurvesIn()
{
    IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), "curves2.abc" );

    ICurves myCurves( IObject( archive, kTop) , "nurbsCurve");
    ICurvesSchema &curves = myCurves.getSchema();

    // get the samples from the curves
    ICurvesSchema::Sample curvesSample;
    curves.get( curvesSample );

    // test the bounding box
    TESTING_ASSERT( curvesSample.getSelfBounds().min == V3d( -1.0, -1.0, -1.0 ) );
    TESTING_ASSERT( curvesSample.getSelfBounds().max == V3d( 1.0, 1.0, 1.0 ) );

    // test other attributes
    TESTING_ASSERT( curvesSample.getPositions() -> size() == 12);

    TESTING_ASSERT( curvesSample.getType() == kVariableOrder );
    TESTING_ASSERT( curvesSample.getWrap() == kNonPeriodic );
    TESTING_ASSERT( curvesSample.getBasis() == kBezierBasis );
    TESTING_ASSERT( ( *curvesSample.getOrders() )[0] == 4 );
    TESTING_ASSERT( ( *curvesSample.getOrders() )[1] == 2 );
    TESTING_ASSERT( curvesSample.getPositionWeights()->size() == 12 );
    TESTING_ASSERT( curvesSample.getKnots()->size() == 18 );

    IFloatGeomParam::Sample widthSample;
    curves.getWidthsParam().getExpanded( widthSample );
    TESTING_ASSERT( widthSample.getVals()->size() == 12);
    TESTING_ASSERT( curves.getWidthsParam().valid() );
    TESTING_ASSERT( widthSample.getScope() == kVertexScope );

    IFloatArrayProperty knotsProp = curves.getKnotsProperty();
    TESTING_ASSERT( knotsProp.getNumSamples() == 2 );
    ArraySampleKey keyA, keyB;
    knotsProp.getKey( keyA, 0 );
    knotsProp.getKey( keyB, 1 );
    TESTING_ASSERT( keyA == keyB );
}

//-*****************************************************************************
void sparseTest()
{
    std::string name = "sparseCurveTest.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), name );
        OCurves curveUVsObj( OObject( archive, kTop ), "curveUVs", kSparse );

        // only set UVs
        OCurvesSchema::Sample curveSamp;
        OV2fGeomParam::Sample uvSamp(
            V2fArraySample( (const V2f *)g_uvs, g_totalVerts ),
            kVertexScope );
        curveSamp.setUVs( uvSamp );
        curveUVsObj.getSchema().set( curveSamp );

        // only set positions
        OCurves curvePosObj( OObject( archive, kTop ), "curvePositions", kSparse );
        OCurvesSchema::Sample curveSamp2;
        curveSamp2.setPositions(
            V3fArraySample( ( const V3f * )g_verts, g_totalVerts ) );
        curvePosObj.getSchema().set( curveSamp2 );
    }

    {
        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), name );

        IObject curveUVsObj( IObject( archive, kTop ), "curveUVs" );

        // This should NOT match
        TESTING_ASSERT( !IPolyMeshSchema::matches( curveUVsObj.getMetaData() ) );
        ICompoundProperty geomProp( curveUVsObj.getProperties(), ".geom" );

        // This shouldn't match either
        TESTING_ASSERT( !IPolyMeshSchema::matches( geomProp.getMetaData() ) );

        // and we should ONLY have UVs
        TESTING_ASSERT( geomProp.getNumProperties() == 1 &&
            geomProp.getPropertyHeader("uv") != NULL );

        IArrayProperty uvsProp( geomProp, "uv" );
        TESTING_ASSERT( uvsProp.getNumSamples() == 1 );

        IObject curvePosObj( IObject( archive, kTop ), "curvePositions" );

        // This should NOT match
        TESTING_ASSERT( !IPolyMeshSchema::matches( curvePosObj.getMetaData() ) );
        geomProp = ICompoundProperty( curvePosObj.getProperties(), ".geom" );

        // This shouldn't match either
        TESTING_ASSERT( !IPolyMeshSchema::matches( geomProp.getMetaData() ) );
        TESTING_ASSERT( geomProp.getNumProperties() == 2 &&
            geomProp.getPropertyHeader("P") != NULL &&
            geomProp.getPropertyHeader(".selfBnds") != NULL );
        IArrayProperty ptsProp( geomProp, "P" );
        TESTING_ASSERT( ptsProp.getNumSamples() == 1 );
        IScalarProperty selfBndsProp( geomProp, ".selfBnds" );
        TESTING_ASSERT( selfBndsProp.getNumSamples() == 1 );
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

    Example1_CurvesIn();

    Example2_CurvesOut();
    Example2_CurvesIn();

    sparseTest();

    return 0;
}
