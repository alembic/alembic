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

#include <Alembic/Asset/IProperty.h>
#include <Alembic/Asset/Body/BodyAll.h>

namespace Alembic {
namespace Asset {

//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// SINGULAR PROPERTIES
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
void ISingularProperty::init( const IObject &ipar,
                              const std::string &nme,
                              const std::string &prot,
                              const DataType &dtype,
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
            
            AAST_THROW( m_errorString );
        }
    }
    
    //-*************************************************************************
    // Check input.
    if ( !ipar )
    {
        m_errorString += "\n";
        m_errorString += ipar.errorString();
        
        std::string err = ( boost::format(
                                "IProperty::IProperty( name=\"%s\", "
                                "protocol=\"%s\" ) Invalid parent." )
                            % nme
                            % prot ).str();
        
        m_errorString += "\n";
        m_errorString += err;
        
        AAST_THROW( m_errorString );
    }

    //-*************************************************************************
    // Create Property Body
    try
    {
        // Inherit asset body.
        m_assetBody = ipar.assetBody();

        // Create property.
        m_body = boost::make_shared<ISingularPropertyBody>(
            m_context.body(), *(ipar.body()),
            nme, prot, dtype );
    }
    catch ( std::exception &exc )
    {
        m_body.reset();
        m_assetBody.reset();
        m_context.release();
        
        std::string err = ( boost::format(
                                "IProperty::IProperty( name=\"%s\", "
                                "protocol=\"%s\" ) Failed Open. "
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
        m_body.reset();
        m_assetBody.reset();
        m_context.release();

        std::string err = ( boost::format(
                                "IProperty::IProperty( name=\"%s\", "
                                "protocol=\"%s\" ) Failed Open. "
                                "Unknown Exception" )
                            % nme
                            % prot ).str();

        m_errorString += "\n";
        m_errorString += err;
        
        AAST_THROW( m_errorString );
    }

    // All done!
}

//-*****************************************************************************
//-*****************************************************************************
// Construction with explicit name and protocol and dtype
// If no context passed, parent's will be used.
// NO EXCEPTIONS
ISingularProperty::ISingularProperty( const IObject &parent,
                                      const std::string &nme,
                                      const std::string &prot,
                                      const DataType &dtype,
                                      const IContext &ctx ) throw()
  : IProperty()
{
    try
    {
        init( parent, nme, prot, dtype, ctx );
    }
    catch ( ... ) {}
}

//-*****************************************************************************
// EXCEPTIONS
ISingularProperty::ISingularProperty( const IObject &parent,
                                      const std::string &nme,
                                      ThrowExceptionFlag tf,
                                      const std::string &prot,
                                      const DataType &dtype,
                                      const IContext &ctx )
  : IProperty()
{
    init( parent, nme, prot, dtype, ctx );
}

//-*****************************************************************************
//-*****************************************************************************
// Construction from PropertyInfo
// NO EXCEPTIONS
ISingularProperty::ISingularProperty( const IObject &parent,
                                      PropertyInfo pinfo,
                                      const IContext &ctx ) throw()
  : IProperty()
{
    if ( !pinfo )
    {
        m_errorString = "ISingularProperty::ISingularProperty() "
            "passed invalid pinfo in ctor.\n";
        return;
    }
    
    try
    {
        init( parent, pinfo->name, pinfo->protocol,
              pinfo->dtype, ctx );
    }
    catch ( ... ) {}
}

//-*****************************************************************************
// EXCEPTIONS
ISingularProperty::ISingularProperty( const IObject &parent,
                                      PropertyInfo pinfo,
                                      ThrowExceptionFlag flag,
                                      const IContext &ctx )
  : IProperty()
{
    if ( !pinfo )
    {
        m_errorString = "ISingularProperty::ISingularProperty() "
            "passed invalid pinfo in ctor.\n";
        AAST_THROW( m_errorString );
    }

    init( parent, pinfo->name, pinfo->protocol, pinfo->dtype, ctx );
}

//-*****************************************************************************
//-*****************************************************************************
// Construction from an asset with a given child index
// NO EXCEPTIONS
ISingularProperty::ISingularProperty( const IObject &parent,
                                      size_t idx,
                                      const IContext &ctx ) throw()
  : IProperty()
{
    PropertyInfo pinfo = parent.propertyInfo( idx );
    
    if ( !pinfo )
    {
        m_errorString = "ISingularProperty::ISingularProperty() "
            "passed invalid oinfo in ctor.\n";
        return;
    }

    try
    {
        init( parent, pinfo->name, pinfo->protocol, pinfo->dtype, ctx );
    }
    catch ( ... ) {}
}

//-*****************************************************************************
// EXCEPTIONS
ISingularProperty::ISingularProperty( const IObject &parent,
                                      size_t idx,
                                      ThrowExceptionFlag flag,
                                      const IContext &ctx )
  : IProperty()
{
    PropertyInfo pinfo = parent.propertyInfo( idx );

    if ( !pinfo )
    {
        m_errorString = "ISingularProperty::ISingularProperty() "
            "passed invalid oinfo in ctor.\n";
        AAST_THROW( m_errorString );
    }

    init( parent, pinfo->name, pinfo->protocol, pinfo->dtype, ctx );
}

//-*****************************************************************************
//-*****************************************************************************
bool ISingularProperty::valid() const throw()
{
    return ( bool )m_body;
}

//-*****************************************************************************
std::string ISingularProperty::name() const throw()
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
std::string ISingularProperty::fullPathName() const throw()
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
std::string ISingularProperty::protocol() const throw()
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
DataType ISingularProperty::dataType() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->dataType();
        }
        catch ( ... ) {}
    }

    return DataType();
}

