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
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreFactory/IFactory.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

using namespace Alembic::AbcGeom;

//-*****************************************************************************
void recurseCreateXform(OObject & iParent, int children, int level,
                        std::vector<OXform> & oCreated)
{
    for (int i = 0; i < children; ++i)
    {
        std::ostringstream strm;
        strm << "level" << "_" << i;
        std::string xformName = strm.str();
        OXform xform( iParent, xformName );
        XformSample samp;
        XformOp transop( kTranslateOperation, kTranslateHint );
        XformOp rotatop( kRotateOperation, kRotateHint );
        XformOp scaleop( kScaleOperation, kScaleHint );
        samp.addOp( transop, V3d(0.0, 0.0, 0.0) );
        samp.addOp( rotatop, V3d(0.0, 0.0, 1.0), 0.0 );
        samp.addOp( rotatop, V3d(0.0, 1.0, 0.0), 0.0 );
        samp.addOp( rotatop, V3d(1.0, 0.0, 0.0), 0.0 );
        samp.addOp( scaleop, V3d(1.0, 1.0, 1.0) );
        xform.getSchema().set(samp);
        oCreated.push_back( xform );
        if ( level > 0 )
        {
            recurseCreateXform( xform, children, level - 1, oCreated );
        }
    }
}

//-*****************************************************************************
void xformTreeCreate()
{
    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), "Xform_tree.abc" );
    std::vector<OXform> xforms;
    OObject root( archive, kTop);
    recurseCreateXform( root, 4, 4, xforms );
    std::cout << "Total xforms created " << xforms.size() << std::endl;

    XformSample samp;
    XformOp transop( kTranslateOperation, kTranslateHint );
    XformOp rotatop( kRotateOperation, kRotateHint );
    XformOp scaleop( kScaleOperation, kScaleHint );
    samp.addOp( transop, V3d(42.0, 42.0, 42.0) );
    samp.addOp( rotatop, V3d(0.0, 0.0, 1.0), 10.0 );
    samp.addOp( rotatop, V3d(0.0, 1.0, 0.0), 20.0 );
    samp.addOp( rotatop, V3d(1.0, 0.0, 0.0), 30.0 );
    samp.addOp( scaleop, V3d(4.0, 4.0, 4.0) );

    for (std::vector<OXform>::iterator i = xforms.begin(); i != xforms.end();
         ++i)
    {
        i->getSchema().set(samp);
    }

}

//-*****************************************************************************
void xformOut()
{
    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), "Xform1.abc" );

    OXform a( OObject( archive, kTop ), "a" );
    OXform b( a, "b" );
    OXform c( b, "c" );
    OXform d( c, "d" );
    OXform e( d, "e" );
    OXform f( e, "f" );
    OXform g( f, "g" );

    XformOp transop( kTranslateOperation, kTranslateHint );
    XformOp scaleop( kScaleOperation, kScaleHint );
    XformOp matrixop( kMatrixOperation, kMatrixHint );

    TESTING_ASSERT( a.getSchema().getNumSamples() == 0 );

    OBox3dProperty childBounds = a.getSchema().getChildBoundsProperty();

    XformSample asamp;
    for ( size_t i = 0; i < 20; ++i )
    {
        asamp.addOp( transop, V3d( 12.0, i + 42.0, 20.0 ) );

        if ( i >= 18 )
        {
            childBounds.set( Abc::Box3d( V3d( -1.0, -1.0, -1.0 ),
                                         V3d( 1.0, 1.0, 1.0 ) ) );
        }
        else
        {
            childBounds.set( Abc::Box3d() );
        }

        a.getSchema().set( asamp );
    }

    XformSample bsamp;
    for ( size_t i = 0 ; i < 20 ; ++i )
    {
        bsamp.setInheritsXforms( (bool)(i&1) );

        b.getSchema().set( bsamp );
    }

    // for c we write nothing

    XformSample dsamp;
    dsamp.addOp( scaleop, V3d( 3.0, 6.0, 9.0 ) );
    d.getSchema().set( dsamp );

    XformSample esamp;
    M44d identmat;
    identmat.makeIdentity();

    esamp.addOp( transop, V3d( 0.0, 0.0, 0.0 ) );
    esamp.addOp( XformOp( kMatrixOperation, kMatrixHint ), identmat );
    esamp.addOp( scaleop, V3d( 1.0, 1.0, 1.0 ) );
    e.getSchema().set( esamp );

    XformSample fsamp;
    fsamp.addOp( transop, V3d( 3.0, -4.0, 5.0 ) );
    f.getSchema().set( fsamp );

    // this will cause the Xform's values property to be an ArrayProperty,
    // since there will be 20 * 16 channels.
    XformSample gsamp;
    Abc::M44d gmatrix;
    gmatrix.makeIdentity();
    for ( size_t i = 0 ; i < 20 ; ++i )
    {
        gmatrix.x[0][1] = (double)i;
        gsamp.addOp( matrixop, gmatrix );
    }
    g.getSchema().set( gsamp );
}

