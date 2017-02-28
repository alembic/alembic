//-*****************************************************************************
//
// Copyright (c) 2016,
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
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreLayer/Util.h>
#include <Alembic/AbcGeom/All.h>

// util which compares the property headers and returns if they are the same
bool headerCmp(const Alembic::Abc::PropertyHeader * iHeaderA,
               const Alembic::Abc::PropertyHeader * iHeaderB)
{
    // we've already done the name check
    if (iHeaderA->getPropertyType() != iHeaderB->getPropertyType() &&
        iHeaderA->getMetaData().serialize() != iHeaderB->getMetaData().serialize())
    {
        return false;
    }

    // nothing more to check for compounds
    if (iHeaderA->isCompound())
    {
        return true;
    }

    return (iHeaderA->getDataType() == iHeaderB->getDataType()) &&
        (*(iHeaderA->getTimeSampling()) == *(iHeaderB->getTimeSampling()));
}

// util used in verbose mode to get the full property name
void fillFullPropName(Alembic::Abc::ICompoundProperty iProp,
                      std::string & ioName)
{
    while(iProp.getParent().valid())
    {
        if (ioName.empty())
        {
            ioName = iProp.getName();
        }
        else
        {
            ioName = iProp.getName() + "/" + ioName;
        }
        iProp = iProp.getParent();
    }
}

// copies the entire array property to iParent
void copyArrayProp(Alembic::Abc::IArrayProperty & iProp,
    Alembic::Abc::OCompoundProperty & iParent, bool iVerbose)
{
    if (iVerbose)
    {
        std::string propName = iProp.getName();
        fillFullPropName(iProp.getParent(), propName);
        printf ("%s array prop diff on %s\n",
            iProp.getObject().getFullName().c_str(),
            propName.c_str());
    }

    Alembic::Abc::OArrayProperty outProp(iParent, iProp.getName(),
        iProp.getDataType(), iProp.getMetaData(), iProp.getTimeSampling());

    std::size_t numSamples = iProp.getNumSamples();

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        Alembic::Abc::ArraySamplePtr samp;
        Alembic::Abc::ISampleSelector sel((Alembic::Abc::index_t) i);
        iProp.get(samp, sel);
        outProp.set(*samp);
    }
}

// copies the entire static array property to iParent
void copyStaticProp(Alembic::Abc::IScalarProperty & iProp,
    Alembic::Abc::OCompoundProperty & iParent, bool iVerbose)
{

    if (iVerbose)
    {
        std::string propName = iProp.getName();
        fillFullPropName(iProp.getParent(), propName);
        printf ("%s scalar prop diff on %s\n",
            iProp.getObject().getFullName().c_str(),
            propName.c_str());
    }

    Alembic::Abc::OScalarProperty outProp(iParent, iProp.getName(),
        iProp.getDataType(), iProp.getMetaData(), iProp.getTimeSampling());

    std::size_t numSamples = iProp.getNumSamples();
    std::vector<std::string> sampStrVec;
    std::vector<std::wstring> sampWStrVec;
    Alembic::Util::PlainOldDataType ptype = iProp.getDataType().getPod();

    if (ptype == Alembic::Util::kStringPOD)
    {
        sampStrVec.resize(iProp.getDataType().getExtent());
    }
    else if (ptype == Alembic::Util::kWstringPOD)
    {
        sampWStrVec.resize(iProp.getDataType().getExtent());
    }

    char samp[4096];

    for (std::size_t i = 0; i < numSamples; ++i)
    {
        Alembic::Abc::ISampleSelector sel((Alembic::Abc::index_t) i);

        if (ptype == Alembic::Util::kStringPOD)
        {
            iProp.get(&sampStrVec.front(), sel);
            outProp.set(&sampStrVec.front());
        }
        else if (ptype == Alembic::Abc::kWstringPOD)
        {
            iProp.get(&sampWStrVec.front(), sel);
            outProp.set(&sampWStrVec.front());
        }
        else
        {
            iProp.get(samp, sel);
            outProp.set(samp);
        }
    }
}

