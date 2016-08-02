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
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>

#if DEBUG
#define DEBUG_OBJ_NAME(OBJ) std::string OBJ##_Name( OBJ.getName() )
#else
#define DEBUG_OBJ_NAME(OBJ)
#endif //DEBUG

typedef Alembic::AbcCoreFactory::IFactory IFactoryNS;

enum ArgMode
{
    kOptions,
    kInFiles,
    kOutFile,
};

class ArgInfo
{
public:

    ArgInfo()
    {
        diffFileType = IFactoryNS::kOgawa;
    }

    std::vector<std::string>    inFiles;
    std::string                 outFile;
    IFactoryNS::CoreType        diffFileType;
};

class AbcDiffProperty
{
public:
    AbcDiffProperty() : m_prune(false) {}
    AbcDiffProperty(std::string iName) : m_name(iName), m_prune(false) {}

    const std::string&              getName() const { return m_name; }
    size_t                          getNumChildren() const { return m_children.size(); }
    std::vector<AbcDiffProperty>&   getChildren() { return m_children; }
    AbcDiffProperty&                getChild( size_t i ) { return m_children[i]; }
    AbcDiffProperty&                addChild(const std::string &iChildName )
                                    { 
                                        m_children.push_back( AbcDiffProperty(iChildName) );
                                        return m_children.back();
                                    }
    AbcDiffProperty&                addChild( const AbcDiffProperty &iProp )
                                    { 
                                        m_children.push_back( iProp );
                                        return m_children.back();
                                    }
    void                            setPrune( const bool iPrune ) { m_prune = iPrune; }
    bool                            shouldPrune() const { return m_prune; }
    
private:
    std::string                 m_name;
    std::vector<AbcDiffProperty>m_children;
    bool                        m_prune;
};

class AbcDiffObject
{
public:
    AbcDiffObject() 
        : m_useMetaData(true),
        m_prune(false)
    { }

    AbcDiffObject(  const std::string &iName)
        : m_name(iName),
        m_useMetaData(true),
        m_prune(false)
    {
        
    }

    const std::string&              getName() const { return m_name; }
    void                            setName( const std::string &iName ) { m_name = iName; }
    size_t                          getNumProperties() const { return m_properties.getNumChildren(); }
    AbcDiffProperty&                getProperty( size_t i ) { return m_properties.getChild( i ); }
    AbcDiffProperty&                getProperties() { return m_properties; }
    AbcDiffProperty&                addProperty( const std::string &iPropName ) 
                                    { 
                                        m_properties.addChild( iPropName  );
                                        return m_properties.getChildren().back();
                                    }
    size_t                          getNumChildren() const { return m_children.size(); }
    AbcDiffObject&                  getChild( size_t i ) { return m_children[i]; }
    std::vector<AbcDiffObject>&     getChildren() { return m_children; }
    void                            addChild( AbcDiffObject &child ) { m_children.push_back( child ); }
    void                            setIsUsingMetaData( bool iUseMetaData ) { m_useMetaData = iUseMetaData; }
    bool                            useMetaData() const { return m_useMetaData; }
    void                            setPrune( const bool iPrune ) { m_prune = iPrune; }
    bool                            shouldPrune() const { return m_prune; }

private:
    AbcDiffProperty                 m_properties;
    std::vector<AbcDiffObject>      m_children;
    std::string                     m_name;
    bool                            m_useMetaData;
    bool                            m_prune;
};

//Display output beautification
static bool s_verbose = false;
#define INDENT_BUFFER_SIZE 512
static char s_indentBuffer[INDENT_BUFFER_SIZE] = "";
static size_t s_curIndentBufferLen = 0;

void increaseOutputIndent()
{
    if(!s_verbose)
        return;

    s_curIndentBufferLen++;
    if(s_curIndentBufferLen >= INDENT_BUFFER_SIZE)
        s_curIndentBufferLen = INDENT_BUFFER_SIZE - 1;
    memset(s_indentBuffer, ' ', s_curIndentBufferLen);
    s_indentBuffer[s_curIndentBufferLen + 1] = 0;
}
void decreaseOutputIndent()
{
    if(!s_verbose)
        return;

    s_curIndentBufferLen--;
    if(s_curIndentBufferLen < 0)
        s_curIndentBufferLen = 0;

    memset(s_indentBuffer, ' ', s_curIndentBufferLen);
    s_indentBuffer[s_curIndentBufferLen + 1] = 0;
}

