//-*****************************************************************************
//
// Copyright (c) 2012,
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

#include <Foundation.h>
//#include <TypedPropertyTraits.h>
#include <PyTypedArraySampleConverter.h>
#include <PyTypeBindingTraits.h>
#include <pybind11/stl.h>

using namespace py;

//-*****************************************************************************
#define DECLARE_TYPETRAITS_VECTOR( TPTraits, TPTraitsVector ) \
typedef std::vector<Abc::TPTraits::value_type> TPTraitsVector;  \
PYBIND11_MAKE_OPAQUE(TPTraitsVector);

// define Pybind11 std::vector for TPTraits
DECLARE_TYPETRAITS_VECTOR(BooleanTPTraits, BoolVector);
DECLARE_TYPETRAITS_VECTOR(Uint8TPTraits, UcharVector);
DECLARE_TYPETRAITS_VECTOR(Int8TPTraits, CharVector);
DECLARE_TYPETRAITS_VECTOR(Uint16TPTraits, UInt16Vector);
DECLARE_TYPETRAITS_VECTOR(Int16TPTraits, Int16Vector);
DECLARE_TYPETRAITS_VECTOR(Uint32TPTraits, UInt32Vector);
DECLARE_TYPETRAITS_VECTOR(Int32TPTraits, Int32Vector);
DECLARE_TYPETRAITS_VECTOR(Uint64TPTraits, UInt64Vector);
DECLARE_TYPETRAITS_VECTOR(Int64TPTraits, Int64Vector);
DECLARE_TYPETRAITS_VECTOR(Float16TPTraits, Float16Vector);
DECLARE_TYPETRAITS_VECTOR(Float32TPTraits, Float32Vector);
DECLARE_TYPETRAITS_VECTOR(Float64TPTraits, Float64Vector);
// DECLARE_TYPETRAITS_VECTOR(StringTPTraits, StringVector);
// DECLARE_TYPETRAITS_VECTOR(WstringTPTraits, WstringVector);

DECLARE_TYPETRAITS_VECTOR(V2sTPTraits, V2sVector);
DECLARE_TYPETRAITS_VECTOR(V2iTPTraits, V2iVector);
DECLARE_TYPETRAITS_VECTOR(V2fTPTraits, V2fVector);
DECLARE_TYPETRAITS_VECTOR(V2dTPTraits, V2dVector);
// DECLARE_TYPETRAITS_VECTOR(P2sTPTraits, P2sVector);
// DECLARE_TYPETRAITS_VECTOR(P2iTPTraits, P2iVector);
// DECLARE_TYPETRAITS_VECTOR(P2fTPTraits, P2fVector);
// DECLARE_TYPETRAITS_VECTOR(P2fTPTraits, P2dVector);
// DECLARE_TYPETRAITS_VECTOR(N2fTPTraits, N2fVector);
// DECLARE_TYPETRAITS_VECTOR(N2dTPTraits, N2dVector);

DECLARE_TYPETRAITS_VECTOR(C3cTPTraits, C3cVector);
DECLARE_TYPETRAITS_VECTOR(C3hTPTraits, C3hVector);
DECLARE_TYPETRAITS_VECTOR(C3fTPTraits, C3fVector);
DECLARE_TYPETRAITS_VECTOR(V3sTPTraits, V3sVector);
DECLARE_TYPETRAITS_VECTOR(V3iTPTraits, V3iVector);
// DECLARE_TYPETRAITS_VECTOR(V3fTPTraits, V3fVector);
DECLARE_TYPETRAITS_VECTOR(V3dTPTraits, V3dVector);
// DECLARE_TYPETRAITS_VECTOR(P3sTPTraits, P3sVector);
// DECLARE_TYPETRAITS_VECTOR(P3sTPTraits, P3sVector);
DECLARE_TYPETRAITS_VECTOR(P3fTPTraits, P3fVector);
// DECLARE_TYPETRAITS_VECTOR(P3dTPTraits, P3dVector);
// DECLARE_TYPETRAITS_VECTOR(N3fTPTraits, N3fVector);
// DECLARE_TYPETRAITS_VECTOR(N3dTPTraits, N3dVector);

DECLARE_TYPETRAITS_VECTOR(C4cTPTraits, C4cVector);
DECLARE_TYPETRAITS_VECTOR(C4hTPTraits, C4hVector);
DECLARE_TYPETRAITS_VECTOR(C4fTPTraits, C4fVector);

DECLARE_TYPETRAITS_VECTOR(Box2iTPTraits, Box2iVector);
DECLARE_TYPETRAITS_VECTOR(Box2sTPTraits, Box2sVector);
DECLARE_TYPETRAITS_VECTOR(Box2fTPTraits, Box2fVector);
DECLARE_TYPETRAITS_VECTOR(Box2dTPTraits, Box2dVector);
DECLARE_TYPETRAITS_VECTOR(QuatfTPTraits, QuatfVector);
DECLARE_TYPETRAITS_VECTOR(QuatdTPTraits, QuatdVector);

