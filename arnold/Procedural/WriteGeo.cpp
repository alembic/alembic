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

#include "WriteGeo.h"
#include "ArbGeomParams.h"

#include <ai.h>
#include <sstream>

//-*****************************************************************************

#if AI_VERSION_ARCH_NUM == 3
    #if AI_VERSION_MAJOR_NUM < 4
        #define AiNodeGetNodeEntry(node)   ((node)->base_node)
    #endif
#endif

bool nodeHasParameter( struct AtNode * node, const std::string & paramName)
{
    return AiNodeEntryLookUpParameter( AiNodeGetNodeEntry( node ),
            paramName.c_str() ) != NULL;
}


//-*****************************************************************************

void ApplyTransformation( struct AtNode * node, 
        MatrixSampleMap * xformSamples, ProcArgs &args )
{
    if ( !node || !xformSamples || xformSamples->empty() )
    {
        return;
    }
    
    // confirm that this node has a parameter
    if ( !nodeHasParameter( node, "matrix" ) )
    {
        return;
    }
    
    // check to see that we're not a single identity matrix
    if (xformSamples->size() == 1 &&
            xformSamples->begin()->second == Imath::M44d())
    {
        return;
    }
    
    
    std::vector<float> sampleTimes;
    sampleTimes.reserve(xformSamples->size());
    
    std::vector<float> mlist;
    mlist.reserve( 16* xformSamples->size() );
    
    for ( MatrixSampleMap::iterator I = xformSamples->begin();
            I != xformSamples->end(); ++I )
    {
        // build up a vector of relative sample times to feed to 
        // "transform_time_samples" or "time_samples"
        sampleTimes.push_back( GetRelativeSampleTime(args, (*I).first) );
        
        
        for (int i = 0; i < 16; i++)
        {
            mlist.push_back( (*I).second.getValue()[i] );
        }
    }
    
    AiNodeSetArray(node, "matrix",
                ArrayConvert(1, xformSamples->size(),
                        AI_TYPE_MATRIX, &mlist[0]));
    
    
    if ( sampleTimes.size() > 1 )
    {
        // persp_camera calls it time_samples while the primitives call it
        // transform_time_samples
        if ( nodeHasParameter( node, "transform_time_samples" ) )
        {
            AiNodeSetArray(node, "transform_time_samples",
                            ArrayConvert(sampleTimes.size(), 1,
                                    AI_TYPE_FLOAT, &sampleTimes[0]));
        }
        else if ( nodeHasParameter( node, "time_samples" ) )
        {
            AiNodeSetArray(node, "time_samples",
                            ArrayConvert(sampleTimes.size(), 1,
                                    AI_TYPE_FLOAT, &sampleTimes[0]));
        }
        else
        {
            //TODO, warn if neither is present? Should be there in all
            //commercial versions of arnold by now.
        }
    }
}

//-*****************************************************************************


template <typename geomParamT>
void ProcessIndexedBuiltinParam(
        geomParamT param,
        const SampleTimeSet & sampleTimes,
        std::vector<float> & values,
        std::vector<AtUInt32> & idxs,
        size_t elementSize)
{
    if ( !param.valid() ) { return; }
    
    bool isFirstSample = true;
    for ( SampleTimeSet::iterator I = sampleTimes.begin();
          I != sampleTimes.end(); ++I, isFirstSample = false)
    {
        ISampleSelector sampleSelector( *I );
        
        
        switch ( param.getScope() )
        {
        case kVaryingScope:
        case kVertexScope:
        {
            // a value per-point, idxs should be the same as vidxs
            // so we'll leave it empty
            
            // we'll get the expanded form here
            typename geomParamT::Sample sample = param.getExpandedValue(
                    sampleSelector);
            
            size_t footprint = sample.getVals()->size() * elementSize;
            
            values.reserve( values.size() + footprint );
            values.insert( values.end(),
                    (float32_t*) sample.getVals()->get(),
                    ((float32_t*) sample.getVals()->get()) + footprint );
            
            break;
        }
        case kFacevaryingScope:
        {
            // get the indexed form and feed to nidxs
            
            typename geomParamT::Sample sample = param.getIndexedValue(
                    sampleSelector);
            
            if ( isFirstSample )
            {
                idxs.reserve( sample.getIndices()->size() );
                idxs.insert( idxs.end(),
                        sample.getIndices()->get(),
                        sample.getIndices()->get() +
                                sample.getIndices()->size() );
            }
            
            size_t footprint = sample.getVals()->size() * elementSize;
            values.reserve( values.size() + footprint );
            values.insert( values.end(),
                    (const float32_t*) sample.getVals()->get(),
                    ((const float32_t*) sample.getVals()->get()) + footprint );
            
            break;
        }
        default:
            break;
        }
        
        
    }
    
    
}