char* getIndentBuffer()
{
    static char emptyString[] = "";

    if(s_verbose)
        return s_indentBuffer;
    else
        return emptyString;
}

void addCompoundPropertyHierarchy(Alembic::Abc::ICompoundProperty iParentCompoundProp, AbcDiffProperty &iParentDiffProperty)
{
    Alembic::Abc::BasePropertyReaderPtr baseProp = iParentCompoundProp.getPtr()->getProperty( iParentDiffProperty.getName() );
    const std::string &propName = baseProp->getName();
    AbcDiffProperty diffProperty( propName );

     const Alembic::Abc::PropertyHeader *childPropHeader = 
            iParentCompoundProp.getPtr()->getPropertyHeader( propName );

     if ( childPropHeader->isCompound() )
     {
         Alembic::Abc::ICompoundProperty compoundProp(iParentCompoundProp, propName);

         size_t numProps = compoundProp.getNumProperties();

         for( size_t i = 0; i < numProps; i++ )
         {
             const Alembic::Abc::PropertyHeader& childPropHeader = 
                 compoundProp.getPtr()->getPropertyHeader(i);

             const std::string childPropName = childPropHeader.getName();

             AbcDiffProperty& childDiffProperty = iParentDiffProperty.addChild( childPropName );
             addCompoundPropertyHierarchy( compoundProp, childDiffProperty );
         }
     }
}

