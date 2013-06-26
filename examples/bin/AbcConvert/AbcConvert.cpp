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

int main(int argc, char *argv[])
{

    std::string toType;
    std::string inFile;
    std::string outFile;
    std::string forceStr;

    if (argc == 4)
    {
        toType = argv[1];
        inFile = argv[2];
        outFile = argv[3];
    }
    else if (argc == 5)
    {
        forceStr = argv[1];
        toType = argv[2];
        inFile = argv[3];
        outFile = argv[4];
    }

    if ((argc == 4 || argc == 5) && (forceStr.empty() || forceStr == "-force"))
    {
        if (inFile == outFile)
        {
            printf("Error: inFile and outFile must not be the same!\n");
            return 1;
        }

        if (toType != "-toHDF" && toType != "-toOgawa")
        {
            printf("Error: Unknown conversion type specified %s\n",
                   toType.c_str());
            printf("Currently only -toHDF and -toOgawa are supported.\n");
            return 1;
        }

        Alembic::AbcCoreFactory::IFactory factory;
        Alembic::AbcCoreFactory::IFactory::CoreType coreType;
        Alembic::Abc::IArchive archive = factory.getArchive(inFile, coreType);
        if (!archive.valid())
        {
            printf("Error: Invalid Alembic file specified: %s\n",
                   inFile.c_str());
            return 1;
        }
        else if ( forceStr != "-force" && (
            (coreType == Alembic::AbcCoreFactory::IFactory::kHDF5 &&
             toType == "-toHDF") ||
            (coreType == Alembic::AbcCoreFactory::IFactory::kOgawa &&
             toType == "-toOgawa")) )
        {
            printf("Warning: Alembic file specified: %s\n",inFile.c_str());
            printf("is already of the type you want to convert to.\n");
            printf("Please specify -force if you want to do this anyway.\n");
            return 1;
        }

        Alembic::Abc::IObject inTop = archive.getTop();
        Alembic::Abc::OArchive outArchive;
        if (toType == "-toHDF")
        {
            outArchive = Alembic::Abc::OArchive(
                Alembic::AbcCoreHDF5::WriteArchive(),
                outFile, inTop.getMetaData(),
                Alembic::Abc::ErrorHandler::kThrowPolicy);
        }
        else if (toType == "-toOgawa")
        {
            outArchive = Alembic::Abc::OArchive(
                Alembic::AbcCoreOgawa::WriteArchive(),
                outFile, inTop.getMetaData(),
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
        return 0;
    }

    printf ("Usage: abcconvert [-force] OPTION inFile outFile\n");
    printf ("Used to convert an Alembic file from one type to another.\n\n");
    printf ("If -force is not provided and inFile happens to be the same\n");
    printf ("type as OPTION no conversion will be done and a message will\n");
    printf ("be printed out.\n");
    printf ("OPTION has to be one of these:\n\n");
    printf ("  -toHDF   Convert to HDF.\n");
    printf ("  -toOgawa Convert to Ogawa.\n");

    return 1;
}
