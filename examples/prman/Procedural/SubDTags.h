#ifndef _Alembic_Prman_SubDTags_h_
#define _Alembic_Prman_SubDTags_h_

#include <ri.h>

#include <Alembic/AbcGeom/All.h>
using namespace Alembic;

class SubDTagBuilder
{
public:
    void add(const std::string & tag);
    void addIntArg(RtInt value);
    void addFloatArg(RtFloat value);
    void addStringArg(const std::string & value);
    
    RtInt nt();
    RtToken * tags();
    RtInt* nargs(bool hierarchicalSubD);
    RtInt* intargs();
    RtFloat* floatargs();
    RtToken* stringargs();
    
private:
    std::vector<std::string> m_tags;
    std::vector<RtToken> m_outputTags;
    
    std::vector<RtInt> m_argCounts;
    std::vector<RtInt> m_argCountsHierarchical;
    
    std::vector<RtInt> m_intArgs;
    std::vector<RtFloat> m_floatArgs;
    std::vector<std::string> m_stringArgs;
    std::vector<RtToken> m_outputStringArgs;
};

void ProcessInterpolateBoundry(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample);

void ProcessFacevaryingInterpolateBoundry(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample);

void ProcessFacevaryingPropagateCorners(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample);

void ProcessHoles(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample);

void ProcessCreases(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample);

void ProcessCorners(
        SubDTagBuilder & tags,
        AbcGeom::ISubDSchema::Sample & sample);

#endif
