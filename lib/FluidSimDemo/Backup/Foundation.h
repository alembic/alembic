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

#ifndef _FluidSimDemo_Foundation_h_
#define _FluidSimDemo_Foundation_h_

//#include <Alembic/TakoSPI/TakoSPI.h>
//#include <Alembic/TakoAbstract/TakoAbstract.h>
#include <Alembic/Tako/Tako.h>
#include <Alembic/GLUtil/GLUtil.h>
#include <Alembic/Util/Util.h>

#include <boost/smart_ptr.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>

#include <ImathMatrix.h>
#include <ImathVec.h>
#include <ImathBox.h>
#include <ImathMatrixAlgo.h>
#include <ImathVec.h>
#include <ImathBoxAlgo.h>
#include <ImathQuat.h>

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <exception>
#include <stdexcept>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

namespace FluidSimDemo {

//-*****************************************************************************
//#define MakeTakoSceneReader Alembic::TakoSPI::v1::ReadScene
//#define MakeTakoSceneWriter Alembic::TakoSPI::v1::WriteScene

//-*****************************************************************************
// Namespace Aliasing
//namespace AtkSPI = Alembic::TakoSPI;
//namespace AbcUtil = Alembic::Util;
using Alembic::Tako::index_t;

using Imath::V3f;
using Imath::V3d;
using Imath::Box3f;
using Imath::Box3d;
using Imath::M44f;
using Imath::M44d;

//-*****************************************************************************
// Just use Tako Abstract.
//using namespace Alembic::TakoAbstract::TAKO_ABSTRACT_LIB_VERSION_NS;
using namespace Alembic::Tako::TAKO_LIB_VERSION_NS;
typedef boost::shared_ptr<TransformWriter> TransformWriterPtr;
typedef boost::shared_ptr<PointPrimitiveWriter> PointPrimitiveWriterPtr;

inline TransformReaderPtr MakeTakoSceneReader( const std::string &name )
{
    TransformReaderPtr reader( new TransformReader( name.c_str() ) );
    return reader;
}

inline TransformWriterPtr MakeTakoSceneWriter( const std::string &name )
{
    TransformWriterPtr writer( new TransformWriter( name.c_str() ) );
    return writer;
}

inline TransformWriterPtr MakeTransformWriterPtr( const std::string &name,
                                                  TransformWriterPtr node )
{
    TransformWriterPtr writer( new TransformWriter( name.c_str(),
                                                    *node ) );
    return writer;
}

inline PointPrimitiveWriterPtr MakePointPrimitiveWriterPtr( const std::string &name,
                                                            TransformWriterPtr node )
{
    PointPrimitiveWriterPtr writer( new PointPrimitiveWriter( name.c_str(),
                                                              *node ) );
    return writer;
}

#ifdef DEBUG

#define FSD_THROW( TEXT ) abort()

#else

//-*****************************************************************************
#define FSD_THROW( TEXT )                         \
    do                                            \
    {                                             \
        std::stringstream sstr;                   \
        sstr << TEXT ;                            \
        std::runtime_error exc( sstr.str() );     \
        boost::throw_exception( exc );            \
    }                                             \
    while( 0 )

#endif

} // End namespace FluidSimDemo

#endif
