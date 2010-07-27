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

#ifndef _Alembic_AbcCoreAbstract_StdPropertyReader_h_
#define _Alembic_AbcCoreAbstract_StdPropertyReader_h_

#include <Alembic/AbcCoreAbstract/Foundation.h>
#include <Alembic/AbcCoreAbstract/PropertyReader.h>

class StdPropertyReader : public PropertyReader
{
public:
    StdPropertyReader( SimpleScalarPropertyReaderPtr iSubProp,
                       ErrorHandlerPtr iErrorHandler )
      : m_subProp( iSubProp ),
        m_errorHandler( iErrorHandler ) {}
    
    StdPropertyReader( AnimScalarPropertyReaderPtr iSubProp,
                       ErrorHandlerPtr iErrorHandler )
      : m_subProp( iSubProp ),
        m_errorHandler( iErrorHandler ) {}
    
    StdPropertyReader( SimpleArrayPropertyReaderPtr iSubProp )
      : m_subProp( iSubProp ) {}
    StdPropertyReader( AnimArrayPropertyReaderPtr iSubProp )
      : m_subProp( iSubProp ) {}
    StdPropertyReader( CompoundPropertyReaderPtr iSubProp )
      : m_subProp( iSubProp ) {}

    virtual const std::string &getName() const;
    virtual PropertyType getType() const;
    virtual const MetaData &getMetaData() const;

    // If one of these is called on a non-existent type, it will
    // simply return an empty ptr.
    virtual SimpleScalarPropertyReaderPtr getSimpleScalar() const;
    virtual AnimScalarPropertyReaderPtr getAnimScalar() const;
    virtual SimpleArrayPropertyReaderPtr getSimpleArray() const;
    virtual AnimArrayPropertyReaderPtr getAnimArray() const;
    virtual CompoundPropertyReaderPtr getCompound() const;

protected:
    boost::variant< SimpleScalarPropertyReaderPtr,
                    AnimScalarPropertyReaderPtr,
                    SimpleArrayPropertyReaderPtr,
                    AnimArrayPropertyReaderPtr,
                    CompoundPropertyReaderPtr > m_subProp;
};

#endif