bool diffProperties( Alembic::Abc::ICompoundProperty iCompoundProp1, Alembic::Abc::ICompoundProperty iCompoundProp2, AbcDiffObject &iDiffObj, AbcDiffProperty &iDiffProperty, const std::string &iObjName )
{
    size_t numObj1Props = iCompoundProp1.getNumProperties();
    size_t numObj2Props = iCompoundProp2.getNumProperties();
    bool foundDifferences = false;
    
    //Is iCompoundProp2 missing any properties that iCompoundProp1 had?
    for( size_t i = 0; i < numObj1Props; i++ )
    {
        const Alembic::Abc::PropertyHeader &childProp1Header = iCompoundProp1.getPropertyHeader( i );
        const std::string &childProp1Name = childProp1Header.getName();

        const Alembic::Abc::PropertyHeader *childProp2Header = 
            iCompoundProp2.getPtr()->getPropertyHeader( childProp1Name );

        if( !childProp2Header )
        {
            AbcDiffProperty childDiffProperty( childProp1Name );
            childDiffProperty.setPrune(true);
            iDiffProperty.addChild( childDiffProperty);

            foundDifferences = true;
        }
    }

    //Are there any differences in the properties that exist in both iCompoundProp1 and iCompoundProp2?
    for( size_t i = 0; i < numObj2Props; i++ )
    {
        std::vector<std::string> compoundPropsToAdd;
        Alembic::Abc::BasePropertyReaderPtr childProp2 = iCompoundProp2.getPtr()->getProperty( i );
        const std::string &childPropName = childProp2->getName();
        AbcDiffProperty childDiffProperty( childPropName );

        const Alembic::Abc::PropertyHeader *childProp1Header = 
            iCompoundProp1.getPtr()->getPropertyHeader( childPropName );

        const Alembic::Abc::PropertyHeader *childProp2Header = 
            iCompoundProp2.getPtr()->getPropertyHeader( childPropName );

        bool propertiesDiffer = false;

        if( childProp1Header && childProp2Header )
        {
            Alembic::Abc::BasePropertyReaderPtr childProp1 = iCompoundProp1.getPtr()->getProperty( childPropName );

            if( childProp1->getPropertyType() != childProp2->getPropertyType() )
            {
                //These properties have the same name but are completely different types
                propertiesDiffer = true;
            }
            else
            {
                //Compare the property samples to look for any difference

                if (childProp1Header->isArray())
                {
                    Alembic::Abc::IArrayProperty arrayProp1(iCompoundProp1, childPropName);
                    Alembic::Abc::IArrayProperty arrayProp2(iCompoundProp2, childPropName);

                    if( arrayProp1.getNumSamples() != arrayProp2.getNumSamples() )
                    {
                        propertiesDiffer = true;
                    }
                    else
                    {
                        const std::size_t numSamples = arrayProp1.getNumSamples();

                        for (std::size_t j = 0; j < numSamples; ++j)
                        {
                            Alembic::AbcCoreAbstract::ArraySamplePtr samp1, samp2;
                            Alembic::Abc::ISampleSelector sel(
                                (Alembic::Abc::index_t) j);
                            
                            arrayProp1.get(samp1, sel);
                            arrayProp2.get(samp2, sel);

                            if( (samp1->getKey() == samp2->getKey()) == false)
                            {
                                propertiesDiffer = true;
                                break;
                            }
                        }
                    }
                }
                else if (childProp1Header->isScalar())
                {
                    Alembic::Abc::IScalarProperty scalarProp1(iCompoundProp1, childPropName);
                    Alembic::Abc::IScalarProperty scalarProp2(iCompoundProp2, childPropName);
                   
                    if( (scalarProp1.getNumSamples() != scalarProp2.getNumSamples()) ||
                        (childProp1Header->getDataType() != childProp2Header->getDataType()) ||
                        (childProp1Header->getDataType().getPod() != childProp2Header->getDataType().getPod()) ) 
                    {
                        propertiesDiffer = true;
                    }
                    else
                    {
                        static std::vector<std::string> sampStrVec1, sampStrVec2;
                        static std::vector<std::wstring> sampWStrVec1, sampWStrVec2;

                        if (childProp1Header->getDataType().getPod() ==
                            Alembic::AbcCoreAbstract::kStringPOD)
                        {
                            sampStrVec1.resize( std::max(sampStrVec1.size(), (size_t)childProp1Header->getDataType().getExtent()) );
                            sampStrVec2.resize( std::max(sampStrVec2.size(), (size_t)childProp1Header->getDataType().getExtent()) );
                        }
                        else if (childProp1Header->getDataType().getPod() ==
                                    Alembic::AbcCoreAbstract::kWstringPOD)
                        {
                            sampWStrVec1.resize( std::max(sampWStrVec1.size(), (size_t)childProp1Header->getDataType().getExtent()) );
                            sampWStrVec2.resize( std::max(sampWStrVec2.size(), (size_t)childProp1Header->getDataType().getExtent()) );
                        }

                        const std::size_t numSamples = scalarProp1.getNumSamples();

                        static char samp1[4096];
                        static char samp2[4096];

                        for (std::size_t j = 0; j < numSamples; ++j)
                        {
                            bool samplesMatch = true;

                            Alembic::Abc::ISampleSelector sel(
                                (Alembic::Abc::index_t) j);

                            if (childProp1Header->getDataType().getPod() ==
                                Alembic::AbcCoreAbstract::kStringPOD)
                            {
                                scalarProp1.get(&sampStrVec1.front(), sel);
                                scalarProp2.get(&sampStrVec2.front(), sel);
                                samplesMatch = (sampStrVec1 == sampStrVec2);
                            }
                            else if (childProp1Header->getDataType().getPod() ==
                                Alembic::AbcCoreAbstract::kWstringPOD)
                            {
                                scalarProp1.get(&sampWStrVec1.front(), sel);
                                scalarProp2.get(&sampWStrVec2.front(), sel);
                                samplesMatch = (sampWStrVec1 == sampWStrVec1);
                            }
                            else
                            {
                                scalarProp1.get(samp1, sel);
                                scalarProp2.get(samp2, sel);
                                Alembic::Abc::ArraySample arraySample1( samp1, 
                                                                        childProp1Header->getDataType(), 
                                                                        Alembic::Abc::Dimensions(1));
                                Alembic::Abc::ArraySample arraySample2( samp2, 
                                                                        childProp2Header->getDataType(), 
                                                                        Alembic::Abc::Dimensions(1));
                                Alembic::Abc::ArraySample::Key key1 = arraySample1.getKey();
                                Alembic::Abc::ArraySample::Key key2 = arraySample2.getKey();

                                samplesMatch = ( key1 == key2 );
                            }

                            if(!samplesMatch)
                            {
                                propertiesDiffer = true;
                                break;
                            }
                        }
                    }
                }
                else if ( childProp1Header->isCompound() )
                {
                    //Compare all the sub-properties of a compound prop to look for differences

                    Alembic::Abc::ICompoundProperty compoundProp1(iCompoundProp1, childPropName);
                    Alembic::Abc::ICompoundProperty compoundProp2(iCompoundProp2, childPropName);

                    if(diffProperties( compoundProp1, compoundProp2, iDiffObj, childDiffProperty, iObjName ))
                    {
                        propertiesDiffer = true;
                    }
                }
            }
        }
        else
        {
            //This property doesn't exist on the base object
            // so add the property (along with sub-properties if this is a compound property)
            propertiesDiffer = true;
            addCompoundPropertyHierarchy(iCompoundProp2, childDiffProperty);
        }

        if( propertiesDiffer )
        {
            foundDifferences = true;
            iDiffProperty.addChild( childDiffProperty);
        }
    }

    return foundDifferences;
}