//-*****************************************************************************
PropertyType ISingularProperty::propertyType() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->propertyType();
        }
        catch ( ... ) {}
    }

    return kUnknownPropertyType;
}

//-*****************************************************************************
bool ISingularProperty::isAnimated() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->isAnimated();
        }
        catch ( ... ) {}
    }

    return false;
}

//-*****************************************************************************
const ITimeSampling *ISingularProperty::timeSampling() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->timeSampling();
        }
        catch ( ... ) {}
    }

    return NULL;
}

//-*****************************************************************************
// Return values. These are stored inside the body
// so it is okay.
const void *ISingularProperty::restSample() const
{
    if ( m_body )
    {
        return m_body->restSample();
    }
    else
    {
        return NULL;
    }
}

//-*****************************************************************************
const void *ISingularProperty::animSample( size_t samp ) const
{
    if ( m_body )
    {
        const ITimeSampling *itsmp = timeSampling();
        if ( !itsmp )
        {
            return restSample();
        }
        
        return m_body->animSample( samp );
    }
    else
    {
        return NULL;
    }
}

//-*****************************************************************************
const void *ISingularProperty::animSampleLowerBound( seconds_t secs ) const
{
    if ( m_body )
    {
        const ITimeSampling *itsmp = timeSampling();
        if ( !itsmp
             || secs == Time::kRestTimeValue() )
        {
            return restSample();
        }
        
        size_t samp = itsmp->lowerBoundSample( secs );
        return m_body->animSample( samp );
    }
    else
    {
        return NULL;
    }
}

//-*****************************************************************************
const void *ISingularProperty::animSampleUpperBound( seconds_t secs ) const
{
    if ( m_body )
    {
        const ITimeSampling *itsmp = timeSampling();
        if ( !itsmp
             || secs == Time::kRestTimeValue() )
        {
            return restSample();
        }
        
        size_t samp = itsmp->upperBoundSample( secs );
        return m_body->animSample( samp );
    }
    else
    {
        return NULL;
    }
}
//-*****************************************************************************
const void *ISingularProperty::animSampleLowerBound( const Time &time ) const
{
    if ( m_body )
    {
        const ITimeSampling *itsmp = timeSampling();
        if ( !itsmp || !time )
        {
            return restSample();
        }
        
        size_t samp = itsmp->lowerBoundSample( time.seconds() );
        return m_body->animSample( samp );
    }
    else
    {
        return NULL;
    }
}

//-*****************************************************************************
const void *ISingularProperty::animSampleUpperBound( const Time &time ) const
{
    if ( m_body )
    {
        const ITimeSampling *itsmp = timeSampling();
        if ( !itsmp || !time )
        {
            return restSample();
        }
        
        size_t samp = itsmp->upperBoundSample( time.seconds() );
        return m_body->animSample( samp );
    }
    else
    {
        return NULL;
    }
}

//-*****************************************************************************
void ISingularProperty::close() throw()
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
void ISingularProperty::close( ThrowExceptionFlag tf )
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
            m_errorString += "Failed to close property.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_body.reset();
            
            AAST_THROW( m_errorString );
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Unknown Exception.";
            m_body.reset();
            
            AAST_THROW( m_errorString );
        }
    }
    release();
}

//-*****************************************************************************
void ISingularProperty::release() throw()
{
    IBase::release();
    if ( m_body )
    {
        m_body.reset();
    }
}

//-*****************************************************************************
SharedIPropertyBody ISingularProperty::asPropertyBody() const throw()
{
    return SharedIPropertyBody( m_body );
}


//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************
// MULTI PROPERTIES
//-*****************************************************************************
//-*****************************************************************************
//-*****************************************************************************