//-*****************************************************************************
void xformIn()
{
    IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), "Xform1.abc" );

    Abc::M44d identity;
    XformSample xs;

    IXform a( IObject( archive, kTop ), "a" );
    std::cout << "'a' num samples: " << a.getSchema().getNumSamples() << std::endl;

    TESTING_ASSERT( a.getSchema().getNumOps() == 1 );
    TESTING_ASSERT( a.getSchema().getInheritsXforms() );
    for ( index_t i = 0; i < 20; ++i )
    {
        XformSample xs;
        a.getSchema().get( xs, Abc::ISampleSelector( i ) );
        TESTING_ASSERT( xs.getNumOps() == 1 );
        TESTING_ASSERT( xs[0].isTranslateOp() );
        TESTING_ASSERT( xs[0].isYAnimated() == true );
        TESTING_ASSERT( xs[0].isXAnimated() == false );
        TESTING_ASSERT( xs[0].isZAnimated() == false );

        TESTING_ASSERT( xs.getTranslation() == V3d( 12.0, i+42.0, 20.0 ) );
        TESTING_ASSERT( xs.getMatrix() ==
                        Abc::M44d().setTranslation( V3d(12.0, i+42.0, 20.0)) );
    }

    IXform b( a, "b" );
    b.getSchema().get( xs );
    TESTING_ASSERT( b.getSchema().getTimeSampling()->getTimeSamplingType().isUniform() );
    // the schema is not static, because set() was called 20 times on it.
    TESTING_ASSERT( !b.getSchema().isConstant() );
    TESTING_ASSERT( b.getSchema().getNumSamples() == 20 );
    TESTING_ASSERT( xs.getNumOps() == 0 );
    TESTING_ASSERT( b.getSchema().getNumOps() == 0 );
    TESTING_ASSERT( xs.getMatrix() == identity );
    for (size_t i = 0; i < 20; ++i)
    {
        AbcA::index_t j = i;
        TESTING_ASSERT( b.getSchema().getInheritsXforms( ISampleSelector( j ) )
                        == (i&1) );
    }

    IXform c( b, "c" );
    xs = c.getSchema().getValue();
    TESTING_ASSERT( xs.getNumOps() == 0 );
    TESTING_ASSERT( c.getSchema().getNumOps() == 0 );
    TESTING_ASSERT( xs.getMatrix() == identity );
    TESTING_ASSERT( c.getSchema().getInheritsXforms() );
    TESTING_ASSERT( c.getSchema().isConstantIdentity() );


    IXform d( c, "d" );
    xs = d.getSchema().getValue();
    TESTING_ASSERT( xs.getNumOps() == 1 );
    TESTING_ASSERT( d.getSchema().getNumOps() == 1 );
    TESTING_ASSERT( xs[0].isScaleOp() );
    TESTING_ASSERT( ! ( xs[0].isXAnimated() || xs[0].isYAnimated()
                        || xs[0].isZAnimated() ) );
    TESTING_ASSERT( xs.getScale().equalWithAbsError( V3d( 3.0, 6.0, 9.0 ),
                                                     VAL_EPSILON ) );
    TESTING_ASSERT( xs.getMatrix() ==
                    Abc::M44d().setScale( V3d(3.0, 6.0, 9.0)) );
    TESTING_ASSERT( d.getSchema().getInheritsXforms() );

    IXform e( d, "e" );
    TESTING_ASSERT( e.getSchema().isConstantIdentity() );
    TESTING_ASSERT( e.getSchema().isConstant() );
    TESTING_ASSERT( e.getSchema().getNumOps() == 3 );

    IXform f( e, "f" );
    TESTING_ASSERT( f.getSchema().isConstant() ); // is constant
    TESTING_ASSERT( ! f.getSchema().isConstantIdentity() ); // not identity

    IXform g( f, "g" );
    Abc::M44d gmatrix;
    gmatrix.makeIdentity();
    XformSample gsamp = g.getSchema().getValue();
    TESTING_ASSERT( gsamp.getNumOps() == 20 );
    TESTING_ASSERT( gsamp.getNumOpChannels() == 20 * 16 );
    TESTING_ASSERT( g.getSchema().getNumSamples() == 1 );
    TESTING_ASSERT( g.getSchema().isConstant() );
    TESTING_ASSERT( !g.getSchema().isConstantIdentity() );
    for ( size_t i = 0 ; i < 20 ; ++i )
    {
        TESTING_ASSERT( gsamp[i].getChannelValue( 1 ) == (double)i );
    }

    std::cout << "Tested all xforms in first test!" << std::endl;

}

