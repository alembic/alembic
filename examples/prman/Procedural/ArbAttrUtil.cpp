#include "ArbAttrUtil.h"


///////////////////////////////////////////////////////////////////////////////

void ParamListBuilder::add(const std::string & declaration, RtPointer value,
        Abc::ArraySamplePtr sampleToRetain)
{
    m_declarations.push_back(declaration);
    m_outputDeclarations.push_back(const_cast<char *>(
            m_declarations.back().c_str()));
    m_values.push_back(value);
    
    if (sampleToRetain)
    {
        m_retainedSamples.push_back(sampleToRetain);
    }
}

RtInt ParamListBuilder::n()
{
    return (RtInt) m_values.size();
}

RtToken* ParamListBuilder::nms()
{
    if (m_outputDeclarations.empty()) return 0;
    return &m_outputDeclarations.front();
}

RtPointer* ParamListBuilder::vals()
{
    if (m_values.empty()) return 0;
    return &m_values.front();
}

RtPointer ParamListBuilder::addStringValue(const std::string & value, bool retainLocally)
{
    if (retainLocally)
    {
        m_retainedStrings.push_back(value);
        m_convertedStrings.push_back(const_cast<RtString>(
                m_retainedStrings.back().c_str()));
    }
    else
    {
        m_convertedStrings.push_back(const_cast<RtString>(value.c_str()));
    }
    return (RtPointer) &m_convertedStrings.back();
}

///////////////////////////////////////////////////////////////////////////////

std::string GetPrmanScopeString(AbcGeom::GeometryScope scope)
{
    switch (scope)
    {
    case AbcGeom::kUniformScope:
        return "uniform";
    case AbcGeom::kVaryingScope:
        return "varying";
    case AbcGeom::kVertexScope:
        return "vertex";
    case AbcGeom::kFacevaryingScope:
        return "facevarying";
    case AbcGeom::kConstantScope:
    default:
        return "constant";
    }
}

///////////////////////////////////////////////////////////////////////////////

template <typename T>
void AddArbitraryPropertyToParamListBuilder(
    Abc::ICompoundProperty & parent,
    const Abc::PropertyHeader & propHeader,
    Abc::ISampleSelector & sampleSelector,
    const std::string & rmanBaseType,
    ParamListBuilder & ParamListBuilder
)
{
    T prop(parent, propHeader.getName());
    
    if (!prop.valid())
    {
        //TODO error message?
        return;
    }
    
    std::string rmanType = GetPrmanScopeString(
            AbcGeom::GetGeometryScope(propHeader.getMetaData())) + " ";
    
    rmanType += rmanBaseType + " " + propHeader.getName();
    
    typename T::sample_ptr_type propSample = prop.getValue(
            sampleSelector);
    
    ParamListBuilder.add(rmanType, (RtPointer)propSample->get(), propSample);
}

///////////////////////////////////////////////////////////////////////////////

void AddArbitraryStringPropertyToParamListBuilder(
    Abc::ICompoundProperty & parent,
    const Abc::PropertyHeader & propHeader,
    Abc::ISampleSelector & sampleSelector,
    ParamListBuilder & ParamListBuilder
)
{
    Abc::IStringArrayProperty prop(parent, propHeader.getName());
    if (!prop.valid())
    {
        //error message?
        return;
    }
    
    std::string rmanType = GetPrmanScopeString(
            AbcGeom::GetGeometryScope(propHeader.getMetaData())) + " ";
    
    rmanType += "string " + propHeader.getName();
    
    Abc::IStringArrayProperty::sample_ptr_type propSample = prop.getValue(
            sampleSelector);
    
    RtPointer dataStart = NULL;
    for (size_t i = 0; i < propSample->size(); ++i)
    {
        RtPointer data = ParamListBuilder.addStringValue((*propSample)[i]);
        if (i == 0) dataStart = data;
    }
    
    ParamListBuilder.add(rmanType, dataStart, propSample);
    
}

///////////////////////////////////////////////////////////////////////////////

void AddArbitraryProperties(
    Abc::ICompoundProperty & parent,
    Abc::ISampleSelector & sampleSelector,
    ParamListBuilder & ParamListBuilder,
    const std::set<std::string> * excludeNames
)
{
    for (size_t i = 0; i < parent.getNumProperties(); ++i)
    {
        const Abc::PropertyHeader & propHeader = 
                parent.getPropertyHeader(i);
        const std::string & propName = propHeader.getName();
        
        if (propName.empty()
            || propName[0] == '.'
            || (excludeNames
                && excludeNames->find(propName) != excludeNames->end())
            || propHeader.getMetaData().get("geoScope").empty()
            || !propHeader.isSimple() //skip compunds for now
        ) { continue;}
        
        if (propHeader.isArray())
        {
            if (Abc::IN3fArrayProperty::matches(propHeader))
            {
                AddArbitraryPropertyToParamListBuilder<Abc::IN3fArrayProperty>(
                        parent,
                        propHeader,
                        sampleSelector,
                        "normal",
                        ParamListBuilder);
            }
            else if (Abc::IV2fArrayProperty::matches(propHeader))
            {
                AddArbitraryPropertyToParamListBuilder<Abc::IV2fArrayProperty>(
                        parent,
                        propHeader,
                        sampleSelector,
                        "float[2]",
                        ParamListBuilder);
            }
            else if (Abc::IV3fArrayProperty::matches(propHeader))
            {
                AddArbitraryPropertyToParamListBuilder<Abc::IV3fArrayProperty>(
                        parent,
                        propHeader,
                        sampleSelector,
                        "point",
                        ParamListBuilder);
            }
            else if (Abc::IM44fArrayProperty::matches(propHeader))
            {
                AddArbitraryPropertyToParamListBuilder<Abc::IM44fArrayProperty>(
                        parent,
                        propHeader,
                        sampleSelector,
                        "matrix",
                        ParamListBuilder);
            }
            else if (Abc::IFloatArrayProperty::matches(propHeader))
            {
                AddArbitraryPropertyToParamListBuilder<Abc::IFloatArrayProperty>(
                        parent,
                        propHeader,
                        sampleSelector,
                        "float",
                        ParamListBuilder);
            }
            else if (Abc::IInt32ArrayProperty::matches(propHeader))
            {
                AddArbitraryPropertyToParamListBuilder<Abc::IInt32ArrayProperty>(
                        parent,
                        propHeader,
                        sampleSelector,
                        "int",
                        ParamListBuilder);
            }
            else if (Abc::IStringArrayProperty::matches(propHeader))
            {
                AddArbitraryStringPropertyToParamListBuilder(
                        parent,
                        propHeader,
                        sampleSelector,
                        ParamListBuilder);
            }
            
        }
        else if (propHeader.isScalar())
        {
            //for now, only array properties are supported
        }
    }
}
