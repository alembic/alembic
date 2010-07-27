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

//-*****************************************************************************
//-*****************************************************************************
// BASE STUFF
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
const std::string &CompoundPropertyReaderImpl::getName() const
{
    return m_baseStuff->name();
}

//-*****************************************************************************
const ABCA::MetaData &CompoundPropertyReaderImpl::getMetaData() const
{
    return m_baseStuff->metaData();
}

//-*****************************************************************************
ABCA::ObjectReaderPtr CompoundPropertyReaderImpl::getObject()
{
    return m_object;
}

//-*****************************************************************************
ABCA::CompoundPropertyReaderPtr CompoundPropertyReaderImpl::getParent()
{
    return m_object;
}

//-*****************************************************************************
//-*****************************************************************************
// COMPOUND STUFF
//-*****************************************************************************
//-*****************************************************************************
size_t
CompoundPropertyReaderImpl::getNumProperties()
{
    return compoundStuff()->numProperties();
}

//-*****************************************************************************
ABCA::BasePropertyReaderPtr
CompoundPropertyReaderImpl::getProperty( size_t i )
{
    return compoundStuff()->property( i );
}

//-*****************************************************************************
std::string
CompoundPropertyReaderImpl::getPropertyName( size_t i )
{
    return compoundStuff()->propertyName( i );
}

//-*****************************************************************************
ABCA::BasePropertyReaderPtr
CompoundPropertyReaderImpl::getProperty( const std::string &iName )
{
    return compoundStuff()->property( iName );
}

//-*****************************************************************************
//-*****************************************************************************
// PRIVATE STUFF
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
CompoundRimplPtr
CompoundPropertyReaderImpl::compoundStuff()
{
    if ( !m_compoundStuff )
    {
        m_compoundStuff.reset( new CompoundRimpl( ... ) );
    }
    return m_compoundStuff;
}