//-*****************************************************************************
void someOpsXform()
{
    std::string name = "someOpsXform.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), name );

        OXform a( OObject( archive, kTop ), "a" );

        OBox3dProperty bnds = CreateOArchiveBounds( archive );

        XformOp transop( kTranslateOperation, kTranslateHint );
        XformOp scaleop( kScaleOperation, kScaleHint );

        XformSample asamp;

        // scale
        asamp.addOp( scaleop, V3d( 2.0, 1.0, 2.0 ) );

        // Maya-like shear
        XformOp shearmatrixop( kMatrixOperation, kMayaShearHint );
        M44d shearmat;
        shearmat.makeIdentity();

        asamp.addOp( shearmatrixop, shearmat );

        // rotate x axis
        XformOp rotop( kRotateOperation, kRotateHint );
        asamp.addOp( rotop, V3d( 1.0, 0.0, 0.0 ), 1.57 );

        // rotate y axis, angle will be animated
        asamp.addOp( rotop, V3d( 0.0, 1.0, 0.0 ), 0.125 );

        // rotate z axis, use a different hint for fun
        XformOp rotorientop( kRotateOperation, kRotateOrientationHint );
        asamp.addOp( rotorientop, V3d( 0.0, 0.0, 1.0 ), 0.1 );

        // translate with animated y and z, different hint for fun
        XformOp transpivotop( kTranslateOperation, kRotatePivotPointHint );
        asamp.addOp( transpivotop, V3d( 0.0, 0.0, 0.0 ) );

        a.getSchema().set( asamp );
        bnds.set( Box3d( V3d( -0.1, -0.1, -0.1 ),
                         V3d(  0.1,  0.1,  0.1 ) ) );

        for (size_t i = 1; i < 5 ; ++i)
        {
            asamp.addOp( scaleop, V3d( 2 * ( i + 1 ),
                                       1.0, 2.0 ) );

            shearmat.x[1][0] = (double)i;
            shearmat.x[2][0] = (double)( (int)i * -1.0 );
            shearmat.x[2][1] = 0.0;

            asamp.addOp( shearmatrixop, shearmat );

            asamp.addOp( rotop, V3d( 1.0, 0.0, 0.0 ),
                         1.57 );
            asamp.addOp( rotop, V3d( 0.0, 1.0, 0.0 ),
                         0.125 * ( i + 1 ) );
            asamp.addOp( rotorientop, V3d( 0.0, 0.0, 1.0 ),
                         0.1 * ( i + 1 ) );

            asamp.addOp( transpivotop, V3d( 0.0, 3.0 * i, 4.0 * i ) );

            a.getSchema().set( asamp );
            double iVal = static_cast< double >( i );
            bnds.set( Box3d( V3d( -iVal, -iVal, -iVal ),
                             V3d(  iVal,  iVal,  iVal ) ) );
        }

    }

    {
        IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(), name );

        IXform a( IObject( archive, kTop ), "a" );
        IBox3dProperty bnds = GetIArchiveBounds( archive );

        XformSample asamp;

        a.getSchema().get( asamp );

        TESTING_ASSERT( a.getSchema().getNumOps() == 6 );

        TESTING_ASSERT( asamp[0].isScaleOp() );
        TESTING_ASSERT( asamp[0].getHint() == kScaleHint );

        TESTING_ASSERT( asamp[1].isMatrixOp() );
        TESTING_ASSERT( asamp[1].getHint() == kMayaShearHint );

        TESTING_ASSERT( asamp[2].isRotateOp() );
        TESTING_ASSERT( asamp[2].getHint() == kRotateHint );

        TESTING_ASSERT( asamp[3].getType() == kRotateOperation );
        TESTING_ASSERT( asamp[3].getHint() == kRotateHint );

        TESTING_ASSERT( asamp[4].getType() == kRotateOperation );
        TESTING_ASSERT( asamp[4].getHint() == kRotateOrientationHint );

        TESTING_ASSERT( asamp[5].getType() == kTranslateOperation );
        TESTING_ASSERT( asamp[5].getHint() == kRotatePivotPointHint );

        TESTING_ASSERT( asamp[0].isXAnimated() );
        TESTING_ASSERT( !asamp[0].isYAnimated() );
        TESTING_ASSERT( !asamp[0].isZAnimated() );

        TESTING_ASSERT( !asamp[1].isChannelAnimated(0) );  // [0][0]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(1) );  // [0][1]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(2) );  // [0][2]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(3) );  // [0][3]
        TESTING_ASSERT( asamp[1].isChannelAnimated(4) );   // [1][0]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(5) );  // [1][1]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(6) );  // [1][2]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(7) );  // [1][3]
        TESTING_ASSERT( asamp[1].isChannelAnimated(8) );   // [2][0]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(9) );  // [2][1]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(10) ); // [2][2]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(11) ); // [2][3]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(12) ); // [3][0]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(13) ); // [3][1]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(14) ); // [3][2]
        TESTING_ASSERT( !asamp[1].isChannelAnimated(15) ); // [3][3]

        TESTING_ASSERT( !asamp[2].isXAnimated() );
        TESTING_ASSERT( !asamp[2].isYAnimated() );
        TESTING_ASSERT( !asamp[2].isZAnimated() );
        TESTING_ASSERT( !asamp[2].isAngleAnimated() );

        TESTING_ASSERT( !asamp[3].isXAnimated() );
        TESTING_ASSERT( !asamp[3].isYAnimated() );
        TESTING_ASSERT( !asamp[3].isZAnimated() );
        TESTING_ASSERT( asamp[3].isAngleAnimated() );

        TESTING_ASSERT( !asamp[4].isXAnimated() );
        TESTING_ASSERT( !asamp[4].isYAnimated() );
        TESTING_ASSERT( !asamp[4].isZAnimated() );
        TESTING_ASSERT( asamp[4].isAngleAnimated() );

        TESTING_ASSERT( !asamp[5].isXAnimated() );
        TESTING_ASSERT( asamp[5].isYAnimated() );
        TESTING_ASSERT( asamp[5].isZAnimated() );

        // OK, now check the values came through
        M44d shearmat;
        shearmat.makeIdentity();

        TESTING_ASSERT( asamp[0].getScale() == V3d( 2.0, 1.0, 2.0 ) );

        TESTING_ASSERT( asamp[1].getMatrix() == shearmat );

        TESTING_ASSERT( asamp[2].getAxis() == V3d( 1.0, 0.0, 0.0 ) );
        TESTING_ASSERT( almostEqual( asamp[2].getAngle(), 1.57 ) );

        TESTING_ASSERT( asamp[3].getAxis() == V3d( 0.0, 1.0, 0.0 ) );
        TESTING_ASSERT( almostEqual( asamp[3].getAngle(), 0.125 ) );

        TESTING_ASSERT( asamp[4].getAxis() == V3d( 0.0, 0.0, 1.0 ) );
        TESTING_ASSERT( almostEqual( asamp[4].getAngle(), 0.1 ) );

        TESTING_ASSERT( asamp[5].getTranslate() == V3d( 0.0, 0.0, 0.0 ) );

        TESTING_ASSERT( bnds.getValue() == Box3d( V3d( -0.1, -0.1, -0.1 ),
                                                  V3d(  0.1,  0.1,  0.1 ) ) );

        for ( index_t i = 1; i < 5 ; ++i )
        {
            a.getSchema().get( asamp, ISampleSelector( i ) );

            TESTING_ASSERT( asamp[0].getScale()
                            == V3d( 2 * ( i + 1 ), 1.0, 2.0 ) );

            shearmat.x[1][0] = (double)i;
            shearmat.x[2][0] = (double)( (int)i * -1.0 );
            shearmat.x[2][1] = 0.0;

            TESTING_ASSERT( asamp[1].getMatrix() == shearmat );

            TESTING_ASSERT( asamp[2].getAxis() == V3d( 1.0, 0.0, 0.0 ) );
            TESTING_ASSERT( almostEqual( asamp[2].getAngle(), 1.57 ) );

            TESTING_ASSERT( asamp[3].getAxis() == V3d( 0.0, 1.0, 0.0 ) );
            TESTING_ASSERT( almostEqual( asamp[3].getAngle(),
                                         0.125 * ( i + 1 ) ) );

            TESTING_ASSERT( asamp[4].getAxis() == V3d( 0.0, 0.0, 1.0 ) );
            TESTING_ASSERT( almostEqual( asamp[4].getAngle(),
                                         0.1 * ( i + 1 ) ) );

            V3d tvec( 0.0, 3.0 * i, 4.0 * i );

            TESTING_ASSERT( tvec.equalWithAbsError( asamp[5].getTranslate(),
                                                    VAL_EPSILON ) );
            Box3d b = bnds.getValue( ISampleSelector( i ) );
            double iVal = static_cast< double >( i );
            TESTING_ASSERT( b == Box3d( V3d( -iVal, -iVal, -iVal ),
                                        V3d(  iVal,  iVal,  iVal ) ) );
        }

        std::cout << "tested all xforms in " << name << std::endl;
    }
}

