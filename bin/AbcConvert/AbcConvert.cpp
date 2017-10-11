//-*****************************************************************************
//
// Copyright (c) 2013,
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

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreFactory/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>

typedef Alembic::AbcCoreFactory::IFactory IFactoryNS;

enum ArgMode
{
    kOptions,
    kInFiles,
    kOutFile,
};

class ConversionOptions
{
public:

    ConversionOptions()
    {
        toType = IFactoryNS::kUnknown;
        force = false;
    }

    std::vector<std::string>    inFiles;
    std::string                 outFile;
    IFactoryNS::CoreType        toType;
    bool                        force;
};

void copyProps(Alembic::Abc::ICompoundProperty & iRead,
    Alembic::Abc::OCompoundProperty & iWrite)
{
    std::size_t numChildren = iRead.getNumProperties();
    for (std::size_t i = 0; i < numChildren; ++i)
    {
        Alembic::AbcCoreAbstract::PropertyHeader header =
            iRead.getPropertyHeader(i);
        if (header.isArray())
        {
            Alembic::Abc::IArrayProperty inProp(iRead, header.getName());
            Alembic::Abc::OArrayProperty outProp(iWrite, header.getName(),
                header.getDataType(), header.getMetaData(),
                header.getTimeSampling());

            std::size_t numSamples = inProp.getNumSamples();

            for (std::size_t j = 0; j < numSamples; ++j)
            {
                Alembic::AbcCoreAbstract::ArraySamplePtr samp;
                Alembic::Abc::ISampleSelector sel(
                    (Alembic::Abc::index_t) j);
                inProp.get(samp, sel);
                outProp.set(*samp);
            }
        }
        else if (header.isScalar())
        {
            Alembic::Abc::IScalarProperty inProp(iRead, header.getName());
            Alembic::Abc::OScalarProperty outProp(iWrite, header.getName(),
                header.getDataType(), header.getMetaData(),
                header.getTimeSampling());

            std::size_t numSamples = inProp.getNumSamples();
            std::vector<std::string> sampStrVec;
            std::vector<std::wstring> sampWStrVec;
            if (header.getDataType().getPod() ==
                Alembic::AbcCoreAbstract::kStringPOD)
            {
                sampStrVec.resize(header.getDataType().getExtent());
            }
            else if (header.getDataType().getPod() ==
                     Alembic::AbcCoreAbstract::kWstringPOD)
            {
                sampWStrVec.resize(header.getDataType().getExtent());
            }

            char samp[4096];

            for (std::size_t j = 0; j < numSamples; ++j)
            {
                Alembic::Abc::ISampleSelector sel(
                    (Alembic::Abc::index_t) j);

                if (header.getDataType().getPod() ==
                    Alembic::AbcCoreAbstract::kStringPOD)
                {
                    inProp.get(&sampStrVec.front(), sel);
                    outProp.set(&sampStrVec.front());
                }
                else if (header.getDataType().getPod() ==
                    Alembic::AbcCoreAbstract::kWstringPOD)
                {
                    inProp.get(&sampWStrVec.front(), sel);
                    outProp.set(&sampWStrVec.front());
                }
                else
                {
                    inProp.get(samp, sel);
                    outProp.set(samp);
                }
            }
        }
        else if (header.isCompound())
        {
            Alembic::Abc::OCompoundProperty outProp(iWrite,
                header.getName(), header.getMetaData());
            Alembic::Abc::ICompoundProperty inProp(iRead, header.getName());
            copyProps(inProp, outProp);
        }
    }
}

void copyObject(Alembic::Abc::IObject & iIn,
    Alembic::Abc::OObject & iOut)
{
    std::size_t numChildren = iIn.getNumChildren();

    Alembic::Abc::ICompoundProperty inProps = iIn.getProperties();
    Alembic::Abc::OCompoundProperty outProps = iOut.getProperties();
    copyProps(inProps, outProps);

    for (std::size_t i = 0; i < numChildren; ++i)
    {
        Alembic::Abc::IObject childIn(iIn.getChild(i));
        Alembic::Abc::OObject childOut(iOut, childIn.getName(),
                                       childIn.getMetaData());
        copyObject(childIn, childOut);
    }
}

void displayHelp()
{
    printf ("Usage (single file conversion):\n");
    printf ("abcconvert [-force] OPTION inFile outFile\n");
    printf ("Usage (convert multiple, layered files to single file):\n");
    printf ("abcconvert OPTION -in inFile1 inFile2 ... -out outFile\n");
    printf ("Used to convert an Alembic file from one type to another.\n\n");
    printf ("If -force is not provided and inFile happens to be the same\n");
    printf ("type as OPTION no conversion will be done and a message will\n");
    printf ("be printed out.\n");
    printf ("OPTION has to be one of these:\n\n");
    printf ("  -toHDF   Convert to HDF.\n");
    printf ("  -toOgawa Convert to Ogawa.\n");
}