// copies all of the compound hierachy to iWrite
void copyProps(Alembic::Abc::ICompoundProperty & iRead,
    Alembic::Abc::OCompoundProperty & iWrite, bool iVerbose)
{
    if (iVerbose)
    {
        std::string propName;
        fillFullPropName(iRead, propName);
        printf ("%s compound prop diff on %s\n",
            iRead.getObject().getFullName().c_str(),
            propName.c_str());
    }

    std::size_t numChildren = iRead.getNumProperties();
    for (std::size_t i = 0; i < numChildren; ++i)
    {
        Alembic::Abc::PropertyHeader header = iRead.getPropertyHeader(i);
        if (header.isArray())
        {
            Alembic::Abc::IArrayProperty inProp(iRead, header.getName());
            copyArrayProp(inProp, iWrite, false);
        }
        else if (header.isScalar())
        {
            Alembic::Abc::IScalarProperty inProp(iRead, header.getName());
            copyStaticProp(inProp, iWrite, false);
        }
        else if (header.isCompound())
        {
            Alembic::Abc::OCompoundProperty outProp(iWrite,
                header.getName(), header.getMetaData());
            Alembic::Abc::ICompoundProperty inProp(iRead, header.getName());
            copyProps(inProp, outProp, false);
        }
    }
}

// copies object hierarchy and its property
void copyObject(Alembic::Abc::IObject & iIn, Alembic::Abc::OObject & iOut)
{
    std::size_t numChildren = iIn.getNumChildren();

    Alembic::Abc::ICompoundProperty inProps = iIn.getProperties();
    Alembic::Abc::OCompoundProperty outProps = iOut.getProperties();
    copyProps(inProps, outProps, false);

    for (std::size_t i = 0; i < numChildren; ++i)
    {
        Alembic::Abc::IObject childIn(iIn.getChild(i));
        Alembic::Abc::OObject childOut(iOut, childIn.getName(),
                                       childIn.getMetaData());
        copyObject(childIn, childOut);
    }
}

// class which walks the hierarchy, writes out hiearchy and properties
// that are different in iInFileB from iInFileA, and prunes hierarchy and
// properties which are in iInFileA but not iInFileB.
class DiffWalker
{
public:
    DiffWalker(const char * iInFileA, const char * iInFileB,
               const char * iOutFile, bool iVerbose)
    {
        m_verbose = iVerbose;
        m_inFileA = iInFileA;
        m_inFileB = iInFileB;
        m_outFile = iOutFile;
    }

    int walk()
    {

        Alembic::AbcCoreFactory::IFactory factory;
        Alembic::AbcCoreFactory::IFactory::CoreType coreType;

        Alembic::Abc::IArchive arc1 = factory.getArchive(m_inFileA, coreType);
        if (coreType != Alembic::AbcCoreFactory::IFactory::kOgawa)
        {
            printf("Error: %s is not a valid Alembic Ogawa file.\n",
                   m_inFileA.c_str());
            return 1;
        }

        Alembic::Abc::IArchive arc2 = factory.getArchive(m_inFileB, coreType);
        if (coreType != Alembic::AbcCoreFactory::IFactory::kOgawa)
        {
            printf("Error: %s is not a valid Alembic Ogawa file.\n",
                   m_inFileB.c_str());
            return 1;
        }

        Alembic::Abc::IObject topA = arc1.getTop();
        Alembic::Abc::IObject topB = arc2.getTop();
        walk(topA, topB);
        if (m_outStack.empty())
        {
            printf("No differences detected, %s was not written.\n",
                   m_outFile.c_str());
        }
        return 0;
    }

private:

