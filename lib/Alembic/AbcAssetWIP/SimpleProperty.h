//-*****************************************************************************
//
// Copyright (c) 2009-2010,
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

class MetaData
{
public:
    const std::string &         getName() const;

    template <class T>
    T                           get( const std::string & iTag ) const;

    template <class T>
    void                        set( const std::string & iTag,
                                     const T & iVal ) const;

};

class SimplePropertyMetaData
{
protected:
    const std::string &         getName() const;
    DataType                    getDataType() const;
    int                         getRank() const;
};

// What does TimeSampling do? TimeSampling is a mapping between
// a sample index and a sample time.
// Sample indices are important, because they define a notion of "next"
// and "previous" sample.
//
// Every property can have different time sampling.

class SimplePropertyReader
{
public:
    const SimplePropertyMetaData &getMetaData( ) const;

    
    

protected:

};

class SimpleSampleReader
{
public:
    // Selects which sample is getting read.
    void select( const SampleSelector &sample );
    const void * getRawData( );
};


//-*****************************************************************************
void inconvenientCoreReadingExample( const std::string & iFileName )
{
    // It is okay to have cache creation be a "global feature" of the
    // library, because cache-management decisions are a part of the user's
    // knowledge and manipulation of the system.
    ReaderCachePtr cache = getGlobalCacheFromSomewhere();

    // It is okay to have error handlers be "global features" of the library,
    // because error-handling policies are part of the user's awareness of the
    // system.
    ErrorHandlerPtr errorHandler = getErrorHandlerFromSomewhere();

    // The "ReadArchive" function is the only explicit link point we have
    // to the whole system.
    ArchiveReaderPtr archive = ReadArchive( iFileName,
                                            cache,
                                            errorHandler );

    // Get the objects
    const CompoundObjectReader & topObjects = archive->getTop();

    // Loop over the children of the top object.
    size_t numObjects = topObjects->getNumObjects();
    for ( size_t objIndex = 0; objIndex < numObjects; ++i )
    {
        // The objects have been loaded, but only really their metadata.
        ObjectReaderPtr childObject = topObjects->getObject( objIndex );

        // The metadata lives only on the properties.
        const CompoundPropertyReader & props = childObject->getProperties();
        
        // The object reads only the child-info struct for each child
        // I assert that metadata is a non-virtual "feature" of the
        // system.
        const ReaderMetaData & objectMetaData = props.getMetaData();

        // Get a tag of the metadata.
        std::string ptag;
        if ( objectMetaData->getTag( "protocol", ptag ) && ptag == "jim" )
        {
            // Loop over the child's properties here. AW YEAH.
            size_t numProps = props.getNumProperties();
            for ( size_t propIdx = 0; propIdx < numProps; ++propIdx )
            {
                if ( (*piter).isSimple() )
                {
                    SimplePropertyReaderPtr sprop = (*piter).getSimple();

                    for ( SimplePropertyReader::iterator siter = sprop->begin();
                          siter != sprop->end(); ++siter )
                    {
                        // We have samples.
                        SampleReaderPtr sample = (*siter);

                        // Samples have indices, time
                        std::cout << "Sample index: " << sample->getIndex()
                                  << std::endl
                                  << "Sample time: " << sample->getTime()
                                  << std::endl
                                  << "Sample dimensions: "
                                  << sample->getDimensions()
                                  << std::endl
                                  << "Sample data: " << 
                    }     
                }
                else
                {
                    CompoundPropertyReaderPtr cprop = (*piter).getCompound();
                }

            }

    }

    
    
    

}
