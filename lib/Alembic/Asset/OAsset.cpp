//-*****************************************************************************
//
// Copyright (c) 2009-2010, Industrial Light & Magic,
//   a division of Lucasfilm Entertainment Company Ltd.
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
// *       Neither the name of Industrial Light & Magic nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
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

#include <Alembic/Asset/OAsset.h>
#include <Alembic/Asset/Body/BodyAll.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
OAsset::OAsset() throw() : OParentObject()
{
    // Nothing
}


//-*****************************************************************************
// Explicit
OAsset::OAsset( const std::string &fname ) throw()
  : OParentObject()
{
    try
    {
        m_assetBody = boost::make_shared<OAssetBody>( fname );
    }
    catch ( std::exception &exc )
    {
        m_errorString = exc.what();
        m_assetBody.reset();
    }
    catch ( ... )
    {
        m_errorString = "Failed to open asset: ";
        m_errorString += fname;
        m_errorString += ". Unknown Exception.";
        m_assetBody.reset();
    }
}

//-*****************************************************************************
OAsset::OAsset( const std::string &fname, ThrowExceptionFlag )
  : OParentObject()
{
    // CJH Improve asset error reporting.
    try
    {
        m_assetBody = boost::make_shared<OAssetBody>( fname );
    }
    catch ( std::exception &exc )
    {
        m_errorString = exc.what();
        m_assetBody.reset();
        Exception newExc( fname );
        newExc.append( "\n" );
        newExc.append( m_errorString );
        throw( newExc );
    }
    catch ( ... )
    {
        m_errorString = "Failed to open asset: ";
        m_errorString += fname;
        m_errorString += ". Unknown Exception.\n";
        m_assetBody.reset();

        AAST_THROW( "OAsset::OAsset() ERROR: " << m_errorString
                    << std::endl
                    << "File Name: " << fname );
    }
}

//-*****************************************************************************
OAsset::OAsset( const OAsset &copy ) throw() : OParentObject( copy )
{
    // Nothing
}

//-*****************************************************************************
OAsset::OAsset( const OBase &copyOther ) throw()
  : OParentObject()
{
    m_assetBody = copyOther.assetBody();
    m_errorString = copyOther.errorString();
}

//-*****************************************************************************
OAsset &OAsset::operator=( const OAsset &copy ) throw()
{
    OParentObject::operator=( copy );
    return *this;
}

//-*****************************************************************************
OAsset &OAsset::operator=( const OBase &copyOther ) throw()
{
    // CJH - slightly sketchy, but whatever.
    OBase::operator=( copyOther );
    return *this;
}

//-*****************************************************************************
void OAsset::setComments( const std::string &cmt ) throw()
{
    if ( m_assetBody )
    {
        try
        {
            m_assetBody->setComments( cmt );
        }
        catch ( ... ) {}
    }
}

//-*****************************************************************************
std::string OAsset::name() const throw()
{
    if ( m_assetBody )
    {
        try
        {
            return m_assetBody->fileName();
        }
        catch ( ... ) {}
    }
    
    return "";
}

//-*****************************************************************************
std::string OAsset::fileName() const throw()
{
    if ( m_assetBody )
    {
        try
        {
            return m_assetBody->fileName();
        }
        catch ( ... ) {}
    }
    
    return "";
}

//-*****************************************************************************
int OAsset::compressionLevel() const throw()
{
    if ( m_assetBody )
    {
        try
        {
            return m_assetBody->compressionLevel();
        }
        catch ( ... ) {}
    }

    return 5;
}

//-*****************************************************************************
void OAsset::setCompressionLevel( int level ) throw()
{
    if ( m_assetBody )
    {
        try
        {
            m_assetBody->setCompressionLevel( level );
        }
        catch ( ... ) {}
    }
}

//-*****************************************************************************
SharedOParentBody OAsset::asParentBody() const throw()
{
    return SharedOParentBody( m_assetBody );
}

//-*****************************************************************************
bool OAsset::valid() const throw()
{
    return ( bool )m_assetBody;
}

//-*****************************************************************************
void OAsset::close() throw()
{
    if ( m_assetBody )
    {
        try
        {
            m_assetBody->close();
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close asset.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close asset.\n";
            m_errorString += "Unknown Exception.";
        }
        m_assetBody.reset();
    }
    release();
}

//-*****************************************************************************
void OAsset::close( ThrowExceptionFlag )
{
    if ( m_assetBody )
    {
        std::string nme = m_assetBody->fileName();
        try
        {
            m_assetBody->close();
            m_assetBody.reset();
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close asset.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_assetBody.reset();

            AAST_THROW( "OAsset::close() ERROR: " << m_errorString
                        << std::endl
                        << "File Name: " << nme
                        << std::endl );
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close asset.\n";
            m_errorString += "Unknown Exception.";
            m_assetBody.reset();

            
            AAST_THROW( "OAsset::close() ERROR: " << m_errorString
                        << std::endl
                        << "File Name: " << nme
                        << std::endl );
        }
    }
    release();
}

} // End namespace Asset
} // End namespace Alembic

