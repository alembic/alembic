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
#include <PyOGeomBaseSchema.h>
#include <PyOSchemaObject.h>

using namespace boost::python;

class SampleDerived : public AbcG::OPointsSchema::Sample
{
    public:
        typedef AbcG::OPointsSchema::Sample super;

        SampleDerived() : super() {}

        SampleDerived( const Abc::P3fArraySample &iPos,
                       const Abc::V3fArraySample &iVelocities =
                       Abc::V3fArraySample(),
                       const AbcG::OFloatGeomParam::Sample &iWidth =
                       AbcG::OFloatGeomParam::Sample() ) :
        super( iPos, iVelocities, iWidth ) {}

        SampleDerived( const Abc::P3fArraySample &iPos,
                       const Abc::UInt64ArraySamplePtr &iIdPtr,
                       const Abc::V3fArraySample &iVelocities =
                       Abc::V3fArraySample(),
                       const AbcG::OFloatGeomParam::Sample &iWidth =
                       AbcG::OFloatGeomParam::Sample() ) :
        super( iPos, *iIdPtr, iVelocities, iWidth )
        {
            m_idsPtr = iIdPtr;
        }

        void setIds( const Abc::UInt64ArraySamplePtr &iSampPtr )
        {
            m_idsPtr = iSampPtr;
            m_ids = *m_idsPtr;
        }

    private:
        Abc::UInt64ArraySamplePtr m_idsPtr;
};

void register_opoints()
{
    // OPoints
    //
    register_OSchemaObject<AbcG::OPoints>( "OPoints" );

    // OGeomBaseSchema
    //
    register_OGeomBaseSchema<AbcG::PointsSchemaInfo>( "OGeomBaseSchema_Points" );

    // overloads for OPointsSchema
    //
    void ( AbcG::OPointsSchema::*setTimeSamplingByIndex )( AbcU::uint32_t )
        = &AbcG::OPointsSchema::setTimeSampling;
    void ( AbcG::OPointsSchema::*setTimeSamplingByTimeSamplingPtr )
        ( AbcA::TimeSamplingPtr ) = &AbcG::OPointsSchema::setTimeSampling;

    struct Overloads
    {
        static void set( AbcG::OPointsSchema& iSchema, SampleDerived& iSamp )
        {
            AbcG::OPointsSchema::Sample samp( iSamp.getPositions(),
                                              iSamp.getIds(),
                                              iSamp.getVelocities(),
                                              iSamp.getWidths() );

            samp.setSelfBounds( iSamp.getSelfBounds() );

            iSchema.set( samp );
        }
    };

    // OPointsSchema
    //
    class_<AbcG::OPointsSchema,
           bases<AbcG::OGeomBaseSchema<AbcG::PointsSchemaInfo> > >(
          "OPointsSchema",
          "The OPointsSchema class is a points schema writer",
          init<>() )
        .def( "getNumSamples",
              &AbcG::OPointsSchema::getNumSamples )
        .def( "set",
              &Overloads::set,
              ( arg( "iSamp" ) ) )
        .def( "setFromPrevious",
              &AbcG::OPointsSchema::setFromPrevious )
        .def( "setTimeSampling",
              setTimeSamplingByIndex,
              ( arg( "index" ) ) )
        .def( "setTimeSampling",
              setTimeSamplingByTimeSamplingPtr,
              ( arg( "TimeSampling" ) ) )
        .def( "valid", &AbcG::OPointsSchema::valid )
        .def( "reset", &AbcG::OPointsSchema::reset )
        .def( "__nonzero__", &AbcG::OPointsSchema::valid )
        ;

    // OPointsSchema::Sample
    //
    class_<SampleDerived>( "OPointsSchemaSample", init<>() )
        .def( init<const Abc::P3fArraySample,
                   const Abc::V3fArraySample,
                   const AbcG::OFloatGeomParam::Sample>(
                   ( arg( "iPos" ),
                     arg( "iVelocities" ) = Abc::V3fArraySample(),
                     arg( "iWidths" ) = AbcG::OFloatGeomParam::Sample() ) )
                   [with_custodian_and_ward<1,2,
                    with_custodian_and_ward<1,3> >()] )
        .def( init<const Abc::P3fArraySample,
                   const Abc::UInt64ArraySamplePtr,
                   const Abc::V3fArraySample,
                   const AbcG::OFloatGeomParam::Sample>(
                   ( arg( "iPos" ),
                     arg( "iId" ),
                     arg( "iVelocities" ) = Abc::V3fArraySample(),
                     arg( "iWidths" ) = AbcG::OFloatGeomParam::Sample() ) )
                   [with_custodian_and_ward<1,2,
                    with_custodian_and_ward<1,3,
                    with_custodian_and_ward<1,4> > >()] )
        .def( "getPositions",
              &SampleDerived::getPositions,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >()
            )
        .def( "setPositions",
              &SampleDerived::setPositions,
              ( arg( "positions" ) ),
              with_custodian_and_ward<1,2>()
            )
        .def( "getIds",
              &SampleDerived::getIds,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >()
            )
        .def( "setIds",
              &SampleDerived::setIds,
              ( arg( "ids" ) ),
              with_custodian_and_ward<1,2>()
            )
        .def( "getVelocities",
              &SampleDerived::getVelocities,
              return_value_policy<copy_const_reference,
                                  with_custodian_and_ward_postcall<0,1> >()
            )
        .def( "setVelocities",
              &SampleDerived::setVelocities,
              ( arg( "velocities" ) ),
              with_custodian_and_ward<1,2>()
            )
        .def( "getWidths",
              &SampleDerived::getWidths,
              return_internal_reference<>() )
        .def( "setWidths",
              &SampleDerived::setWidths,
              ( arg( "width" ) )
            )
        .def( "getSelfBounds",
              &SampleDerived::getSelfBounds,
              return_internal_reference<>() )
        .def( "setSelfBounds",
              &SampleDerived::setSelfBounds,
              ( arg( "bounds" ) )
            )
        .def( "reset", &SampleDerived::reset )
        ;
}
