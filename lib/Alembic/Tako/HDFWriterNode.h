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

#ifndef _Alembic_Tako_HDFWriterNode_h_
#define _Alembic_Tako_HDFWriterNode_h_

#include <Alembic/Tako/Foundation.h>
#include <Alembic/Tako/HDFNode.h>

namespace Alembic {

namespace Tako {

namespace TAKO_LIB_VERSION_NS {

/**
\brief Base class for writing out a node's property map.

The main responsibility of this class is to write out the sampled, and
non-sampled property maps.  These two maps share keys, so a key in one map
is not found in another map.
*/
class HDFWriterNode
{
    public:

        /**
        \brief Constructor which creates the HDF file.
        \param iName The file name.

        Creates an HDF file named iName.
        */
        explicit HDFWriterNode(const char * iName);

        /**
        \brief Constructor which creates an HDF group parented to iParent.
        \param iName The name of the node to create.
        \param iParent The HDF handle which will be the parent of the new node.

        Creates an HDF group named iName whose parent is iParent.
        */
        HDFWriterNode(const char * iName, hid_t iParent);

        /**
        \brief The destructor.

        Closes the HDF group created in the constructor.
        */
        virtual ~HDFWriterNode();

        /**
        \brief Sets a value in the non-sampled property map.
        \param iName The key for the property.
        \param iProp The value for the property.

        Tries to set a key in the non-sampled property map.  If the
        key already exists in the sampled property an error will be thrown.
        Once writeProperty has been called no more properties may be set.
        */
        void setNonSampledProperty(const char * iName,
            const PropertyPair & iProp);

        /**
        \brief Sets a value in the sampled property map.
        \param iName The key for the property.
        \param iProp The value for the property.

        Tries to set a key in the sampled property map.  If the
        key already exists in the non-sampled property an error will be thrown.
        Once writeProperty has been called no more properties may be set,
        instead updateSample to update the value on a sampled property.
        */
        void setSampledProperty(const char * iName, const PropertyPair & iProp);

        /**
        \brief Clears the sampled and non-sampled property maps.

        This function is provided for convenience.  Once writeProperties
        is called clearProperties may not be called, otherwise an error is
        thrown.
        */
        void clearProperties();

        /**
        \brief Writes out the sampled and possibly the non-sampled properties.
        \param iFrame The frame at which the sampled properties correspond to.

        Writes out the sampled properties at the given frame.  The non-sampled
        properties may be written out if they have not been previously written.
        Once this function has been called, the property maps may not be changed
        via the corresponding set function.  updateSample may be called to
        update the the sampled property map before writing a new frame.
        */
        void writeProperties(float iFrame);

        /**
        \brief Convenience function that fills in the value of a property.
        \param iName The name of the property we are trying to get.
        \param oProp If the property exists, it's value will be set in oProp.
        If it does not exist oProp will not be changed.
        \return If the property doesn't exist (or the properties have not
        been read yet) PROPERTY_NONE is returned, if
        it is not sampled PROPERTY_STATIC is returned, if it is animated
        PROPERTY_ANIMATED is returned.

        Looks in non-sampled and then the sampled property map for a
        property named iName.  If no property is found, a default property
        is returned.
        */
        PropertyType getProperty(const char * iName, PropertyPair & oProp);

        /**
        \brief Returns whether or not the property maps have been written.
        \return true if they have been written, otherwise false.

        Returns whether or not writeProperties has been called yet.
        */
        bool propertiesWritten();

        /**
        \brief Updates a value in the sampled property map.
        \param iName The name of the property to update.
        \param iAttr The property value we are updating.

        When updating a sample value, the property must exist, and the
        type on the property must not have changed.
        */
        void updateSample(const char * iName, const PropertyPair & iAttr);

        /**
        \brief Returns an iterator to the beginning of the non-sampled property
        map.
        \return A PropertyMap::const_iterator
        */
        PropertyMap::const_iterator beginNonSampledProperties() const;

        /**
        \brief Returns an iterator to the end of the non-sampled property map.
        \return A PropertyMap::const_iterator
        */
        PropertyMap::const_iterator endNonSampledProperties() const;

        /**
        \brief Returns an iterator to the beginning of the sampled property map.
        \return A PropertyMap::const_iterator
        */
        PropertyMap::const_iterator beginSampledProperties() const;

        /**
        \brief Returns an iterator to the end of the sampled property map.
        \return A PropertyMap::const_iterator
        */
        PropertyMap::const_iterator endSampledProperties() const;

    protected:
        /**
        A handle to the HDF group which is created by the constructor, and
        closed by the destructor.  It is protected since derived classes
        will need to have direct access to it.
        */
        hid_t mGroup;

    private:
        // disallow copying
        HDFWriterNode(const HDFWriterNode &);
        void operator=(const HDFWriterNode &);

        class PrivateData;
        boost::scoped_ptr < PrivateData > mData;
};
}

using namespace TAKO_LIB_VERSION_NS;  // NOLINT
}

} // End namespace Alembic

#endif  // _Alembic_Tako_HDFWriterNode_h_
