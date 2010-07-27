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

#include <Alembic/Util/Singleton.h>
#include <iostream>

#ifdef ALEMBIC_USE_BOOST_THREADS
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#endif

//-*****************************************************************************
class MyClass : public Alembic::Util::Singleton<MyClass>
{ 
private:
    friend class Alembic::Util::Singleton<MyClass>;
    MyClass() : m_register( 0 ) {}

public:
    void printRegister() const { std::cout << "MyClass::register = "
                                           << m_register << std::endl; }
    void incrementRegisterBy( int i )
    {
        m_register += i;
    }

protected:

    int m_register;
};


//-*****************************************************************************
// This isn't a particularly thread-safe example, but whatever.
void test( void )
{
    MyClass::instance().printRegister();
    MyClass::instance().incrementRegisterBy( 3 );
    MyClass::instance().printRegister();
}

#ifdef ALEMBIC_USE_BOOST_THREADS

//-*****************************************************************************
int main( int argc, char* argv[] )
{
    boost::thread thread1( &test );
    boost::thread thread2( &test );
    thread1.join();
    thread2.join();
    return 0;
}

#else

//-*****************************************************************************
int main( int argc, char *argv[] )
{
    test();
    test();
    return 0;
}

#endif



