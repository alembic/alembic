//-*****************************************************************************
//
// Copyright (c) 2009-2011,
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
void ProcessXform( IXform &xform, ProcArgs &args )
{
    IXformSchema &xs = xform.getSchema();

    TimeSamplingPtr ts = xs.getTimeSampling();

    size_t xformSamps = xs.getNumSamples();

    SampleTimeSet sampleTimes;
    GetRelevantSampleTimes( args, ts, xformSamps, sampleTimes );

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
    for ( size_t i = 0, e = xs.getNumOps(); i < e; ++i )
    {
        if ( multiSample ) { WriteMotionBegin(args, sampleTimes); }

        for ( size_t j = 0; j < sampleVectors.size(); ++j )
        {
            XformOp &op = sampleVectors[j][i];

            switch ( op.getType() )
            {
            case kScaleOperation:
            {
                V3d value = op.getScale();
                RiScale( value.x, value.y, value.z );
                break;
            }
            case kTranslateOperation:
            {
                V3d value = op.getTranslate();
                RiTranslate( value.x, value.y, value.z );
                break;
            }
            case kRotateOperation:
            case kRotateXOperation:
            case kRotateYOperation:
            case kRotateZOperation:
            {
                V3d axis = op.getAxis();
                float degrees = op.getAngle();
                RiRotate( degrees, axis.x, axis.y, axis.z );
                break;
            }
            case kMatrixOperation:
            {
                WriteConcatTransform( op.getMatrix() );
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

    TimeSamplingPtr ts = ps.getTimeSampling();

    SampleTimeSet sampleTimes;
    GetRelevantSampleTimes( args, ts, ps.getNumSamples(), sampleTimes );

    bool multiSample = sampleTimes.size() > 1;

    if ( multiSample ) { WriteMotionBegin( args, sampleTimes ); }


    for ( SampleTimeSet::iterator iter = sampleTimes.begin();
          iter != sampleTimes.end(); ++ iter )
    {

        ISampleSelector sampleSelector( *iter );

        IPolyMeshSchema::Sample sample = ps.getValue( sampleSelector );

        RtInt npolys = (RtInt) sample.getFaceCounts()->size();

        ParamListBuilder ParamListBuilder;

        ParamListBuilder.add( "P", (RtPointer)sample.getPositions()->get() );

        IV2fGeomParam uvParam = ps.getUVsParam();
        if ( uvParam.valid() )
        {
            ICompoundProperty parent = uvParam.getParent();
            
            AddGeomParamToParamListBuilder<IV2fGeomParam>(
                parent,
                uvParam.getHeader(),
                sampleSelector,
                "float",
                ParamListBuilder,
                2,
                "st");
        }
        IN3fGeomParam nParam = ps.getNormalsParam();
        if ( nParam.valid() )
        {
            ICompoundProperty parent = nParam.getParent();
            
            AddGeomParamToParamListBuilder<IN3fGeomParam>(
                parent,
                nParam.getHeader(),
                sampleSelector,
                "normal",
                ParamListBuilder);

        }



        ICompoundProperty arbGeomParams = ps.getArbGeomParams();
        AddArbitraryGeomParams( arbGeomParams,
                    sampleSelector, ParamListBuilder );

        RiPointsPolygonsV(
            npolys,
            (RtInt*) sample.getFaceCounts()->get(),
            (RtInt*) sample.getFaceIndices()->get(),
            ParamListBuilder.n(),
            ParamListBuilder.nms(),
            ParamListBuilder.vals() );
    }

    if (multiSample) RiMotionEnd();

}

//-*****************************************************************************
void ProcessSubD( ISubD &subd, ProcArgs &args, const std::string & facesetName )
{
    ISubDSchema &ss = subd.getSchema();

    TimeSamplingPtr ts = ss.getTimeSampling();

    SampleTimeSet sampleTimes;
    GetRelevantSampleTimes( args, ts, ss.getNumSamples(), sampleTimes );

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

        IV2fGeomParam uvParam = ss.getUVsParam();
        if ( uvParam.valid() )
        {
            ICompoundProperty parent = uvParam.getParent();
            
            AddGeomParamToParamListBuilder<IV2fGeomParam>(
                parent,
                uvParam.getHeader(),
                sampleSelector,
                "float",
                ParamListBuilder,
                2,
                "st");
        }

        ICompoundProperty arbGeomParams = ss.getArbGeomParams();
        AddArbitraryGeomParams( arbGeomParams,
                    sampleSelector, ParamListBuilder );

        std::string subdScheme = sample.getSubdivisionScheme();

        SubDTagBuilder tags;

        ProcessFacevaryingInterpolateBoundry( tags, sample );
        ProcessInterpolateBoundry( tags, sample );
        ProcessFacevaryingPropagateCorners( tags, sample );
        ProcessHoles( tags, sample );
        ProcessCreases( tags, sample );
        ProcessCorners( tags, sample );
        
        if ( !facesetName.empty() )
        {
            if ( ss.hasFaceSet( facesetName ) )
            {
                IFaceSet faceSet = ss.getFaceSet( facesetName );
                IFaceSetSchema::Sample faceSetSample = 
                        faceSet.getSchema().getValue( sampleSelector );
                
                std::set<int> facesToKeep;
                
                facesToKeep.insert( faceSetSample.getFaces()->get(),
                        faceSetSample.getFaces()->get() +
                                faceSetSample.getFaces()->size() );
                
                for ( int i = 0; i < npolys; ++i )
                {
                    if ( facesToKeep.find( i ) == facesToKeep.end() )
                    {
                        tags.add( "hole" );
                        tags.addIntArg( i );
                    }
                }
            }
        }


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
void ProcessNuPatch( INuPatch &patch, ProcArgs &args )
{
    INuPatchSchema &ps = patch.getSchema();
    
    TimeSamplingPtr ts = ps.getTimeSampling();
    
    SampleTimeSet sampleTimes;
    GetRelevantSampleTimes( args, ts, ps.getNumSamples(), sampleTimes );
    
    
    //trim curves are described outside the motion blocks
    if ( ps.hasTrimCurve() )
    {
        //get the current time sample independent of any shutter values
        INuPatchSchema::Sample sample = ps.getValue(
                ISampleSelector( args.frame / args.fps ) );
        
        RiTrimCurve( sample.getTrimNumCurves()->size(), //numloops
                (RtInt*) sample.getTrimNumCurves()->get(),
                (RtInt*) sample.getTrimOrders()->get(),
                (RtFloat*) sample.getTrimKnots()->get(),
                (RtFloat*) sample.getTrimMins()->get(),
                (RtFloat*) sample.getTrimMaxes()->get(),
                (RtInt*) sample.getTrimNumVertices()->get(),
                (RtFloat*) sample.getTrimU()->get(),
                (RtFloat*) sample.getTrimV()->get(),
                (RtFloat*) sample.getTrimW()->get() );
    }
    
    
    bool multiSample = sampleTimes.size() > 1;
    
    if ( multiSample ) { WriteMotionBegin( args, sampleTimes ); }
    
    for ( SampleTimeSet::iterator iter = sampleTimes.begin();
          iter != sampleTimes.end(); ++iter )
    {
        ISampleSelector sampleSelector( *iter );
        
        INuPatchSchema::Sample sample = ps.getValue( sampleSelector );
        
        
        ParamListBuilder ParamListBuilder;
        
        //build this here so that it's still in scope when RiNuPatchV is
        //called.
        std::vector<RtFloat> pwValues;
        
        if ( sample.getPositionWeights() )
        {
            if ( sample.getPositionWeights()->size() == sample.getPositions()->size() )
            {
                //need to combine P with weight form Pw
                pwValues.reserve( sample.getPositions()->size() * 4 );
                
                const float32_t * pStart = reinterpret_cast<const float32_t * >(
                        sample.getPositions()->get() );
                const float32_t * wStart = reinterpret_cast<const float32_t * >(
                        sample.getPositionWeights()->get() );
                
                for ( size_t i = 0, e = sample.getPositionWeights()->size();
                        i < e;  ++i )
                {
                    pwValues.push_back( pStart[i*3] );
                    pwValues.push_back( pStart[i*3+1] );
                    pwValues.push_back( pStart[i*3+2] );
                    pwValues.push_back( wStart[i] );
                }
                
                ParamListBuilder.add( "Pw", (RtPointer) &pwValues[0] );
            }
        }
        
        if ( pwValues.empty() )
        {
            //no Pw so go straight with P
            ParamListBuilder.add( "P",
                    (RtPointer)sample.getPositions()->get() );
        }
        
        ICompoundProperty arbGeomParams = ps.getArbGeomParams();
        AddArbitraryGeomParams( arbGeomParams,
                    sampleSelector, ParamListBuilder );
        
        //For now, use the last knot value for umin and umax as it's
        //not described in the alembic data 
        
        RiNuPatchV(
                sample.getNumU(),
                sample.getUOrder(),
                (RtFloat *) sample.getUKnot()->get(),
                0.0, //umin
                sample.getUKnot()->get()[sample.getUKnot()->size()-1],//umax
                sample.getNumV(),
                sample.getVOrder(),
                (RtFloat *) sample.getVKnot()->get(),
                0.0, //vmin
                sample.getVKnot()->get()[sample.getVKnot()->size()-1], //vmax
                ParamListBuilder.n(),
                ParamListBuilder.nms(),
                ParamListBuilder.vals() );
    }
    
    if ( multiSample ) { RiMotionEnd(); }
    
    
}

//-*****************************************************************************
void ProcessPoints( IPoints &points, ProcArgs &args )
{
    IPointsSchema &ps = points.getSchema();
    TimeSamplingPtr ts = ps.getTimeSampling();
    
    SampleTimeSet sampleTimes;
    
    //for now, punt on the changing point count case -- even for frame ranges
    //for which the point count isn't changing
    
    if ( ps.getIdsProperty().isConstant() )
    {
        //grab only the current time
        sampleTimes.insert( args.frame / args.fps );
    }
    else
    {
         GetRelevantSampleTimes( args, ts, ps.getNumSamples(), sampleTimes );
    }
    
    
    bool multiSample = sampleTimes.size() > 1;
    
    if ( multiSample ) { WriteMotionBegin( args, sampleTimes ); }
    
    for ( SampleTimeSet::iterator iter = sampleTimes.begin();
          iter != sampleTimes.end(); ++iter )
    {
        ISampleSelector sampleSelector( *iter );
        
        IPointsSchema::Sample sample = ps.getValue( sampleSelector );
        
        
        ParamListBuilder ParamListBuilder;
        ParamListBuilder.add( "P", (RtPointer)sample.getPositions()->get() );
        
        ICompoundProperty arbGeomParams = ps.getArbGeomParams();
        AddArbitraryGeomParams( arbGeomParams,
                    sampleSelector, ParamListBuilder );
        
        RiPointsV(sample.getPositions()->size(),
                ParamListBuilder.n(),
                ParamListBuilder.nms(),
                ParamListBuilder.vals() );
    }
    
    if ( multiSample ) { RiMotionEnd(); }
    
}

//-*****************************************************************************
void ProcessCurves( ICurves &curves, ProcArgs &args )
{
    ICurvesSchema &cs = curves.getSchema();
    TimeSamplingPtr ts = cs.getTimeSampling();
    
    SampleTimeSet sampleTimes;
    
    GetRelevantSampleTimes( args, ts, cs.getNumSamples(), sampleTimes );
    
    bool multiSample = sampleTimes.size() > 1;
    
    bool firstSample = true;
    
    for ( SampleTimeSet::iterator iter = sampleTimes.begin();
          iter != sampleTimes.end(); ++iter )
    {
        ISampleSelector sampleSelector( *iter );
        
        ICurvesSchema::Sample sample = cs.getValue( sampleSelector );
        
        //need to set the basis prior to the MotionBegin block
        if ( firstSample )
        {
            firstSample = false;
            
            BasisType basisType = sample.getBasis();
            if ( basisType != kNoBasis )
            {
                RtBasis * basis = NULL;
                RtInt step = 0;
                
                switch ( basisType )
                {
                case kBezierBasis:
                    basis = &RiBezierBasis;
                    step = RI_BEZIERSTEP;
                    break;
                case kBsplineBasis:
                    basis = &RiBSplineBasis;
                    step = RI_BSPLINESTEP;
                    break;
                case kCatmullromBasis:
                    basis = &RiCatmullRomBasis;
                    step = RI_CATMULLROMSTEP;
                    break;
                case kHermiteBasis:
                    basis = &RiHermiteBasis;
                    step = RI_HERMITESTEP;
                    break;
                case kPowerBasis:
                    basis = &RiPowerBasis;
                    step = RI_POWERSTEP;
                    break;
                default:
                    break;
                }
                
                if ( basis != NULL )
                {
                    RiBasis( *basis, step, *basis, step);
                }
            }
            
            
            if ( multiSample ) { WriteMotionBegin( args, sampleTimes ); }
        }
        
        ParamListBuilder ParamListBuilder;
        ParamListBuilder.add( "P", (RtPointer)sample.getPositions()->get() );
        
        IFloatGeomParam widthParam = cs.getWidthsParam();
        if ( widthParam.valid() )
        {
            ICompoundProperty parent = widthParam.getParent();
            
            //prman requires "width" to be named "constantwidth" when
            //constant instead of declared as "constant float width".
            //It's even got an error message specifically for it.
            std::string widthName;
            if ( widthParam.getScope() == kConstantScope ||
                    widthParam.getScope() == kUnknownScope )
            {
                widthName = "constantwidth";
            }
            else
            {
                widthName = "width";
            }
            
            AddGeomParamToParamListBuilder<IFloatGeomParam>(
                parent,
                widthParam.getHeader(),
                sampleSelector,
                "float",
                ParamListBuilder,
                1,
                widthName);
        }
        
        IN3fGeomParam nParam = cs.getNormalsParam();
        if ( nParam.valid() )
        {
            ICompoundProperty parent = nParam.getParent();
            
            AddGeomParamToParamListBuilder<IN3fGeomParam>(
                parent,
                nParam.getHeader(),
                sampleSelector,
                "normal",
                ParamListBuilder);
        }
        
        IV2fGeomParam uvParam = cs.getUVsParam();
        if ( uvParam.valid() )
        {
            ICompoundProperty parent = uvParam.getParent();
            
            AddGeomParamToParamListBuilder<IV2fGeomParam>(
                parent,
                uvParam.getHeader(),
                sampleSelector,
                "float",
                ParamListBuilder,
                2,
                "st");
        }

        ICompoundProperty arbGeomParams = cs.getArbGeomParams();
        AddArbitraryGeomParams( arbGeomParams,
                    sampleSelector, ParamListBuilder );
        
        RtToken curveType;
        switch ( sample.getType() )
        {
        case kCubic:
            curveType = const_cast<RtToken>( "cubic" );
            break;
        default:
            curveType = const_cast<RtToken>( "linear" );
        }
        
        
        RtToken wrap;
        switch ( sample.getWrap() )
        {
        case kPeriodic:
            wrap = const_cast<RtToken>( "periodic" );
            break;
        default:
            wrap = const_cast<RtToken>( "nonperiodic" );
        }

        RiCurvesV(curveType,
                sample.getNumCurves(),
                (RtInt*) sample.getCurvesNumVertices()->get(),
                wrap,
                ParamListBuilder.n(),
                ParamListBuilder.nms(),
                ParamListBuilder.vals() );

    }
    
    if ( multiSample ) { RiMotionEnd(); }
    
}



//-*****************************************************************************
void WriteIdentifier( const ObjectHeader &ohead )
{
    std::string name = ohead.getFullName();
    char* nameArray[] = { const_cast<char*>( name.c_str() ), RI_NULL };

    RiAttribute(const_cast<char*>( "identifier" ), const_cast<char*>( "name" ),
                nameArray, RI_NULL );
}