void copyChildHierarchyToDiff( Alembic::Abc::IObject &iObj, AbcDiffObject &iDiffObj )
{

    size_t numChildren = iObj.getNumChildren();
    std::vector<AbcDiffObject> &diffChildren = iDiffObj.getChildren();

    diffChildren.resize( numChildren );

    for( size_t i = 0; i < numChildren; i++ )
    {
        Alembic::Abc::IObject child = iObj.getChild( i );
        AbcDiffObject& diffChild = diffChildren[i];

        copyChildHierarchyToDiff( child, diffChild );
    }
}

void copyPropertiesToDiff( Alembic::Abc::IObject &iObj, AbcDiffObject &iDiffObj )
{
    iDiffObj.setName( iObj.getName() );
    iDiffObj.setIsUsingMetaData( true );

    Alembic::Abc::ICompoundProperty objProps = iObj.getProperties();
    AbcDiffProperty& diffObjProperties = iDiffObj.getProperties();

    size_t numProps = objProps.getNumProperties();

    for( size_t i = 0; i < numProps; i++ )
    {
        const Alembic::Abc::PropertyHeader& propHeader = objProps.getPropertyHeader( i );
        
        AbcDiffProperty diffProperty( propHeader.getName() );

        //Get the child properties if this is a compound property
        if( propHeader.getPropertyType() == Alembic::Abc::kCompoundProperty)
        {
            Alembic::Abc::CompoundPropertyReaderPtr objPropPtr = objProps.getPtr();
            Alembic::Abc::CompoundPropertyReaderPtr propPtr = objPropPtr->getCompoundProperty(i);
            size_t numChildProps = propPtr->getNumProperties();

            for( size_t c = 0; c < numChildProps; c++ )
            {
                Alembic::Abc::BasePropertyReaderPtr childPropPtr = propPtr->getProperty( c );
                diffProperty.addChild( childPropPtr->getName() );
            }
        }

        diffObjProperties.addChild( diffProperty );
    }
}

void copyEntireObjectToDiff( Alembic::Abc::IObject &iObj, AbcDiffObject &iDiffObj )
{
    copyPropertiesToDiff( iObj, iDiffObj );
    copyChildHierarchyToDiff( iObj, iDiffObj );
}

bool doesMetaDataMatch( Alembic::Abc::IObject &iObj1, Alembic::Abc::IObject &iObj2 )
{
    Alembic::Abc::MetaData obj1MetaData = iObj1.getMetaData();
    Alembic::Abc::MetaData obj2MetaData = iObj2.getMetaData();

    bool matches = obj1MetaData.matchesExactly(obj2MetaData);

    return matches;
}