//-*****************************************************************************
void IMultiProperty::init( const IObject &ipar,
                           const std::string &nme,
                           const std::string &prot,
                           const DataType &dtype,
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
            
            AAST_THROW( m_errorString );
        }
    }
    
    //-*************************************************************************
    // Check input.
    if ( !ipar )
    {
        m_errorString += "\n";
        m_errorString += ipar.errorString();
        
        std::string err = ( boost::format(
                                "IProperty::IProperty( name=\"%s\", "
                                "protocol=\"%s\" ) Invalid parent." )
                            % nme
                            % prot ).str();
        
        m_errorString += "\n";
        m_errorString += err;
        
        AAST_THROW( m_errorString );
    }

    //-*************************************************************************
    // Create Property Body
#ifndef DEBUG
    try
#endif
    {
        // Inherit asset body.
        m_assetBody = ipar.assetBody();

        // Create property.
        m_body = boost::make_shared<IMultiPropertyBody>(
            m_context.body(), *(ipar.body()),
            nme, prot, dtype );
    }
#ifndef DEBUG
    catch ( std::exception &exc )
    {
        m_body.reset();
        m_assetBody.reset();
        m_context.release();
        
        std::string err = ( boost::format(
                                "IProperty::IProperty( name=\"%s\", "
                                "protocol=\"%s\" ) Failed Open. "
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
        m_body.reset();
        m_assetBody.reset();
        m_context.release();

        std::string err = ( boost::format(
                                "IProperty::IProperty( name=\"%s\", "
                                "protocol=\"%s\" ) Failed Open. "
                                "Unknown Exception" )
                            % nme
                            % prot ).str();

        m_errorString += "\n";
        m_errorString += err;
        
        AAST_THROW( m_errorString );
    }
#endif

    // All done!
}

//-*****************************************************************************
//-*****************************************************************************
// Construction with explicit name and protocol and dtype
// If no context passed, parent's will be used.
// NO EXCEPTIONS
IMultiProperty::IMultiProperty( const IObject &parent,
                                const std::string &nme,
                                const std::string &prot,
                                const DataType &dtype,
                                const IContext &ctx ) throw()
  : IProperty()
{
    try
    {
        init( parent, nme, prot, dtype, ctx );
    }
    catch ( ... ) {}
}

//-*****************************************************************************
// EXCEPTIONS
IMultiProperty::IMultiProperty( const IObject &parent,
                                      const std::string &nme,
                                      ThrowExceptionFlag tf,
                                      const std::string &prot,
                                      const DataType &dtype,
                                      const IContext &ctx )
  : IProperty()
{
    init( parent, nme, prot, dtype, ctx );
}

//-*****************************************************************************
//-*****************************************************************************
// Construction from PropertyInfo
// NO EXCEPTIONS
IMultiProperty::IMultiProperty( const IObject &parent,
                                      PropertyInfo pinfo,
                                      const IContext &ctx ) throw()
  : IProperty()
{
    if ( !pinfo )
    {
        m_errorString = "IMultiProperty::IMultiProperty() "
            "passed invalid pinfo in ctor.\n";
        return;
    }
    
    try
    {
        init( parent, pinfo->name, pinfo->protocol,
              pinfo->dtype, ctx );
    }
    catch ( ... ) {}
}

//-*****************************************************************************
// EXCEPTIONS
IMultiProperty::IMultiProperty( const IObject &parent,
                                      PropertyInfo pinfo,
                                      ThrowExceptionFlag flag,
                                      const IContext &ctx )
  : IProperty()
{
    if ( !pinfo )
    {
        m_errorString = "IMultiProperty::IMultiProperty() "
            "passed invalid pinfo in ctor.\n";
        AAST_THROW( m_errorString );
    }

    init( parent, pinfo->name, pinfo->protocol, pinfo->dtype, ctx );
}

//-*****************************************************************************
//-*****************************************************************************
// Construction from an asset with a given child index
// NO EXCEPTIONS
IMultiProperty::IMultiProperty( const IObject &parent,
                                size_t idx,
                                const IContext &ctx ) throw()
  : IProperty()
{
    PropertyInfo pinfo = parent.propertyInfo( idx );
    
    if ( !pinfo )
    {
        m_errorString = "IMultiProperty::IMultiProperty() "
            "passed invalid oinfo in ctor.\n";
        return;
    }

    try
    {
        init( parent, pinfo->name, pinfo->protocol, pinfo->dtype, ctx );
    }
    catch ( ... ) {}
}

//-*****************************************************************************
// EXCEPTIONS
IMultiProperty::IMultiProperty( const IObject &parent,
                                size_t idx,
                                ThrowExceptionFlag flag,
                                const IContext &ctx )
  : IProperty()
{
    PropertyInfo pinfo = parent.propertyInfo( idx );

    if ( !pinfo )
    {
        m_errorString = "IMultiProperty::IMultiProperty() "
            "passed invalid oinfo in ctor.\n";
        AAST_THROW( m_errorString );
    }

    init( parent, pinfo->name, pinfo->protocol, pinfo->dtype, ctx );
}

//-*****************************************************************************
//-*****************************************************************************
bool IMultiProperty::valid() const throw()
{
    return ( bool )m_body;
}

//-*****************************************************************************
std::string IMultiProperty::name() const throw()
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
std::string IMultiProperty::fullPathName() const throw()
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
std::string IMultiProperty::protocol() const throw()
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
DataType IMultiProperty::dataType() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->dataType();
        }
        catch ( ... ) {}
    }

    return DataType();
}