//-*****************************************************************************
void sparseTest()
{
    XformOp transOp( kTranslateOperation, kTranslateHint );
    XformOp scaleOp( kScaleOperation, kScaleHint );

    std::string nameA = "sparseXformTestA.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), nameA );

        OXform transStatic( OObject( archive ),  "transStatic" );
        XformSample asamp;
        asamp.addOp( scaleOp, V3d( 2.0, 1.0, 2.0 ) );
        transStatic.getSchema().set( asamp );

        OXform transAnim( transStatic, "transAnim" );
        XformSample bsamp;
        bsamp.addOp( transOp, V3d( 3.0, 4.0, 5.0 ) );
        transAnim.getSchema().set( bsamp );
        bsamp[0].setTranslate( V3d( 4.0, 5.0, 6.0 ) );
        transAnim.getSchema().set( bsamp );

        OXform identA( transAnim, "ident" );
        OXform identB( identA, "ident" );

        OXform transStatic2( identB, "transStatic" );
        transStatic2.getSchema().set( asamp );

        OXform transAnim2( transStatic2, "transAnim" );
        transAnim2.getSchema().set( bsamp );
        bsamp[0].setTranslate( V3d( 5.0, 6.0, 7.0 ) );
        transAnim.getSchema().set( bsamp );

    }

    std::string nameB = "sparseXformTestB.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), nameB );

        // set this now as animated translate
        OXform transStatic( OObject( archive ),  "transStatic", kSparse );
        XformSample asamp;
        asamp.addOp( transOp, V3d( 1.0, 1.0, 1.0) );
        transStatic.getSchema().set( asamp );
        asamp[0].setTranslate( V3d( 2.0, 2.0, 2.0 ) );
        transStatic.getSchema().set( asamp );

        // this one will be static
        OXform transAnim( transStatic, "transAnim", kSparse );
        XformSample bsamp;
        bsamp.addOp( scaleOp, V3d( 0.5, 0.5, 0.5 ) );
        transAnim.getSchema().set( bsamp );

        // from identity to static
        OXform identA( transAnim, "ident", kSparse );
        identA.getSchema().set( bsamp );

        // from identity to animated
        OXform identB( identA, "ident", kSparse );
        identB.getSchema().set( asamp );
        asamp[0].setTranslate( V3d( 3.0, 3.0, 3.0 ) );
        identB.getSchema().set( asamp );

        // don't set anything on these so they will be identity
        OXform transStatic2( identB, "transStatic", kSparse );
        OXform transAnim2( transStatic2, "transAnim", kSparse );
    }

    {
        std::vector< std::string > names;
        names.push_back( nameB );
        names.push_back( nameA );
        Alembic::AbcCoreFactory::IFactory factory;
        IArchive archive = factory.getArchive( names );

        IXform transStatic( IObject( archive ), "transStatic" );
        TESTING_ASSERT( !transStatic.getSchema().isConstantIdentity() );
        TESTING_ASSERT( !transStatic.getSchema().isConstant() );

        IXform transAnim( transStatic, "transAnim" );
        TESTING_ASSERT( !transAnim.getSchema().isConstantIdentity() );
        TESTING_ASSERT( transAnim.getSchema().isConstant() );

        IXform identA( transAnim, "ident" );
        TESTING_ASSERT( !identA.getSchema().isConstantIdentity() );
        TESTING_ASSERT( identA.getSchema().isConstant() );

        IXform identB( identA, "ident");
        TESTING_ASSERT( !identB.getSchema().isConstantIdentity() );
        TESTING_ASSERT( !identB.getSchema().isConstant() );

        IXform transStatic2( identB, "transStatic" );
        TESTING_ASSERT( transStatic2.getSchema().isConstantIdentity() );

        IXform transAnim2( transStatic2, "transAnim" );
        TESTING_ASSERT( transAnim2.getSchema().isConstantIdentity() );
    }
}

