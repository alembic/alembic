#include "SubDTags.h"

using namespace Alembic;

void SubDTagBuilder::add(const std::string & tag)
{
    m_tags.push_back(tag);
    m_outputTags.push_back(const_cast<RtToken>(m_tags.back().c_str()));

    m_argCounts.push_back(0);
    m_argCounts.push_back(0);

    m_argCountsHierarchical.push_back(0);
    m_argCountsHierarchical.push_back(0);
    m_argCountsHierarchical.push_back(0);
}

void SubDTagBuilder::addIntArg(RtInt value)
{
    if (m_tags.empty()) return;
    m_argCounts[(m_tags.size()-1)*2] += 1;
    m_argCountsHierarchical[(m_tags.size()-1)*3] += 1;
}

void SubDTagBuilder::addFloatArg(RtFloat value)
{
    if (m_tags.empty()) return;
    m_argCounts[(m_tags.size()-1)*2+1] += 1;
    m_argCountsHierarchical[(m_tags.size()-1)*3+1] += 1;
}

void SubDTagBuilder::addStringArg(const std::string & value)
{
    if (m_tags.empty()) return;
    m_argCountsHierarchical[(m_tags.size()-1)*3+2] += 1;
}

RtInt SubDTagBuilder::nt()
{
    return m_outputTags.size();
}

RtToken * SubDTagBuilder::tags()
{
    if (m_outputTags.empty()) return NULL;
    return (RtToken*) &m_outputTags.front();
}

RtInt* SubDTagBuilder::nargs(bool hierarchicalSubD)
{
    if (hierarchicalSubD)
    {
        if (m_argCountsHierarchical.empty()) return NULL;
        return (RtInt*) &m_argCountsHierarchical.front();
    }
    else
    {
        if (m_argCounts.empty()) return NULL;
        return (RtInt*) &m_argCounts.front();
    }
}

RtInt* SubDTagBuilder::intargs()
{
    if (m_intArgs.empty()) return NULL;
    return &m_intArgs.front();
}

RtFloat* SubDTagBuilder::floatargs()
{
    if (m_floatArgs.empty()) return NULL;
    return &m_floatArgs.front();
}

RtToken* SubDTagBuilder::stringargs()
{
    if (m_outputStringArgs.empty()) return NULL;
    return &m_outputStringArgs.front();
}



void ProcessInterpolateBoundry(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample)
{
    if (sample.getInterpolateBoundary() > 0)
    {
        tags.add("interpolateboundary");
        tags.addIntArg(sample.getInterpolateBoundary());
    }
}


void ProcessFacevaryingInterpolateBoundry(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample)
{
    if (sample.getFaceVaryingInterpolateBoundary() > 0)
    {
        tags.add("facevaryinginterpolateboundary");
        tags.addIntArg(sample.getFaceVaryingInterpolateBoundary());
    }
}


void ProcessFacevaryingPropagateCorners(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample)
{
    if (sample.getFaceVaryingPropagateCorners() > 0)
    {
        tags.add("facevaryingpropagatecorners");
        tags.addIntArg(sample.getFaceVaryingPropagateCorners());
    }
}


void ProcessHoles(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample)
{
    if (Abc::Int32ArraySamplePtr holes = sample.getHoles())
    {
        if (!holes->size() > 0)
        {
            tags.add("holes");
            //TODO, unfortunate to add these one-at-a-time
            for (size_t i = 0, e = holes->size(); i < e; ++i)
            {
                tags.addIntArg(holes->get()[i]);
            }
        }
    }
}

void ProcessCreases(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample)
{
    if (sample.getCreaseLengths()
            && sample.getCreaseIndices()
            && sample.getCreaseSharpnesses())
    {
        Abc::Int32ArraySamplePtr creaseLengths = sample.getCreaseLengths();
        Abc::Int32ArraySamplePtr creaseIndices = sample.getCreaseIndices();
        Abc::FloatArraySamplePtr creaseSharpnesses =
                sample.getCreaseSharpnesses();
        
        size_t totalIndices = 0;
        
        for (size_t i = 0, e = creaseLengths->size(); i < e; ++i)
        {
            totalIndices += creaseLengths->get()[i];
        }
        
        if (totalIndices == creaseIndices->size()
                && creaseLengths->size() == creaseSharpnesses->size())
        {
            size_t indicesIndex = 0;
            for (size_t i = 0, e = creaseLengths->size(); i < e; ++i)
            {
                tags.add("crease");
                tags.addFloatArg(creaseSharpnesses->get()[i]);
                
                int lengthValue = creaseLengths->get()[i];
                for (int j = 0; j < lengthValue; ++j, ++indicesIndex)
                {
                    tags.addIntArg(creaseIndices->get()[indicesIndex]);
                }
            }
        }
        else
        {
            //TODO, error. For now, just exclude creases
        }
    }
}


void ProcessCorners(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample)
{
    if (sample.getCornerIndices() && sample.getCornerSharpnesses())
    {
        Abc::Int32ArraySamplePtr cornerIndices = sample.getCornerIndices();
        Abc::FloatArraySamplePtr cornerSharpnesses =
                sample.getCornerSharpnesses();
        
        if (cornerIndices->size() == cornerSharpnesses->size())
        {
            if (cornerIndices->size() > 0)
            {
                size_t count = cornerIndices->size();
                
                tags.add("corner");
                for (size_t i = 0; i < count; ++i)
                {
                    tags.addIntArg(cornerIndices->get()[i]);
                    tags.addFloatArg(cornerSharpnesses->get()[i]);
                }
            }
        }
        else
        {
            //TODO, error, For now, just exclude corners
        }
    }
}
