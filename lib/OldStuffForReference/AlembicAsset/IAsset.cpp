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

#include <AlembicAsset/IAsset.h>
#include <AlembicAsset/Body/IAssetBody.h>
#include <AlembicAsset/Body/IContextBody.h>

namespace AlembicAsset {

//-*****************************************************************************
// No exceptions.
IAsset::IAsset( const std::string &fname, const IContext &ctx ) throw()
  : IParentObject()
{
    //-*************************************************************************
    // Make or set context.
    m_context = ctx;
    if ( !m_context )
    {
        // Okay, non-existent context. We must therefore make our own.
        // 0 means no limit on max bytes.
        // This does not throw.
        m_context = IContext( ( size_t )0 );
        if ( !m_context )
        {
            // Okay, contexts REALLY failed.
            m_errorString =
                "ERROR: IAsset::IAsset() Could not create context.\n";
            m_errorString += m_context.errorString();
            m_context.release();
            return;
        }
    }

    //-*************************************************************************
    // Create Asset Body
    try
    {
        m_assetBody = boost::make_shared<IAssetBody>( fname );
    }
    catch ( std::exception &exc )
    {
        m_errorString =
            "ERROR: IAsset::IAsset() failed to create IAssetBody\n";
        m_errorString += "Exception: ";
        m_errorString = exc.what();
        m_assetBody.reset();
        m_context.release();
    }
    catch ( ... )
    {
        m_errorString =
            "ERROR: IAsset::IAsset() failed to create IAssetBody\n";
        m_errorString += "Unknown Exception\n";
        m_assetBody.reset();
        m_context.release();
    }
}

//-*****************************************************************************
// Exceptions
IAsset::IAsset( const std::string &fname, ThrowExceptionFlag,
                const IContext &ctx )
  : IParentObject()
{
    //-*************************************************************************
    // Make or set context.
    m_context = ctx;
    if ( !m_context )
    {
        // Okay, non-existent context. We must therefore make our own.
        // 0 means no limit on max bytes.
        try
        {
            m_context = IContext( ( size_t )0, kThrowException );
        }
        catch ( std::exception &exc )
        {
            // Okay, contexts REALLY failed.
            m_errorString =
                "ERROR: IAsset::IAsset() Could not create context.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_errorString += "\n";
            m_errorString += m_context.errorString();
            m_context.release();

            AAH5_THROW( m_errorString );
        }
        catch ( ... )
        {
            // Okay, contexts REALLY failed.
            m_errorString =
                "ERROR: IAsset::IAsset() Could not create context.\n";
            m_errorString += "Unknown Exception\n ";
            m_errorString += m_context.errorString();
            m_context.release();

            AAH5_THROW( m_errorString );
        }
    }

    //-*************************************************************************
    // Create Asset Body
    try
    {
        m_assetBody = boost::make_shared<IAssetBody>( fname );
    }
    catch ( std::exception &exc )
    {
        m_errorString =
            "ERROR: IAsset::IAsset() failed to create IAssetBody\n";
        m_errorString += "Exception: ";
        m_errorString = exc.what();
        m_assetBody.reset();
        m_context.release();

        AAH5_THROW( m_errorString );
    }
    catch ( ... )
    {
        m_errorString =
            "ERROR: IAsset::IAsset() failed to create IAssetBody\n";
        m_errorString += "Unknown Exception\n";
        m_assetBody.reset();
        m_context.release();

        AAH5_THROW( m_errorString );
    }

    // As long as the roots are not severed, all is well.
    // And all will be well in the garden.
}

//-*****************************************************************************
IAsset::IAsset( const IAsset &copy ) throw()
  : IParentObject( copy ) {}

//-*****************************************************************************
IAsset::IAsset( const IBase &copyOther ) throw()
  : IParentObject()
{
    m_errorString = copyOther.errorString();
    m_context = copyOther.context();
    m_assetBody = copyOther.assetBody();
}

//-*****************************************************************************
IAsset &IAsset::operator=( const IAsset &copy ) throw()
{
    IParentObject::operator=( copy );
    return *this;
}

//-*****************************************************************************
IAsset &IAsset::operator=( const IBase &copyOther ) throw()
{
    m_errorString = copyOther.errorString();
    m_context = copyOther.context();
    m_assetBody = copyOther.assetBody();
    return *this;
}

//-*****************************************************************************
std::string IAsset::fileName() const throw()
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
std::string IAsset::name() const throw()
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
std::string IAsset::fullPathName() const throw()
{
    return "/";
}

//-*****************************************************************************
std::string IAsset::protocol() const throw()
{
    return "AlembicAsset_v0001";
}

//-*****************************************************************************
std::string IAsset::comments() const throw()
{
    if ( m_assetBody )
    {
        try
        {
            return m_assetBody->comments();
        }
        catch ( ... ) {}
    }
    
    return "";
}

//-*****************************************************************************
size_t IAsset::numChildren() const throw()
{
    if ( m_assetBody )
    {
        try
        {
            return m_assetBody->numChildren();
        }
        catch ( ... ) {}
    }
    
    return 0;
}

//-*****************************************************************************
ObjectInfo IAsset::childInfo( size_t p ) const throw()
{
    if ( m_assetBody )
    {
        try
        {
            return m_assetBody->childInfo( p );
        }
        catch ( ... ) {}
    }
    
    ObjectInfo o;
    return o;
}

//-*****************************************************************************
ObjectInfo IAsset::childInfo( const std::string &nme,
                              const std::string &prot ) const throw()
{
    if ( m_assetBody )
    {
        try
        {
            return m_assetBody->childInfo( nme, prot );
        }
        catch ( ... ) {}
    }
    
    ObjectInfo o;
    return o;
}

//-*****************************************************************************
bool IAsset::valid() const throw()
{
    return ( bool )m_assetBody;
}

//-*****************************************************************************
// Non-exceptions
void IAsset::close() throw()
{
    if ( m_assetBody )
    {
        try
        {
            m_assetBody->close();
            release();
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\nIAsset::close() failed.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_errorString += "\n";
            m_assetBody.reset();
            m_context.release();
        }
        catch ( ... )
        {
            m_errorString += "\nIAsset::close() failed.\n";
            m_errorString += "Unknown Exception\n";
            m_assetBody.reset();
            m_context.release();
        }
    }
    else
    {
        release();
    }
}

//-*****************************************************************************
// With exceptions
void IAsset::close( ThrowExceptionFlag )
{
    if ( m_assetBody )
    {
        std::string nme;
        try
        {
            nme = m_assetBody->fileName();
            m_assetBody->close();
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\nIAsset::close() failed.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_errorString += "\n";
            m_assetBody.reset();
            m_context.release();

            AAH5_THROW( m_errorString );
        }
        catch ( ... )
        {
            m_errorString += "\nIAsset::close() failed.\n";
            m_errorString += "Unknown Exception\n";
            m_assetBody.reset();
            m_context.release();

            AAH5_THROW( m_errorString );
        }
    }

    release();
}

//-*****************************************************************************
void IAsset::release() throw()
{
    IBase::release();
}

//-*****************************************************************************
SharedIParentBody IAsset::asParentBody() const throw()
{
    SharedIParentBody sipb( m_assetBody );
    return sipb;
}

} // End namespace AlembicAsset
