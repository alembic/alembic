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

#ifndef _Alembic_TakoSPI_HDFReaderNode_h_
#define _Alembic_TakoSPI_HDFReaderNode_h_

#include <Alembic/TakoSPI/Foundation.h>

namespace Alembic {

namespace TakoSPI {

namespace TAKO_SPI_LIB_VERSION_NS {

/**
\brief HDFReaderNode is the base class for reading the various groupings of
node data in HDF.

The HDFReaderNode encapsulates several HDF groups and data sets.  The purpose
of this class is to provide a property map interface which can be used by
all other derived node types.
*/
class HDFReaderNodeIMPL
{
    public:
    typedef TakoAbstractV1::HDFReaderNode::PropInfo PropInfo;
    typedef TakoAbstractV1::HDFReaderNode::PropNameInfoMap PropNameInfoMap;

        /**
        \brief Constructor which opens an HDF file.
        \param iName The name of the HDF file.

        This constructor opens up an HDF file and serves as the implicit
        root node, "/".
        */
        explicit HDFReaderNodeIMPL(const std::string & iName);

        /**
        \brief Constructor which creates the base HDF group.
        \param iName The name to give the HDF group.
        \param iParent The parent HDF group, or file handle.

        This constructor creates a new HDF group named iName as a child
        of iParent.  If iParent is an HDF file handle, then the child
        will be parented to the implicit root node, "/"
        */
        HDFReaderNodeIMPL(const std::string & iName, hid_t iParent);

        /**
        \brief The destructor.

        The destructor closes the HDF group
        */
        virtual ~HDFReaderNodeIMPL();

        /**
        \brief Reads the properties into the property maps.
        \param iFrame The frame to load for the sampled properties.

        This function reads properties into the non sampled (static) property
        map and reads sampled properties at the specified frame.  The non
        sampled properties are only read once.
        */
        void readProperties(float iFrame);

        /**
        \brief Reads a specific property and returns it's value.
        \param iName The name of the property to load.
        \param iFrame The frame to load for the sampled properties.
        \param oProp This property value will be filled only if the property
        actually exists.
        \return If the property doesn't exist PROPERTY_NONE is returned, if
        it is not sampled PROPERTY_STATIC is returned, if it is animated
        PROPERTY_ANIMATED is returned.

        This method is meant to be used in situations where an application
        only needs to read some specific properties.
        */
        PropertyType readProperty(const std::string & iName,
                                          float iFrame,
                                          PropertyPair & oProp);

        /**
        \brief Fills in the map of names to property info.
        \param oMap The map of std::string names to PropInfo which will be
        filled in.

        This method is meant to provide a hint as to the kind of data is
        stored in the non sampled and sampled property maps without actually
        reading all of the data.  This function can help find large data sets
        that you may not want to read in certain circumstances.
        */
        void getPropertyNamesAndInfo(PropNameInfoMap & oMap);

        /**
        \brief Convenience function that fills in the value of a property.
        \param iName The name of the property we are trying to get.
        \param oProp If the property exists, it's value will be set in oProp.
        If it does not exist oProp will not be changed.
        \return If the property doesn't exist (or the properties have not
        been read yet) PROPERTY_NONE is returned, if
        it is not sampled PROPERTY_STATIC is returned, if it is animated
        PROPERTY_ANIMATED is returned.

        This function should only be called after a call to readProperties as
        it doesnt actually read any data from disk.  Instead it looks for a
        property in the non sampled and sampled property maps.
        */
        PropertyType getProperty(const std::string & iName,
                                         PropertyPair & oProp);

        /**
        \brief Convenience function which empties the non sampled property map.

        Since the non sampled property map doesn't change as the frame changes,
        this function is provided to free up some memory after doing an initial
        call to readProperties.
        */
        void clearNonSampledProperties();

        /**
        \brief Returns a const_iterator to the first non sampled property.
        \return A const_iterator to the first item in the non sampled
        PropertyMap.
        */
        PropertyMap::const_iterator beginNonSampledProperties() const;

        /**
        \brief Returns a const_iterator to the end of the non sampled property
        map.
        \return A const_iterator to the end of the non sampled PropertyMap.
        */
        PropertyMap::const_iterator endNonSampledProperties() const;

        /**
        \brief Returns a const_iterator to the first sampled property.
        \return A const_iterator to the first item in the sampled PropertyMap.
        */
        PropertyMap::const_iterator beginSampledProperties() const;

        /**
        \brief Returns a const_iterator to the end of the sampled property map.
        \return A const_iterator to the end of the sampled PropertyMap.
        */
        PropertyMap::const_iterator endSampledProperties() const;

        /**
        \brief Adds any sampled frames on this node into ioSampled.
        \param ioFrames A set which will have this nodes available sample
        frames added to it.
        */
        void getPropertyFrames(std::set<float> & ioFrames) const;

        /**
        \brief Returns whether or not this node has any sampled properties.
        \return true if this node has at least one sampled property.
        */
        bool hasPropertyFrames() const;

        /**
        \brief Returns the name of this node.
        \return A const char * that represents the name of this node.
        */
        const std::string & getName() const;

    // protected:
    public: // CJH: Had to do this
    
        // derived classes will need to use mGroup, but nobody else should
        // use it directly
        hid_t mGroup;

    private:
        // disallow copying
        HDFReaderNodeIMPL(const HDFReaderNodeIMPL &);
        void operator=(const HDFReaderNodeIMPL &);

        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}

using namespace TAKO_SPI_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_TakoSPI_HDFReaderNode_h_