bool diffObjects( Alembic::Abc::IObject &iObj1, Alembic::Abc::IObject &iObj2, AbcDiffObject &iDiffObj, bool considerMetaData )
{
    DEBUG_OBJ_NAME( iObj1);
    DEBUG_OBJ_NAME( iObj2);

    bool namesMatch = ( iObj1.getName() == iObj2.getName() );
    if( !namesMatch )
    {
        if( s_verbose )
        {
            printf( "Internal error: Unexpected object diff. Tried to diff \"%s\" and \"%s\"'. Aborting diff.\n", 
                iObj1.getFullName().c_str(), iObj2.getFullName().c_str());

            return false;
        }
    }

    bool foundDifferences = false;

    if( considerMetaData && ( doesMetaDataMatch( iObj1, iObj2 ) == false))
    {
        // Objects have the same name but different metadata (and possibly schema?)
        foundDifferences = true;
        copyPropertiesToDiff(iObj2, iDiffObj);
    }
    else
    {
        foundDifferences = diffProperties( iObj1.getProperties(), iObj2.getProperties(), iDiffObj, iDiffObj.getProperties(), iObj2.getName() );
    }

    //Is iObj2 missing any children that iObj1 had?
    size_t numChildren = iObj1.getNumChildren();

    for( size_t i = 0; i < numChildren; i++ )
    {
         Alembic::Abc::IObject obj1Child = iObj1.getChild( i );
        const Alembic::Abc::ObjectHeader *childHeader = iObj2.getChildHeader( obj1Child.getName() );

        if( !childHeader )
        {
            AbcDiffObject childDiff( obj1Child.getName() );
            childDiff.setPrune( true );
            iDiffObj.addChild( childDiff );
            foundDifferences = true;
        }
    }


    //Are there differences between matching iObj1 and iOb2 children?
    numChildren = iObj2.getNumChildren();

    for( size_t i = 0; i < numChildren; i++ )
    {
        Alembic::Abc::IObject obj2Child = iObj2.getChild( i );
        DEBUG_OBJ_NAME( obj2Child);
        const Alembic::Abc::ObjectHeader *childHeader = iObj1.getChildHeader( obj2Child.getName() );
        AbcDiffObject childDiff(obj2Child.getName());

        bool foundChildDifferences = false;

        if( childHeader )
        {
            //Both hierarchies have this object so continue 
            //down the hierarchy to look for differences
            Alembic::Abc::IObject obj1Child = iObj1.getChild( obj2Child.getName() );
            foundChildDifferences = diffObjects( obj1Child, obj2Child, childDiff, true );
        }
        else
        {
            //This object is missing from the base hierarchy so add it
            copyEntireObjectToDiff( obj2Child, childDiff );
            foundChildDifferences = true;
        }

        if( foundChildDifferences )
        {
            iDiffObj.addChild( childDiff );
            foundDifferences = true;
        }
    }

    return foundDifferences;
}