DECLARE_TYPETRAITS_VECTOR(Box3iTPTraits, Box3iVector);
DECLARE_TYPETRAITS_VECTOR(Box3sTPTraits, Box3sVector);
DECLARE_TYPETRAITS_VECTOR(Box3fTPTraits, Box3fVector);
DECLARE_TYPETRAITS_VECTOR(Box3dTPTraits, Box3dVector);

DECLARE_TYPETRAITS_VECTOR(M33fTPTraits, M33fVector);
DECLARE_TYPETRAITS_VECTOR(M33dTPTraits, M33dVector);

DECLARE_TYPETRAITS_VECTOR(M44fTPTraits, M44fVector);
DECLARE_TYPETRAITS_VECTOR(M44dTPTraits, M44dVector);

template <class TPTraits>
Abc::TypedArraySample<TPTraits> setVals(py::array& vals)
{
  typedef typename TPTraits::value_type value_type;
  typedef typename std::vector<value_type> value_vector;

  std::vector<value_type> array_vals(vals.size());
  std::memcpy(array_vals.data(), vals.data(), vals.size() * sizeof(value_type));
  Abc::TypedArraySample<TPTraits> array_sample(array_vals);
  return array_sample;

}

//-*****************************************************************************
#define REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR( module_handle, TPTraits, TPTraitsArraySample, TPTraitsVector )                                      \
{                                                                                         \
    typedef Abc::TPTraitsArraySample TypedArray;      \
    typedef Abc::TPTraits::value_type value_type;      \
    typedef typename std::vector<value_type> value_vector;\
    struct TPTraitsArraySampleOverloads \
    {\
          static const value_type &getOpByIndex(TypedArray typeArray, size_t iIndex )  \
          { \
            return typeArray[iIndex];  \
          } \
          static Abc::TypedArraySample<Abc::TPTraits> getObject(py::array vals)\
          {\
            AbcU::Dimensions dims(vals.shape()[0]);  \
            if( size_t(vals.shape()[0]) != size_t(vals.size()))  \
            { \
              std::vector<value_type> array_vals(vals.size()); \
              std::memcpy(array_vals.data(), vals.data(), vals.size() * sizeof(value_type));\
              Abc::TypedArraySample<Abc::TPTraits> array_sample(array_vals.data(), dims);\
              return array_sample;\
            } \
            else  \
            { \
              std::vector<value_type> array_vals(vals.size()); \
              std::memcpy(array_vals.data(), vals.data(), vals.size() * sizeof(value_type));\
              std::vector<value_type> array_vals1 = vals.cast<std::vector<value_type>>(); \
              Abc::TypedArraySample<Abc::TPTraits> array_sample(array_vals1);\
              return array_sample;\
            } \
          }\
    };\
    class_<TypedArray, std::shared_ptr<TypedArray>> ( module_handle, #TPTraitsArraySample ) \
    .def( init(&TPTraitsArraySampleOverloads::getObject), "Create Array Sample using the given value array" )  \
    .def( init< const value_vector& >(), arg("valueVector"), keep_alive<1,2>(), "Create Array Sample using the given value array" )  \
    .def( init< const value_vector&, const AbcU::Dimensions >(), arg("valueVector"), arg("dimensions"), "Create Array Sample using the given value array and dimensions" )  \
    .def( init< const AbcA::ArraySample& >(), arg("arraySample"), "Create Array Sample by Copying the given Array Sample")  \
    .def( init<>() ) \
    .def( "getObject", TPTraitsArraySampleOverloads::getObject)  \
    .def( "size", &TypedArray::size, "Return the size of the array sample" ) \
    .def( "__getitem__", &TypedArray::operator[], arg( "index" ), return_value_policy::reference_internal )\
    .def( ALEMBIC_PYTHON_BOOL_NAME, &AbcA::ArraySample::valid );\
    py::bind_vector<TPTraitsVector>(module_handle, #TPTraitsVector, py::module_local(true));\
}

//-*****************************************************************************
#define REGISTER_TPTRAITS_ARRAY_SAMPLE( module_handle, TPTraits, TPTraitsArraySample )                                      \
{                                                                                         \
    typedef Abc::TPTraitsArraySample TypedArray;      \
    typedef Abc::TPTraits::value_type value_type;      \
    typedef typename std::vector<value_type> value_vector;\
    struct TPTraitsArraySampleOverloads \
    {\
          static const value_type &getOpByIndex(TypedArray typeArray, size_t iIndex )  \
          { \
            return typeArray[iIndex];  \
          } \
          static Abc::TypedArraySample<Abc::TPTraits> getObject(py::array vals)\
          {\
            AbcU::Dimensions dims(vals.shape()[0]);  \
            if( size_t(vals.shape()[0]) != size_t(vals.size()))  \
            { \
              std::vector<value_type> array_vals(vals.size()); \
              std::memcpy(array_vals.data(), vals.data(), vals.size() * sizeof(value_type));\
              Abc::TypedArraySample<Abc::TPTraits> array_sample(array_vals.data(), dims);\
              return array_sample;\
            } \
            else  \
            { \
              std::vector<value_type> array_vals(vals.size()); \
              std::memcpy(array_vals.data(), vals.data(), vals.size() * sizeof(value_type));\
              std::vector<value_type> array_vals1 = vals.cast<std::vector<value_type>>(); \
              Abc::TypedArraySample<Abc::TPTraits> array_sample(array_vals1);\
              return array_sample;\
            } \
          }\
    };\
    class_<TypedArray,  std::shared_ptr<TypedArray>> ( module_handle, #TPTraitsArraySample ) \
    .def( init(&TPTraitsArraySampleOverloads::getObject), "Create Array Sample using the given value array" )  \
    .def( init< const value_vector& >(), arg("valueVector"), keep_alive<1,2>(), "Create Array Sample using the given value array" )  \
    .def( init< const value_vector&, const AbcU::Dimensions >(), arg("valueVector"), arg("dimensions"), "Create Array Sample using the given value array and dimensions" )  \
    .def( init< const AbcA::ArraySample& >(), arg("arraySample"), "Create Array Sample by Copying the given Array Sample")  \
    .def( init<>() ) \
    .def( "getObject", TPTraitsArraySampleOverloads::getObject)  \
    .def( "size", &TypedArray::size, "Return the size of the array sample" ) \
    .def( "__getitem__", &TypedArray::operator[], arg( "index" ), return_value_policy::reference_internal )\
    .def( ALEMBIC_PYTHON_BOOL_NAME, &AbcA::ArraySample::valid );\
}

//-*****************************************************************************
void register_typedarraysampleconverters(py::module_& module_handle)
{
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, BooleanTPTraits, BoolArraySample, BoolVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Uint8TPTraits, UcharArraySample, UcharVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Int8TPTraits, CharArraySample, CharVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Uint16TPTraits, UInt16ArraySample, UInt16Vector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Int16TPTraits, Int16ArraySample, Int16Vector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Uint32TPTraits, UInt32ArraySample, UInt32Vector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Int32TPTraits, Int32ArraySample, Int32Vector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Uint64TPTraits, UInt64ArraySample, UInt64Vector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Int64TPTraits, Int64ArraySample, Int64Vector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Float16TPTraits, HalfArraySample, Float16Vector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Float32TPTraits, FloatArraySample, Float32Vector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, Float64TPTraits, DoubleArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, StringTPTraits, StringArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, WstringTPTraits, WstringArraySample);

    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, V2sTPTraits, V2sArraySample, V2sVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, V2iTPTraits, V2iArraySample, V2iVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, V2fTPTraits, V2fArraySample, V2fVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, V2dTPTraits, V2dArraySample, V2dVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, P2sTPTraits, P2sArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, P2iTPTraits, P2iArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, P2fTPTraits, P2fArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, P2dTPTraits, P2dArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, N2fTPTraits, N2fArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, N2dTPTraits, N2dArraySample);
    //
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, C3cTPTraits, C3cArraySample, C3cVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, C3hTPTraits, C3hArraySample, C3hVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, C3fTPTraits, C3fArraySample, C3fVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, V3sTPTraits, V3sArraySample, V3sVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, V3iTPTraits, V3iArraySample, V3iVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, V3fTPTraits, V3fArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, V3dTPTraits, V3dArraySample, V3dVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, P3sTPTraits, P3sArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, P3iTPTraits, P3iArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, P3fTPTraits, P3fArraySample, P3fVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, P3dTPTraits, P3dArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, N3fTPTraits, N3fArraySample);
    REGISTER_TPTRAITS_ARRAY_SAMPLE(module_handle, N3dTPTraits, N3dArraySample);
    //
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, C4cTPTraits, C4cArraySample, C4cVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, C4hTPTraits, C4hArraySample, C4hVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, C4fTPTraits, C4fArraySample, C4fVector);
    //
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Box2sTPTraits, Box2sArraySample, Box2sVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Box2iTPTraits, Box2iArraySample, Box2iVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Box2fTPTraits, Box2fArraySample, Box2fVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Box2dTPTraits, Box2dArraySample, Box2dVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, QuatfTPTraits, QuatfArraySample, QuatfVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, QuatdTPTraits, QuatdArraySample, QuatdVector);
    //
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Box3sTPTraits, Box3sArraySample, Box3sVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Box3iTPTraits, Box3iArraySample, Box3iVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Box3fTPTraits, Box3fArraySample, Box3fVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, Box3dTPTraits, Box3dArraySample, Box3dVector);
    //
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, M33fTPTraits, M33fArraySample, M33fVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, M33dTPTraits, M33dArraySample, M33dVector);
    //
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, M44fTPTraits, M44fArraySample, M44fVector);
    REGISTER_TPTRAITS_ARRAY_SAMPLE_VECTOR(module_handle, M44dTPTraits, M44dArraySample, M44dVector);

    //typedef Alembic::Util::shared_ptr<Abc::FloatArraySample> FloatArraySamplePtr;

}