//-*****************************************************************************

namespace
{
    // Arnold scene build is single-threaded so we don't have to lock around
    // access to this for now.
    typedef std::map<std::string, AtNode *> NodeCache;
    NodeCache g_meshCache;
}


//-*************************************************************************
// This is templated to handle shared behavior of IPolyMesh and ISubD

// We send in our empty sampleTimes and vidxs because polymesh needs those
// for processing animated normal.


// The return value is the polymesh node. If instanced, it will be returned
// for the first created instance only.
template <typename primT>
AtNode * ProcessPolyMeshBase(
        primT & prim, ProcArgs & args,
        SampleTimeSet & sampleTimes,
        std::vector<AtUInt32> & vidxs,
        int subdiv_iterations,
        MatrixSampleMap * xformSamples, 
        const std::string & facesetName = "" )
{
    if ( !prim.valid() )
    {
        return NULL;
    }
    
    typename primT::schema_type  &ps = prim.getSchema();
    TimeSamplingPtr ts = ps.getTimeSampling();
    
    if ( ps.getTopologyVariance() != kHeterogenousTopology )
    {
        GetRelevantSampleTimes( args, ts, ps.getNumSamples(), sampleTimes );
    }
    else
    {
        sampleTimes.insert( args.frame / args.fps );
    }
    
    std::string name = args.nameprefix + prim.getFullName();
    
    AtNode * instanceNode = NULL;
    
    std::string cacheId;
    
    if ( args.makeInstance )
    {
        std::ostringstream buffer;
        AbcA::ArraySampleKey sampleKey;
        
        
        for ( SampleTimeSet::iterator I = sampleTimes.begin();
                I != sampleTimes.end(); ++I )
        {
            ISampleSelector sampleSelector( *I );
            ps.getPositionsProperty().getKey(sampleKey, sampleSelector);
            
            buffer << GetRelativeSampleTime( args, (*I) ) << ":";
            sampleKey.digest.print(buffer);
            buffer << ":";
        }
        
        buffer << "@" << subdiv_iterations;
        buffer << "@" << facesetName;
        
        cacheId = buffer.str();
        
        instanceNode = AiNode( "ginstance" );
        AiNodeSetStr( instanceNode, "name", name.c_str() );
        args.createdNodes.push_back(instanceNode);
        
        if ( args.proceduralNode )
        {
            AiNodeSetInt( instanceNode, "visibility",
                    AiNodeGetInt( args.proceduralNode, "visibility" ) );
        
        }
        else
        {
            AiNodeSetInt( instanceNode, "visibility", AI_RAY_ALL );
        }
        
        ApplyTransformation( instanceNode, xformSamples, args );
        
        
        NodeCache::iterator I = g_meshCache.find(cacheId);
        if ( I != g_meshCache.end() )
        {
            AiNodeSetPtr(instanceNode, "node", (*I).second );
            return NULL;
        }
        
    }
    
    
    
    SampleTimeSet singleSampleTimes;
    singleSampleTimes.insert( args.frame / args.fps );
    
    
    std::vector<AtByte> nsides;
    std::vector<float> vlist;
    
    std::vector<float> uvlist;
    std::vector<AtUInt32> uvidxs;
    
    
    // POTENTIAL OPTIMIZATIONS LEFT TO THE READER
    // 1) vlist needn't be copied if it's a single sample
    
    bool isFirstSample = true;
    for ( SampleTimeSet::iterator I = sampleTimes.begin();
          I != sampleTimes.end(); ++I, isFirstSample = false)
    {
        ISampleSelector sampleSelector( *I );
        typename primT::schema_type::Sample sample = ps.getValue( sampleSelector );
        
        if ( isFirstSample )
        {
            size_t numPolys = sample.getFaceCounts()->size();
            nsides.reserve( sample.getFaceCounts()->size() );
            for ( size_t i = 0; i < numPolys; ++i ) 
            {
                int32_t n = sample.getFaceCounts()->get()[i];
                
                if ( n > 255 )
                {
                    // TODO, warning about unsupported face
                    return NULL;
                }
                
                nsides.push_back( (AtByte) n );
            }
            
            size_t vidxSize = sample.getFaceIndices()->size();
            vidxs.reserve( vidxSize );
            vidxs.insert( vidxs.end(), sample.getFaceIndices()->get(),
                    sample.getFaceIndices()->get() + vidxSize );
        }
        
        
        vlist.reserve( vlist.size() + sample.getPositions()->size() * 3);
        vlist.insert( vlist.end(),
                (const float32_t*) sample.getPositions()->get(),
                ((const float32_t*) sample.getPositions()->get()) +
                        sample.getPositions()->size() * 3 );
    }
    
    ProcessIndexedBuiltinParam(
            ps.getUVsParam(),
            singleSampleTimes,
            uvlist,
            uvidxs,
            2);
    
    
    AtNode* meshNode = AiNode( "polymesh" );
    
    if (!meshNode)
    {
        AiMsgError("Failed to make polymesh node for %s",
                prim.getFullName().c_str());
        return NULL;
    }
    
    args.createdNodes.push_back(meshNode);
    
    if ( instanceNode != NULL)
    {
        AiNodeSetStr( meshNode, "name", (name + ":src").c_str() );
    }
    else
    {
        AiNodeSetStr( meshNode, "name", name.c_str() );
    }
    
    
    
    
    AiNodeSetArray(meshNode, "vidxs", 
            ArrayConvert(vidxs.size(), 1, AI_TYPE_UINT,
                    (void*)&vidxs[0]));
    
    AiNodeSetArray(meshNode, "nsides",
            ArrayConvert(nsides.size(), 1, AI_TYPE_BYTE,
                    &(nsides[0])));
    
    AiNodeSetArray(meshNode, "vlist",
            ArrayConvert( vlist.size() / sampleTimes.size(), 
                    sampleTimes.size(), AI_TYPE_FLOAT, (void*)(&(vlist[0]))));
    
    if ( !uvlist.empty() )
    {
        //TODO, option to disable v flipping
        for (size_t i = 1, e = uvlist.size(); i < e; i += 2)
        {
            uvlist[i] = 1.0 - uvlist[i];
        }
        
        AiNodeSetArray(meshNode, "uvlist",
            ArrayConvert( uvlist.size(), 1, AI_TYPE_FLOAT,
                (void*)(&(uvlist[0]))));
        
        if ( !uvidxs.empty() )
        {
            AiNodeSetArray(meshNode, "uvidxs",
                    ArrayConvert(uvidxs.size(), 1, AI_TYPE_UINT,
                            &(uvidxs[0])));
        }
        else
        {
            AiNodeSetArray(meshNode, "uvidxs",
                    ArrayConvert(vidxs.size(), 1, AI_TYPE_UINT,
                            &(vidxs[0])));
        }
    }
    
    if ( sampleTimes.size() > 1 )
    {
        std::vector<float> relativeSampleTimes;
        relativeSampleTimes.reserve( sampleTimes.size() );
        
        for (SampleTimeSet::const_iterator I = sampleTimes.begin();
                I != sampleTimes.end(); ++I )
        {
            relativeSampleTimes.push_back(
                    GetRelativeSampleTime( args, (*I) ) );
                    
        }
        
        AiNodeSetArray( meshNode, "deform_time_samples",
                ArrayConvert(relativeSampleTimes.size(), 1,
                        AI_TYPE_FLOAT, &relativeSampleTimes[0]));
    }
    
    // faceset visibility array
    if ( !facesetName.empty() )
    {
        if ( ps.hasFaceSet( facesetName ) )
        {
            ISampleSelector frameSelector( *singleSampleTimes.begin() );
            
            
            IFaceSet faceSet = ps.getFaceSet( facesetName );
            IFaceSetSchema::Sample faceSetSample = 
                    faceSet.getSchema().getValue( frameSelector );
            
            std::set<int> facesToKeep;
            
            
            facesToKeep.insert( faceSetSample.getFaces()->get(),
                    faceSetSample.getFaces()->get() +
                            faceSetSample.getFaces()->size() );
            
            bool *faceVisArray = new bool(nsides.size());
            
            for ( int i = 0; i < (int) nsides.size(); ++i )
            {
                faceVisArray[i] = facesToKeep.find( i ) != facesToKeep.end();
            }
            
            if ( AiNodeDeclare( meshNode, "face_visibility", "uniform BOOL" ) )
            {
                AiNodeSetArray( meshNode, "face_visibility",
                        ArrayConvert( nsides.size(), 1, AI_TYPE_BOOLEAN,
                                faceVisArray ) );
            }
            
            delete[] faceVisArray;
        }
    }
    
    {
        ICompoundProperty arbGeomParams = ps.getArbGeomParams();
        ISampleSelector frameSelector( *singleSampleTimes.begin() );
        
        AddArbitraryGeomParams( arbGeomParams, frameSelector, meshNode );
    }
    
    
    if ( instanceNode == NULL )
    {
        if ( xformSamples )
        {
            ApplyTransformation( meshNode, xformSamples, args );
        }
        
        return meshNode;
    }
    else
    {
        AiNodeSetInt( meshNode, "visibility", 0 );
        
        AiNodeSetPtr(instanceNode, "node", meshNode );
        g_meshCache[cacheId] = meshNode;
        return meshNode;
        
    }
    
}

