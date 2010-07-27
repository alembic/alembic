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

#include <Alembic/Asset/OProperty.h>
#include <Alembic/Asset/Body/BodyAll.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
//-*****************************************************************************
// BASE PROPERTY
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
bool OProperty::valid() const throw()
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    return (( bool )baseBody);
}

//-*****************************************************************************
std::string OProperty::name() const throw()
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        try
        {
            return baseBody->name();
        }
        catch ( ... ) {}
    }
    
    return "";
}

//-*****************************************************************************
std::string OProperty::protocol() const throw()
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        try
        {
            return baseBody->protocol();
        }
        catch ( ... ) {}
    }
    
    return "";
}

//-*****************************************************************************
DataType OProperty::dataType() const throw()
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        try
        {
            return baseBody->dataType();
        }
        catch ( ... ) {}
    }
    return DataType();
}

//-*****************************************************************************
PropertyType OProperty::propertyType() const throw()
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        try
        {
            return baseBody->propertyType();
        }
        catch ( ... ) {}
    }

    return kUnknownPropertyType;
}

//-*****************************************************************************
void OProperty::setRestSample( const void *data, const Dimensions &dims )
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        baseBody->setRestSample( data, dims );
    }
}

//-*****************************************************************************
bool OProperty::isAnimated() const throw()
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        try
        {
            return baseBody->isAnimated();
        }
        catch ( ... ) {}
    }
    
    return false;
}

//-*****************************************************************************
TimeSamplingInfo OProperty::timeSamplingInfo() const throw()
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        try
        {
            return baseBody->timeSamplingInfo();
        }
        catch ( ... ) {}
    }

    TimeSamplingInfo tinfo;
    return tinfo;
}

//-*****************************************************************************
seconds_t OProperty::sampleToSeconds( size_t samp ) const throw()
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        try
        {
            return baseBody->sampleToSeconds( samp );
        }
        catch ( ... ) {}
    }

    return Time::kRestTimeValue();
}

//-*****************************************************************************
size_t OProperty::secondsToSample( seconds_t secs ) const throw()
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        try
        {
            return baseBody->secondsToSample( secs );
        }
        catch ( ... ) {}
    }

    return 0;
}

//-*****************************************************************************
void OProperty::makeAnimated( const TimeSamplingInfo &tinfo )
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        baseBody->makeAnimated( tinfo );
    }
}

//-*****************************************************************************
size_t OProperty::sampleTimesSize() const throw()
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        try
        {
            return baseBody->sampleTimesSize();
        }
        catch ( ... ) {}
    }
    
    return 0;
}

//-*****************************************************************************
void OProperty::setAnimSample( size_t samp,
                               seconds_t seconds,
                               const void *data,
                               const Dimensions &dims )
{
    SharedOPropertyBody baseBody = this->asPropertyBody();
    if ( baseBody )
    {
        baseBody->setAnimSample( samp, seconds, data, dims );
    }
}

#if 0

//-*****************************************************************************
void OProperty::close() throw()
{
    if ( m_baseBody )
    {
        try
        {
            m_baseBody->close();
            release();
            
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Unknown Exception.";
        }
        m_baseBody.reset();
    }
    release();
}

//-*****************************************************************************
void OProperty::close( ThrowExceptionFlag )
{
    if ( m_baseBody )
    {
        std::string nme = m_baseBody->name();
        try
        {
            m_baseBody->close();
            
            // std::cout << "OProperty::close() - finished body close"
            //          << std::endl;
            
            m_baseBody.reset();
            
            //std::cout << "OProperty::close() - called release."
            //          << std::endl;
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_baseBody.reset();

            AAST_THROW( "OProperty::close() ERROR: "
                        << m_errorString << std::endl );
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Unknown Exception.";
            m_baseBody.reset();
            
            AAST_THROW( "OProperty::close() ERROR: "
                        << m_errorString << std::endl );
        }
    }
    release();
}

//-*****************************************************************************
void OProperty::release() throw()
{
    OBase::release();
    // m_baseBody.reset();
}

