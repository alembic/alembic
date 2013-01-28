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

#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/Abc/All.h>
#include <Alembic/AbcGeom/Visibility.h>
#include <Alembic/AbcGeom/ArchiveBounds.h>
#include <Alembic/AbcGeom/IGeomParam.h>

#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

namespace Abc = Alembic::Abc;

using namespace Alembic::AbcGeom; // Contains Abc, AbcCoreAbstract

using Alembic::AbcCoreAbstract::chrono_t;
using Alembic::AbcCoreAbstract::index_t;

//
// The tests in this file are intended to exercise Abc helper
// library functions.
//


void writeSimpleProperties(const std::string &archiveName)
{
    const size_t numChildren = 3;

    const size_t numSamples = 5;
    const chrono_t dt = 1.0 / 24.0;

    TimeSampling ts(dt, 666.0); // uniform with cycle=dt starting at 666.0

    // Create an archive for writing. Indicate that we want Alembic to
    //   throw exceptions on errors.
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                      archiveName, ErrorHandler::kThrowPolicy );
    OObject archiveTop = archive.getTop();

    Alembic::Util::uint32_t tsidx = archive.addTimeSampling(ts);

    OObject foochild( archiveTop, "foochild" );

    ODoubleProperty foodub( foochild.getProperties(), "foodub", 0 );

    for ( size_t i = 0 ; i < 10 ; i++ )
    {
        foodub.set( 2.0 );
    }

    for ( size_t ii=0 ; ii < numChildren ; ++ii )
    {
        // Create 'numChildren' children, all parented under
        //  the archive
        std::ostringstream strm;
        strm << "child_" <<ii;
        std::string name = strm.str();
        OObject child( archiveTop, name );
        OCompoundProperty childProps = child.getProperties();

        // Create a scalar property on this child object named 'mass'
        ODoubleProperty mass( childProps,  // owner
                              "mass"); // name

        // Write out the samples
        for ( size_t tt=0 ; tt < numSamples ; ++tt )
        {
            double mm = (1.0 + 0.1*tt); // vary the mass
            mass.set( mm );

        }
        mass.setTimeSampling(tsidx);
    }

    // Done - the archive closes itself
}


template <class PROPERTY_CLASS>
void
printSampleValue( PROPERTY_CLASS& iProp, const ISampleSelector &iSS )
{
    std::cout << iProp.getValue( iSS ) << " ";
}



