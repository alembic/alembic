#include <ri.h>
#include "WriteGeo.h"
#include "SampleUtil.h"
#include "ArbAttrUtil.h"
#include "SubDTags.h"

///////////////////////////////////////////////////////////////////////////////

void ProcessSimpleTransform(AbcGeom::ISimpleXform xform, ProcArgs & args)
{
    Abc::TimeSampling ts = xform.getSchema().getTimeSampling();
    
    SampleTimeSet sampleTimes;
    GetRelevantSampleTimes(args, ts, sampleTimes);
    
    bool multiSample = sampleTimes.size() > 1;
    
    if (multiSample)
    {
        WriteMotionBegin(args, sampleTimes);
    }
    
    for (SampleTimeSet::iterator I = sampleTimes.begin();
            I != sampleTimes.end(); ++I)
    {
        AbcGeom::ISimpleXformSchema::sample_type sample =
                xform.getSchema().getValue(
                        Abc::ISampleSelector(*I));
        Abc::M44d m = sample.getMatrix();
        
        if (!multiSample && m == Abc::M44d())
        {
            continue;
        }
        
        WriteConcatTransform(m);
    }
    
    if (multiSample)
    {
        RiMotionEnd();
    }
}

///////////////////////////////////////////////////////////////////////////////

void ProcessPolyMesh(AbcGeom::IPolyMesh polymesh, ProcArgs & args)
{
    Abc::TimeSampling ts = polymesh.getSchema().getTimeSampling();
    
    SampleTimeSet sampleTimes;
    GetRelevantSampleTimes(args, ts, sampleTimes);
    
    bool multiSample = sampleTimes.size() > 1;
    
    if (multiSample) WriteMotionBegin(args, sampleTimes);
    
    
    for (SampleTimeSet::iterator I = sampleTimes.begin();
                I != sampleTimes.end(); ++I)
    {
        Abc::ISampleSelector sampleSelector(*I);
        
        AbcGeom::IPolyMeshSchema::Sample sample =
                polymesh.getSchema().getValue(sampleSelector);
        
        RtInt npolys = (RtInt) sample.getCounts()->size();
        
        ParamListBuilder ParamListBuilder;
        
        ParamListBuilder.add("P", (RtPointer)sample.getPositions()->get());
        
        std::set<std::string> excludeNames;
        excludeNames.insert("P");
        
        AddArbitraryProperties(
            polymesh.getSchema(),
            sampleSelector,
            ParamListBuilder,
            &excludeNames);
        
        RiPointsPolygonsV(
            npolys,
            (RtInt*) sample.getCounts()->get(),
            (RtInt*) sample.getIndices()->get(),
            ParamListBuilder.n(),
            ParamListBuilder.nms(),
            ParamListBuilder.vals());
    }
    
    if (multiSample) RiMotionEnd();
    
}

///////////////////////////////////////////////////////////////////////////////

void ProcessSubD(AbcGeom::ISubD subd, ProcArgs & args)
{
    Abc::TimeSampling ts = subd.getSchema().getTimeSampling();
    
    SampleTimeSet sampleTimes;
    GetRelevantSampleTimes(args, ts, sampleTimes);
    
    bool multiSample = sampleTimes.size() > 1;
    
    //include this code path for future expansion
    bool isHierarchicalSubD = false;
    
    if (multiSample) WriteMotionBegin(args, sampleTimes);
    
    for (SampleTimeSet::iterator I = sampleTimes.begin();
                I != sampleTimes.end(); ++I)
    {
        Abc::ISampleSelector sampleSelector(*I);
        
        AbcGeom::ISubDSchema::Sample sample =
                subd.getSchema().getValue(sampleSelector);
        
        RtInt npolys = (RtInt) sample.getFaceCounts()->size();
        
        ParamListBuilder ParamListBuilder;
        
        ParamListBuilder.add("P", (RtPointer)sample.getPositions()->get());
        
        std::set<std::string> excludeNames;
        excludeNames.insert("P");
        
        AddArbitraryProperties(
            subd.getSchema(),
            sampleSelector,
            ParamListBuilder,
            &excludeNames);
        
        std::string subdScheme = sample.getSubdivisionScheme();
        
        SubDTagBuilder tags;
        
        ProcessFacevaryingInterpolateBoundry(tags, sample);
        ProcessInterpolateBoundry(tags, sample);
        ProcessFacevaryingPropagateCorners(tags, sample);
        ProcessHoles(tags, sample);
        ProcessCreases(tags, sample);
        ProcessCorners(tags, sample);
        
        if (isHierarchicalSubD)
        {
            RiHierarchicalSubdivisionMeshV(
                const_cast<RtToken>(subdScheme.c_str()),
                npolys,
                (RtInt*) sample.getFaceCounts()->get(),
                (RtInt*) sample.getFaceIndices()->get(),
                tags.nt(),
                tags.tags(),
                tags.nargs(true),
                tags.intargs(),
                tags.floatargs(),
                tags.stringargs(),
                ParamListBuilder.n(),
                ParamListBuilder.nms(),
                ParamListBuilder.vals());
        }
        else
        {
            RiSubdivisionMeshV(
                const_cast<RtToken>(subdScheme.c_str()),
                npolys,
                (RtInt*) sample.getFaceCounts()->get(),
                (RtInt*) sample.getFaceIndices()->get(),
                tags.nt(),
                tags.tags(),
                tags.nargs(false),
                tags.intargs(),
                tags.floatargs(),
                ParamListBuilder.n(),
                ParamListBuilder.nms(),
                ParamListBuilder.vals());
        }
    }
    
    if (multiSample) RiMotionEnd();
}

///////////////////////////////////////////////////////////////////////////////

void WriteIdentifier(const Abc::ObjectHeader & ohead)
{
    std::string name = ohead.getFullName();
    name = name.substr(4, name.size()-1); //for now, shave off the /ABC
    char* nameArray[] = {const_cast<char*>(name.c_str()), RI_NULL};
    RiAttribute(const_cast<char*>("identifier"), const_cast<char*>("name"),
            nameArray, RI_NULL);
}