    void walkProps(Alembic::Abc::ICompoundProperty & iPropA,
                   Alembic::Abc::ICompoundProperty & iPropB,
                   Alembic::Abc::OCompoundProperty & oProp)
    {
        for (size_t i = 0; i < iPropA.getNumProperties(); ++i)
        {
            Alembic::Abc::PropertyHeader childAHeader =
                iPropA.getPropertyHeader(i);
            const Alembic::Abc::PropertyHeader * childBHeader =
                iPropB.getPropertyHeader(childAHeader.getName());

            // prune this property
            if (!childBHeader)
            {
                if (m_verbose)
                {
                    std::string propName = childAHeader.getName();
                    fillFullPropName(iPropA, propName);
                    printf("%s pruning prop: %s.\n",
                           iPropA.getObject().getFullName().c_str(),
                           propName.c_str());
                }
                Alembic::Abc::MetaData md;
                Alembic::AbcCoreLayer::SetPrune(md, true);
                Alembic::Abc::OCompoundProperty pruneProp(oProp,
                    childAHeader.getName(), md);
                continue;
            }

            if (!headerCmp(&childAHeader, childBHeader))
            {
                if (childBHeader->isArray())
                {
                    Alembic::Abc::IArrayProperty inProp(iPropB,
                        childBHeader->getName());
                    copyArrayProp(inProp, oProp, m_verbose);
                }
                else if (childBHeader->isScalar())
                {
                    Alembic::Abc::IScalarProperty inProp(iPropB,
                        childBHeader->getName());
                    copyStaticProp(inProp, oProp, m_verbose);
                }
                else if (childBHeader->isCompound())
                {
                    Alembic::Abc::OCompoundProperty outProp(oProp,
                        childBHeader->getName(), childBHeader->getMetaData());
                    Alembic::Abc::ICompoundProperty inProp(iPropB,
                        childBHeader->getName());
                    copyProps(inProp, outProp, m_verbose);
                }

                continue;
            } // if childAHeader != *childBHeader


            // headers are equal do more checks
            if (childAHeader.isCompound())
            {
                Alembic::Abc::ICompoundProperty childA(iPropA,
                    childAHeader.getName());
                Alembic::Abc::ICompoundProperty childB(iPropB,
                    childAHeader.getName());

                // dont copy metadata here, since it was the same
                Alembic::Abc::OCompoundProperty outProp(oProp,
                    childAHeader.getName());

                walkProps(childA, childB, outProp);
            }
            else if (childAHeader.isScalar())
            {
                Alembic::Abc::IScalarProperty childA(iPropA,
                    childAHeader.getName());
                Alembic::Abc::IScalarProperty childB(iPropB,
                    childAHeader.getName());
                if (childA.getNumSamples() != childB.getNumSamples())
                {
                    copyStaticProp(childB, oProp, m_verbose);
                    continue;
                }

                std::vector<std::string> sampStrVecA, sampStrVecB;
                std::vector<std::wstring> sampWStrVecA, sampWStrVecB;
                char sampA[4096];
                char sampB[4096];
                std::size_t numBytes = 0;

                Alembic::Util::PlainOldDataType ptype =
                    childB.getDataType().getPod();

                if (ptype == Alembic::Util::kStringPOD)
                {
                    sampStrVecA.resize(childB.getDataType().getExtent());
                    sampStrVecB.resize(childB.getDataType().getExtent());
                }
                else if (ptype == Alembic::Util::kWstringPOD)
                {
                    sampWStrVecA.resize(childB.getDataType().getExtent());
                    sampWStrVecB.resize(childB.getDataType().getExtent());
                }
                else
                {
                    memset(sampA, 0, 4096);
                    memset(sampB, 0, 4096);
                    numBytes = childB.getDataType().getNumBytes();
                }

                Alembic::Abc::index_t j;
                Alembic::Abc::index_t numSamples = childB.getNumSamples();
                for (j = 0; j < numSamples; ++j)
                {
                    if (ptype == Alembic::Util::kStringPOD)
                    {
                        childA.get(&sampStrVecA.front(), j);
                        childB.get(&sampStrVecB.front(), j);
                        if (sampStrVecA != sampStrVecB)
                        {
                            copyStaticProp(childB, oProp, m_verbose);
                            break;
                        }
                    }
                    else if (ptype == Alembic::Util::kWstringPOD)
                    {
                        childA.get(&sampWStrVecA.front(), j);
                        childB.get(&sampWStrVecB.front(), j);
                        if (sampWStrVecA != sampWStrVecB)
                        {
                            copyStaticProp(childB, oProp, m_verbose);
                            break;
                        }
                    }
                    else
                    {
                        childA.get(sampA, j);
                        childB.get(sampB, j);

                        if (memcmp(sampA, sampB, numBytes) != 0)
                        {
                            copyStaticProp(childB, oProp, m_verbose);
                            break;
                        }
                    }
                }
            }
            else if (childAHeader.isArray())
            {
                Alembic::Abc::IArrayProperty childA(iPropA,
                    childAHeader.getName());
                Alembic::Abc::IArrayProperty childB(iPropB,
                    childAHeader.getName());

                if (childA.getNumSamples() != childB.getNumSamples())
                {
                    copyArrayProp(childB, oProp, m_verbose);
                    continue;
                }

                Alembic::Abc::index_t j;
                Alembic::Abc::index_t numSamples = childB.getNumSamples();
                for (j = 0; j < numSamples; ++j)
                {
                    Alembic::Abc::ArraySampleKey keyA, keyB;
                    childA.getKey(keyA, j);
                    childB.getKey(keyB, j);
                    if (keyA != keyB)
                    {
                        copyArrayProp(childB, oProp, m_verbose);
                        break;
                    }
                }
            }
        }
    }

