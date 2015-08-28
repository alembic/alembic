
#include "anim.hpp"
#include "mod.hpp"

extern AnimConfig animConfig;

AlembicPath withoutMayaNamespaces(string_view const);
string_view withoutMayaNamespace(string_view const);

template <typename TV>
TV unique(TV);

template <typename Vec>
typename Vec::const_iterator findMatch(
        Vec const& dataVec,
        typename Vec::value_type::first_type const& match);

namespace levCheck1
{
bool equal(AnimObj::Topo const& a, AnimObj::Topo const& b);
}

namespace modGather
{
ModelData const* findMatch(
        AlembicPath matchPath,
        string_view const modName);
}

namespace animOut
{
bool hasCreases(ModelData const& modelData);

bool isSubDShape(string_view const n)
{
    string_view::size_type const s = n.length();
    if (9 > s)
    {
        return false;
    }
    return "SubDShape" == n.substr(s - 9);
}

template <typename OGeoSample>
void putGeoTopo(AnimObj::Topo const& topo, OGeoSample& oGeoSample)
{
    oGeoSample.setFaceIndices(*topo.ds_);
    oGeoSample.setFaceCounts(*topo.cs_);
}

template <typename Out>
static inline void putSampling(
        AnimObj::Sampling const& samplingPtr,
        Alembic::Abc::OArchive& oArch,
        Out& out,
        double const offset)
{
    if (!samplingPtr)
    {
        return;
    }
    AnimObj::SamplingData const& sampling = *samplingPtr;
    if (sampling.times_)
    {
        AnimObj::SamplingData::Times times(*sampling.times_);
        std::transform(times.cbegin(), times.cend(), times.begin(),
                [offset](AnimObj::SamplingData::Time const time)
                { return time + offset; });
        if (sampling.cyclic_)
        {
            out.setTimeSampling(oArch.addTimeSampling({Alembic::Abc::TimeSamplingType(
                    times.size(), sampling.cycle_), times}));
            return;
        }
        out.setTimeSampling(oArch.addTimeSampling({Alembic::Abc::TimeSamplingType(
                Alembic::Abc::TimeSamplingType::kAcyclic), times}));
        return;
    }
    out.setTimeSampling(oArch.addTimeSampling({sampling.cycle_, sampling.start_ + offset}));
}

template <typename OObj>
static inline void putVisible(
        AnimObj const& animObj,
        Alembic::Abc::OArchive& oArch,
        OObj& oObj,
        double const offset)
{
    std::unique_ptr<AnimObj::Visible> const& visiblePtr = animObj.visible_;
    if (!visiblePtr)
    {
        return;
    }
    Alembic::Abc::OCharProperty oVisibleProp(oObj.getProperties(), visiblePropName);
    AnimObj::Visible const& visible = *visiblePtr;
    putSampling(visible.sampling_, oArch, oVisibleProp, offset);
    boost::dynamic_bitset<> const& flags = visible.flags_;
    size_t const ns = flags.size();
    for (size_t i = 0; i < ns; ++i)
    {
        oVisibleProp.set(flags.test(i));
    }
}

template <typename ParamType>
static inline void putAnimGeoParams(
        AnimObj::Attributes::AttributeContainer<typename ParamType::value_type> const& attrsPtr,
        Alembic::Abc::OCompoundProperty const& arbParams,
        std::vector<std::string>& attrList,
        Alembic::Abc::OArchive& arch,
        double const offset)
{
    if (!attrsPtr)
    {
        return;
    }
    for (AnimObj::Attributes::Attribute<typename ParamType::value_type> const& attr : *attrsPtr)
    {
        std::unique_ptr<std::vector<typename ParamType::value_type>> const& valuesPtr = attr.values_;
        if (!valuesPtr)
        {
            continue;
        }
        std::string const& attrName = attr.name_;
        if (materialParamName == attrName)
        {
            continue;
        }
        attrList.emplace_back(attrName);
        typename Alembic::AbcGeom::OTypedGeomParam<ParamType> oGeomParam(
                arbParams, attrName, false, Alembic::AbcGeom::kConstantScope, 1);
        putSampling(attr.sampling_, arch, oGeomParam, offset);
        for (typename ParamType::value_type const& value : *valuesPtr)
        {
            oGeomParam.set(typename Alembic::AbcGeom::OTypedGeomParam<ParamType>::Sample(
                    {&value, 1}, Alembic::AbcGeom::kConstantScope));
        }
    }
}

static inline void putAnimArb(
        AnimObj const& animObj,
        Alembic::Abc::OCompoundProperty const& userProps,
        Alembic::Abc::OArchive& arch,
        Alembic::Abc::OCompoundProperty const& arbParams,
        double const offset,
        std::vector<std::string> const& modAttrList = {})
{
    std::vector<std::string> attrList;
    std::unique_ptr<AnimObj::Attributes> const& attrsPtr = animObj.attributes_;
    if (attrsPtr)
    {
        AnimObj::Attributes const& attrs = *attrsPtr;
        putAnimGeoParams<Alembic::Abc::Float64TPTraits>(attrs.doubles_, arbParams, attrList, arch, offset);
        putAnimGeoParams<Alembic::Abc::Float32TPTraits>(attrs.floats_, arbParams, attrList, arch, offset);
        putAnimGeoParams<Alembic::Abc::Int32TPTraits>(attrs.ints_, arbParams, attrList, arch, offset);
        putAnimGeoParams<Alembic::Abc::BooleanTPTraits>(attrs.bools_, arbParams, attrList, arch, offset);
        putAnimGeoParams<Alembic::Abc::StringTPTraits>(attrs.strings_, arbParams, attrList, arch, offset);
    }
    if (!modAttrList.empty())
    {
        attrList.insert(attrList.cend(), modAttrList.cbegin(), modAttrList.cend());
    }
    if (attrList.empty())
    {
        return;
    }
    Alembic::Abc::OStringArrayProperty(userProps, "attributes").set(unique(attrList));
}

namespace geo
{
template <typename OGeometrySample>
static inline void spec(
        AnimObj::Topo const&,
        AnimObj::Geom const&,
        OGeometrySample&,
        size_t const = 0)
{}

template <>
inline void spec(
        AnimObj::Topo const& modelTopo,
        AnimObj::Geom const& geom,
        Alembic::AbcGeom::OPolyMeshSchema::Sample& oPolyMeshSample,
        size_t const ndx)
{
    std::unique_ptr<AnimObj::NormalData> const& normalDataPtr = geom.normalData_;
    if (!normalDataPtr)
    {
        return;
    }
    AnimObj::NormalData const& normalData = *normalDataPtr;
    std::unique_ptr<AnimObj::NormalData::Normals> const& nsPtr = normalData.normals_;
    if (!nsPtr)
    {
        return;
    }
    AnimObj::NormalData::Normals const& normals = *nsPtr;
    if (!levCheck1::equal(geom.topo_, modelTopo))
    {
        return;
    }
    std::unique_ptr<AnimObj::NormalData::Indices> const& nxsPtr = normalData.indices_;
    std::unique_ptr<AnimObj::NormalData::NxV> const& nxPtr = normalData.index_;
    if (nxPtr)
    {
        AnimObj::NormalData::Indices const& indices = *nxsPtr;
        AnimObj::NormalData::NxV const& index = *nxPtr;
        oPolyMeshSample.setNormals({*normals[ndx], *indices[index[ndx]],
                Alembic::AbcGeom::kFacevaryingScope});
    }
    else if (nxsPtr)
    {
        AnimObj::NormalData::Indices const& indices = *nxsPtr;
        oPolyMeshSample.setNormals({*normals[ndx], *indices[ndx],
                Alembic::AbcGeom::kFacevaryingScope});
    }
    else
    {
        oPolyMeshSample.setNormals({*normals[ndx], Alembic::AbcGeom::kFacevaryingScope});
    }
}

template <typename OGeometry>
static inline void out(
        AnimObj const& animObj,
        Alembic::Abc::OObject& parentOut,
        Alembic::Abc::OArchive& archOut,
        double const offset)
{
    std::string const& childName = parentOut.getName() + "Shape";
    if (parentOut.getChildHeader(childName))
    {
        return;
    }
    OGeometry oGeometry(parentOut, childName);
    putVisible(animObj, archOut, oGeometry, offset);
    typename OGeometry::schema_type& oGeoSchema = oGeometry.getSchema();
    putAnimArb(animObj, oGeoSchema.getUserProperties(), archOut,
            oGeoSchema.getArbGeomParams(), offset);
    putSampling(animObj.sampling_, archOut, oGeoSchema, offset);
    typename OGeometry::schema_type::Sample oGeoSample;
    AnimObj::Geom const& geom = *animObj.geom_;
    AnimObj::Points const& points = *geom.points_;
    oGeoSample.setPositions(*points.front());
    AnimObj::Topo const& topo = geom.topo_;
    putGeoTopo(topo, oGeoSample);
    spec(topo, geom, oGeoSample);
    oGeoSchema.set(oGeoSample);
    size_t const sz = points.size();
    for (size_t i = 1; i < sz; ++i)
    {
        typename OGeometry::schema_type::Sample deformSample;
        deformSample.setPositions(*points[i]);
        spec(topo, geom, deformSample, i);
        oGeoSchema.set(deformSample);
    }
}

template <typename OGeometry>
static inline void out(
        ModelData const& modelData,
        AnimObj const& animObj,
        Alembic::Abc::OObject& parentOut,
        Alembic::Abc::OArchive& archOut,
        double const offset)
{
    AnimObj::Geom const& geom = *animObj.geom_;
    AnimObj::Points const& points = *geom.points_;
    size_t const sz = points.size();
    bool const replace = sz == 1 && !animConfig.noReplace;
    AnimObj::PsV const& lps = modGather::getPs(modelData);
    AnimObj::PsV const* psPtr = &lps;
    if (!replace)
    {
        std::size_t const lpsz = lps.size();
        psPtr = &*points.front();
        if (psPtr->size() != lpsz)
        {
            return;
        }
        for (size_t i = 1; i < sz; ++i)
        {
            if (points[i]->size() != lpsz)
            {
                return;
            }
        }
    }
    std::string const& childName = parentOut.getName() + "Shape";
    if (parentOut.getChildHeader(childName))
    {
        return;
    }
    OGeometry oGeometry(parentOut, childName);
    putVisible(animObj, archOut, oGeometry, offset);
    typename OGeometry::schema_type& oGeoSchema = oGeometry.getSchema();
    Alembic::Abc::OCompoundProperty const& userProps = oGeoSchema.getUserProperties();
    ModAttrs const& modAttrs = modGather::getModAttrs(modelData);
    modGather::putUserProps(userProps, modAttrs);
    Alembic::Abc::OCompoundProperty const& arbGeoParams = oGeoSchema.getArbGeomParams();
    putAnimArb(animObj, userProps, archOut, arbGeoParams, offset,
            modGather::getModAttrList(modAttrs));
    modGather::putParams(arbGeoParams, modAttrs);
    putSampling(animObj.sampling_, archOut, oGeoSchema, offset);
    typename OGeometry::schema_type::Sample oGeoSample;
    oGeoSample.setPositions(*psPtr);
    AnimObj::Topo const& topo = modGather::topoExtract(modelData);
    putGeoTopo(topo, oGeoSample);
    modGather::putDefUVset(modelData, oGeoSchema, oGeoSample);
    modGather::putUVsets(modelData, arbGeoParams);
    modGather::putRestPoints(modelData, arbGeoParams);
    modGather::putRestNormals(modelData, arbGeoParams);
    modGather::putAnimGeometrySpec(modelData, !isSubDShape(childName) && replace, oGeoSample);
    if (!replace)
    {
        spec(topo, geom, oGeoSample);
    }
    oGeoSchema.set(oGeoSample);
    for (size_t i = 1; i < sz; ++i)
    {
        typename OGeometry::schema_type::Sample deformSample;
        deformSample.setPositions(*points[i]);
        spec(topo, geom, deformSample, i);
        oGeoSchema.set(deformSample);
    }
}
}

static inline bool checkMatch(
        string_view const modName,
        AnimObj const& animObj,
        AlembicPath path,
        ModelData const*& mdPtr)
{
    AnimObj::AnimData const& animDataPtr = animObj.anims_;
    if (!animDataPtr)
    {
        return true;
    }
    if (animDataPtr->size() != 1)
    {
        return true;
    }
    std::unique_ptr<AnimObj> const& animObjPtr = animDataPtr->front();
    if (!animObjPtr->geom_)
    {
        return true;
    }
    path.emplace_back(withoutMayaNamespace(animObj.name_));
    mdPtr = modGather::findMatch(path, modName);
    if (nullptr == mdPtr)
    {
        return animConfig.passNoMatch;
    }
    return true;
}

static inline std::string formClone(
        string_view const modName,
        short int const ndx)
{
    char bf[5];
    snprintf(bf, 5, "%.4d", ndx);
    return std::string("CH") + bf + '_' + std::string(modName);
}

static inline std::string formNamespace(
        string_view const modName,
        short int const ndx,
        string_view const animName)
{
    if (-1 != ndx)
    {
        return formClone(modName, ndx) + ':';
    }
    if (animName.empty())
    {
        return {};
    }
    if (animConfig.formNamespaces)
    {
        return std::string(animName) + ':';
    }
    return {};
}

static inline std::string formChildNamespace(std::string n)
{
    if (animConfig.formNamespaces)
    {
        return n + ':' + n;
    }
    return n;
}

static inline std::string formChildName(
        string_view const modName,
        short int const ndx,
        string_view const animName)
{
    if (-1 != ndx)
    {
        return formChildNamespace(formClone(modName, ndx));
    }
    if (animName.empty())
    {
        return {};
    }
    return formChildNamespace(std::string(animName));
}

class Closure
{
   public:
    Closure(
        string_view const modName,
        Alembic::Abc::OArchive& archOut,
        string_view const animName = {},
        double const offset = 0,
        short int const ndx = -1)
    :
        modName_(modName),
        archOut_(archOut),
        offset_(offset),
        namespace_(formNamespace(modName, ndx, animName)),
        childName_(formChildName(modName, ndx, animName))
    {}
    void iterate(
            AnimObj::AnimData::element_type const* const animDataPtr,
            Alembic::Abc::OObject& topOut) const
    {
        AnimObj::AnimData::element_type const& animData = *animDataPtr;
        if (animConfig.groupByInstance)
        {
            if (childName_.empty())
            {
                return;
            }
            if (topOut.getChildHeader(childName_))
            {
                return;
            }
            Alembic::AbcGeom::OXform oXform(topOut, childName_);
            iterate(animData, oXform);
            return;
        }
        iterate(animData, topOut);
    }
   private:
    void iterate(
            AnimObj::AnimData::element_type const& animData,
            Alembic::Abc::OObject& parentOut) const
    {
        for (std::unique_ptr<AnimObj> const& animObj : animData)
        {
            recurse(*animObj, parentOut);
        }
    }
    void recurse(
            AnimObj const& animObj,
            Alembic::Abc::OObject& parentOut,
            ModelData const* mdPtr = nullptr) const
    {
        AlembicPath path(withoutMayaNamespaces(parentOut.getFullName()));
        if (animConfig.groupByInstance)
        {
            path.erase(path.cbegin());
        }
        if (animObj.geom_)
        {
            if (animConfig.forceNoMatch)
            {
                geo::out<Alembic::AbcGeom::OPolyMesh>(animObj, parentOut,
                        archOut_, offset_);
            }
            if (nullptr == mdPtr)
            {
                mdPtr = modGather::findMatch(path, modName_);
            }
            if (nullptr == mdPtr)
            {
                if (animConfig.passNoMatch)
                {
                    geo::out<Alembic::AbcGeom::OPolyMesh>(animObj,
                            parentOut, archOut_, offset_);
                }
                return;
            }
            ModelData const& modelData = *mdPtr;
            if (hasCreases(modelData))
            {
                geo::out<Alembic::AbcGeom::OSubD>(modelData, animObj,
                        parentOut, archOut_, offset_);
            }
            else
            {
                geo::out<Alembic::AbcGeom::OPolyMesh>(modelData, animObj,
                        parentOut, archOut_, offset_);
            }
        }
        else
        {
            if (!animConfig.forceNoMatch && !checkMatch(modName_,
                    animObj, path, mdPtr))
            {
                return;
            }
            std::string const& childName = namespace_ +
                    withoutMayaNamespace(animObj.name_).to_string();
            if (parentOut.getChildHeader(childName))
            {
                return;
            }
            Alembic::AbcGeom::OXform oXform(parentOut, childName);
            putVisible(animObj, archOut_, oXform, offset_);
            Alembic::AbcGeom::OXformSchema& oXformSchema = oXform.getSchema();
            putAnimArb(animObj, oXformSchema.getUserProperties(), archOut_,
                    oXformSchema.getArbGeomParams(), offset_);
            if (animObj.xForms_)
            {
                AnimObj::Xforms const& xForms = *animObj.xForms_;
                putSampling(animObj.sampling_, archOut_, oXformSchema, offset_);
                Alembic::AbcGeom::XformSample xformSample;
                xformSample.setMatrix(xForms.front());
                oXformSchema.set(xformSample);
                size_t const numSamples = xForms.size();
                for (size_t i = 1; i < numSamples; ++i)
                {
                    Alembic::AbcGeom::XformSample animSample;
                    animSample.setMatrix(xForms[i]);
                    oXformSchema.set(animSample);
                }
            }
            AnimObj::AnimData const& animPtr = animObj.anims_;
            if (!animPtr)
            {
                return;
            }
            for (std::unique_ptr<AnimObj> const& animObjPtr : *animPtr)
            {
                recurse(*animObjPtr, oXform, mdPtr);
            }
        }
    }
    string_view modName_;
    Alembic::Abc::OArchive& archOut_;
    double offset_;
    std::string namespace_;
    std::string childName_;
};

AnimObj::AnimData::element_type const* findMatchAnim(
        AnimFiles const& animFiles,
        std::string const& animFileName)
{
    AnimFiles::const_iterator const found = findMatch(animFiles, animFileName);
    if (animFiles.cend() == found)
    {
        return nullptr;
    }
    if (animFileName != found->first)
    {
        return nullptr;
    }
    return &*found->second;
}

template <bool dupAnim>
static inline void traverse(
        std::vector<std::string> const& modFileNames,
        std::vector<double> const& offsets,
        std::vector<std::string> const& animFileNames,
        AnimFiles const& animFiles,
        Alembic::Abc::OArchive& archOut);

template <>
inline void traverse<true>(
        std::vector<std::string> const& modFileNames,
        std::vector<double> const& offsets,
        std::vector<std::string> const& animFileNames,
        AnimFiles const& animFiles,
        Alembic::Abc::OArchive& archOut)
{
    Alembic::Abc::OObject topOut = archOut.getTop();
    size_t const sz = modFileNames.size();
    for (std::size_t i = 0; i < sz; ++i)
    {
        AnimObj::AnimData::element_type const* const animDataPtr =
                findMatchAnim(animFiles, animFileNames[i]);
        if (nullptr == animDataPtr)
        {
            continue;
        }
        Closure(extractAssetNameFromFilePath(modFileNames[i]),
                archOut, {}, offsets[i], i).iterate(animDataPtr, topOut);
    }
}

template <>
inline void traverse<false>(
        std::vector<std::string> const& modFileNames,
        std::vector<double> const& offsets,
        std::vector<std::string> const& animFileNames,
        AnimFiles const& animFiles,
        Alembic::Abc::OArchive& archOut)
{
    Alembic::Abc::OObject topOut = archOut.getTop();
    size_t const sz = modFileNames.size();
    for (std::size_t i = 0; i < sz; ++i)
    {
        Closure(extractAssetNameFromFilePath(modFileNames[i]), archOut,
                extractAssetNameFromFilePath(animFileNames[i]),
                offsets[i]).iterate(&*animFiles[i].second, topOut);
    }
}

inline void traverse(
        std::vector<std::string> const& modFileNames,
        std::vector<double> const& offsets,
        AnimFiles const& animFiles,
        Alembic::Abc::OArchive& archOut)
{
    Alembic::Abc::OObject topOut = archOut.getTop();
    size_t const sz = modFileNames.size();
    for (std::size_t i = 0; i < sz; ++i)
    {
        Closure(extractAssetNameFromFilePath(modFileNames[i]),
                archOut, {}, offsets[i]).iterate(&*animFiles[i].second, topOut);
    }
}

template <bool dupAnim = true, bool difAnim = true>
static inline void traverse(
        std::vector<std::string> const& modFileNames,
        AnimFiles const& animFiles,
        std::vector<std::string> const& animFileNames,
        Alembic::Abc::OArchive& archOut);

template <>
inline void traverse<true, true>(
        std::vector<std::string> const& modFileNames,
        AnimFiles const& animFiles,
        std::vector<std::string> const& animFileNames,
        Alembic::Abc::OArchive& archOut)
{
    Alembic::Abc::OObject topOut = archOut.getTop();
    size_t const sz = modFileNames.size();
    for (std::size_t i = 0; i < sz; ++i)
    {
        std::string const& animFileName = animFileNames[i];
        AnimObj::AnimData::element_type const* const animDataPtr =
                findMatchAnim(animFiles, animFileName);
        if (nullptr == animDataPtr)
        {
            continue;
        }
        Closure(extractAssetNameFromFilePath(modFileNames[i]), archOut,
                extractAssetNameFromFilePath(animFileName)
                ).iterate(animDataPtr, topOut);
    }
}

template <>
inline void traverse<true, false>(
        std::vector<std::string> const& modFileNames,
        AnimFiles const& animFiles,
        std::vector<std::string> const& animFileNames,
        Alembic::Abc::OArchive& archOut)
{
    Alembic::Abc::OObject topOut = archOut.getTop();
    size_t const sz = modFileNames.size();
    for (std::size_t i = 0; i < sz; ++i)
    {
        AnimObj::AnimData::element_type const* const animDataPtr =
                findMatchAnim(animFiles, animFileNames[i]);
        if (nullptr == animDataPtr)
        {
            continue;
        }
        Closure(extractAssetNameFromFilePath(modFileNames[i]),
                archOut).iterate(animDataPtr, topOut);
    }
}

template <>
inline void traverse<false, true>(
        std::vector<std::string> const& modFileNames,
        AnimFiles const& animFiles,
        std::vector<std::string> const& animFileNames,
        Alembic::Abc::OArchive& archOut)
{
    Alembic::Abc::OObject topOut = archOut.getTop();
    size_t const sz = modFileNames.size();
    for (std::size_t i = 0; i < sz; ++i)
    {
        Closure(extractAssetNameFromFilePath(modFileNames[i]), archOut,
                extractAssetNameFromFilePath(animFileNames[i])
                ).iterate(&*animFiles[i].second, topOut);
    }
}

inline void traverse(
        std::vector<std::string> const& modFileNames,
        AnimFiles const& animFiles,
        Alembic::Abc::OArchive& archOut)
{
    Alembic::Abc::OObject topOut = archOut.getTop();
    size_t const sz = modFileNames.size();
    for (std::size_t i = 0; i < sz; ++i)
    {
        Closure(extractAssetNameFromFilePath(modFileNames[i]),
                archOut).iterate(&*animFiles[i].second, topOut);
    }
}

void merge(
        bool const dupAnim,
        std::vector<std::string> const& modFileNames,
        std::vector<double> const& offsets,
        std::vector<std::string> const& animFileNames,
        AnimFiles const& animFiles,
        bool const difAnim,
        Alembic::Abc::OArchive& archOut)
{
    if (offsets.empty())
    {
        if (dupAnim)
        {
            if (difAnim)
            {
                animOut::traverse(modFileNames, animFiles, animFileNames, archOut);
            }
            else
            {
                animOut::traverse<true, false>(modFileNames, animFiles, animFileNames, archOut);
            }
        }
        else
        {
            if (difAnim)
            {
                animOut::traverse<false, true>(modFileNames, animFiles, animFileNames, archOut);
            }
            else
            {
                animOut::traverse(modFileNames, animFiles, archOut);
            }
        }
        return;
    }
    if (dupAnim)
    {
        animOut::traverse<true>(modFileNames, offsets, animFileNames, animFiles, archOut);
    }
    else
    {
        if (difAnim)
        {
            animOut::traverse<false>(modFileNames, offsets, animFileNames, animFiles, archOut);
        }
        else
        {
            animOut::traverse(modFileNames, offsets, animFiles, archOut);
        }
    }
}
}
