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
#include <ri.h>
#include "WriteGeo.h"
#include "SampleUtil.h"
#include "ArbAttrUtil.h"
#include "SubDTags.h"

//-*****************************************************************************
void ProcessSimpleTransform( ISimpleXform &xform, ProcArgs &args )
{
    ISimpleXformSchema &xs = xform.getSchema();

    const TimeSampling &ts = xs.getTimeSampling();

    SampleTimeSet sampleTimes;
    GetRelevantSampleTimes( args, ts, sampleTimes );

    bool multiSample = sampleTimes.size() > 1;

    if ( multiSample )
    {
        WriteMotionBegin( args, sampleTimes );
    }

    for ( SampleTimeSet::iterator iter = sampleTimes.begin();
          iter != sampleTimes.end(); ++iter )
    {
        ISampleSelector iss( *iter );

        SimpleXformSample sample = xs.getValue( iss );

        M44d m = sample.getMatrix();

        if ( ! multiSample && m == M44d() )
        {
            continue;
        }

        WriteConcatTransform( m );
    }

    if ( multiSample )
    {
        RiMotionEnd();
    }
}

//-*****************************************************************************
void ProcessXform( IXform &xform, ProcArgs &args )
{
    IXformSchema &xs = xform.getSchema();

    const TimeSampling &ts = xs.getTimeSampling();

    SampleTimeSet sampleTimes;
    GetRelevantSampleTimes( args, ts, sampleTimes );

    bool multiSample = sampleTimes.size() > 1;

    std::vector<XformSample> sampleVectors;
    sampleVectors.resize( sampleTimes.size() );

    //fetch all operators at each sample time first
    size_t sampleTimeIndex = 0;
    for ( SampleTimeSet::iterator I = sampleTimes.begin();
          I != sampleTimes.end(); ++I, ++sampleTimeIndex )
    {
        ISampleSelector sampleSelector( *I );

        xs.get( sampleVectors[sampleTimeIndex], sampleSelector );
    }

    //loop through the operators individually since a MotionBegin block
    //can enclose only homogenous statements
    for ( size_t i = 0, e = sampleVectors.front().getNum(); i < e; ++i )
    {
        if ( multiSample ) { WriteMotionBegin(args, sampleTimes); }

        for ( size_t j = 0; j < sampleVectors.size(); ++j )
        {
            XformDataPtr sample = sampleVectors[j].get(i);

            switch ( sample->getType() )
            {
            case kScaleOperation:
            {
                V3d value = ScaleData( sample ).get();
                RiScale( value.x, value.y, value.z );
                break;
            }
            case kTranslateOperation:
            {
                V3d value = TranslateData( sample ).get();
                RiTranslate( value.x, value.y, value.z );
                break;
            }
            case kRotateOperation:
            {
                RotateData rotateSample( sample );
                V3d axis = rotateSample.getAxis();
                // Xform stores rotation in radians, rman wants it in degrees
                float degrees = 180.0 * rotateSample.getAngle() / M_PI;
                RiRotate( degrees, axis.x, axis.y, axis.z );
                break;
            }
            case kMatrixOperation:
            {
                M44d m = MatrixData( sample ).get();
                WriteConcatTransform( m );
                break;
            }
            }
        }

        if ( multiSample ) { RiMotionEnd(); }
    }
}