#endif

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// SINGULAR PROPERTY
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// No exceptions.
OSingularProperty::OSingularProperty( const OObject &opar,
                                      const std::string &nme,
                                      const std::string &prot,
                                      const DataType &dtype ) throw()
  : OProperty()
{
    if ( !opar )
    {
        m_errorString += "\n";
        m_errorString += opar.errorString();
        
        std::string err = ( boost::format(
                                "OSingularProperty::OSingularProperty( "
                                "name=\"%s\", "
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
        // Subtle reasons why make shared doesn't work.
        m_body.reset(
            new OSingularPropertyBody( *(opar.body()),
                                       nme, prot, dtype ) );
        OProperty::init( m_body );
    }
    catch ( std::exception &exc )
    {
        release();

        std::string err = ( boost::format(
                                "OSingularProperty::OSingularProperty( "
                                "name=\"%s\", "
                                "protocol=\"%s\" ) "
                                "Duplicate Property Name."
                                "Exception: %s" )
                            % nme
                            % prot
                            % exc.what() ).str();
        
        m_errorString += "\n";
        m_errorString += err;
    }
    catch ( ... )
    {
        release();

        std::string err = ( boost::format(
                                "OSingularProperty::OSingularProperty( "
                                "name=\"%s\", "
                                "protocol=\"%s\" ) Unknown Exception." )
                            % nme
                            % prot ).str();
        m_errorString += "\n";
        m_errorString += err;
    }
}

//-*****************************************************************************
// Exceptions.
OSingularProperty::OSingularProperty( const OObject &opar,
                                      const std::string &nme,
                                      const std::string &prot,
                                      const DataType &dtype,
                                      ThrowExceptionFlag )
  : OProperty()
{
    if ( !opar )
    {
        m_errorString += "\n";
        m_errorString += opar.errorString();
        
        std::string err = ( boost::format(
                                "OSingularProperty::OSingularProperty( "
                                "name=\"%s\", "
                                "protocol=\"%s\" ) Invalid parent." )
                            % nme
                            % prot ).str();
        
        m_errorString += "\n";
        m_errorString += err;
        
        AAST_THROW( m_errorString );
    }
    
    try
    {
        m_assetBody = opar.assetBody();
        // Subtle reasons why make shared doesn't work.
        m_body.reset(
            new OSingularPropertyBody( *(opar.body()),
                                       nme, prot, dtype ) );
        OProperty::init( m_body );
    }
    catch ( std::exception &exc )
    {
        release();

        std::string err = ( boost::format(
                                "OSingularProperty::OSingularProperty( "
                                "name=\"%s\", "
                                "protocol=\"%s\" ) "
                                "Duplicate Property Name."
                                "Exception: %s" )
                            % nme
                            % prot
                            % exc.what() ).str();
        
        m_errorString += "\n";
        m_errorString += err;
        
        AAST_THROW( m_errorString );
    }
    catch ( ... )
    {
        release();

        std::string err = ( boost::format(
                                "OSingularProperty::OSingularProperty( "
                                "name=\"%s\", "
                                "protocol=\"%s\" ) Unknown Exception." )
                            % nme
                            % prot ).str();
        m_errorString += "\n";
        m_errorString += err;
        
        AAST_THROW( m_errorString );
    }
}

//-*****************************************************************************
void OSingularProperty::close() throw()
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
            m_errorString += "Failed to close property.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Unknown Exception.";
        }
        m_body.reset();
    }
    release();
}

//-*****************************************************************************
void OSingularProperty::close( ThrowExceptionFlag tf )
{
    if ( m_body )
    {
        std::string nme = m_body->name();
        try
        {
            m_body->close();
            m_body.reset();
            release();
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_body.reset();
            release();

            AAST_THROW( m_errorString );
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Unknown Exception.";
            m_body.reset();
            release();
            
            AAST_THROW( m_errorString );
        }
    }
}

//-*****************************************************************************
void OSingularProperty::release() throw()
{
    if ( m_body )
    {
        m_body.reset();
    }
    
    OProperty::release();
}