void readSimpleProperties(const std::string &archiveName)
{
    // Open an existing archive for reading. Indicate that we want
    //   Alembic to throw exceptions on errors.
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                      archiveName, ErrorHandler::kThrowPolicy );
    IObject archiveTop = archive.getTop();

    // Determine the number of (top level) children the archive has
    const size_t numChildren = archiveTop.getNumChildren();
    TESTING_ASSERT( numChildren == 4 );
    std::cout << "The archive has " << numChildren << " children:"
              << std::endl;



    std::cout << "Does the archive-top object have VisibilityProperty? "
              << (bool) (GetVisibilityProperty (archiveTop))
              << std::endl;

    // Iterate through them, print out their names
    for ( size_t ii = 0 ; ii < numChildren ; ++ii )
    {
        IObject child( archiveTop, archiveTop.getChildHeader( ii ).getName() );
        std::cout << "  " << child.getName();

        std::cout << " has " << child.getNumChildren() << " children"
                  << std::endl;

        // Properties
        ICompoundProperty props = child.getProperties();
        size_t numProperties = props.getNumProperties();

        std::cout << "  ..and " << numProperties << " simple properties"
                  << std::endl;

        std::vector<std::string> propNames;
        for ( size_t pp=0 ; pp < numProperties ; ++pp )
        {
            propNames.push_back( props.getPropertyHeader(pp).getName() );
        }

        for ( size_t jj=0 ; jj < numProperties ; ++jj )
        {
            std::cout << "    ..named " << propNames[jj] << std::endl;

            std::cout << "    ..with type: ";
            PropertyType pType = props.getPropertyHeader(jj).getPropertyType();
            if (pType == kCompoundProperty)
            {
                std::cout << "compound" << std::endl;
            }
            else if (pType == kScalarProperty)
            {
                std::cout << "scalar" << std::endl;
            }
            else if (pType == kArrayProperty)
            {
                std::cout << "array" << std::endl;
            }

            DataType dType = props.getPropertyHeader(jj).getDataType();
            std::cout << "    ..with POD-type: ";

            switch (dType.getPod())
            {
                case  kBooleanPOD:
                    std::cout << "boolean" << std::endl;
                    break;

                // Char/UChar
                case kUint8POD:
                    std::cout << "unsigned char" << std::endl;
                    break;
                case kInt8POD:
                    std::cout << "char" << std::endl;
                    break;

                // Short/UShort
                case kUint16POD:
                    std::cout << "short unsigned int" << std::endl;
                    break;
                case kInt16POD:
                    std::cout << "short int" << std::endl;
                    break;

                // Int/UInt
                case kUint32POD:
                    std::cout << "unsigned int" << std::endl;
                    break;
                case kInt32POD:
                    std::cout << "int" << std::endl;
                    break;

                // Long/ULong
                case kUint64POD:
                    std::cout << "unsigned long int" << std::endl;
                    break;
                case kInt64POD:
                    std::cout << "long int" << std::endl;
                    break;

                // Half/Float/Double
                case kFloat16POD:
                    std::cout << "half" << std::endl;
                    break;
                case kFloat32POD:
                    std::cout << "float" << std::endl;
                    break;
                case kFloat64POD:
                    std::cout << "double" << std::endl;
                    break;

                case kStringPOD:
                    std::cout << "string" << std::endl;
                    break;

                case kUnknownPOD:
                default:
                    std::cout << " Unknown! (this is bad)" << std::endl;
            };

            TimeSamplingPtr ts =
                GetCompoundPropertyReaderPtr(props)->
                getScalarProperty( propNames[jj] )->getTimeSampling();

            size_t numSamples = ts->getNumStoredTimes();


            std::cout << "    ..and "
                      << ts->getTimeSamplingType() << std::endl
                      << "    ..and " << numSamples << " samples at times: ";


            if (numSamples > 0)
            {
                std::cout << " ( ";
                for ( size_t ss=0 ; ss < numSamples ; ++ss )
                {
                    std::cout << ts->getSampleTime(ss) << " ";
                }
                std::cout << ")";
            }
            std::cout << std::endl;

            std::cout << "    ..and values: ";
            if (numSamples > 0)
            {
                for ( size_t ss=0 ; ss < numSamples ; ++ss )
                {
                    ISampleSelector iss( (index_t) ss);
                    switch (dType.getPod())
                    {
                        // Boolean
                        case  kBooleanPOD:
                        {
                            IBoolProperty prop( props,  propNames[jj] );
                            TESTING_ASSERT( !IBoolGeomParam::matches(
                                                prop.getHeader() ) );
                            printSampleValue( prop, iss );
                            break;
                        }

                        // Char/UChar
                        case kUint8POD:
                        {
                            IUcharProperty prop( props,  propNames[jj] );
                            TESTING_ASSERT( !IUcharGeomParam::matches(
                                                prop.getHeader() ) );
                            printSampleValue( prop, iss );
                            break;
                        }
                        case kInt8POD:
                        {
                            ICharProperty prop( props,  propNames[jj] );
                            TESTING_ASSERT( !ICharGeomParam::matches(
                                                prop.getHeader() ) );
                            printSampleValue( prop, iss );
                            break;
                        }

                        // Short/UShort
                        case kUint16POD:
                        {
                            IUInt16Property prop( props,  propNames[jj] );
                            TESTING_ASSERT( !IUInt16GeomParam::matches(
                                                prop.getHeader() ) );
                            printSampleValue( prop, iss );
                            break;
                        }
                        case kInt16POD:
                        {
                            IInt16Property prop( props,  propNames[jj] );
                            TESTING_ASSERT( !IInt16GeomParam::matches(
                                                prop.getHeader() ) );
                            printSampleValue( prop, iss );
                            break;
                        }

                        // Int/UInt
                        case kUint32POD:
                        {
                            IUInt32Property prop( props,  propNames[jj] );
                            TESTING_ASSERT( !IUInt32GeomParam::matches(
                                                prop.getHeader() ) );
                            printSampleValue( prop, iss );
                            break;
                        }
                        case kInt32POD:
                        {
                            IInt32Property prop( props,  propNames[jj] );
                            TESTING_ASSERT( !IInt32GeomParam::matches(
                                                prop.getHeader() ) );
                            printSampleValue( prop, iss );
                            break;
                        }

                        // Long/ULong
                        case kUint64POD:
                        {
                            IUInt64Property prop( props,  propNames[jj] );
                            TESTING_ASSERT( !IUInt64GeomParam::matches(
                                                prop.getHeader() ) );
                            printSampleValue( prop, iss );
                            break;
                        }
                        case kInt64POD:
                        {
                            IInt64Property prop( props,  propNames[jj] );
                            TESTING_ASSERT( !IInt64GeomParam::matches(
                                                prop.getHeader() ) );
                            printSampleValue( prop, iss );
                            break;
                        }

                        // Half/Float/Double
                        case kFloat16POD:
                        {
                            // iostream doesn't understand float_16's
                            //printSampleValue( IHalfProperty( props,  propNames[jj] ),
                            //                  iss );
                            IHalfProperty prop( props,  propNames[jj] );
                            TESTING_ASSERT( !IHalfGeomParam::matches(
                                                prop.getHeader() ) );
                            break;
                        }

                        case kFloat32POD:
                        {
                            IFloatProperty prop( props,  propNames[jj] );
                            TESTING_ASSERT( !IFloatGeomParam::matches(
                                                prop.getHeader() ) );
                            printSampleValue( prop, iss );
                            break;
                        }
                        case kFloat64POD:
                        {
                            IDoubleProperty prop( props,  propNames[jj] );
                            TESTING_ASSERT( !IDoubleGeomParam::matches(
                                                prop.getHeader() ) );
                            printSampleValue( prop, iss );
                            break;
                        }

                        case kUnknownPOD:
                        default:
                            std::cout << " Unknown! (this is bad)" << std::endl;
                    };

                }
            }
            std::cout << std::endl;



            std::cout << std::endl; // done parsing property
        }
    }

    // Done - the archive closes itself
}

