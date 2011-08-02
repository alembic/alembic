//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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
#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>

#include "Assert.h"

namespace Abc = Alembic::Abc;

using namespace Alembic::AbcGeom; // Contains Abc, AbcCoreAbstract

using Alembic::AbcCoreAbstract::chrono_t;
using Alembic::AbcCoreAbstract::index_t;
using Alembic::Util::uint32_t;

//
// The tests in this file are intended to exercise Abc helper
// library functions.
//

void writeNestedCommpoundWithVis(const std::string &archiveName)
{
    const int numChildren = 2;

    // Create an archive for writing. Indicate that we want Alembic to
    //   throw exceptions on errors.
    OArchive archive( Alembic::AbcCoreHDF5::WriteArchive(),
                      archiveName, ErrorHandler::kThrowPolicy );
    OObject archiveTop = archive.getTop();

    const chrono_t dt = 1.0 / 24.0;
    TimeSampling ts(dt, 666.0); // uniform with cycle=dt starting at 666.0
    uint32_t tsidx = archive.addTimeSampling(ts);

    // Create several objects under top called "child_N"
    // child_1 will be set to have hidden visibility.
    for (int ii=0; ii<numChildren; ii++)
    {
        // Create 'numChildren' children, all parented under
        //  the archive
        std::string name = "child_";
        name.append( boost::lexical_cast<std::string>( ii ) );
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
    const int numChildren = archiveTop.getNumChildren();
    ABCA_ASSERT( numChildren == 2, "Wrong number of children (expected 2)");
    std::cout << "The archive has " << numChildren << " children:"
              << std::endl;

    IObject child1;
    IObject otherChild;
    // Iterate through them, print out their names
    for (int ii=0; ii<numChildren; ii++)
    {
        IObject child( archiveTop, archiveTop.getChildHeader(ii).getName() );
        std::cout << "  " << child.getName();

        std::cout << " has " << child.getNumChildren() << " children"
                  << std::endl;

        // Properties
        ICompoundProperty props = child.getProperties();
        int numProperties = props.getNumProperties();

        std::cout << "  ..and " << numProperties << " properties"
                  << std::endl;

        std::vector<std::string> propNames;
        for (int pp=0; pp<numProperties; pp++)
            propNames.push_back( props.getPropertyHeader(pp).getName() );

        for (int jj=0; jj<numProperties; jj++)
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
    ABCA_ASSERT( IsAncestorInvisible (child1SubObject) == false, "object under child1 should eval to being not visible");
    ABCA_ASSERT( IsAncestorInvisible (child1) == false, "child1 should eval to being not visible");
    ABCA_ASSERT( IsAncestorInvisible (otherChild) == true, "other object should eval to being visible");

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

    }
    catch (char * str )
    {
        std::cout << "Exception raised: " << str;
        std::cout << " during *FlatHierarchy tests" << std::endl;
        return 1;
    }

    return 0;
}
