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

#include <Alembic/Tako/Tests/Foundation.h>
#include <Alembic/Tako/Tests/CameraTest.h>
#include <Alembic/Tako/Tests/GenericNodeTest.h>
#include <Alembic/Tako/Tests/HDFNodeTest.h>
#include <Alembic/Tako/Tests/NurbsCurveTest.h>
#include <Alembic/Tako/Tests/NurbsSurfaceTest.h>
#include <Alembic/Tako/Tests/PointPrimitiveTest.h>
#include <Alembic/Tako/Tests/PolyMeshTest.h>
#include <Alembic/Tako/Tests/SubDTest.h>
#include <Alembic/Tako/Tests/TransformTest.h>

//-*****************************************************************************
// #include <fstream>
// #include "cppunit/XmlOutputter.h"
// #include "cppunit/extensions/TestFactoryRegistry.h"
// #include "cppunit/ui/text/TestRunner.h"
//
// #include "H5Epublic.h"
//
// using namespace SPI::CppUnit;
//
// int main( int argc, char *argv[] )
// {
//    // Turn off error reporting from HDF5
//    H5Eset_auto2(H5E_DEFAULT, NULL, NULL);
//
//    TextUi::TestRunner runner;
//
//    runner.addTest(TestFactoryRegistry::getRegistry().makeTest());
//
//    std::ofstream stream("TestResults.xml");
//    XmlOutputter* outputter(new XmlOutputter(&runner.result(), stream));
//
//    outputter->setStyleSheet("test/TestResults.xsl");
//
//    runner.setOutputter(outputter);
//
//    return runner.run() ? 0 : 1;
//}
//-*****************************************************************************

int main( int argc, char *argv[] )
{
    // Turn off error reporting from HDF5
    H5Eset_auto2(H5E_DEFAULT, NULL, NULL);
    
    try
    {
        
        std::cout << "\n\nBEGINNING CAMERA TEST" << std::endl;
        {
            CameraTest cameraTest;
            cameraTest.testCameraSimple();
            cameraTest.testCameraAnimated();
        }

        std::cout << "\n\nBEGINNING GENERIC NODE TEST" << std::endl;
        {
            GenericNodeTest genTest;
            genTest.testGenericNode();
        }

        std::cout << "\n\nBEGINNING HDF NODE TEST" << std::endl;
        {
            HDFNodeTest nodeTest;
            nodeTest.test();
            nodeTest.testSmallVec();
        }
        
        std::cout << "\n\nBEGINNING NURBS CURVE TEST" << std::endl;
        {
            NurbsCurveTest crvTest;
            crvTest.testNurbsCurveSimple();
            crvTest.testNurbsCurveAnimated();
        }

        std::cout << "\n\nBEGINNING NURBS SURFACE TEST" << std::endl;
        {
            NurbsSurfaceTest srfTest;
            srfTest.testNurbsSurfaceSimple();
            srfTest.testNurbsSurfaceAnimated();
        }

        std::cout << "\n\nBEGINNING POINT PRIMITIVE TEST" << std::endl;
        {
            PointPrimitiveTest ptTest;
            ptTest.testPointPrimitiveSimple();
            ptTest.testPointPrimitiveAnimated();
            ptTest.testPointPrimitiveIdsAnimated();
        }

        std::cout << "\n\nBEGINNING POLY MESH TEST" << std::endl;
        {
            PolyMeshTest meshTest;
            meshTest.testPolyMeshSimple();
            meshTest.testPolyMeshAnimated();
        }

        std::cout << "\n\nBEGINNING SUBD TEST" << std::endl;
        {
            SubDTest subdTest;
            subdTest.testSubDSimple();
            subdTest.testSubDAnimated();
        }

        std::cout << "\n\nBEGINNING TRANSFORM TEST" << std::endl;
        {
            TransformTest xformTest;
            xformTest.testTransformOperations();
            xformTest.testTransformWriterSimple();
            xformTest.testTransformWriterAnimated();
            xformTest.testTransformHierarchy();
            xformTest.testDeepHierarchy();
        }

        std::cout << "\n\nTESTS SUCCESSFUL" << std::endl;
    }
    catch ( std::exception &exc )
    {
        std::cerr << "ERROR: EXCEPTION: " << exc.what() << std::endl;
        exit( -1 );
    }
    catch ( ... )
    {
        std::cerr << "ERROR: UNKNOWN EXCEPTION" << std::endl;
        exit( -1 );
    }

    return 0;
}