//-*************************************************************************

void ProcessPolyMesh( IPolyMesh &polymesh, ProcArgs &args,
        MatrixSampleMap * xformSamples, const std::string & facesetName )
{
    SampleTimeSet sampleTimes;
    std::vector<AtUInt32> vidxs;
    
    AtNode * meshNode = ProcessPolyMeshBase(
            polymesh, args, sampleTimes, vidxs, 0, xformSamples,
                    facesetName );
    
    // This is a valid condition for the second instance onward and just
    // means that we don't need to do anything further.
    if ( !meshNode )
    {
        return;
    }
    
    IPolyMeshSchema &ps = polymesh.getSchema();
    
    std::vector<float> nlist;
    std::vector<AtUInt32> nidxs;
    
    ProcessIndexedBuiltinParam(
            ps.getNormalsParam(),
            sampleTimes,
            nlist,
            nidxs,
            3);
    
    if ( !nlist.empty() )
    {
        AiNodeSetArray(meshNode, "nlist",
            ArrayConvert( nlist.size() / sampleTimes.size(), 
                    sampleTimes.size(), AI_TYPE_FLOAT, (void*)(&(nlist[0]))));
        
        if ( !nidxs.empty() )
        {
            AiNodeSetArray(meshNode, "nidxs",
                    ArrayConvert(nidxs.size(), 1, AI_TYPE_UINT,
                            &(nidxs[0])));
        }
        else
        {
            AiNodeSetArray(meshNode, "nidxs",
                    ArrayConvert(vidxs.size(), 1, AI_TYPE_UINT,
                            &(vidxs[0])));
        }
    }
    
}

//-*************************************************************************

void ProcessSubD( ISubD &subd, ProcArgs &args,
        MatrixSampleMap * xformSamples, const std::string & facesetName )
{
    SampleTimeSet sampleTimes;
    std::vector<AtUInt32> vidxs;
    
    AtNode * meshNode = ProcessPolyMeshBase(
            subd, args, sampleTimes, vidxs, args.subdIterations,
                    xformSamples, facesetName );
    
    // This is a valid condition for the second instance onward and just
    // means that we don't need to do anything further.
    if ( !meshNode )
    {
        return;
    }
    
    
    AiNodeSetStr( meshNode, "subdiv_type", "catclark" );
}

