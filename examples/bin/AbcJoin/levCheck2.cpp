
#include <experimental/string_view>
#include "anim.hpp"

using std::experimental::string_view;

string_view extractAssetNameFromFilePath(string_view const filePath)
{
    string_view::size_type const lastSlash = filePath.find_last_of(":/\\");
    string_view const fileNameWithExt =
            filePath.substr(lastSlash == string_view::npos ? 0 : lastSlash + 1);
    string_view::size_type const lastDot = fileNameWithExt.find_last_of('.');
    return fileNameWithExt.substr(0, lastDot);
}

string_view withoutMayaNamespace(string_view const s)
{
    string_view::size_type const p = s.find_last_of(':');
    if (string_view::npos == p)
    {
        return s;
    }
    return s.substr(p + 1);
}

namespace levCheck2
{
using ModFileObjPaths =
        std::vector<std::pair<string_view,
        std::vector<std::reference_wrapper<
        std::vector<std::string> const>>>>;

template <bool multiMod>
static inline std::string join(std::vector<std::string> const& modObjPath)
{
    std::string r;
    std::size_t const sz = modObjPath.size();
    for (std::size_t i = multiMod ? 1 : 0; i < sz; ++i)
    {
        r += '|' + modObjPath[i];
    }
    return r;
}

template <bool multiMod>
static inline bool find(
        AnimObj::AnimData::element_type const& animObjs,
        std::vector<std::string> const& modObjPath)
{
    AnimObj::AnimData::element_type const* curAnimObjs = &animObjs;
    std::size_t const msz = modObjPath.size();
    for (std::size_t j = multiMod ? 1 : 0; j < msz; ++j)
    {
        std::string const& modObjPathFrag = modObjPath[j];
        std::size_t const asz = curAnimObjs->size();
        std::size_t i = 0;
        for (; i < asz; ++i)
        {
            AnimObj const& animObj = *(*curAnimObjs)[i];
            if (withoutMayaNamespace(animObj.name_) == modObjPathFrag)
            {
                AnimObj::AnimData const& anims = animObj.anims_;
                if (!anims)
                {
                    return false;
                }
                curAnimObjs = &*anims;
                break;
            }
        }
        if (asz == i)
        {
            return false;
        }
    }
    return true;
}

template <bool multiMod>
static inline void iterate(
        ModFileObjPaths::value_type const& modFileObjPath,
        AnimObj::AnimData::element_type const& animObjs,
        std::string const& animName,
        std::vector<std::string>& msgs)
{
    std::string const modName(modFileObjPath.first);
    for (ModFileObjPaths::value_type::second_type::value_type const&
            modObjPath : modFileObjPath.second)
    {
        if (find<multiMod>(animObjs, modObjPath))
        {
            continue;
        }
        msgs.emplace_back(
                "not used object " + join<multiMod>(modObjPath) +
                " in model " + modName +
                " from animation " + animName);
    }
}

template <bool multiMod>
void iterate(
        AnimFiles const& animFiles,
        ModFileObjPaths const& modFileObjPaths,
        std::vector<std::string>& msgs)
{
    size_t const msz = modFileObjPaths.size();
    if (1 == msz)
    {
        ModFileObjPaths::value_type const& modFileObjPath =
                modFileObjPaths.front();
        for (AnimFiles::value_type const& animFile : animFiles)
        {
            iterate<multiMod>(
                    modFileObjPath, *animFile.second,
                    extractAssetNameFromFilePath(animFile.first).to_string(),
                    msgs);
        }
    }
    else
    {
        size_t const nsz = animFiles.size();
        if (msz != nsz)
        {
            return;
        }
        for (std::size_t i = 0; i < nsz; ++i)
        {
            AnimFiles::value_type const& animFile = animFiles[i];
            iterate<multiMod>(
                    modFileObjPaths[i], *animFile.second,
                    extractAssetNameFromFilePath(animFile.first).to_string(),
                    msgs);
        }
    }
}

template void iterate<false>(
        AnimFiles const& animFiles,
        ModFileObjPaths const& modFileObjPaths,
        std::vector<std::string>& msgs);

template void iterate<true>(
        AnimFiles const& animFiles,
        ModFileObjPaths const& modFileObjPaths,
        std::vector<std::string>& msgs);
}