//////////////////


void writeNestedCommpoundWithVis(const std::string &archiveName)
{
    const size_t numChildren = 2;

    // Create an archive for writing. Indicate that we want Alembic to
    //   throw exceptions on errors.
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                      archiveName, ErrorHandler::kThrowPolicy );
    OObject archiveTop = archive.getTop();

    const chrono_t dt = 1.0 / 24.0;
    TimeSampling ts(dt, 666.0); // uniform with cycle=dt starting at 666.0
    Alembic::Util::uint32_t tsidx = archive.addTimeSampling(ts);

    // Create several objects under top called "child_N"
    // child_1 will be set to have hidden visibility.
    for ( size_t ii=0 ; ii < numChildren ; ++ii )
    {
        // Create 'numChildren' children, all parented under
        //  the archive
        std::ostringstream strm;
        strm << "child_" << ii;
        std::string name = strm.str();
        OObject child( archiveTop, name );

        // Create a compound property on this child object named
        //  'rigid_body' containing 'mass' and 'friction' scalar
        //  properties
        OCompoundProperty props_0 = child.getProperties();

        OCompoundProperty props_1( props_0, "props_1" );
        OCompoundProperty props_2( props_0, "props_2" );
        OCompoundProperty props_3( props_0, "props_3" );

        if (ii == 1)
        {
            OVisibilityProperty visibilityProperty = CreateVisibilityProperty (
                child, tsidx);
            std::cout << " setting child_1 visibility to hidden\n";
            visibilityProperty.set (kVisibilityHidden);

            // Now we'll create an object under this hidden object.
            OObject child1SubObject( child, "nested_object" );
        }
    }

    // Done - the archive closes itself
}



