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

#include <AlembicAsset/AlembicAsset.h>

#include <boost/python.hpp>

#include <Python.h>

using namespace boost::python;

using namespace AlembicAsset;

//-*****************************************************************************
void register_icontext()
{
    class_<IContext>( "IContext" )
        .def( init<size_t>() )
        .def( "valid", &IContext::valid )
        .def( "release", &IContext::release )
        .def( "errorString", &IContext::errorString,
              return_value_policy<copy_const_reference>() )
        ;
}

#if 0
//-*****************************************************************************
class IContext
{
public:
    typedef IContext this_type;

    //-*************************************************************************
    IContext() throw() : m_errorString( "" ), m_body() {}

    IContext( const IContext &copy ) throw()
      : m_errorString( copy.m_errorString ),
        m_body( copy.m_body ) {}

    // Non-exception-throwy.
    // If problematic, error will be in errorString.
    explicit IContext( size_t maxMemory ) throw();

    // Exception throwy.
    IContext( size_t maxMemory, ThrowExceptionFlag );

    // It's hard to say what this will throw because destructors
    // are involved? CJH: More later.
    IContext &operator=( const IContext &copy ) throw()
    {
        m_errorString = copy.m_errorString;
        m_body = copy.m_body;
        return *this;
    }

    // Virtual destructor
    virtual ~IContext() throw() {}

    bool valid() const throw() { return ( bool )m_body; }

    // Release lets go of this object's hold on its resources.
    void release() throw()
    {
        // Not sure?
        m_errorString = "";
        m_body.reset();
    }

    // Add operator bool and operator!
    ALEMBIC_OPERATOR_BOOL_NOTHROW( this->valid() );

    // Access!
    SharedIContextBody body() const throw() { return m_body; }

    // Error string.
    const std::string &errorString() const throw() { return m_errorString; }

protected:
    std::string m_errorString;
    SharedIContextBody m_body;
};

} // End namespace AlembicAsset

#endif