    void walkProps(Alembic::Abc::ICompoundProperty & iPropA,
                   Alembic::Abc::ICompoundProperty & iPropB)
    {
        if (Alembic::AbcGeom::IXform::matches(iPropB.getMetaData()))
        {
            fillStack(iPropB.getObject().getParent().getFullName());
            Alembic::Abc::MetaData md = iPropB.getMetaData();
            Alembic::AbcCoreLayer::SetReplace(md, true);
            m_outStack.push_back(Alembic::Abc::OObject(m_outStack.back(),
                iPropB.getObject().getName(), md));
            Alembic::Abc::OCompoundProperty oprop =
                m_outStack.back().getProperties();
            copyProps(iPropB, oprop, m_verbose);

            // This does not handle arbGeomParam and user props that are in
            // iPropA but not iPropB (normally would prune)
            return;
        }
        else if (iPropA.getMetaData().serialize() !=
                 iPropB.getMetaData().serialize() &&
                 iPropB.getObject().getParent().valid())
        {
            fillStack(iPropB.getObject().getParent().getFullName());
            m_outStack.push_back(Alembic::Abc::OObject(m_outStack.back(),
                iPropB.getObject().getName(), iPropB.getMetaData()));
        }
        else
        {
            fillStack(iPropB.getObject().getFullName());
        }

        Alembic::Abc::OCompoundProperty prop =
            m_outStack.back().getProperties();
        walkProps(iPropA, iPropB, prop);
    }

    void walk(Alembic::Abc::IObject & iObjA, Alembic::Abc::IObject & iObjB)
    {

        Alembic::Util::Digest hashPropA, hashPropB;
        iObjA.getPropertiesHash(hashPropA);
        iObjB.getPropertiesHash(hashPropB);

        // lets check our properties
        if (hashPropA != hashPropB)
        {
            Alembic::Abc::ICompoundProperty propA = iObjA.getProperties();
            Alembic::Abc::ICompoundProperty propB = iObjB.getProperties();
            walkProps(propA, propB);
        }

        Alembic::Util::Digest hashChildrenA, hashChildrenB;
        iObjA.getChildrenHash(hashChildrenA);
        iObjB.getChildrenHash(hashChildrenB);
        if (hashChildrenA == hashChildrenB)
        {
            if (m_outStack.size() > 1 &&
                m_outStack.back().getFullName() == iObjA.getFullName())
            {
                m_outStack.pop_back();
            }

            return;
        }

        for (size_t i = 0; i < iObjA.getNumChildren(); ++i)
        {
            Alembic::Abc::ObjectHeader headerA = iObjA.getChildHeader(i);
            const Alembic::Abc::ObjectHeader * headerB =
                iObjB.getChildHeader(headerA.getName());

            // prune
            if (!headerB)
            {
                fillStack(iObjA.getParent().getFullName());

                Alembic::Abc::MetaData md;
                Alembic::AbcCoreLayer::SetPrune(md, true);
                Alembic::Abc::OObject pruneObj(m_outStack.back(),
                                               headerA.getName(), md);
                if (m_verbose)
                {
                    printf("%s pruned.\n", headerA.getFullName().c_str());
                }
            }
            else
            {
                Alembic::Abc::IObject childA(iObjA, headerA.getName());
                Alembic::Abc::IObject childB(iObjB, headerA.getName());
                walk(childA, childB);
            }
        }

        // check to see if B introduced any new children
        for (size_t i = 0; i < iObjB.getNumChildren(); ++i)
        {
            Alembic::Abc::ObjectHeader headerB = iObjB.getChildHeader(i);
            const Alembic::Abc::ObjectHeader * headerA =
                iObjA.getChildHeader(headerB.getName());
            if (headerA == NULL)
            {
                Alembic::Abc::IObject childB(iObjB, headerB.getName());
                fillStack(iObjB.getParent().getFullName());
                Alembic::Abc::OObject out(m_outStack.back(), childB.getName(),
                    childB.getMetaData());
                if (m_verbose)
                {
                    printf("%s copying hierarchy.\n",
                           childB.getFullName().c_str());
                }
                copyObject(childB, out);
            }
        }

        if (m_outStack.size() > 1 &&
            m_outStack.back().getFullName() == iObjA.getFullName())
        {
            m_outStack.pop_back();
        }

    }

