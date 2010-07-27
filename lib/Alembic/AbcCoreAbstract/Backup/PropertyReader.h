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

//-*****************************************************************************
// Unlike the other property abstract interfaces, the amalgam "Property"
// is really just a lightweight union of the 5 different underlying property
// types. Because we wish to avoid using derivation at the abstract level
// of the library, we don't use a Property base class, but rather this
// simple "union" class.
//
// An abstact interface is provided, and a default implementation,
// called "StdPropertyReader".
//
// The virtuals approach is provided here for consistency of expression
// and a possible entry point for custom tracking code - however, it is
// really unlikely you'd ever need to deviate from the StdPropertyReader.
class PropertyReader
{
public:
    virtual ~PropertyReader();

    virtual const std::string &getName() const = 0;
    virtual PropertyType getType() const = 0;
    virtual const MetaData &getMetaData() const = 0;

    // If one of these is called on a non-existent type, it will
    // simply return an empty ptr.
    virtual SimpleScalarPropertyReaderPtr getSimpleScalar() const = 0;
    virtual AnimScalarPropertyReaderPtr getAnimScalar() const = 0;
    virtual SimpleArrayPropertyReaderPtr getSimpleArray() const = 0;
    virtual AnimArrayPropertyReaderPtr getAnimArray() const = 0;
    virtual CompoundPropertyReaderPtr getCompound() const = 0;
};
