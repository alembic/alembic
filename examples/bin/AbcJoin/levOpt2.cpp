
#include <future>
#include <experimental/optional>
#include "anim.hpp"

using std::unique_ptr;
using std::tuple;
using std::experimental::optional;

namespace levOpt2
{
template <typename TV, typename TX>
static inline bool apply(TV const& vs, TX const& rx, TV& vd)
{
    typename TX::size_type const rsz = rx.size();
    if (vd.size() != rsz)
    {
        return false;
    }
    typename TX::size_type const vsz = vs.size();
    for (typename TX::size_type i = 0; i < rsz; ++i)
    {
        typename TX::value_type const rxv = rx[i];
        if (rxv >= vsz)
        {
            return false;
        }
        vd[i] = vs[rxv];
    }
    return true;
}

template <typename TV, typename TX>
static inline bool equalApply(TV const& vs, TX const& rx, TV const& vn)
{
    typename TX::size_type const rsz = rx.size();
    if (vn.size() != rsz)
    {
        return false;
    }
    typename TX::size_type const vsz = vs.size();
    for (typename TX::size_type i = 0; i < rsz; ++i)
    {
        typename TX::value_type const rxv = rx[i];
        if (rxv >= vsz)
        {
            return false;
        }
        if (vn[i] != vs[rxv])
        {
            return false;
        }
    }
    return true;
}

static tuple<
        unique_ptr<AnimObj::Geom::nds_vt>,
        unique_ptr<AnimObj::Geom::NormalData::NsV>,
        unique_ptr<AnimObj::Geom::nds_vt>> extract(AnimObj::Geom::NormalData::NsV const& ons)
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
    Alembic::AbcGeom::UInt32ArraySample::value_vector ndrxv(d);
    decltype(nrmN)::const_iterator const& nrmNB = nrmN.cbegin();
    decltype(nrmN)::const_iterator const& nrmNE = nrmN.cend();
    for (size_t m = 0; m < nsz; ++m)
    {
        decltype(nrmN)::const_iterator const& found = std::lower_bound(
                nrmNB, nrmNE, nrmf[m],
                [](decltype(nrmN)::value_type const& a, decltype(nrmf)::value_type const& b)
                { return a.first < b; } );
        if (nrmNE == found)
        {
            continue;
        }
        ndrxv[std::distance(nrmNB, found)] = m;
        ndxv[m] = found->second;
    }
    return std::make_tuple(
            std::make_unique<AnimObj::Geom::nds_vt>(ndxv),
            std::make_unique<AnimObj::Geom::NormalData::NsV>(nsv),
            std::make_unique<AnimObj::Geom::nds_vt>(ndrxv));
}

using NormGen = optional<tuple<
        unique_ptr<AnimObj::Geom::NormalData::Normals>,
        unique_ptr<AnimObj::Geom::NormalData::Indices>,
        unique_ptr<AnimObj::Geom::nds_vt>>>;

static NormGen generate(AnimObj::Geom::NormalData::Normals const& oldNormals)
{
    unique_ptr<AnimObj::Geom::NormalData::Normals> newNormals =
            std::make_unique<AnimObj::Geom::NormalData::Normals>();
    AnimObj::Geom::NormalData::Normals::const_iterator const& one = oldNormals.cend();
    AnimObj::Geom::NormalData::Normals::const_iterator nit = oldNormals.cbegin();
    AnimObj::Geom::NormalData::NsV const* cn = &**nit;
    std::size_t const nsz = cn->size();
    AnimObj::Geom::NormalData::Indices::value_type nrmNdx;
    AnimObj::Geom::NormalData::Normals::value_type nrmVls;
    AnimObj::Geom::NormalData::Indices::value_type nrmRdx;
    std::tie(nrmNdx, nrmVls, nrmRdx) = extract(*cn);
    Alembic::AbcGeom::N3fArraySample::value_vector nrmls(nrmVls->size());
    newNormals->emplace_back(std::move(nrmVls));
    ++nit;
    AnimObj::Geom::NormalData::Indices::value_type index = std::make_unique<AnimObj::Geom::nds_vt>();
    AnimObj::Geom::nds_vt::value_type topIndex = 0;
    index->emplace_back(topIndex);
    unique_ptr<AnimObj::Geom::NormalData::Indices> indices =
            std::make_unique<AnimObj::Geom::NormalData::Indices>();
    AnimObj::Geom::NormalData::Indices& inds = *indices;
    inds.emplace_back(std::move(nrmNdx));
    AnimObj::Geom::NormalData::Indices rinds;
    rinds.emplace_back(std::move(nrmRdx));
    for (; nit != one; ++nit)
    {
        cn = &**nit;
        if (cn->size() != nsz)
        {
            return {};
        }
        std::size_t const rsz = rinds.size();
        if (rsz > 10)
        {
            return {};
        }
        std::size_t i = 0;
        for (; i < rsz; ++i)
        {
            apply(*cn, *rinds[i], nrmls);
            if (equalApply(nrmls, *inds[i], *cn))
            {
                break;
            }
        }
        if (i < rsz)
        {
            newNormals->emplace_back(std::make_unique<AnimObj::Geom::NormalData::NsV>(nrmls));
            index->emplace_back(i);
            continue;
        }
        std::tie(nrmNdx, nrmVls, nrmRdx) = extract(*cn);
        nrmls.resize(nrmVls->size());
        newNormals->emplace_back(std::move(nrmVls));
        inds.emplace_back(std::move(nrmNdx));
        rinds.emplace_back(std::move(nrmRdx));
        index->emplace_back(++topIndex);
    }
    return std::make_tuple(std::move(newNormals), std::move(indices), std::move(index));
}

static void optimize(AnimObj::Geom::NormalData& normalData)
{
    NormGen normGen = generate(*normalData.normals_);
    if (normGen)
    {
        std::tie(normalData.normals_, normalData.indices_, normalData.index_) = std::move(*normGen);
    }
}

static void recurse(AnimObj const& animObj, std::vector<std::future<void>>& futures)
{
    std::unique_ptr<AnimObj::Geom> const& geomPtr = animObj.geom_;
    if (geomPtr)
    {
        std::unique_ptr<AnimObj::Geom::NormalData>& normalDataPtr = geomPtr->normalData_;
        if (normalDataPtr && normalDataPtr->normals_)
        {
            futures.emplace_back(std::async(std::launch::async, optimize, std::ref(*normalDataPtr)));
        }
    }
    else
    {
        AnimObj::AnimData const& animPtr = animObj.anims_;
        if (!animPtr)
        {
            return;
        }
        for (unique_ptr<AnimObj> const& childAnimPtr : *animPtr)
        {
            recurse(*childAnimPtr, futures);
        }
    }
}

void traverse(AnimFiles const& animFiles)
{
    std::vector<std::future<void>> futures;
    for (AnimFiles::value_type const& animFile : animFiles)
    {
        for (unique_ptr<AnimObj> const& animObjPtr : *animFile.second)
        {
            recurse(*animObjPtr, futures);
        }
    }
    for (std::future<void>& future : futures)
    {
        future.get();
    }
}
}
