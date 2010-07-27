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

#include <AlembicAsset/IObject.h>
#include <AlembicAsset/Body/IObjectBody.h>
#include <AlembicAsset/Body/IAssetBody.h>
#include <AlembicAsset/Body/IContextBody.h>

namespace AlembicAsset {

//-*****************************************************************************
// Construction with explicit name and protocol.
void IObject::init( const IParentObject &ipar,
                    const std::string &nme,
                    const std::string &prot,
                    const IContext &ctx )
{
    //-*************************************************************************
    // Make or set context.
    m_context = ctx;
    if ( !m_context )
    {
        m_context = ipar.context();
        if ( !m_context )
        {
            // Okay, contexts REALLY failed.
            m_errorString =
                "ERROR: IObject::IObject() Passed invalid context\n";
            m_errorString += m_context.errorString();
            m_context.release();

            AAH5_THROW( m_errorString );
        }
    }

    //-*************************************************************************
    // Check input.
    if ( !ipar )
    {
        m_errorString += "\n";
        m_errorString += ipar.errorString();

        std::string err = ( boost::format(
                                "IObject::IObject( name=\"%s\", "
                                "protocol=\"%s\" ) Invalid parent." )
                            % nme
                            % prot ).str();

        m_errorString += "\n";
        m_errorString += err;

        m_context.release();
        
        AAH5_THROW( m_errorString );
    }

    //-*************************************************************************
    // Create Object Body
    try
    {
        // Inherit asset body.
        m_assetBody = ipar.assetBody();

        // Create body.
        const H5G &pgrp = ipar.asParentBody()->childrenGroup();
        m_body = boost::make_shared<IObjectBody>( pgrp, nme, prot );
    }
    catch ( std::exception &exc )
    {
        m_body.reset();
        m_assetBody.reset();
        m_context.release();

        std::string err = ( boost::format(
                                "IObject::IObject( name=\"%s\", "
                                "protocol=\"%s\" ) Failed Open. "
                                "Exception: %s" )
                            % nme
                            % prot
                            % exc.what() ).str();

        m_errorString += "\n";
        m_errorString += err;
        
        AAH5_THROW( m_errorString );
    }
    catch ( ... )
    {
        m_body.reset();
        m_assetBody.reset();
        m_context.release();

        std::string err = ( boost::format(
                                "IObject::IObject( name=\"%s\", "
                                "protocol=\"%s\" ) Failed Open. "
                                "Unknown Exception" )
                            % nme
                            % prot ).str();

        m_errorString += "\n";
        m_errorString += err;
        
        AAH5_THROW( m_errorString );
    }

    // All done!
}

//-*****************************************************************************
// Construction with explicit name and protocol
// If no context passed, parent's will be used.
// NO EXCEPTIONS
IObject::IObject( const IParentObject &parent,
                  const std::string &nme,
                  const std::string &prot,
                  const IContext &ctx ) throw()
  : IParentObject()
{
    try
    {
        init( parent, nme, prot, ctx );
    }
    catch ( ... ) {}
}

//-*****************************************************************************
// EXCEPTIONS
IObject::IObject( const IParentObject &parent,
                  const std::string &nme,
                  ThrowExceptionFlag tf,
                  const std::string &prot,
                  const IContext &ctx )
  : IParentObject()
{
    init( parent, nme, prot, ctx );
}

//-*****************************************************************************
// Construction with property info
// NO EXCEPTIONS
IObject::IObject( const IParentObject &parent,
                  ObjectInfo oinfo,
                  const IContext &ctx ) throw()
  : IParentObject()
{
    if ( !oinfo )
    {
        m_errorString = "IObject::IObject() passed invalid oinfo in ctor.\n";
        return;
    }

    try
    {
        init( parent, oinfo->name, oinfo->protocol, ctx );
    }
    catch ( ... ) {}
}

//-*****************************************************************************
// EXCEPTIONS
IObject::IObject( const IParentObject &parent,
                  ObjectInfo oinfo,
                  ThrowExceptionFlag flag,
                  const IContext &ctx )
  : IParentObject()
{
    if ( !oinfo )
    {
        m_errorString = "IObject::IObject() passed invalid oinfo in ctor.\n";
        AAH5_THROW( m_errorString );
    }

    init( parent, oinfo->name, oinfo->protocol, ctx );
}

//-*****************************************************************************
// Construction from an asset with a given child index
// NO EXCEPTIONS
IObject::IObject( const IParentObject &parent,
                  size_t idx,
                  const IContext &ctx ) throw()
  : IParentObject()
{
    ObjectInfo oinfo = parent.childInfo( idx );

    if ( !oinfo )
    {
        m_errorString = "IObject::IObject() passed invalid oinfo in ctor.\n";
        return;
    }

    try
    {
        init( parent, oinfo->name, oinfo->protocol, ctx );
    }
    catch ( ... ) {}
}

//-*****************************************************************************
// EXCEPTIONS
IObject::IObject( const IParentObject &parent,
                  size_t idx,
                  ThrowExceptionFlag flag,
                  const IContext &ctx )
  : IParentObject()
{
    ObjectInfo oinfo = parent.childInfo( idx );

    if ( !oinfo )
    {
        m_errorString = "IObject::IObject() passed invalid oinfo in ctor.\n";
        AAH5_THROW( m_errorString );
    }

    init( parent, oinfo->name, oinfo->protocol, ctx );
}

//-*****************************************************************************
// copy constructor, no exceptions
IObject::IObject( const IObject &copy ) throw()
  : IParentObject( copy ),
    m_body( copy.m_body )
{}

//-*****************************************************************************
IObject &IObject::operator=( const IObject &copy ) throw()
{
    IParentObject::operator=( copy );
    m_body = copy.m_body;
    return *this;
}

//-*****************************************************************************
std::string IObject::name() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->name();
        }
        catch ( ... ) {}
    }

    return "";
}

