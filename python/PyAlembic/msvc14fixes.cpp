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

// Linker fix for msvc14 update 3: missing reference(s) to get_pointer()
// boost::python bug report
// https://github.com/boostorg/python/issues/116

// msvc14 update 3
#if ( defined(_MSC_VER) && (_MSC_VER > 1800) && (_MSC_FULL_VER > 190023918) )

#include <Foundation.h>

#define DEFINE_GET_POINTER_MSVC14( X )                                            \
namespace boost                                                                   \
{                                                                                 \
    template <>                                                                   \
    X const volatile * get_pointer<X const volatile >( X const volatile *c )      \
    {                                                                             \
        return c;                                                                 \
    }                                                                             \
}

DEFINE_GET_POINTER_MSVC14( class AbcG::ISubDSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::OSubDSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::IXformSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::OXformSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::IPointsSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::OPointsSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::IPolyMeshSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::OPolyMeshSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::INuPatchSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::ONuPatchSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::IGeomBase )
DEFINE_GET_POINTER_MSVC14( class AbcG::ILightSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::OLightSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::IFaceSetSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::ICurvesSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::OCurvesSchema )
DEFINE_GET_POINTER_MSVC14( class AbcC::ICollectionsSchema )
DEFINE_GET_POINTER_MSVC14( class AbcC::OCollectionsSchema )  
DEFINE_GET_POINTER_MSVC14( class AbcG::ICameraSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::OCameraSchema )
DEFINE_GET_POINTER_MSVC14( class AbcM::IMaterialSchema )
DEFINE_GET_POINTER_MSVC14( class AbcM::OMaterialSchema )
DEFINE_GET_POINTER_MSVC14( class AbcG::OFaceSetSchema )
DEFINE_GET_POINTER_MSVC14( class Abc::OSchemaObject<class AbcG::OFaceSetSchema> )

#endif // msvc14 update 3
