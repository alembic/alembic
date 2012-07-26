//-*****************************************************************************
//
// Copyright (c) 2009-2012,
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

#include <boost/python/implicit.hpp>

using namespace boost::python;

//-*****************************************************************************
void register_abcgeomtypes()
{
    // MeshTopologyVariance
    //
    enum_<AbcG::MeshTopologyVariance>( "MeshTopologyVariance" )
        .value( "kConstantTopology", AbcG::kConstantTopology )
        .value( "kHomogenousTopology", AbcG::kHomogenousTopology )
        .value( "kHeterogenousTopology", AbcG::kHeterogenousTopology )
        ;

    // XformOperationType
    //
    enum_<AbcG::XformOperationType>( "XformOperationType" )
        .value( "kScaleOperation", AbcG::kScaleOperation )
        .value( "kTranslateOperation", AbcG::kTranslateOperation )
        .value( "kRotateOperation", AbcG::kRotateOperation )
        .value( "kMatrixOperation", AbcG::kMatrixOperation )
        .value( "kRotateXOperation", AbcG::kRotateXOperation )
        .value( "kRotateYOperation", AbcG::kRotateYOperation )
        .value( "kRotateZOperation", AbcG::kRotateZOperation )
        ;

    // FilmBackXformOperationType
    //
    enum_<AbcG::FilmBackXformOperationType>( "FilmBackXformOperationType" )
        .value( "kScaleFilmBackOperation", AbcG::kScaleFilmBackOperation )
        .value( "kTranslateFilmBackOperation", AbcG::kTranslateFilmBackOperation )
        .value( "kMatrixFilmBackOperation", AbcG::kMatrixFilmBackOperation )
        ;

    // GeometryScope
    //
    enum_<AbcG::GeometryScope>( "GeometryScope" )
        .value( "kConstantScope", AbcG::kConstantScope )
        .value( "kUniformScope", AbcG::kUniformScope )
        .value( "kVaryingScope", AbcG::kVaryingScope )
        .value( "kVertexScope", AbcG::kVertexScope )
        .value( "kFacevaryingScope", AbcG::kFacevaryingScope )
        .value( "kUnknownScope", AbcG::kUnknownScope )
        ;

    // CurvePeriodicity
    //
    enum_<AbcG::CurvePeriodicity>( "CurvePeriodicity" )
        .value( "kNonPeriodic", AbcG::kNonPeriodic )
        .value( "kPeriodic", AbcG::kPeriodic)
        ;

    // CurveType
    //
    enum_<AbcG::CurveType>( "CurveType" )
        .value( "kCubic", AbcG::kCubic )
        .value( "kLinear", AbcG::kLinear )
        ;

    // BasisType
    enum_<AbcG::BasisType>( "BasisType" )
        .value( "kNoBasis", AbcG::kNoBasis )
        .value( "kBezierBasis", AbcG::kBezierBasis )
        .value( "kBsplineBasis", AbcG::kBsplineBasis )
        .value( "kCatmullromBasis", AbcG::kCatmullromBasis )
        .value( "kHermiteBasis", AbcG::kHermiteBasis )
        .value( "kPowerBasis", AbcG::kPowerBasis )
        ;

    // FaceSetExclusivity
    enum_<AbcG::FaceSetExclusivity>( "FaceSetExclusivity" )
        .value( "kFaceSetNonExclusive", AbcG::kFaceSetNonExclusive )
        .value( "kFaceSetExclusive", AbcG::kFaceSetExclusive )
        ;
}

