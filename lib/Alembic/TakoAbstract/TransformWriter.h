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

#ifndef _Alembic_TakoAbstract_TransformWriter_h_
#define _Alembic_TakoAbstract_TransformWriter_h_

#include <Alembic/TakoAbstract/Foundation.h>
#include <Alembic/TakoAbstract/HDFWriterNode.h>
#include <Alembic/TakoAbstract/TransformOperation.h>
#include <Alembic/TakoAbstract/SharedTypes.h>

namespace Alembic {

namespace TakoAbstract {

namespace TAKO_ABSTRACT_LIB_VERSION_NS {

//-*****************************************************************************
class TransformWriter : public HDFWriterNode
{
public:
    virtual ~TransformWriter();

    //-*************************************************************************
    // These are the regular TransformWriter functions
    //-*************************************************************************
    virtual void push( TransformOperationVariant iOp ) = 0;

    virtual void clear() = 0;
    
    virtual void writeTransformStack( float iFrame,
                                      bool inheritsTransform ) = 0;

    typedef std::vector<TransformOperationVariant>::const_iterator
    const_stack_iterator;

    virtual const_stack_iterator begin() const = 0;

    virtual const_stack_iterator end() const = 0;

    virtual void writeSamples( float iFrame,
                               const std::vector<double> & iSamples ) = 0;

    virtual Mat4x4 getMatrix() const = 0;

    //-*************************************************************************
    // Because this class family is purely abstract, we can't really construct
    // objects directly - it would require knowing the specific type at
    // compiletime, and would destroy the polymorphic basis for having this
    // abstract API.
    //
    // In the same way that the HDFReaderNode creates children nodes directly
    // and returns them to the client code via a ChildNodePtr, so too the
    // HDFWriterNode acts as the "class factory" for creating its own
    // children. This also allows for derived classes to optionally implement
    // journaling of all created instances.
    //
    // Use these functions exactly as you would use the constructors
    // for each of the writer functions. They each, generally, take a name.
    //-*************************************************************************

    virtual TransformWriterPtr
    createTransformChild( const std::string & iName ) = 0;

    virtual PolyMeshWriterPtr
    createPolyMeshChild( const std::string & iName ) = 0;

    virtual SubDWriterPtr
    createSubDChild( const std::string & iName ) = 0;

    virtual NurbsCurveWriterPtr
    createNurbsCurveChild( const std::string & iName ) = 0;

    virtual NurbsSurfaceWriterPtr
    createNurbsSurfaceChild( const std::string & iName ) = 0;

    virtual CameraWriterPtr
    createCameraChild( const std::string & iName ) = 0;

    virtual PointPrimitiveWriterPtr
    createPointPrimitiveChild( const std::string & iName ) = 0;

    virtual GenericNodeWriterPtr
    createGenericNodeChild( const std::string & iName ) = 0;

};

} // End namespace TAKO_ABSTRACT_LIB_VERSION_NS

} // End namespace TakoAbstract

} // End namespace Alembic

#endif  // _Alembic_TakoAbstract_TransformWriter_h_
