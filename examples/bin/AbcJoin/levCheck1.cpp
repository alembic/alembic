
#include "anim.hpp"
#include "mod.hpp"

namespace animOut
{
AnimObj::AnimData::element_type const* findMatchAnim(
        AnimFiles const& animFiles,
        std::string const& animFileName);
}

namespace levCheck1
{
static inline bool equal(AnimObj::TpV const& a, AnimObj::TpV const& b)
{
    std::size_t const s = a.size();
    if (b.size() != s)
    {
        return false;
    }
    return 0 == memcmp(a.data(), b.data(), sizeof(AnimObj::TpV::value_type) * s);
}

bool equal(AnimObj::Topo const& a, AnimObj::Topo const& b)
{
    return equal(*a.cs_, *b.cs_) && equal(*a.ds_, *b.ds_);
}

ModelData const* findMatchFromParent(
        string_view const modelName,
        string_view const parentFullName)
{
    string_view::size_type const lastSlash = parentFullName.find_last_of('/');
    return modGather::findMatch(parentFullName.substr(0, lastSlash), modelName);
}

static inline std::string a2m(string_view const v)
{
    std::string r(v);
    std::replace(r.begin(), r.end(), '/', '|');
    return r;
}

static void recurse(
        std::vector<std::string>& msgs,
        string_view const modelName,
        AnimObj const& animObj,
        string_view const parentFullName)
{
    std::unique_ptr<AnimObj::Geom> const& geomPtr = animObj.geom_;
    if (geomPtr)
    {
        ModelData const* const mdPtr = findMatchFromParent(modelName, parentFullName);
        if (nullptr == mdPtr)
        {
            msgs.emplace_back("cannot find match for " + a2m(parentFullName) +
                    " in model " + modelName.to_string());
            return;
        }
        AnimObj::Geom const& geom = *geomPtr;
        AnimObj::Points const& points = *geom.points_;
        std::size_t const psz = points.size();
        if (1 == psz)
        {
            return;
        }
        ModelData const& modelData = *mdPtr;
        std::size_t const msz = modGather::getPs(modelData).size();
        std::size_t i = 0;
        for (; i < psz; ++i)
        {
            if (points[i]->size() != msz)
            {
                break;
            }
        }
        if (i < psz)
        {
            msgs.emplace_back(std::string(points[i]->size() > msz ? "too much" : "not enough") +
                    " points in " + a2m(parentFullName) +
                    " (" + (i ? "not " : "") +
                    "first frame) compared with model " + modelName.to_string());
            return;
        }
        if (!geom.normalData_)
        {
            return;
        }
        if (!equal(modGather::topoExtract(modelData), geom.topo_))
        {
            msgs.emplace_back("[NORMALS] topology in " + a2m(parentFullName) +
                    " not identical to model " + modelName.to_string());
        }
    }
    else
    {
        AnimObj::AnimData const& animPtr = animObj.anims_;
        if (!animPtr)
        {
            return;
        }
        for (std::unique_ptr<AnimObj> const& childAnim : *animPtr)
        {
            recurse(msgs, modelName, *childAnim,
                    parentFullName.to_string() + "/" + childAnim->name_);
        }
    }
}

void traverse(
        AnimFiles const& animFiles,
        std::vector<std::pair<std::string, std::string>> const& modAnimFileNames,
        std::vector<std::string>& msgs)
{
    size_t const sz = modAnimFileNames.size();
    for (std::size_t i = 0; i < sz; ++i)
    {
        AnimObj::AnimData::element_type const* const animDataPtr =
                animOut::findMatchAnim(animFiles, modAnimFileNames[i].second);
        if (nullptr == animDataPtr)
        {
            continue;
        }
        for (std::unique_ptr<AnimObj> const& animObj : *animDataPtr)
        {
            recurse(msgs, extractAssetNameFromFilePath(modAnimFileNames[i].first),
                    *animObj, "/" + animObj->name_);
        }
    }
}

void traverse(
        AnimFiles const& animFiles,
        std::vector<std::string> const& modFileNames,
        std::vector<std::string>& msgs)
{
    size_t const sz = modFileNames.size();
    for (std::size_t i = 0; i < sz; ++i)
    {
        for (std::unique_ptr<AnimObj> const& animObj : *animFiles[i].second)
        {
            recurse(msgs, extractAssetNameFromFilePath(modFileNames[i]),
                    *animObj, "/" + animObj->name_);
        }
    }
}
}