void writeProperty(Alembic::Abc::ICompoundProperty & iRead,
    Alembic::Abc::OCompoundProperty & iWrite,
    AbcDiffProperty &iDiffProperty)
{
    increaseOutputIndent();

    if( iDiffProperty.shouldPrune() )
    {
        if(s_verbose)
        {
            printf("%sProperty \"%s\" was pruned\n", getIndentBuffer(), iDiffProperty.getName().c_str());
        }

        Alembic::Abc::MetaData metaData;
        metaData.set("prune", "1");

        Alembic::Abc::DataType dataType;
        dataType.setExtent( 1 );
        dataType.setPod(Alembic::Util::kBooleanPOD);

        //TODO: Just writing out a dummy scalar value here. 
        //Do we need to write an accurate array/scalar/compound type to 
        //more accurately reflect what is being pruned?
        Alembic::Abc::OScalarProperty outProp(iWrite, iDiffProperty.getName(),
            dataType, metaData, Alembic::Abc::TimeSamplingPtr());

        return;
    }

    const Alembic::AbcCoreAbstract::PropertyHeader* header =
        iRead.getPropertyHeader( iDiffProperty.getName() );

    if (header->isArray())
    {
        if(s_verbose)
        {
            printf("%sArray property \"%s\"\n", getIndentBuffer(), iDiffProperty.getName().c_str());
        }

        Alembic::Abc::IArrayProperty inProp(iRead, header->getName());
        Alembic::Abc::OArrayProperty outProp(iWrite, header->getName(),
            header->getDataType(), header->getMetaData(),
            header->getTimeSampling());

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
    else if (header->isScalar())
    {
        if(s_verbose)
        {
            printf("%sScalar property \"%s\"\n", getIndentBuffer(), iDiffProperty.getName().c_str());
        }

        Alembic::Abc::IScalarProperty inProp(iRead, header->getName());
        Alembic::Abc::OScalarProperty outProp(iWrite, header->getName(),
            header->getDataType(), header->getMetaData(),
            header->getTimeSampling());

        std::size_t numSamples = inProp.getNumSamples();
        std::vector<std::string> sampStrVec;
        std::vector<std::wstring> sampWStrVec;
        if (header->getDataType().getPod() ==
            Alembic::AbcCoreAbstract::kStringPOD)
        {
            sampStrVec.resize(header->getDataType().getExtent());
        }
        else if (header->getDataType().getPod() ==
                    Alembic::AbcCoreAbstract::kWstringPOD)
        {
            sampWStrVec.resize(header->getDataType().getExtent());
        }

        char samp[4096];

        for (std::size_t j = 0; j < numSamples; ++j)
        {
            Alembic::Abc::ISampleSelector sel(
                (Alembic::Abc::index_t) j);

            if (header->getDataType().getPod() ==
                Alembic::AbcCoreAbstract::kStringPOD)
            {
                inProp.get(&sampStrVec.front(), sel);
                outProp.set(&sampStrVec.front());
            }
            else if (header->getDataType().getPod() ==
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
    else if (header->isCompound() )
    {
        if(s_verbose)
        {
            printf("%sCompound property \"%s\"\n", getIndentBuffer(), iDiffProperty.getName().c_str());
        }

        Alembic::Abc::OCompoundProperty outProp(iWrite,
                header->getName(), header->getMetaData());
        Alembic::Abc::ICompoundProperty inProp(iRead, header->getName());
            
        size_t numChildProps = iDiffProperty.getNumChildren();

        for( size_t c = 0; c < numChildProps; c++ )
        {
            writeProperty(inProp, outProp, iDiffProperty.getChild(c) );
        }
    }

    decreaseOutputIndent();
}

bool writeProperties(Alembic::Abc::IObject iObjectRead, Alembic::Abc::OObject iObjectWrite, AbcDiffObject &iDiffObject)
{
    Alembic::Abc::ICompoundProperty propsRead = iObjectRead.getProperties();
    Alembic::Abc::OCompoundProperty propsWrite = iObjectWrite.getProperties();
    AbcDiffProperty& diffObjProperties = iDiffObject.getProperties();
    size_t numProperties = diffObjProperties.getNumChildren();
    
    for( size_t i = 0; i < numProperties; i++ )
    {
        writeProperty( propsRead, propsWrite, diffObjProperties.getChild(i) );
    }

    return true;
}

bool writeObjectToFile(Alembic::Abc::IObject iObjectIn, Alembic::Abc::OObject iObjectOut, AbcDiffObject &iDiffObject)
{
    increaseOutputIndent();

    if(s_verbose)
    {
        printf("%sObject \"%s\"%s\n", 
            getIndentBuffer(), iDiffObject.getName().c_str(), (iDiffObject.shouldPrune() ? " (pruned)" : ""));
    }
    
    writeProperties(iObjectIn, iObjectOut, iDiffObject);

    size_t numChildren = iDiffObject.getNumChildren();
    for( size_t i = 0; i < numChildren; i++ )
    {
        AbcDiffObject &diffChild = iDiffObject.getChild( i );

        Alembic::Abc::IObject childSource(iObjectIn.getChild( diffChild.getName() ));
        bool prune = diffChild.shouldPrune();

        if( diffChild.useMetaData() || prune)
        {
            Alembic::Abc::MetaData metaData;

            if( diffChild.useMetaData() )
            {
                metaData = childSource.getMetaData();
            }
                
            if( prune )
            {
                metaData.set("prune", "1");
            }

            Alembic::Abc::OObject childDest(iObjectOut, diffChild.getName(), metaData);
            writeObjectToFile(childSource, childDest, diffChild);
        }
        else
        {
            Alembic::Abc::OObject childDest(iObjectOut, diffChild.getName());
            writeObjectToFile(childSource, childDest, diffChild);
        }
    }

    decreaseOutputIndent();

    return true;
}

bool validateTimeSampling(Alembic::Abc::IArchive &iArchive1, Alembic::Abc::IArchive &iArchive2)
{
    if ( iArchive1.getNumTimeSamplings() != iArchive2.getNumTimeSamplings() )
    {
        printf("Error: inputFile1 and inputFile2 have a different number of time samplings"
                " so they are not compatible for diffing"
                " (inputFile1 has %d time samples and inputFile2 has %d time samples)\n",
                iArchive1.getNumTimeSamplings(), iArchive2.getNumTimeSamplings());
        return false;
    }

    uint32_t numTimeSamplings = iArchive1.getNumTimeSamplings();
    for( uint32_t i = 0; i < numTimeSamplings; i++ )
    {
        Alembic::Abc::TimeSamplingPtr timeSampling1 = iArchive1.getTimeSampling( i );
        Alembic::Abc::TimeSamplingPtr timeSampling2 = iArchive2.getTimeSampling( i );

        if( !(*timeSampling1 == *timeSampling2) )
        {
            printf("TimeSampling %d is different between inputFile1 and inputFile2"
                    " so they are not compatible for diffing.\n", i);
            return false;
        }
    }

    return true;
}

void displayHelp()
{
    printf("Usage:\n");
    printf("abcdiff [<DiffFormat>] [-v] inputFilename1 inputFilename2 outputFilename\n\n");

    printf("Used to compare two Alembic files and write an Alembic file that contains the differences.\n\n");
    printf("inputFilename1 is the \"base\" file. If there a difference in the object hierarchy of inputFilename2, "
            "that different object will be added to the diff, along with a skeleton of its parent hierarchy. If an "
            "object exists in both inputFilename1 and inputFilename2 but the properties are different, an object will "
            "be added to the diff that contains only the differing properties.\n\n");

    printf("Parameters:\n");
    printf("<DiffFormat>\tOPTIONAL\tSpecifies the archive format of the resultant diff file. Use either -Ogawa or -HDF5\n"
            "\t\t\t\tDefaults to -Ogawa\n");
    printf("-v\t\tOPTIONAL\tVerbose mode prints more detailed information about the diff process\n");
    printf("inputFilename1\tREQUIRED\tThe first Alembic file to compare\n");
    printf("inputFilename2\tREQUIRED\tThe second Alembic file to compare\n");
    printf("outputFilename\tREQUIRED\tThe filename to write out the Alembic diff file\n");
}

bool parseArgs( int iArgc, char *iArgv[], ArgInfo &oArgInfo, bool &oDoProcessing )
{
    oDoProcessing = true;
    ArgMode argMode = kOptions;

    for( int i = 1; i < iArgc; i++ )
    {
        bool argHandled = true;
        std::string arg = iArgv[i];

        switch( argMode )
        {
            case kOptions:
            {
                if(arg == "-HDF5")
                {
                    oArgInfo.diffFileType = IFactoryNS::kHDF5;
                }
                else if(arg == "-Ogawa")
                {
                    oArgInfo.diffFileType = IFactoryNS::kOgawa;
                }
                else if( (arg == "-help") || (arg == "--help") )
                {
                    displayHelp();
                    oDoProcessing = false;
                    return true;
                }
                else if( (arg == "-v") || (arg == "-verbose"))
                {
                    s_verbose = true;
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
                if( i == (iArgc - 1) )
                {
                    argMode = kOutFile;
                    i--;
                }
                else
                {
                    oArgInfo.inFiles.push_back( arg );
                }
            }
            break;

            case kOutFile:
            {
                if(oArgInfo.outFile == "")
                {
                    oArgInfo.outFile = arg;
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
            printf( "Bad syntax!\n\n");
            displayHelp();
            oDoProcessing = false;
            return false;
        }
    }

    if( (oArgInfo.inFiles.size() != 2) ||
        (oArgInfo.outFile.length() == 0) )
    {
        printf( "Bad syntax!\n\n");
        displayHelp();
        oDoProcessing = false;
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    ArgInfo argInfo;
    bool process = false;

    if ( parseArgs( argc, argv, argInfo, process ) == false)
        return 1;

    if ( process )
    {
        for(size_t i = 0; i < argInfo.inFiles.size(); i++)
        {
            if ( strncmp(argInfo.inFiles[i].c_str(), argInfo.outFile.c_str(), argInfo.outFile.length()) == 0 )
            {
                printf("Error: inputFile%d and outputFile must not be the same!\n", i);
                return 1;
            }
        }

        Alembic::AbcCoreFactory::IFactory factory;
        Alembic::AbcCoreFactory::IFactory::CoreType coreType;

        Alembic::Abc::IArchive archive1 = factory.getArchive(argInfo.inFiles[0], coreType);
        Alembic::Abc::IArchive archive2 = factory.getArchive(argInfo.inFiles[1], coreType);

        if (!archive1.valid())
        {
            printf("Error: Invalid Alembic file specified: %s\n",
                    argInfo.inFiles[0].c_str());
            return 1;
        }

        if (!archive2.valid())
        {
            printf("Error: Invalid Alembic file specified: %s\n",
                    argInfo.inFiles[1].c_str());
            return 1;
        }

        if( !validateTimeSampling(archive1, archive2) )
        {
            return 1;
        }

        Alembic::Abc::IObject top1 = archive1.getTop();
        Alembic::Abc::IObject top2 = archive2.getTop();

        printf("Starting diff...\n");
        AbcDiffObject topDiffObj("ABC");
        bool foundDifferences = diffObjects(top1, top2, topDiffObj, false);
        printf("Diff complete\n");

        if ( foundDifferences )
        {
            printf("Found differences\nWriting diff file \"%s\"\n", argInfo.outFile.c_str());

            Alembic::Abc::OArchive outArchive;
            {
                std::string appWriter = "abcdiff";
                std::string userInfo = "Diff file between: \"";
                userInfo += argInfo.inFiles[0];
                userInfo += "\" and \"";
                userInfo += argInfo.inFiles[1];
                userInfo += "\"";

                if (argInfo.diffFileType == IFactoryNS::kHDF5)
                {
                    outArchive = CreateArchiveWithInfo(Alembic::AbcCoreHDF5::WriteArchive(),
                                    argInfo.outFile, appWriter, userInfo,
                                    Alembic::Abc::ErrorHandler::kThrowPolicy);
                }
                else if (argInfo.diffFileType == IFactoryNS::kOgawa)
                {
                    outArchive = CreateArchiveWithInfo(Alembic::AbcCoreOgawa::WriteArchive(),
                                    argInfo.outFile, appWriter, userInfo,
                                    Alembic::Abc::ErrorHandler::kThrowPolicy);
                }

                // start at 1, we don't need to worry about intrinsic default case
                for (Alembic::Util::uint32_t i = 1; i < archive1.getNumTimeSamplings();
                     ++i)
                {
                    outArchive.addTimeSampling(*archive1.getTimeSampling(i));
                }
            }

            Alembic::Abc::IObject topObjectIn = archive2.getTop();
            Alembic::Abc::OObject topObjectOut = outArchive.getTop();
            writeObjectToFile(topObjectIn, topObjectOut, topDiffObj);

            printf("Finished writing diff file\n");
        }
        else
        {
            printf("Files are identical\n");
        }
    }

    printf("Done!\n");
    return 0;
}
