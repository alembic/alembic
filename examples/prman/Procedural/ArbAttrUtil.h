#ifndef _Alembic_Prman_ArbAttrUtil_h_
#define _Alembic_Prman_ArbAttrUtil_h_

#include <ri.h>

#include <Alembic/AbcGeom/All.h>
using namespace Alembic;

#include <set>

class ParamListBuilder
{
public:
    void add(const std::string & declaration, RtPointer value,
            Abc::ArraySamplePtr sampleToRetain=Abc::ArraySamplePtr());
    
    RtPointer addStringValue(const std::string & value,
            bool retainLocally=false);
    
    RtInt n();
    RtToken* nms();
    RtPointer* vals();
    
private:
    std::vector<std::string> m_declarations;
    std::vector<RtToken> m_outputDeclarations;
    std::vector<RtPointer> m_values;
    std::vector<Abc::ArraySamplePtr> m_retainedSamples;
    
    //Used for converting std::string arrays to RtString arrays
    std::vector<RtString> m_convertedStrings;
    std::vector<std::string> m_retainedStrings;
};

std::string GetPrmanScopeString(AbcGeom::GeometryScope scope);

void AddArbitraryProperties(
        Abc::ICompoundProperty & parent,
        Abc::ISampleSelector & sampleSelector,
        ParamListBuilder & ParamListBuilder,
        const std::set<std::string> * excludeNames = 0);

#endif