bool parseArgs( int iArgc, char *iArgv[], ConversionOptions &oOptions, bool &oDoConversion )
{
    oDoConversion = true;
    ArgMode argMode = kOptions;

    for( int i = 1; i < iArgc; i++ )
    {
        bool argHandled = true;
        std::string arg = iArgv[i];

        switch( argMode )
        {
            case kOptions:
            {
                if(arg == "-toHDF")
                {
                    oOptions.toType = IFactoryNS::kHDF5;
                }
                else if(arg == "-toOgawa")
                {
                    oOptions.toType = IFactoryNS::kOgawa;
                }
                else if(arg == "-force")
                {
                    oOptions.force = true;
                }
                else if(arg == "-in" )
                {
                    argMode = kInFiles;
                }
                else if( (arg == "-help") || (arg == "--help") )
                {
                    displayHelp();
                    oDoConversion = false;
                    return true;
                }
                else if(arg.c_str()[0] == '-')
                {
                    argHandled = false;
                }
                else
                {
                    argMode = kInFiles;
                    i--;
                }
            }
            break;

            case kInFiles:
            {
                if(arg == "-out")
                {
                    argMode = kOutFile;
                }
                else if( i == (iArgc - 1) )
                {
                    argMode = kOutFile;
                    i--;
                }
                else
                {
                    oOptions.inFiles.push_back( arg );
                }
            }
            break;

            case kOutFile:
            {
                if(oOptions.outFile == "")
                {
                    oOptions.outFile = arg;
                }
                else
                {
                    argHandled = false;
                }
            }
            break;
        }

        if( !argHandled )
        {
            displayHelp();
            oDoConversion = false;
            return false;
        }
    }

    if( (oOptions.inFiles.size() == 0) ||
        (oOptions.outFile.length() == 0) ||
        (oOptions.toType == IFactoryNS::kUnknown) )
    {
        printf( "Bad syntax!\n\n");
        displayHelp();
        oDoConversion = false;
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    ConversionOptions options;
    bool doConversion = false;

    if (parseArgs( argc, argv, options, doConversion ) == false)
        return 1;

    if (doConversion)
    {
        for( std::vector<std::string>::const_iterator inFile = options.inFiles.begin(); inFile != options.inFiles.end(); inFile++ )
        {
            if (*inFile == options.outFile)
            {
                printf("Error: inFile and outFile must not be the same!\n");
                return 1;
            }
        }

        if (options.toType != IFactoryNS::kHDF5 && options.toType != IFactoryNS::kOgawa)
        {
            printf("Currently only -toHDF and -toOgawa are supported.\n");
            return 1;
        }

        Alembic::AbcCoreFactory::IFactory factory;
        Alembic::AbcCoreFactory::IFactory::CoreType coreType;

        Alembic::Abc::IArchive archive;
        if(options.inFiles.size() == 1)
        {
            archive = factory.getArchive(*options.inFiles.begin(), coreType);
            if (!archive.valid())
            {
                printf("Error: Invalid Alembic file specified: %s\n",
                       options.inFiles.begin()->c_str());
                return 1;
            }
            else if ( !options.force && (
                (coreType == IFactoryNS::kHDF5 &&
                 options.toType == IFactoryNS::kHDF5) ||
                (coreType == IFactoryNS::kOgawa &&
                 options.toType == IFactoryNS::kOgawa)) )
            {
                printf("Warning: Alembic file specified: %s\n", options.inFiles.begin()->c_str());
                printf("is already of the type you want to convert to.\n");
                printf("Please specify -force if you want to do this anyway.\n");
                return 1;
            }
        }
        else
        {
            archive = factory.getArchive(options.inFiles, coreType);
        }

        Alembic::Abc::IObject inTop = archive.getTop();
        Alembic::Abc::OArchive outArchive;
        if (options.toType == IFactoryNS::kHDF5)
        {
            outArchive = Alembic::Abc::OArchive(
                Alembic::AbcCoreHDF5::WriteArchive(),
                options.outFile, inTop.getMetaData(),
                Alembic::Abc::ErrorHandler::kThrowPolicy);
        }
        else if (options.toType == IFactoryNS::kOgawa)
        {
            outArchive = Alembic::Abc::OArchive(
                Alembic::AbcCoreOgawa::WriteArchive(),
                options.outFile, inTop.getMetaData(),
                Alembic::Abc::ErrorHandler::kThrowPolicy);
        }

        // start at 1, we don't need to worry about intrinsic default case
        for (Alembic::Util::uint32_t i = 1; i < archive.getNumTimeSamplings();
             ++i)
        {
            outArchive.addTimeSampling(*archive.getTimeSampling(i));
        }

        Alembic::Abc::OObject outTop = outArchive.getTop();
        copyObject(inTop, outTop);
    }

    return 0;
}