//-*****************************************************************************
std::string IObject::fullPathName() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->fullPathName();
        }
        catch ( ... ) {}
    }

    return "";
}

//-*****************************************************************************
std::string IObject::protocol() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->protocol();
        }
        catch ( ... ) {}
    }

    return "";
}

//-*****************************************************************************
std::string IObject::comments() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->comments();
        }
        catch ( ... ) {}
    }

    return "";
}

//-*****************************************************************************
size_t IObject::numProperties() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->numProperties();
        }
        catch ( ... ) {}
    }

    return 0;
}

//-*****************************************************************************
PropertyInfo IObject::propertyInfo( size_t p ) const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->propertyInfo( p );
        }
        catch ( ... ) {}
    }

    PropertyInfo pinfo;
    return pinfo;
}

//-*****************************************************************************
PropertyInfo IObject::propertyInfo( const std::string &nme,
                                    const std::string &prot ) const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->propertyInfo( nme, prot );
        }
        catch ( ... ) {}
    }

    PropertyInfo pinfo;
    return pinfo;
}

//-*****************************************************************************
size_t IObject::numChildren() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->numChildren();
        }
        catch ( ... ) {}
    }

    return 0;
}

//-*****************************************************************************
ObjectInfo IObject::childInfo( size_t p ) const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->childInfo( p );
        }
        catch ( ... ) {}
    }

    ObjectInfo o;
    return o;
}

//-*****************************************************************************
ObjectInfo IObject::childInfo( const std::string &nme,
                              const std::string &prot ) const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->childInfo( nme, prot );
        }
        catch ( ... ) {}
    }

    ObjectInfo o;
    return o;
}

//-*****************************************************************************
bool IObject::valid() const throw()
{
    return ( bool )m_body;
}

//-*****************************************************************************
// Non-exceptions
void IObject::close() throw()
{
    if ( m_body )
    {
        try
        {
            m_body->close();
            release();
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\nIObject::close() failed.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_errorString += "\n";
            m_body.reset();
            m_assetBody.reset();
            m_context.release();
        }
        catch ( ... )
        {

            m_errorString += "\nIObject::close() failed.\n";
            m_errorString += "Unknown Exception\n";
            m_body.reset();
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
void IObject::close( ThrowExceptionFlag )
{
    if ( m_body )
    {
        std::string nme;
        try
        {
            nme = m_body->name();
            m_body->close();
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\nIObject::close() failed.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_errorString += "\n";
            m_body.reset();
            m_assetBody.reset();
            m_context.release();
            
            AAH5_THROW( m_errorString );
        }
        catch ( ... )
        {
            m_errorString += "\nIObject::close() failed.\n";
            m_errorString += "Unknown Exception\n";
            m_body.reset();
            m_assetBody.reset();
            m_context.release();
            
            AAH5_THROW( m_errorString );
        }
    }

    release();
}

//-*****************************************************************************
void IObject::release() throw()
{
    m_body.reset();
    IBase::release();
}

//-*****************************************************************************
SharedIParentBody IObject::asParentBody() const throw()
{
    SharedIParentBody sipb( m_body );
    return sipb;
}

} // End namespace AlembicAsset