//-*****************************************************************************
PropertyType IMultiProperty::propertyType() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->propertyType();
        }
        catch ( ... ) {}
    }

    return kUnknownPropertyType;
}

//-*****************************************************************************
bool IMultiProperty::isAnimated() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->isAnimated();
        }
        catch ( ... ) {}
    }

    return false;
}

//-*****************************************************************************
const ITimeSampling *IMultiProperty::timeSampling() const throw()
{
    if ( m_body )
    {
        try
        {
            return m_body->timeSampling();
        }
        catch ( ... ) {}
    }

    return NULL;
}

//-*****************************************************************************
ISample IMultiProperty::restSample() const
{
    if ( m_body )
    {
        return ISample( m_body->restSample( context().body() ) );
    }
    else
    {
        ISample smp;
        return smp;
    }
}

//-*****************************************************************************
ISample IMultiProperty::animSample( size_t samp ) const
{
    if ( m_body )
    {
        const ITimeSampling *itsmp = timeSampling();
        if ( !itsmp )
        {
            return ISample( m_body->restSample( context().body() ) );
        }
        
        return ISample( m_body->animSample( context().body(), samp ) );
    }
    else
    {
        ISample smp;
        return smp;
    }
}

//-*****************************************************************************
ISample IMultiProperty::animSampleLowerBound( seconds_t secs ) const
{
    if ( m_body )
    {
        const ITimeSampling *itsmp = timeSampling();
        if ( !itsmp
             || secs == Time::kRestTimeValue() )
        {
            return ISample( m_body->restSample( context().body() ) );
        }
        
        size_t samp = itsmp->lowerBoundSample( secs );
        return ISample( m_body->animSample( context().body(), samp ) );
    }
    else
    {
        ISample smp;
        return smp;
    }
}

//-*****************************************************************************
ISample IMultiProperty::animSampleUpperBound( seconds_t secs ) const
{
    if ( m_body )
    {
        const ITimeSampling *itsmp = timeSampling();
        if ( !itsmp
             || secs == Time::kRestTimeValue() )
        {
            return ISample( m_body->restSample( context().body() ) );
        }
        
        size_t samp = itsmp->upperBoundSample( secs );
        return ISample( m_body->animSample( context().body(), samp ) );
    }
    else
    {
        ISample smp;
        return smp;
    }
}

//-*****************************************************************************
ISample IMultiProperty::animSampleLowerBound( const Time &time ) const
{
    if ( m_body )
    {
        const ITimeSampling *itsmp = timeSampling();
        if ( !itsmp || !time )
        {
            return ISample( m_body->restSample( context().body() ) );
        }
        
        size_t samp = itsmp->lowerBoundSample( time.seconds() );
        return ISample( m_body->animSample( context().body(), samp ) );
    }
    else
    {
        ISample smp;
        return smp;
    }
}

//-*****************************************************************************
ISample IMultiProperty::animSampleUpperBound( const Time &time ) const
{
    if ( m_body )
    {
        const ITimeSampling *itsmp = timeSampling();
        if ( !itsmp || !time )
        {
            return ISample( m_body->restSample( context().body() ) );
        }
        
        size_t samp = itsmp->upperBoundSample( time.seconds() );
        return ISample( m_body->animSample( context().body(), samp ) );
    }
    else
    {
        ISample smp;
        return smp;
    }
}

//-*****************************************************************************
void IMultiProperty::close() throw()
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
void IMultiProperty::close( ThrowExceptionFlag )
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
            m_errorString += "Failed to close property.\n";
            m_errorString += "Exception: ";
            m_errorString += exc.what();
            m_body.reset();
            
            AAST_THROW( m_errorString );
        }
        catch ( ... )
        {
            m_errorString += "\n";
            m_errorString += "Failed to close property.\n";
            m_errorString += "Unknown Exception.";
            m_body.reset();
            
            AAST_THROW( m_errorString );
        }
    }
    release();
}

//-*****************************************************************************
void IMultiProperty::release() throw()
{
    IBase::release();
    if ( m_body )
    {
        m_body.reset();
    }
}

//-*****************************************************************************
SharedIPropertyBody IMultiProperty::asPropertyBody() const throw()
{
    return SharedIPropertyBody( m_body );
}

} // End namespace Asset
} // End namespace Alembic