//-*****************************************************************************
SharedOPropertyBody OSingularProperty::asPropertyBody() const throw()
{
    return SharedOPropertyBody( m_body );
}

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MULTI PROPERTY
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
// No exceptions.
OMultiProperty::OMultiProperty( const OObject &opar,
                                const std::string &nme,
                                const std::string &prot,
                                const DataType &dtype ) throw()
  : OProperty()
{
    if ( !opar )
    {
        m_errorString += "\n";
        m_errorString += opar.errorString();
        
        std::string err = ( boost::format(
                                "OMultiProperty::OMultiProperty( "
                                "name=\"%s\", "
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
        // Subtle reasons why make shared doesn't work.
        m_body.reset(
            new OMultiPropertyBody( *(opar.body()),
                                    nme, prot, dtype ) );
        OProperty::init( m_body );
    }
    catch ( std::exception &exc )
    {
        release();

        std::string err = ( boost::format(
                                "OMultiProperty::OMultiProperty( "
                                "name=\"%s\", "
                                "protocol=\"%s\" ) "
                                "Duplicate Property Name."
                                "Exception: %s" )
                            % nme
                            % prot
                            % exc.what() ).str();
        
        m_errorString += "\n";
        m_errorString += err;
    }
    catch ( ... )
    {
        release();

        std::string err = ( boost::format(
                                "OMultiProperty::OMultiProperty( "
                                "name=\"%s\", "
                                "protocol=\"%s\" ) Unknown Exception." )
                            % nme
                            % prot ).str();
        m_errorString += "\n";
        m_errorString += err;
    }
}

//-*****************************************************************************
// Exceptions.
OMultiProperty::OMultiProperty( const OObject &opar,
                                const std::string &nme,
                                const std::string &prot,
                                const DataType &dtype,
                                ThrowExceptionFlag )
  : OProperty()
{
    if ( !opar )
    {
        m_errorString += "\n";
        m_errorString += opar.errorString();
        
        std::string err = ( boost::format(
                                "OMultiProperty::OMultiProperty( "
                                "name=\"%s\", "
                                "protocol=\"%s\" ) Invalid parent." )
                            % nme
                            % prot ).str();
        
        m_errorString += "\n";
        m_errorString += err;
        
        AAST_THROW( m_errorString );
    }
    
    try
    {
        m_assetBody = opar.assetBody();
        // Subtle reasons why make shared doesn't work.
        m_body.reset(
            new OMultiPropertyBody( *(opar.body()),
                                    nme, prot, dtype ) );
        OProperty::init( m_body );
    }
    catch ( std::exception &exc )
    {
        release();

        std::string err = ( boost::format(
                                "OMultiProperty::OMultiProperty( "
                                "name=\"%s\", "
                                "protocol=\"%s\" ) "
                                "Duplicate Property Name."
                                "Exception: %s" )
                            % nme
                            % prot
                            % exc.what() ).str();
        
        m_errorString += "\n";
        m_errorString += err;
        
        AAST_THROW( m_errorString );
    }
    catch ( ... )
    {
        release();

        std::string err = ( boost::format(
                                "OMultiProperty::OMultiProperty( "
                                "name=\"%s\", "
                                "protocol=\"%s\" ) Unknown Exception." )
                            % nme
                            % prot ).str();
        m_errorString += "\n";
        m_errorString += err;
        
        AAST_THROW( m_errorString );
    }
}

//-*****************************************************************************
void OMultiProperty::close() throw()
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
            m_errorString += "Failed to close property.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Unknown Exception.";
        }
        m_body.reset();
    }
    release();
}

//-*****************************************************************************
void OMultiProperty::close( ThrowExceptionFlag tf )
{
    if ( m_body )
    {
        std::string nme = m_body->name();
        try
        {
            m_body->close();
            m_body.reset();
            release();
        }
        catch ( std::exception &exc )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_body.reset();
            release();

            AAST_THROW( m_errorString );
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Unknown Exception.";
            m_body.reset();
            release();
            
            AAST_THROW( m_errorString );
        }
    }
}

//-*****************************************************************************
void OMultiProperty::release() throw()
{
    if ( m_body )
    {
        m_body.reset();
    }
    
    OProperty::release();
}

//-*****************************************************************************
SharedOPropertyBody OMultiProperty::asPropertyBody() const throw()
{
    return SharedOPropertyBody( m_body );
}

} // End namespace Asset
} // End namespace Alembic