//-*****************************************************************************
void sparseTest2()
{
    XformOp scaleOp( kScaleOperation, kScaleHint );

    std::string nameA = "sparseXformTest2A.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), nameA );

        OXform axform( OObject( archive ),  "a" );
        OXform bxform( axform, "b" );
        OXform cxform( bxform, "c" );
    }

    std::string nameB = "sparseXformTest2B.abc";
    {
        OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), nameB );
        OObject axform( OObject( archive ),  "a" );
        OObject bxform( axform, "b" );
        OXform cxform( bxform, "c" );
        XformSample csamp;
        csamp.addOp( scaleOp, V3d( 0.5, 0.5, 0.5 ) );
        cxform.getSchema().set( csamp );
    }

    {
        std::vector< std::string > names;
        names.push_back( nameB );
        names.push_back( nameA );
        Alembic::AbcCoreFactory::IFactory factory;
        IArchive archive = factory.getArchive( names );

        IObject aobj( IObject( archive ), "a" );
        TESTING_ASSERT( IXform::matches( aobj.getHeader() ) );
        IXform axform( IObject( archive ), "a" );
        TESTING_ASSERT( axform.getSchema().isConstantIdentity() );

        IObject bobj( aobj, "b" );
        TESTING_ASSERT( IXform::matches( bobj.getHeader() ) );
        IXform bxform( aobj, "b" );
        TESTING_ASSERT( bxform.getSchema().isConstantIdentity() );

        IObject cobj( bobj, "c" );
        TESTING_ASSERT( IXform::matches( cobj.getHeader() ) );
        IXform cxform( bobj, "c" );
        TESTING_ASSERT( !cxform.getSchema().isConstantIdentity() );
    }
}

//-*****************************************************************************
void issue188()
{
    std::string name = "issue188.abc";

    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(), name);
    OXform axform( OObject( archive ),  "a" );

    Alembic::AbcGeom::OXformSchema schema0, schema1;
    schema0 = axform.getSchema();
    schema1 = schema0;

    Alembic::Abc::OCompoundProperty user0 = schema0.getUserProperties();
    Alembic::Abc::OCompoundProperty user1 = schema1.getUserProperties();

    Alembic::Abc::OBox3dProperty boxy( user0, "boxy" );
    TESTING_ASSERT( user0.getNumProperties() == 1 );
    TESTING_ASSERT( user1.getNumProperties() == 1 );
}

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    xformOut();
    xformIn();
    someOpsXform();
    xformTreeCreate();
    sparseTest();
    sparseTest2();
    issue188();
    return 0;
}
