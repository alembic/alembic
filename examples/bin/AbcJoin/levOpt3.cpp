
#include <future>
#include <experimental/optional>
#include "anim.hpp"

using std::vector;
using std::experimental::optional;

namespace levOpt3
{
static optional<vector<AnimObj::TpV>> extract(
        std::size_t const rsz,
        AnimObj::Topo const& tp)
{
    vector<AnimObj::TpV> rx(rsz);
    AnimObj::TpV::value_type k = 0;
    AnimObj::TpV const& ds = *tp.ds_;
    AnimObj::TpV const& cs = *tp.cs_;
    size_t const csz = cs.size();
    for (size_t i = 0; i < csz; ++i)
    {
        size_t const msz = cs[i];
        for (size_t m = 0; m < msz; ++m)
        {
            AnimObj::TpV::value_type const n = ds[k];
            if (static_cast<std::size_t>(n) >= rsz)
            {
                return {};
            }
            rx[n].emplace_back(k);
            ++k;
        }
    }
    for (AnimObj::TpV const& rv : rx)
    {
        if (rv.empty())
        {
            return {};
        }
    }
    return rx;
}

AnimObj::NormalData::NsV normGen(
        AnimObj::PsV const& ps,
        AnimObj::Topo const& tp)
{
    AnimObj::NormalData::NsV ns(ps.size(), Alembic::Abc::N3f(0));
    AnimObj::TpV::const_iterator f = tp.ds_->begin();
    for (AnimObj::TpV::value_type const c : *tp.cs_)
    {
        Alembic::AbcGeom::P3fArraySample::value_type const& a = ps[*f];
        Alembic::Abc::N3f const n = (a - ps[f[2]]) % (a - ps[f[1]]);
        for (int i = 0; i < c; ++i)
        {
            ns[*f] += n;
            ++f;
        }
    }
    return ns;
}

namespace test
{
static bool sample(
        std::vector<AnimObj::TpV> const& rx,
        AnimObj::NormalData::NsV const& ns)
{
    size_t const rsz = rx.size();
    for (size_t i = 0; i < rsz; ++i)
    {
        AnimObj::TpV const& x = rx[i];
        AnimObj::NormalData::NsV::value_type const& n0 = ns[x[0]];
        size_t const xsz = x.size();
        for (size_t j = 1; j < xsz; ++j)
        {
            if (n0 != ns[x[j]])
            {
                return false;
            }
        }
    }
    return true;
}

static bool sample(
        AnimObj::NormalData::NsV const& nd,
        std::vector<AnimObj::TpV> const& rx,
        AnimObj::NormalData::NsV const& ns)
{
    std::size_t const sz = nd.size();
    if (rx.size() != sz)
    {
        return false;
    }
    std::size_t vc = 0;
    for (std::size_t i = 0; i < sz; ++i)
    {
        float const d = nd[i].normalized() ^ ns[rx[i].front()].normalized();
        if (d < 0.8f)
        {
            ++vc;
        }
    }
    if (sz / 8u < vc)
    {
        return false;
    }
    return true;
}

static bool all(
        AnimObj::Points const& ps,
        AnimObj::Topo const& tp,
        AnimObj::NormalData::Normals const& ns)
{
    std::size_t const sz = ps.size();
    if (ns.size() != sz)
    {
        return false;
    }
    optional<vector<AnimObj::TpV>> const& prx =
            extract(ps.front()->size(), tp);
    if (!prx)
    {
        return false;
    }
    vector<AnimObj::TpV> const& rx = *prx;
    for (std::size_t i = 0; i < sz; ++i)
    {
        AnimObj::NormalData::NsV const& nv = *ns[i];
        if (!sample(rx, nv))
        {
            return false;
        }
        if (!sample(normGen(*ps[i], tp), rx, nv))
        {
            return false;
        }
    }
    return true;
}
}

static void optimize(
        AnimObj::Points const& points,
        AnimObj::Topo const& topo,
        std::unique_ptr<AnimObj::NormalData>& normalDataPtr)
{
    std::unique_ptr<AnimObj::NormalData::Normals> const& nsPtr = normalDataPtr->normals_;
    if (!nsPtr)
    {
        return;
    }
    if (test::all(points, topo, *nsPtr))
    {
        normalDataPtr.reset();
    }
}

static void recurse(AnimObj& animObj, std::vector<std::future<void>>& futures)
{
    std::unique_ptr<AnimObj::Geom> const& geomPtr = animObj.geom_;
    if (geomPtr)
    {
        AnimObj::Geom& geom = *geomPtr;
        std::unique_ptr<AnimObj::NormalData>& normalDataPtr = geom.normalData_;
        if (!normalDataPtr)
        {
            return;
        }
        futures.emplace_back(std::async(std::launch::async, optimize,
                std::cref(*geom.points_), std::cref(geom.topo_),
                std::ref(normalDataPtr)));
    }
    else
    {
        AnimObj::AnimData const& animPtr = animObj.anims_;
        if (!animPtr)
        {
            return;
        }
        for (std::unique_ptr<AnimObj> const& childAnimPtr : *animObj.anims_)
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
        for (std::unique_ptr<AnimObj> const& animObjPtr : *animFile.second)
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
