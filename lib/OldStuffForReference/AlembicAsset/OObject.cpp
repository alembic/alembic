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

#include <AlembicAsset/OObject.h>
#include <AlembicAsset/Body/OAssetBody.h>
#include <AlembicAsset/Body/OParentBody.h>
#include <AlembicAsset/Body/OObjectBody.h>
#include <boost/format.hpp>

namespace AlembicAsset {

//-*****************************************************************************
OObject::OObject() throw()
  : OParentObject()
{
    // Nothing
}

//-*****************************************************************************
// No Exceptions
OObject::OObject( const OParentObject &opar,
                  const std::string &nme,
                  const std::string &prot ) throw()
  : OParentObject()
{
    if ( !opar )
    {
        m_errorString += "\n";
        m_errorString += opar.errorString();
        
        std::string err = ( boost::format(
                                "OObject::OObject( name=\"%s\", "
                                "protocol=\"%s\" ) Invalid parent." )
                            % nme
                            % prot ).str();
        
        m_errorString += "\n";
        m_errorString += err;
        
        return;
    }
    
    try
    {
        m_assetBody = opar.assetBody();
        m_body = boost::make_shared<OObjectBody>( *(opar.asParentBody()),
                                                  nme, prot );
    }
    catch ( std::exception &exc )
    {
        m_assetBody.reset();
        m_body.reset();

        std::string err = ( boost::format(
                                "OObject::OObject( name=\"%s\", "
                                "protocol=\"%s\" ) Duplicate Object Name."
                                "Exception: %s" )
                            % nme
                            % prot
                            % exc.what() ).str();
        
        m_errorString += "\n";
        m_errorString += err;

    }
    catch ( ... )
    {
        m_assetBody.reset();
        m_body.reset();

        std::string err = ( boost::format(
                                "OObject::OObject( name=\"%s\", "
                                "protocol=\"%s\" ) Unknown Exception." )
                            % nme
                            % prot ).str();
        m_errorString += "\n";
        m_errorString += err;
    }
}

//-*****************************************************************************
// Exceptions
OObject::OObject( const OParentObject &opar,
                  const std::string &nme,
                  const std::string &prot,
                  ThrowExceptionFlag )
  : OParentObject()
{
    if ( !opar )
    {
        m_errorString += "\n";
        m_errorString += opar.errorString();

        std::string err = ( boost::format(
                                "OObject::OObject( name=\"%s\", "
                                "protocol=\"%s\" ) Invalid parent." )
                            % nme
                            % prot ).str();

        m_errorString += "\n";
        m_errorString += err;

        AAH5_THROW( "OObject::OObject() ERROR: " << m_errorString
                    << std::endl );
    }
    
    try
    {
        m_assetBody = opar.assetBody();
        m_body = boost::make_shared<OObjectBody>( *(opar.asParentBody()),
                                                  nme, prot );
    }
    catch ( AlembicHDF5::DuplicateNameExc &exc )
    {
        m_body.reset();
        m_assetBody.reset();

        std::string err = ( boost::format(
                                "OObject::OObject( name=\"%s\", "
                                "protocol=\"%s\" ) Duplicate Object Name. "
                                "Exception: %s" )
                            % nme
                            % prot
                            % exc.what() ).str();
        
        m_errorString += "\n";
        m_errorString += err;
        
        AAH5_THROW( "OObject::OObject() ERROR: " << m_errorString
                    << std::endl );
    }
    catch ( ... )
    {
        m_assetBody.reset();
        m_body.reset();

        std::string err = ( boost::format(
                                "OObject::OObject( name=\"%s\", "
                                "protocol=\"%s\" ) Failed Create. "
                                "Unknown Exception" )
                            % nme
                            % prot ).str();

        m_errorString += "\n";
        m_errorString += err;

        AAH5_THROW( "OObject::OObject() ERROR: " << m_errorString
                    << std::endl );
    }
}

//-*****************************************************************************
// Copy
OObject::OObject( const OObject &copy ) throw()
  : OParentObject( copy ),
    m_body( copy.m_body )
{
    // Nothing
}

//-*****************************************************************************
OObject& OObject::operator=( const OObject &copy ) throw()
{
    OParentObject::operator=( copy );
    m_body = copy.m_body;
    return *this;
}

//-*****************************************************************************
void OObject::setComments( const std::string &cmt ) throw()
{
    if ( m_body )
    {
        m_body->setComments( cmt );
    }
}

//-*****************************************************************************
std::string OObject::name() const throw()
{
    if ( m_body )
    {
        return m_body->name();
    }
    else
    {
        return "";
    }
}

//-*****************************************************************************
std::string OObject::protocol() const throw()
{
    if ( m_body )
    {
        return m_body->name();
    }
    else
    {
        return "";
    }
}

//-*****************************************************************************
SharedOParentBody OObject::asParentBody() const throw()
{
    return SharedOParentBody( m_body );
}

//-*****************************************************************************
bool OObject::valid() const throw()
{
    return ( bool )m_body;
}

//-*****************************************************************************
// No exception version.
void OObject::close() throw()
{
    if ( m_body )
    {
        try
        {
            m_body->close();
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close object.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close object.\n";
            m_errorString += "Unknown Exception.";
        }
        m_body.reset();
    }
    release();
}

//-*****************************************************************************
void OObject::close( ThrowExceptionFlag )
{
    if ( m_body )
    {
        std::string nme = m_body->name();
        try
        {
            m_body->close();
            m_body.reset();
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close object.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_body.reset();
            
            AAH5_THROW( "OObject::close() ERROR: " << m_errorString
                        << std::endl );
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close object.\n";
            m_errorString += "Unknown Exception.";
            m_body.reset();
            
            AAH5_THROW( "OObject::close() ERROR: " << m_errorString
                        << std::endl );
        }
    }
    release();
}

//-*****************************************************************************
void OObject::release() throw()
{
    OBase::release();
    if ( m_body )
    {
        m_body.reset();
    }
}

} // End namespace AlembicAsset