    // fills in m_outStack with empty OObjects based on where we are in the
    // hierarchy
    void fillStack(const std::string & iFullName)
    {
        if (m_outStack.empty())
        {
            std::string appStr = "AbcDiff";
            std::string userDesc = m_inFileA + " " + m_inFileB;
            Alembic::Abc::OArchive arc = Alembic::Abc::CreateArchiveWithInfo(
                Alembic::AbcCoreOgawa::WriteArchive(),
                m_outFile, appStr, userDesc);
            m_outStack.push_back(arc.getTop());
        }

        // and we are done
        if (iFullName.empty() || iFullName == m_outStack.back().getFullName())
        {
            return;
        }

        // only create what we need in the stack
        size_t lastIdx = m_outStack.back().getFullName().size();
        if (lastIdx > 1)
        {
            lastIdx ++;
        }

        size_t curIdx = std::string::npos;
        do
        {
            curIdx = iFullName.find('/', lastIdx);

            m_outStack.push_back(Alembic::Abc::OObject(m_outStack.back(),
                iFullName.substr(lastIdx, curIdx - lastIdx)));
            if (curIdx != std::string::npos)
            {
                lastIdx = curIdx + 1;
            }
        }
        while (curIdx != std::string::npos);
    }

    bool m_verbose;
    std::string m_inFileA;
    std::string m_inFileB;
    std::string m_outFile;
    std::vector<Alembic::Abc::OObject> m_outStack;
};

void displayHelp()
{
    printf("Usage:\n");
    printf("abcdiff [-v] inputFilename1 inputFilename2 outputFilename\n\n");

    printf("Used to compare two Alembic files and write an Alembic file that contains the differences.\n\n");
    printf("inputFilename1 is the \"base\" file. If there a difference in the object hierarchy of inputFilename2, "
            "that different object will be added to the diff, along with a skeleton of its parent hierarchy. If an "
            "object exists in both inputFilename1 and inputFilename2 but the properties are different, an object will "
            "be added to the diff that contains only the differing properties.\n\n");

    printf("Parameters:\n");
    printf("-v\t\tOPTIONAL\tVerbose mode prints more detailed information about the diff process\n");
    printf("inputFilename1\tREQUIRED\tThe first Alembic file to compare\n");
    printf("inputFilename2\tREQUIRED\tThe second Alembic file to compare\n");
    printf("outputFilename\tREQUIRED\tThe filename to write out the Alembic diff file\n");
}

int main(int argc, char *argv[])
{

    if ((argc != 4 && argc != 5)|| (argc == 5 && std::string("-v") != argv[1]))
    {
        displayHelp();
        return 1;
    }

    bool verbose = false;
    // we've already checked for -v above
    if (argc == 5)
    {
        verbose = true;
    }
    DiffWalker dw(argv[argc-3], argv[argc-2], argv[argc-1], verbose);
    dw.walk();
}