//-*****************************************************************************
void ProcessPolyMesh( IPolyMesh &polymesh, ProcArgs &args )
{
    IPolyMeshSchema &ps = polymesh.getSchema();

    const TimeSampling &ts = ps.getTimeSampling();

    SampleTimeSet sampleTimes;
    GetRelevantSampleTimes( args, ts, sampleTimes );

    bool multiSample = sampleTimes.size() > 1;

    if ( multiSample ) { WriteMotionBegin( args, sampleTimes ); }


    for ( SampleTimeSet::iterator iter = sampleTimes.begin();
          iter != sampleTimes.end(); ++ iter )
    {

        ISampleSelector sampleSelector( *iter );

        IPolyMeshSchema::Sample sample = ps.getValue( sampleSelector );

        RtInt npolys = (RtInt) sample.getCounts()->size();

        ParamListBuilder ParamListBuilder;

        ParamListBuilder.add( "P", (RtPointer)sample.getPositions()->get() );

        std::set<std::string> excludeNames;
        excludeNames.insert( "P" );

        AddArbitraryProperties( ps, sampleSelector, ParamListBuilder,
                                &excludeNames );

        RiPointsPolygonsV(
            npolys,
            (RtInt*) sample.getCounts()->get(),
            (RtInt*) sample.getIndices()->get(),
            ParamListBuilder.n(),
            ParamListBuilder.nms(),
            ParamListBuilder.vals() );
    }

    if (multiSample) RiMotionEnd();

}

//-*****************************************************************************
void ProcessSubD( ISubD &subd, ProcArgs &args )
{
    ISubDSchema &ss = subd.getSchema();

    const TimeSampling &ts = ss.getTimeSampling();

    SampleTimeSet sampleTimes;
    GetRelevantSampleTimes( args, ts, sampleTimes );

    bool multiSample = sampleTimes.size() > 1;

    //include this code path for future expansion
    bool isHierarchicalSubD = false;

    if ( multiSample ) { WriteMotionBegin( args, sampleTimes ); }

    for ( SampleTimeSet::iterator iter = sampleTimes.begin();
          iter != sampleTimes.end(); ++iter )
    {
        ISampleSelector sampleSelector( *iter );

        ISubDSchema::Sample sample = ss.getValue( sampleSelector );

        RtInt npolys = (RtInt) sample.getFaceCounts()->size();

        ParamListBuilder ParamListBuilder;

        ParamListBuilder.add( "P", (RtPointer)sample.getPositions()->get() );

        std::set<std::string> excludeNames;
        excludeNames.insert( "P" );

        AddArbitraryProperties( ss, sampleSelector, ParamListBuilder,
                                &excludeNames );

        std::string subdScheme = sample.getSubdivisionScheme();

        SubDTagBuilder tags;

        ProcessFacevaryingInterpolateBoundry( tags, sample );
        ProcessInterpolateBoundry( tags, sample );
        ProcessFacevaryingPropagateCorners( tags, sample );
        ProcessHoles( tags, sample );
        ProcessCreases( tags, sample );
        ProcessCorners( tags, sample );

        if ( isHierarchicalSubD )
        {
            RiHierarchicalSubdivisionMeshV(
                const_cast<RtToken>( subdScheme.c_str() ),
                npolys,
                (RtInt*) sample.getFaceCounts()->get(),
                (RtInt*) sample.getFaceIndices()->get(),
                tags.nt(),
                tags.tags(),
                tags.nargs( true ),
                tags.intargs(),
                tags.floatargs(),
                tags.stringargs(),
                ParamListBuilder.n(),
                ParamListBuilder.nms(),
                ParamListBuilder.vals()
                                          );
        }
        else
        {
            RiSubdivisionMeshV(
                const_cast<RtToken>(subdScheme.c_str() ),
                npolys,
                (RtInt*) sample.getFaceCounts()->get(),
                (RtInt*) sample.getFaceIndices()->get(),
                tags.nt(),
                tags.tags(),
                tags.nargs( false ),
                tags.intargs(),
                tags.floatargs(),
                ParamListBuilder.n(),
                ParamListBuilder.nms(),
                ParamListBuilder.vals()
                              );
        }
    }

    if ( multiSample ) { RiMotionEnd(); }
}

//-*****************************************************************************
void WriteIdentifier( const ObjectHeader &ohead )
{
    std::string name = ohead.getFullName();
    name = name.substr( 4, name.size() - 1 ); //for now, shave off the /ABC
    char* nameArray[] = { const_cast<char*>( name.c_str() ), RI_NULL };

    RiAttribute(const_cast<char*>( "identifier" ), const_cast<char*>( "name" ),
                nameArray, RI_NULL );
}
