
#include <future>
#include <experimental/string_view>
#include <experimental/optional>
#include "libmv/multiview/euclidean.h"
#include "anim.hpp"
#include "mod.hpp"

using std::experimental::optional;

namespace animOut
{
AnimObj::AnimData::element_type const* findMatchAnim(
        AnimFiles const& animFiles,
        std::string const& animFileName);
}

namespace levCheck1
{
ModelData const* findMatchFromParent(
        string_view const modelName,
        string_view const parentFullName);
}

namespace levOpt4
{
static inline void prpMx(
        std::size_t const n,
        AnimObj::Geom::PsV const& v,
        libmv::Mat& m,
        std::size_t const s)
{
    for (std::size_t j = 0; j < n; ++j)
    {
        std::size_t const i = s * j;
        m(0, j) = v[i].x;
        m(1, j) = v[i].y;
        m(2, j) = v[i].z;
    }
}

namespace xForm
{
static inline bool estimate(
        AnimObj::Geom::PsV const& va,
        AnimObj::Geom::PsV const& vb,
        std::size_t const vs,
        libmv::Mat4& mc)
{
    std::size_t const dms = 1000;
    if (dms < 4)
    {
        return false;
    }
    std::size_t const ems = vs < dms ? vs : dms;
    std::size_t const st = vs / ems;
    libmv::Mat ma(3, ems);
    prpMx(ems, va, ma, st);
    libmv::Mat mb(3, ems);
    prpMx(ems, vb, mb, st);
    return libmv::Euclidean3DFromCorrespondencesLinear(ma, mb, &mc, 0.0000001);
}

static inline optional<Alembic::Abc::M44d> generate(
        AnimObj::Geom::PsV const& va,
        Alembic::Abc::M44d const& mx,
        AnimObj::Geom::PsV const& vb)
{
    std::size_t const vs = va.size();
    if (vb.size() != vs)
    {
        return {};
    }
    if (vs < 4)
    {
        return {};
    }
    libmv::Mat4 m;
    m.setIdentity();
    if (!xForm::estimate(va, vb, vs, m))
    {
        return {};
    }
    Alembic::Abc::M44d const mr(
            m(0, 0), m(1, 0), m(2, 0), 0,
            m(0, 1), m(1, 1), m(2, 1), 0,
            m(0, 2), m(1, 2), m(2, 2), 0,
            m(0, 3), m(1, 3), m(2, 3), 1
    );
    for (size_t i = 0; i < vs; ++i)
    {
        if((vb[i] - va[i] * mr).length2() > 0.01f)
        {
            return {};
        }
    }
    return mr * mx;
}

static inline bool find(
        std::unique_ptr<AnimObj::Xforms> const& xFormsPtr,
        Alembic::Abc::M44d& xForm)
{
    if (!xFormsPtr)
    {
        return true;
    }
    AnimObj::Xforms const& xForms = *xFormsPtr;
    if (xForms.empty())
    {
        return true;
    }
    if (xForms.size() != 1)
    {
        return false;
    }
    xForm = xForms.front();
    return true;
}
}

static std::unique_ptr<AnimObj::Xforms> iterate(
        AnimObj::Geom::PsV const& ps,
        Alembic::Abc::M44d const& xForm,
        AnimObj::Geom::Points const& points)
{
    std::unique_ptr<AnimObj::Xforms> mvp = std::make_unique<AnimObj::Xforms>();
    for (AnimObj::Geom::Points::value_type const& point : points)
    {
        optional<Alembic::Abc::M44d> const& m = xForm::generate(ps, xForm, *point);
        if (m)
        {
            mvp->emplace_back(*m);
            continue;
        }
        return {};
    }
    return mvp;
}

static inline void copy(AnimObj::Geom::PsV const& ps, std::unique_ptr<AnimObj::Geom::Points>& psPtr)
{
    psPtr = std::make_unique<AnimObj::Geom::Points>();
    AnimObj::Geom::Points& points = *psPtr;
    points.reserve(1);
    points.emplace_back(std::make_unique<AnimObj::Geom::PsV>(ps));
}

static void optimize(
        AnimObj::Geom::PsV const& ps,
        AnimObj::Geom::Points const& points,
        AnimObj& anim,
        AnimObj& parent)
{
    Alembic::Abc::M44d xForm;
    if (!xForm::find(parent.xForms_, xForm))
    {
        return;
    }
    std::unique_ptr<AnimObj::Xforms> mvp = iterate(ps, xForm, points);
    if (!mvp)
    {
        return;
    }
    parent.xForms_ = std::move(mvp);
    parent.sampling_ = std::move(anim.sampling_);
    anim.sampling_.reset();
    AnimObj::Geom& geom = *anim.geom_;
    geom.normalData_.reset();
    copy(ps, geom.points_);
}

static void recurse(
        std::vector<std::future<void>>& futures,
        string_view const modelName,
        AnimObj& animObj,
        string_view const parentFullName,
        AnimObj* const prntPtr = nullptr)
{
    std::unique_ptr<AnimObj::Geom> const& geomPtr = animObj.geom_;
    if (geomPtr)
    {
        AnimObj::Geom::Points const& points = *geomPtr->points_;
        if (points.size() < 2)
        {
            return;
        }
        ModelData const* const mdPtr =
                levCheck1::findMatchFromParent(modelName, parentFullName);
        if (nullptr == mdPtr)
        {
            return;
        }
        if (nullptr == prntPtr)
        {
            return;
        }
        futures.emplace_back(std::async(std::launch::async, optimize,
                std::cref(modGather::getPs(*mdPtr)), std::cref(points),
                std::ref(animObj), std::ref(*prntPtr)));
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
            recurse(futures, modelName, *childAnimPtr,
                    parentFullName.to_string() + "/" + childAnimPtr->name_,
                    &animObj);
        }
    }
}

static inline void getFutures(std::vector<std::future<void>>& futures)
{
    for (std::future<void>& future : futures)
    {
        future.get();
    }
}

void traverse(
        AnimFiles const& animFiles,
        std::vector<std::pair<std::string, std::string>> const& modAnimFileNames)
{
    std::vector<std::future<void>> futures;
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
            recurse(futures,
                    extractAssetNameFromFilePath(modAnimFileNames[i].first),
                    *animObj,
                    "/" + animObj->name_);
        }
    }
    getFutures(futures);
}

void traverse(
        AnimFiles const& animFiles,
        std::vector<std::string> const& modFileNames)
{
    std::vector<std::future<void>> futures;
    size_t const sz = modFileNames.size();
    for (std::size_t i = 0; i < sz; ++i)
    {
        for (std::unique_ptr<AnimObj> const& animObj : *animFiles[i].second)
        {
            recurse(futures, extractAssetNameFromFilePath(modFileNames[i]),
                    *animObj, "/" + animObj->name_);
        }
    }
    getFutures(futures);
}
}
