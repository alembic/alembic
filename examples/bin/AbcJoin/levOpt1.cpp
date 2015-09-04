
#include <future>
#include "anim.hpp"

namespace levOpt1
{
using IndexNormal = std::tuple<
        std::unique_ptr<AnimObj::Geom::nds_vt>,
        std::unique_ptr<AnimObj::Geom::NormalData::NsV>>;

static IndexNormal optimize(AnimObj::Geom::NormalData::NsV const& ons)
{
    std::vector<std::array<float, 3>> nrmf;
    Alembic::AbcGeom::N3fArraySample::value_vector::size_type const nsz = ons.size();
    nrmf.reserve(nsz);
    for (size_t k = 0; k < nsz; ++k)
    {
        Alembic::AbcGeom::N3fArraySample::value_type const& v = ons[k];
        decltype(nrmf)::value_type a = {v[0], v[1], v[2]};
        nrmf.emplace_back(a);
    }
    decltype(nrmf) nrms(nrmf);
    std::sort(nrms.begin(), nrms.end());
    std::size_t const d = std::distance(nrms.begin(), std::unique(nrms.begin(), nrms.end()));
    Alembic::AbcGeom::N3fArraySample::value_vector nsv(d);
    std::vector<std::pair<decltype(nrmf)::value_type, int>> nrmN;
    for (size_t m = 0; m < d; ++m)
    {
        decltype(nrms)::value_type const& n = nrms[m];
        nrmN.emplace_back(n, m);
        nsv[m] = {n[0], n[1], n[2]};
    }
    Alembic::AbcGeom::UInt32ArraySample::value_vector ndxv(nsz);
    decltype(nrmN)::const_iterator const& nrmNE = nrmN.cend();
    for (size_t m = 0; m < nsz; ++m)
    {
        decltype(nrmN)::const_iterator const& found = std::lower_bound(
                nrmN.cbegin(), nrmNE, nrmf[m],
                [](decltype(nrmN)::value_type const& a, decltype(nrmf)::value_type const& b)
                { return a.first < b; } );
        ndxv[m] = nrmNE == found ? 0 : found->second;
    }
    return std::make_tuple(
            std::make_unique<AnimObj::Geom::nds_vt>(ndxv),
            std::make_unique<AnimObj::Geom::NormalData::NsV>(nsv));
}

static void recurse(AnimObj const& animObj)
{
    std::unique_ptr<AnimObj::Geom> const& geomPtr = animObj.geom_;
    if (geomPtr)
    {
        std::unique_ptr<AnimObj::Geom::NormalData>& normalDataPtr = geomPtr->normalData_;
        if (!normalDataPtr)
        {
            return;
        }
        AnimObj::Geom::NormalData& normalData = *normalDataPtr;
        if (normalData.normals_ && !normalData.index_)
        {
            normalData.indices_ = std::make_unique<AnimObj::Geom::NormalData::Indices>();
            AnimObj::Geom::NormalData::Indices& indices = *normalData.indices_;
            std::vector<std::future<IndexNormal>> futures;
            for (std::unique_ptr<AnimObj::Geom::NormalData::NsV> const& normals : *normalData.normals_)
            {
                futures.emplace_back(std::async(std::launch::async, optimize, std::cref(*normals)));
            }
            decltype(futures)::iterator fit = futures.begin();
            for (std::unique_ptr<AnimObj::Geom::NormalData::NsV>& normals : *normalData.normals_)
            {
                IndexNormal t = fit->get();
                ++fit;
                normals = std::move(std::get<AnimObj::Geom::NormalData::Normals::value_type>(t));
                indices.emplace_back(std::move(std::get<AnimObj::Geom::NormalData::Indices::value_type>(t)));
            }
        }
    }
    else
    {
        AnimObj::AnimData const& animPtr = animObj.anims_;
        if (!animPtr)
        {
            return;
        }
        for (std::unique_ptr<AnimObj> const& childAnimPtr : *animPtr)
        {
            recurse(*childAnimPtr);
        }
    }
}

void traverse(AnimFiles const& animFiles)
{
    for (AnimFiles::value_type const& animFile : animFiles)
    {
        for (std::unique_ptr<AnimObj> const& animObjPtr : *animFile.second)
        {
            recurse(*animObjPtr);
        }
    }
}
}