void readNestedCommpoundWithVis(const std::string &archiveName)
{
    // Open an existing archive for reading. Indicate that we want
    //   Alembic to throw exceptions on errors.
    IArchive archive( Alembic::AbcCoreHDF5::ReadArchive(),
                      archiveName, ErrorHandler::kThrowPolicy );
    IObject archiveTop = archive.getTop();

    TESTING_ASSERT_THROW( GetIArchiveBounds( archive ),
                          Alembic::Util::Exception );

    IBox3dProperty boxProp = GetIArchiveBounds( archive,
                                               ErrorHandler::kQuietNoopPolicy );
    TESTING_ASSERT( !boxProp.valid() );

    ICharProperty topVisibility = GetVisibilityProperty (archiveTop);
    std::cout << "Does this object have VisibilityProperty? "
              << (bool) (true == topVisibility)
              << std::endl;
    ABCA_ASSERT( topVisibility == false, "top object should not have a visibility property");

    // Determine the number of (top level) children the archive has
    const size_t numChildren = archiveTop.getNumChildren();
    ABCA_ASSERT( numChildren == 2, "Wrong number of children (expected 2)");
    std::cout << "The archive has " << numChildren << " children:"
              << std::endl;

    IObject child1;
    IObject otherChild;
    // Iterate through them, print out their names
    for ( size_t ii=0; ii <numChildren; ii++)
    {
        IObject child( archiveTop, archiveTop.getChildHeader(ii).getName() );
        std::cout << "  " << child.getName();

        std::cout << " has " << child.getNumChildren() << " children"
                  << std::endl;

        // Properties
        ICompoundProperty props = child.getProperties();
        size_t numProperties = props.getNumProperties();

        std::cout << "  ..and " << numProperties << " properties"
                  << std::endl;

        std::vector<std::string> propNames;
        for ( size_t pp=0 ; pp < numProperties ; ++pp )
        {
            propNames.push_back( props.getPropertyHeader(pp).getName() );
        }

        for ( size_t jj=0; jj<numProperties; ++jj)
        {
            std::cout << "    ..named " << propNames[jj] << std::endl;

            std::cout << "    ..with type: ";
            PropertyType pType = props.getPropertyHeader(jj).getPropertyType();
            if (pType == kCompoundProperty)
            {
                std::cout << "compound" << std::endl;
                ICompoundProperty prop( props,  propNames[jj] );
                std::cout << " Accessing this nested prop for its obj -" << prop.getObject ().getName () << std::endl;
            }
            else if (pType == kScalarProperty)
            {
                std::cout << "scalar" << std::endl;
            }
            else if (pType == kArrayProperty)
            {
                std::cout << "array" << std::endl;
            }
        }
        ICharProperty childVisibility;
        childVisibility = GetVisibilityProperty (child);
        if (ii == 1)
        {
            child1 = child;
            // This is child_1. It should have visibility property
            // and it should be set to hidden
            ABCA_ASSERT( childVisibility == true, "child_1 should have a visibility property");
            ABCA_ASSERT( childVisibility.getValue () == kVisibilityHidden,
                "child_1 visibility should be hidden");
            ABCA_ASSERT( childVisibility.getValue () == GetVisibility (child),
                "child_1 visibility should match call value from GetVisibility ()");
            std::cout << " child_1 visibility correctly set to hidden" << std::endl;
        }
        else
        {
            otherChild = child;
            ABCA_ASSERT( childVisibility == false, "child object should not have a visibility property");
        }
    }
    // Test that IsAncestorInvisible () works
    //
    // child of child1 (which is hidden) means that this child should be
    // hidden too
    IObject child1SubObject( child1, "nested_object");
    ABCA_ASSERT( IsAncestorInvisible (archiveTop) == false, "top root should be visible");
    ABCA_ASSERT( IsAncestorInvisible (child1SubObject) == true, "object under child1 should eval to being not visible");
    ABCA_ASSERT( IsAncestorInvisible (child1) == true, "child1 should eval to being not visible");
    ABCA_ASSERT( IsAncestorInvisible (otherChild) == false, "other object should eval to being visible");

    // Done - the archive closes itself
}


int main( int argc, char *argv[] )
{
    try
    {
        std::cout << "Write and read an archive containing one child, ";
        std::cout << " with a compound property" << std::endl;
        std::cout << " that contains three sub compound properties "
                  << std::endl;
        std::cout << " Will test the visibility property helper functions"
                  << std::endl;

        std::string archiveName("nestedCompounds.abc");
        writeNestedCommpoundWithVis( archiveName );
        readNestedCommpoundWithVis( archiveName );

        std::string archiveName2("simpleHelperProps.abc");
        writeSimpleProperties(archiveName2);
        readSimpleProperties(archiveName2);
    }
    catch (char * str )
    {
        std::cout << "Exception raised: " << str;
        std::cout << " during *FlatHierarchy tests" << std::endl;
        return 1;
    }

    return 0;
}
