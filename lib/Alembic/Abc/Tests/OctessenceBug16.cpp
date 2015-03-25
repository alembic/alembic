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

#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/Abc/All.h>

namespace Abc = Alembic::Abc;
using namespace Abc;

using Alembic::AbcCoreAbstract::chrono_t;
using Alembic::AbcCoreAbstract::index_t;

//
// The tests in this file were originally intended to demonstrate
// Alembic Issue 16  "memcpy error in Alembic::HDF5::Attribute::writeAll"
// revealed by valgrind testing.  We'll test this with Ogawa instead
//

void writeSimpleProperties(const std::string &archiveName)
{
    const Alembic::Util::uint32_t numSamples = 5;
    const chrono_t dt = 1.0 / 24.0;

    TimeSamplingType tst( dt ); // uniform with cycle=dt
    std::vector < chrono_t > timeSamp(1, 666.0);
    TimeSampling ts(tst, timeSamp);

    // Create an archive for writing. Indicate that we want Alembic to
    //   throw exceptions on errors.
    OArchive archive( Alembic::AbcCoreOgawa::WriteArchive(),
                      archiveName, ErrorHandler::kThrowPolicy );

    Alembic::Util::uint32_t tsidx = archive.addTimeSampling(ts);
    OObject archiveTop = archive.getTop();

    // Create a child, parented under  the archive
    std::string name = "child";
    OObject child( archiveTop, name );
    OCompoundProperty childProps = child.getProperties();

    // Create a scalar property on this child object named 'mass'
    ODoubleProperty mass( childProps,  // owner
                          "mass", // name
                          tsidx );

    // Write out the samples
    for (Alembic::Util::uint32_t tt=0; tt<numSamples; tt++)
    {
        double mm = (1.0 + 0.1*tt); // vary the mass
        // either one works. Is one the 'correct' method?
        mass.set( mm );
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
    IArchive archive( Alembic::AbcCoreOgawa::ReadArchive(),
                      archiveName, ErrorHandler::kThrowPolicy );
    IObject archiveTop = archive.getTop();

    // Determine the number of (top level) children the archive has
    const unsigned int numChildren = archiveTop.getNumChildren();
    ABCA_ASSERT( numChildren == 1, "Wrong number of children (expected 1)");
    std::cout << "The archive has " << numChildren << " children:"
              << std::endl;

    // Iterate through them, print out their names
    IObject child( archiveTop, archiveTop.getChildHeader( 0 ).getName() );
    std::cout << "  " << child.getName();

    std::cout << " has " << child.getNumChildren() << " children"
              << std::endl;

    // Properties
    ICompoundProperty props = child.getProperties();
    size_t numProperties = props.getNumProperties();

    std::cout << "  ..and " << numProperties << " simple properties"
              << std::endl;

    std::vector<std::string> propNames;
    for (unsigned int pp=0; pp<numProperties; pp++)
        propNames.push_back( props.getPropertyHeader(pp).getName() );

    for (unsigned int jj=0; jj<numProperties; jj++)
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

        size_t numSamples = GetCompoundPropertyReaderPtr( props )->
            getScalarProperty( propNames[jj] )->getNumSamples();

        std::cout << "    ..with time sampling: ";
        std::cout << "  " << numSamples << " samples";

        if (numSamples > 0)
        {
            std::cout << " ( ";
            for (unsigned int ss=0; ss<numSamples; ss++)
                std::cout << ts->getSampleTime(ss) << " ";
            std::cout << ")";
        }
        std::cout << std::endl;

        std::cout << "    ..and values: ";
        if (numSamples > 0)
        {
            for (unsigned int ss=0; ss<numSamples; ss++)
            {
                ISampleSelector iss( (index_t) ss);
                switch (dType.getPod())
                {
                    // Boolean
                    case  kBooleanPOD:
                    {
                        IBoolProperty prop( props,  propNames[jj] );
                        printSampleValue( prop, iss );
                        break;
                    }

                    // Char/UChar
                    case kUint8POD:
                    {
                        IUcharProperty prop( props,  propNames[jj] );
                        printSampleValue( prop, iss );
                        break;
                    }
                    case kInt8POD:
                    {
                        ICharProperty prop( props,  propNames[jj] );
                        printSampleValue( prop, iss );
                        break;
                    }

                    // Short/UShort
                    case kUint16POD:
                    {
                        IUInt16Property prop( props,  propNames[jj] );
                        printSampleValue( prop, iss );
                        break;
                    }
                    case kInt16POD:
                    {
                        IInt16Property prop( props,  propNames[jj] );
                        printSampleValue( prop, iss );
                        break;
                    }

                    // Int/UInt
                    case kUint32POD:
                    {
                        IUInt32Property prop( props,  propNames[jj] );
                        printSampleValue( prop, iss );
                        break;
                    }
                    case kInt32POD:
                    {
                        IInt32Property prop( props,  propNames[jj] );
                        printSampleValue( prop, iss );
                        break;
                    }

                    // Long/ULong
                    case kUint64POD:
                    {
                        IUInt64Property prop( props,  propNames[jj] );
                        printSampleValue( prop, iss );
                        break;
                    }
                    case kInt64POD:
                    {
                        IInt64Property prop( props,  propNames[jj] );
                        printSampleValue( prop, iss );
                        break;
                    }

                    // Half/Float/Double
                    case kFloat16POD:
                        // iostream doesn't understand float_16's
                        //printSampleValue( IHalfProperty( props,  propNames[jj] ),
                        //                  iss );
                        break;
                    case kFloat32POD:
                    {
                        IFloatProperty prop( props,  propNames[jj] );
                        printSampleValue( prop, iss );
                        break;
                    }
                    case kFloat64POD:
                    {
                        IDoubleProperty prop( props,  propNames[jj] );
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

    // Done - the archive closes itself
}

//////////////////


int main( int argc, char *argv[] )
{
    try
    {
        std::cout << "Write and read a simple archive: ten children, ";
        std::cout << "each with one simple property" << std::endl;

        std::string archiveName("flatHierarchy.abc");
        writeSimpleProperties ( archiveName );
        readSimpleProperties  ( archiveName );
    }
    catch (char * str )
    {
        std::cout << "Exception raised: " << str;
        std::cout << " during *FlatHierarchy tests" << std::endl;
        return 1;
    }

    return 0;
}
