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

#ifndef Alembic_AbcCoreOgawa_AprImpl_h
#define Alembic_AbcCoreOgawa_AprImpl_h

#include <Alembic/AbcCoreOgawa/Foundation.h>

namespace Alembic {
namespace AbcCoreOgawa {
namespace ALEMBIC_VERSION_NS {

//-*****************************************************************************
class AprImpl :
    public AbcA::ArrayPropertyReader,
    public Alembic::Util::enable_shared_from_this<AprImpl>
{
public:
    AprImpl( AbcA::CompoundPropertyReaderPtr iParent,
             Ogawa::IGroupPtr iGroup,
             PropertyHeaderPtr iHeader );

    // BasePropertyReader overrides
    virtual const AbcA::PropertyHeader & getHeader() const;
    virtual AbcA::ObjectReaderPtr getObject();
    virtual AbcA::CompoundPropertyReaderPtr getParent();
    virtual AbcA::ArrayPropertyReaderPtr asArrayPtr();

    // ArrayPropertyReader overrides
    virtual size_t getNumSamples();
    virtual bool isConstant();
    virtual void getSample( index_t iSampleIndex,
                            AbcA::ArraySamplePtr &oSample );
    virtual std::pair<index_t, chrono_t> getFloorIndex( chrono_t iTime );
    virtual std::pair<index_t, chrono_t> getCeilIndex( chrono_t iTime );
    virtual std::pair<index_t, chrono_t> getNearIndex( chrono_t iTime );
    virtual bool getKey( index_t iSampleIndex, AbcA::ArraySampleKey & oKey );
    virtual void getDimensions( index_t iSampleIndex,
                                Alembic::Util::Dimensions & oDim );
    virtual bool isScalarLike();
    virtual void getAs( index_t iSample, void *iIntoLocation,
                        Alembic::Util::PlainOldDataType iPod );

private:

    // Parent compound property writer. It must exist.
    AbcA::CompoundPropertyReaderPtr m_parent;

    // group from which all samples are read
    Ogawa::IGroupPtr m_group;

    // Stores the PropertyHeader and other info
    PropertyHeaderPtr m_header;
};

} // End namespace ALEMBIC_VERSION_NS

using namespace ALEMBIC_VERSION_NS;

} // End namespace AbcCoreOgawa
} // End namespace Alembic

#endif
