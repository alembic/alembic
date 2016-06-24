
#include <vector>
#include <string>
#include <algorithm>
#include <experimental/string_view>
#include <boost/variant.hpp>
#include <Alembic/AbcMaterial/MaterialAssignment.h>
#include "anim.hpp"

using std::experimental::string_view;

string_view extractAssetNameFromFilePath(string_view const filePath);

string_view withoutMayaNamespace(string_view const s);

namespace animGather
{
Alembic::Abc::IArchive getArchive(std::string const&);
template <typename IGeoSample> AnimObj::Geom::Topo get(IGeoSample const&);
AnimObj::Geom::UVset defUVset(Alembic::AbcGeom::IV2fGeomParam& uvParam);
}

namespace levOpt3
{
AnimObj::Geom::NormalData::NsV normGen(
        AnimObj::Geom::PsV const& ps,
        AnimObj::Geom::Topo const& tp);
}

namespace
{
class XformStack
{
   public:
    XformStack(XformStack const&) = delete;
    XformStack& operator=(XformStack const&) = delete;
    XformStack() : ms_(1) {}
    void pop() { ms_.pop_back(); }
    Alembic::Abc::M44d const& top() const { return ms_.back(); }
    void push(Alembic::Abc::M44d const& mx) { ms_.emplace_back(mx * top()); }
    void setWorldScale(double const factor) { ms_.front().setScale(factor); }

   private:
    std::vector<Alembic::Abc::M44d> ms_;
};

class UVset
{
   public:
    UVset() = delete;
    UVset(UVset const&) = delete;
    UVset& operator=(UVset const&) = delete;
    UVset(AnimObj::Geom::uvs_vt::value_type const* const uvs,
          std::size_t const uvLen,
          std::string const& nm,
          AnimObj::Geom::nds_vt::value_type const* const nds = nullptr,
          std::size_t const ndLen = 0)
    {
        std::unique_ptr<AnimObj::Geom::uvs_vt> uvsPtr =
                std::make_unique<AnimObj::Geom::uvs_vt>();
        putValues(uvs, uvLen, *uvsPtr);
        if (0 == ndLen)
        {
            uvSet_ = std::make_tuple(std::move(uvsPtr), nullptr, nm);
            return;
        }
        std::unique_ptr<AnimObj::Geom::nds_vt> ndsPtr =
                std::make_unique<AnimObj::Geom::nds_vt>();
        putValues(nds, ndLen, *ndsPtr);
        uvSet_ = std::make_tuple(std::move(uvsPtr), std::move(ndsPtr), nm);
    }
    UVset(AnimObj::Geom::UVset uvSet) : uvSet_(std::move(uvSet)) {}
    bool ndx() const
    {
        return ndsPtr() ? false : true;
    }
    AnimObj::Geom::nds_vt const& nds() const
    {
        return *ndsPtr();
    }
    std::string const& nm() const
    {
        return std::get<std::string>(uvSet_);
    }
    AnimObj::Geom::uvs_vt const& uvs() const
    {
        return *std::get<std::unique_ptr<AnimObj::Geom::uvs_vt>>(uvSet_);
    }
   private:
    std::unique_ptr<AnimObj::Geom::nds_vt> const& ndsPtr() const
    {
        return std::get<std::unique_ptr<AnimObj::Geom::nds_vt>>(uvSet_);
    }
    AnimObj::Geom::UVset uvSet_;
};

class Positions
{
   public:
    Positions() = delete;
    Positions(Positions const&) = delete;
    Positions& operator=(Positions const&) = delete;
    using ps_vt = Alembic::AbcGeom::P3fArraySample::value_vector;
    Positions(ps_vt::value_type const* const ps, std::size_t const ln)
    {
        ps_.resize(ln);
        memcpy(ps_.data(), ps, sizeof(ps_vt::value_type) * ln);
    }
    Positions(ps_vt::value_type const* const ps,
              std::size_t const ln,
              Alembic::Abc::M44d const& mx)
        : Positions(ps, ln)
    {
        wps_.resize(ln);
        for (std::size_t i = 0; i < ln; ++i)
        {
            mx.multVecMatrix(ps_[i], wps_[i]);
        }
    }
    ps_vt const& worldSpace() const
    {
        return wps_.empty() ? ps_ : wps_;
    }
    ps_vt const& localSpace() const
    {
        return ps_;
    }
    
   private:
    ps_vt ps_;
    ps_vt wps_;
};

class Normals
{
   public:
    Normals() = delete;
    Normals(Normals const&) = delete;
    Normals& operator=(Normals const&) = delete;
    using ns_vt = Alembic::AbcGeom::N3fArraySample::value_vector;
    void l2w(Alembic::Abc::M44d const& mx)
    {
        Alembic::Abc::M44d const m = mx.inverse().transposed();
        std::size_t const sz = lns_.size();
        wns_.resize(sz);
        for (std::size_t i = 0; i < sz; ++i)
        {
            ns_vt::value_type& n = wns_[i];
            m.multDirMatrix(lns_[i], n);
            n.normalize();
        }
    }
    Normals(Alembic::AbcGeom::N3fArraySamplePtr const ns,
            Alembic::Abc::M44d const& mx,
            Alembic::AbcGeom::GeometryScope sc = Alembic::AbcGeom::kVertexScope,
            Alembic::AbcGeom::UInt32ArraySamplePtr const ds = nullptr)
        : sc_(sc)
    {
        if (ds)
        {
            std::size_t const dsLen = ds->size();
            if (dsLen)
            {
                ds_.resize(dsLen);
                memcpy(ds_.data(), ds->get(), sizeof(Alembic::Abc::uint32_t) * dsLen);
            }
        }
        std::size_t const nsLen = ns->size();
        ns_vt::value_type const* const nsDat = ns->get();
        lns_.resize(nsLen);
        memcpy(lns_.data(), nsDat, sizeof(ns_vt::value_type) * nsLen);
        l2w(mx);
    }
    Normals(Positions::ps_vt const& ps,
            AnimObj::Geom::Topo const& tp,
            Alembic::Abc::M44d const& mx)
        : sc_(Alembic::AbcGeom::kVertexScope), lns_(levOpt3::normGen(ps, tp))
    {
        for (ns_vt::value_type& n : lns_)
        {
            n.normalize();
        }
        l2w(mx);
    }
    Alembic::AbcGeom::GeometryScope sc_;
    ns_vt wns_;
    ns_vt lns_;
    std::vector<Alembic::Abc::uint32_t> ds_;
};

class Creases
{
   public:
    Creases() = delete;
    Creases(Creases const&) = delete;
    Creases& operator=(Creases const&) = delete;
    Creases(Alembic::AbcGeom::Int32ArraySamplePtr const ds,
            Alembic::AbcGeom::Int32ArraySamplePtr const ls,
            Alembic::AbcGeom::FloatArraySamplePtr const ss)
    {
        cp(ds, ds_);
        cp(ls, ls_);
        cp(ss, ss_);
    }
    using sharp_vt = Alembic::AbcGeom::FloatArraySample::value_vector;
    AnimObj::Geom::Topo::TpV ds_;
    AnimObj::Geom::Topo::TpV ls_;
    sharp_vt ss_;

   private:
    template <typename Pt, typename Vc>
    static void cp(Pt const pt, Vc& vc)
    {
        if (!pt)
        {
            return;
        }
        if (!pt->valid())
        {
            return;
        }
        std::size_t const ln = pt->size();
        if (0 == ln)
        {
            return;
        }
        vc.resize(ln);
        memcpy(vc.data(), pt->get(), sizeof(typename Vc::value_type) * ln);
    }
};

using UVsetPtr = std::unique_ptr<UVset>;

class UVsets
{
   public:
    UVsets() = default;
    UVsets(Alembic::Abc::ICompoundProperty const& arbGeoParams)
    {
        std::size_t const numProperties = arbGeoParams.getNumProperties();
        for (std::size_t j = 0; j < numProperties; ++j)
        {
            Alembic::Abc::PropertyHeader const& propertyHeader =
                    arbGeoParams.getPropertyHeader(j);
            if (!Alembic::AbcGeom::IV2fGeomParam::matches(propertyHeader))
            {
                continue;
            }
            if (!Alembic::AbcGeom::isUV(propertyHeader))
            {
                continue;
            }
            std::string const& propertyName = propertyHeader.getName();
            Alembic::AbcGeom::IV2fGeomParam uvParam(arbGeoParams, propertyName);
            if (!uvParam.valid())
            {
                continue;
            }
            Alembic::AbcGeom::V2fArraySamplePtr const uvPtr =
                    uvParam.getValueProperty().getValue();
            if (uvParam.isIndexed())
            {
                Alembic::AbcGeom::UInt32ArraySamplePtr const uvNdxPtr =
                        uvParam.getIndexProperty().getValue();
                uvSets_.emplace_back(std::make_unique<UVset>(
                        uvPtr->get(), uvPtr->size(), propertyName,
                        uvNdxPtr->get(), uvNdxPtr->size()));
            }
            else
            {
                uvSets_.emplace_back(std::make_unique<UVset>(uvPtr->get(),
                        uvPtr->size(), propertyName));
            }
        }
    }
    void put(Alembic::Abc::OCompoundProperty const& arbGeoParams) const
    {
        if (uvSets_.empty())
        {
            return;
        }
        for (UVsetPtr const& uvSetPtr : uvSets_)
        {
            if (!uvSetPtr)
            {
                continue;
            }
            UVset const& uvSet = *uvSetPtr;
            if (uvSet.ndx())
            {
                Alembic::AbcGeom::OV2fGeomParam(arbGeoParams, uvSet.nm(), false,
                        Alembic::AbcGeom::kFacevaryingScope, 1).set({uvSet.uvs(),
                                Alembic::AbcGeom::kFacevaryingScope});
            }
            else
            {
                Alembic::AbcGeom::OV2fGeomParam(arbGeoParams, uvSet.nm(), true,
                        Alembic::AbcGeom::kFacevaryingScope, 1).set({uvSet.uvs(),
                                uvSet.nds(), Alembic::AbcGeom::kFacevaryingScope});
            }
        }
    }

   private:
    std::vector<UVsetPtr> uvSets_;
};

template <typename ParamType>
class IndexedAttrs
{
   public:
    IndexedAttrs() = delete;
    IndexedAttrs(IndexedAttrs const&) = delete;
    IndexedAttrs& operator=(IndexedAttrs const&) = delete;
    using NdsPtr = std::unique_ptr<std::vector<Alembic::Abc::uint32_t>>;
    IndexedAttrs(
          typename ParamType::value_type const* const pas,
          std::size_t const paLen,
          std::string const& nm,
          NdsPtr::element_type::value_type const* const nds,
          std::size_t const ndLen)
        : nm_(nm),
          pas_(std::make_unique<typename ParamType::value_vector>(pas, pas + paLen)),
          nds_(std::make_unique<NdsPtr::element_type>(ndLen))
    {
        memcpy(nds_->data(), nds, sizeof(NdsPtr::element_type::value_type) * ndLen);
    }
    IndexedAttrs(
            std::unique_ptr<typename ParamType::value_vector> psPtr,
            std::string const& nm,
            NdsPtr ndsPtr)
        : nm_(nm), pas_(std::move(psPtr)), nds_(std::move(ndsPtr)) {}
    std::string nm_;
    std::unique_ptr<typename ParamType::value_vector> pas_;
    NdsPtr nds_;
};

class ParamVisitor : public boost::static_visitor<>
{
   public:
    ParamVisitor(
            Alembic::Abc::OCompoundProperty const& outParams,
            std::string const& paramName)
        : outParams_(outParams), paramName_(paramName)
    {}
    void operator()(Alembic::AbcGeom::OBoolGeomParam::value_type const& b) const
    {
        Alembic::AbcGeom::OBoolGeomParam(outParams_, paramName_, false,
                Alembic::AbcGeom::kConstantScope, 1).set({{&b, 1},
                        Alembic::AbcGeom::kConstantScope});
    }
    void operator()(int const& i) const
    {
        Alembic::AbcGeom::OInt32GeomParam(outParams_, paramName_, false,
                Alembic::AbcGeom::kConstantScope, 1).set({{&i, 1},
                        Alembic::AbcGeom::kConstantScope});
    }
    void operator()(float const& f) const
    {
        Alembic::AbcGeom::OFloatGeomParam(outParams_, paramName_, false,
                Alembic::AbcGeom::kConstantScope, 1).set({{&f, 1},
                        Alembic::AbcGeom::kConstantScope});
    }
    void operator()(double const& d) const
    {
        Alembic::AbcGeom::ODoubleGeomParam(outParams_, paramName_, false,
                Alembic::AbcGeom::kConstantScope, 1).set({{&d, 1},
                        Alembic::AbcGeom::kConstantScope});
    }
    void operator()(std::string const& s) const
    {
        Alembic::AbcGeom::OStringGeomParam(outParams_, paramName_, false,
                Alembic::AbcGeom::kConstantScope, 1).set({{&s, 1},
                        Alembic::AbcGeom::kConstantScope});
    }
    
   private:
    Alembic::Abc::OCompoundProperty const& outParams_;
    std::string const& paramName_;
};
}

class ModAttrs
{
   public:
    ModAttrs() = default;
    using attrs_t = std::vector<std::pair<std::string, boost::variant<
            Alembic::AbcGeom::OBoolGeomParam::value_type, int, float, double, std::string>>>;
    template <typename TV>
    void add(std::string const& n, TV const& v)
    {
        attrs_.emplace_back(n, v);
    }
    using sample_t = Alembic::Abc::StringArraySample;
    using ss_t = std::vector<std::unique_ptr<IndexedAttrs<sample_t>>>;
    ss_t ss_;
    attrs_t attrs_;
};

struct ModelData
{
    ModAttrs as_;
    UVsetPtr defUVset_;
    UVsets uvSets_;
    AnimObj::Geom::Topo tp_;
    std::unique_ptr<Positions> ps_;
    std::unique_ptr<Normals> ns_;
    std::unique_ptr<Creases> cs_;
    Alembic::Abc::M44d mx_;
};

using DataModels = std::vector<std::pair<AlembicPath, std::unique_ptr<ModelData>>>;

static XformStack xFormStack;
static std::unique_ptr<DataModels> dataModels;

extern ModConfig modConfig;

template <typename IGeoSchema>
static inline void gatherModelSpecific(IGeoSchema const& iGeoSchema, ModelData& modelData);

template <>
inline void gatherModelSpecific(Alembic::AbcGeom::IPolyMeshSchema const& iMeshSchema, ModelData& modelData)
{
    if (!modConfig.outNormals)
    {
        return;
    }
    Alembic::AbcGeom::IN3fGeomParam normalParam = iMeshSchema.getNormalsParam();
    if (!normalParam.valid())
    {
        return;
    }
    Alembic::Abc::IN3fArrayProperty const& normalProp = normalParam.getValueProperty();
    Alembic::AbcGeom::N3fArraySamplePtr const nsPtr = normalProp.getValue();
    if (!nsPtr->valid())
    {
        return;
    }
    if (normalParam.isIndexed())
    {
        Alembic::AbcGeom::UInt32ArraySamplePtr const nsNdxPtr = normalParam.getIndexProperty().getValue();
        if (!nsNdxPtr->valid())
        {
            return;
        }
        modelData.ns_ = std::make_unique<Normals>(
                nsPtr, modelData.mx_, Alembic::AbcGeom::kFacevaryingScope, nsNdxPtr);
    }
    else
    {
        modelData.ns_ = std::make_unique<Normals>(nsPtr, modelData.mx_, normalParam.getScope());
    }
}

template <>
inline void gatherModelSpecific(Alembic::AbcGeom::ISubDSchema const& iSubDschema, ModelData& modelData)
{
    Alembic::Abc::IInt32ArrayProperty const& ds = iSubDschema.getCreaseIndicesProperty();
    if (!ds.valid())
    {
        return;
    }
    Alembic::Abc::IInt32ArrayProperty const& ls = iSubDschema.getCreaseLengthsProperty();
    if (!ls.valid())
    {
        return;
    }
    Alembic::Abc::IFloatArrayProperty const ss = iSubDschema.getCreaseSharpnessesProperty();
    if (!ss.valid())
    {
        return;
    }
    modelData.cs_ = std::make_unique<Creases>(ds.getValue(), ls.getValue(), ss.getValue());
}

static inline AlembicPath split(string_view const s)
{
    AlembicPath r;
    if (!s.empty() && s.length() > 1)
    {
        string_view::size_type a = 1;
        string_view::size_type b = s.find_first_of('/', a);
        for (;;)
        {
            bool const e = string_view::npos == b;
            string_view::size_type c = a;
            if (e)
            {
                c = 0;
            }
            r.emplace_back(s.substr(a, b - c));
            if (e)
            {
                break;
            }
            a = b + 1;
            b = s.find_first_of('/', a);
        }
    }
    return r;
}

template <typename ParamType>
static inline void gatherDataParam(
        Alembic::Abc::ICompoundProperty const& geoParams,
        std::string const& paramName,
        ModAttrs& modAttrs)
{
    typename Alembic::AbcGeom::ITypedGeomParam<ParamType> iGeomParam(geoParams, paramName);
    if (!iGeomParam.valid())
    {
        return;
    }
    typename Alembic::AbcGeom::ITypedArrayProperty<ParamType> const& iValueProperty = iGeomParam.getValueProperty();
    if (!iValueProperty.valid())
    {
        return;
    }
    Alembic::Util::shared_ptr<Alembic::Abc::TypedArraySample<ParamType>> const arraySamplePtr = iValueProperty.getValue();
    if (arraySamplePtr->size() != 1)
    {
        return;
    }
    modAttrs.add(paramName, arraySamplePtr->get()[0]);
}

static inline void gatherDataParams(Alembic::Abc::ICompoundProperty const& geoParams, ModAttrs& modAttrs)
{
    size_t const numProperties = geoParams.getNumProperties();
    for (size_t i = 0; i < numProperties; ++i)
    {
        Alembic::Abc::PropertyHeader const& propertyHeader = geoParams.getPropertyHeader(i);
        std::string const& paramName = propertyHeader.getName();
        if (paramName.length() < 4)
        {
            continue;
        }
        if (paramName.substr(0, 4) != "ABC_")
        {
            continue;
        }
        Alembic::Abc::DataType const& dataType = propertyHeader.getDataType();
        uint8_t const extent = dataType.getExtent();
        if (extent)
        {
            if (1 != extent)
            {
                continue;
            }
            switch (dataType.getPod())
            {
                case Alembic::Abc::kBooleanPOD:
                    gatherDataParam<Alembic::Abc::BooleanTPTraits>(geoParams, paramName, modAttrs);
                    break;
                case Alembic::Abc::kInt32POD:
                    gatherDataParam<Alembic::Abc::Int32TPTraits>(geoParams, paramName, modAttrs);
                    break;
                case Alembic::Abc::kFloat32POD:
                    gatherDataParam<Alembic::Abc::Float32TPTraits>(geoParams, paramName, modAttrs);
                    break;
                case Alembic::Abc::kFloat64POD:
                    gatherDataParam<Alembic::Abc::Float64TPTraits>(geoParams, paramName, modAttrs);
                    break;
                case Alembic::Abc::kStringPOD:
                    gatherDataParam<Alembic::Abc::StringTPTraits>(geoParams, paramName, modAttrs);
                    break;
                default:
                    break;
            }
        }
        else
        {
            Alembic::AbcGeom::IStringGeomParam iStringGeomParam(geoParams, paramName);
            if (!iStringGeomParam.valid())
            {
                continue;
            }
            Alembic::AbcGeom::IStringArrayProperty const& iStringArrayProperty = iStringGeomParam.getValueProperty();
            if (!iStringArrayProperty.valid())
            {
                continue;
            }
            Alembic::Abc::StringArraySamplePtr const stringArraySamplePtr = iStringArrayProperty.getValue();
            if (!iStringGeomParam.isIndexed())
            {
                continue;
            }
            Alembic::AbcGeom::IUInt32ArrayProperty const& iUInt32ArrayProperty = iStringGeomParam.getIndexProperty();
            if (!iUInt32ArrayProperty.valid())
            {
                continue;
            }
            Alembic::Abc::UInt32ArraySamplePtr const uInt32ArraySamplePtr = iUInt32ArrayProperty.getValue();
            if (uInt32ArraySamplePtr->size() < 2)
            {
                continue;
            }
            modAttrs.ss_.emplace_back(std::make_unique<ModAttrs::ss_t::value_type::element_type>(
                    stringArraySamplePtr->get(), stringArraySamplePtr->size(), paramName,
                    uInt32ArraySamplePtr->get(), uInt32ArraySamplePtr->size()));
        }
    }
}

static bool multiMod = false;

static inline string_view strpSG(string_view const n)
{
    string_view::size_type const s = n.length();
    if (s < 2)
    {
        return n;
    }
    if (n.substr(s - 2) == "SG")
    {
        return n.substr(0, s - 2);
    }
    return n;
}

static inline std::string formShaderName(
        string_view const materialName,
        string_view const assetName)
{
    return "/obj/" + modConfig.nameProject + '_' + assetName.to_string() +
            "_shaders1/shops/" + materialName.to_string() + "_out";
}

static inline bool gatherModelShaderName(
        string_view const assetName,
        Alembic::Abc::IObject const& child,
        std::string& shaderName)
{
    std::string materialAssignmentPath;
    if (!Alembic::AbcMaterial::getMaterialAssignmentPath(child, materialAssignmentPath))
    {
        return false;
    }
    if (materialAssignmentPath.empty())
    {
        return false;
    }
    string_view const materialName = materialAssignmentPath;
    string_view::size_type const lastSlash = materialName.find_last_of('/');
    shaderName = formShaderName(materialName.substr(
            lastSlash == string_view::npos ? 0 : lastSlash + 1), assetName);
    return true;
}

template <typename IGeoSchema>
static inline void gatherModelFaces(
        size_t const faceCount,
        string_view const assetName,
        IGeoSchema& iGeoSchema,
        ModAttrs& modAttrs)
{
    using NdsPtr = ModAttrs::ss_t::value_type::element_type::NdsPtr;
    NdsPtr ndsPtr = std::make_unique<NdsPtr::element_type>(faceCount);
    using FsPtr = std::unique_ptr<ModAttrs::sample_t::value_vector>;
    FsPtr fsPtr = std::make_unique<FsPtr::element_type>();
    using StrVc = FsPtr::element_type;
    StrVc& setNames = *fsPtr;
    iGeoSchema.getFaceSetNames(setNames);
    StrVc::size_type const lenSetNames = setNames.size();
    if (0 == lenSetNames)
    {
        return;
    }
    for (StrVc::size_type i = 0; i < lenSetNames; ++i)
    {
        StrVc::value_type& setName = setNames[i];
        Alembic::AbcGeom::IFaceSet const& iFaceSet = iGeoSchema.getFaceSet(setName);
        if (!iFaceSet.valid())
        {
            continue;
        }
        Alembic::Abc::Int32ArraySamplePtr const faceSetPtr = iFaceSet.getSchema().getValue().getFaces();
        if (!faceSetPtr->valid())
        {
            continue;
        }
        NdsPtr::element_type& mis = *ndsPtr;
        Alembic::Abc::int32_t const* const faces = faceSetPtr->get();
        size_t const faceSetLen = faceSetPtr->size();
        for (size_t j = 0; j < faceSetLen; ++j)
        {
            mis[faces[j]] = i;
        }
        if (gatherModelShaderName(assetName, iFaceSet, setName))
        {
            continue;
        }
        setName = formShaderName(strpSG(setName), assetName);
    }
    modAttrs.ss_.emplace_back(std::make_unique<ModAttrs::ss_t::value_type::element_type>(
            std::move(fsPtr), materialParamName, std::move(ndsPtr)));
}

static inline void gatherModelMaterial(
        string_view const assetName,
        Alembic::Abc::IObject const& child,
        ModAttrs& modAttrs)
{
    std::string shaderName;
    if (!gatherModelShaderName(assetName, child, shaderName))
    {
        return;
    }
    modAttrs.add(materialParamName, shaderName);
}

static inline AlembicPath withoutMayaNamespaces(AlembicPath p)
{
    std::transform(p.cbegin(), p.cend(), p.begin(), [](AlembicPath::value_type const& s)
            { return withoutMayaNamespace(s).to_string(); });
    return p;
}

AlembicPath withoutMayaNamespaces(string_view const v)
{
    return withoutMayaNamespaces(split(v));
}

template <typename IGeometry>
static inline void gatherModelGeometry(
        Alembic::Abc::IObject const& parent,
        size_t const childIndex,
        string_view const prefix)
{
    Alembic::Abc::IObject const& child = parent.getChild(childIndex);
    if (!child.valid())
    {
        return;
    }
    std::unique_ptr<ModelData> modelDataPtr = std::make_unique<ModelData>();
    ModelData& modelData = *modelDataPtr;
    modelData.mx_ = xFormStack.top();
    ModAttrs& modAttrs = modelData.as_;
    gatherModelMaterial(prefix, child, modAttrs);
    IGeometry iGeometry(child, Alembic::Abc::kWrapExisting);
    if (!iGeometry.valid())
    {
        return;
    }
    typename IGeometry::schema_type const& iGeoSchema = iGeometry.getSchema();
    typename IGeometry::schema_type::Sample const& iGeoSample = iGeoSchema.getValue();
    modelData.tp_ = animGather::get(iGeoSample);
    gatherModelFaces(modelData.tp_.cs_->size(), prefix, iGeometry.getSchema(), modAttrs);
    gatherModelSpecific(iGeoSchema, modelData);
    Alembic::AbcGeom::P3fArraySamplePtr const psPtr = iGeoSample.getPositions();
    modelData.ps_ = modConfig.outPositions ?
            std::make_unique<Positions>(psPtr->get(), psPtr->size(), modelData.mx_) :
            std::make_unique<Positions>(psPtr->get(), psPtr->size());
    if (modConfig.genNormals && !modelData.ns_)
    {
        modelData.ns_ = std::make_unique<Normals>(modelData.ps_->localSpace(), modelData.tp_, modelData.mx_);
    }
    if (modConfig.outUVs)
    {
        Alembic::AbcGeom::IV2fGeomParam uvParam = iGeoSchema.getUVsParam();
        if (uvParam.valid())
        {
            modelData.defUVset_ = std::make_unique<UVset>(animGather::defUVset(uvParam));
        }
    }
    Alembic::Abc::ICompoundProperty const& arbGeoParams = iGeoSchema.getArbGeomParams();
    if (arbGeoParams.valid())
    {
        if (modConfig.outUVs)
        {
            modelData.uvSets_ = UVsets(arbGeoParams);
        }
        gatherDataParams(arbGeoParams, modAttrs);
    }
    AlembicPath path(withoutMayaNamespaces(parent.getFullName()));
    if (multiMod)
    {
        path.emplace(path.begin(), prefix);
    }
    dataModels->emplace_back(path, std::move(modelDataPtr));
}

static inline Alembic::Abc::M44d gatherModelMatrix(Alembic::AbcGeom::IXformSchema const& iXformSchema)
{
    size_t const numSamples = iXformSchema.getNumSamples();
    if (0 == numSamples)
    {
        return Alembic::Abc::M44d();
    }
    Alembic::AbcGeom::XformSample const& xFormSample = iXformSchema.getValue();
    return xFormSample.getMatrix();
}

static void gatherDataModel(Alembic::Abc::IObject const& parent, string_view const prefix);

inline void gatherModelXform(Alembic::Abc::IObject const& child, string_view const prefix)
{
    if (
            modConfig.outPositions ||
            modConfig.outNormals ||
            modConfig.genNormals)
    {
        Alembic::AbcGeom::IXform const iXform(child, Alembic::Abc::kWrapExisting);
        Alembic::AbcGeom::IXformSchema const& iXformSchema = iXform.getSchema();
        xFormStack.push(gatherModelMatrix(iXformSchema));
        gatherDataModel(child, prefix);
        xFormStack.pop();
        return;
    }
    gatherDataModel(child, prefix);
}

static void gatherDataModel(Alembic::Abc::IObject const& parent, string_view const prefix)
{
    size_t const numChildren = parent.getNumChildren();
    for (size_t i = 0; i < numChildren; ++i)
    {
        Alembic::Abc::ObjectHeader const& childHeader = parent.getChildHeader(i);
        if (Alembic::AbcGeom::IXform::matches(childHeader))
        {
            gatherModelXform(parent.getChild(i), prefix);
        }
        else if (Alembic::AbcGeom::IPolyMesh::matches(childHeader))
        {
            gatherModelGeometry<Alembic::AbcGeom::IPolyMesh>(parent, i, prefix);
        }
        else if (Alembic::AbcGeom::ISubD::matches(childHeader))
        {
            gatherModelGeometry<Alembic::AbcGeom::ISubD>(parent, i, prefix);
        }
    }
}

template <typename TV>
bool compareFirsts(
        typename TV::value_type const& a,
        typename TV::value_type const& b)
{
    return a.first < b.first;
}

template bool compareFirsts<AnimFiles>(
        typename AnimFiles::value_type const& a,
        typename AnimFiles::value_type const& b);

namespace dataModel
{
void sort()
{
    std::sort(dataModels->begin(), dataModels->end(), compareFirsts<DataModels>);
}

bool duplicates()
{
    return static_cast<std::size_t>(std::distance(dataModels->begin(),
            std::unique(dataModels->begin(), dataModels->end(),
            [](DataModels::value_type const& a, DataModels::value_type const& b)
            { return a.first == b.first; }))) < dataModels->size();
}

void reset()
{
    dataModels.reset();
}

bool empty()
{
    return dataModels->empty();
}

void scale(double const scaleFactor)
{
    xFormStack.setWorldScale(scaleFactor);
}
}

namespace animOut
{
bool hasCreases(ModelData const& modelData)
{
    return modelData.cs_ ? true : false;
}
}

template <typename Vec>
typename Vec::const_iterator findMatch(
        Vec const& dataVec,
        typename Vec::value_type::first_type const& match)
{
    return std::lower_bound(dataVec.cbegin(), dataVec.cend(),
            std::make_pair(match, typename Vec::value_type::second_type()),
            compareFirsts<Vec>);
}

template typename AnimFiles::const_iterator findMatch(
        AnimFiles const& dataVec,
        typename AnimFiles::value_type::first_type const& match);

static inline ModelData const* findMatchMod(AlembicPath const& matchPath)
{
    DataModels::const_iterator const found = findMatch(*dataModels, matchPath);
    if (dataModels->cend() == found)
    {
        return nullptr;
    }
    if (matchPath != found->first)
    {
        return nullptr;
    }
    return &*found->second;
}

namespace modGather
{
template <typename OGeoSample>
void putAnimGeometrySpec(
        ModelData const& modelData,
        const bool setNorms,
        OGeoSample& oGeoSample);

template <>
void putAnimGeometrySpec(
        ModelData const& modelData,
        const bool setNorms,
        Alembic::AbcGeom::OPolyMeshSchema::Sample& oPolyMeshSample)
{
    if (!setNorms)
    {
        return;
    }
    std::unique_ptr<Normals> const& nsPtr = modelData.ns_;
    if (!nsPtr)
    {
        return;
    }
    Normals::ns_vt const& ns = nsPtr->lns_;
    std::vector<Alembic::Abc::uint32_t> const& ds = nsPtr->ds_;
    if (ds.empty())
    {
        oPolyMeshSample.setNormals({ns, nsPtr->sc_});
        return;
    }
    oPolyMeshSample.setNormals({ns, ds, Alembic::AbcGeom::kFacevaryingScope});
}

template <>
void putAnimGeometrySpec(
        ModelData const& modelData,
        const bool,
        Alembic::AbcGeom::OSubDSchema::Sample& oSubDsample)
{
    std::unique_ptr<Creases> const& csPtr = modelData.cs_;
    if (!csPtr)
    {
        return;
    }
    Creases const& cs = *csPtr;
    AnimObj::Geom::Topo::TpV const& ds = cs.ds_;
    if (ds.empty())
    {
        return;
    }
    AnimObj::Geom::Topo::TpV const& ls = cs.ls_;
    if (ls.empty())
    {
        return;
    }
    Creases::sharp_vt const& ss = cs.ss_;
    if (ss.empty())
    {
        return;
    }
    oSubDsample.setCreases(ds, ls, ss);
}

std::vector<std::string> getModAttrList(ModAttrs const& modAttrs)
{
    std::vector<std::string> r;
    for (ModAttrs::attrs_t::value_type const& p : modAttrs.attrs_)
    {
        std::string const& attrName = p.first;
        if (attrName == materialParamName)
        {
            continue;
        }
        r.emplace_back(attrName);
    }
    return r;
}

static inline std::string materialParamStrip(std::string const& paramName)
{
    if (materialParamName != paramName)
    {
        return paramName;
    }
    return paramName.substr(4);
}

void putParams(
        Alembic::Abc::OCompoundProperty const& outParams,
        ModAttrs const& modAttrs)
{
    for (ModAttrs::ss_t::value_type const& m: modAttrs.ss_)
    {
        std::string const& paramName = materialParamStrip(m->nm_);
        if (outParams.getPropertyHeader(paramName))
        {
            continue;
        }
        Alembic::AbcGeom::OStringGeomParam(outParams, paramName, true,
        Alembic::AbcGeom::kUniformScope, 1).set({*m->pas_, *m->nds_,
        Alembic::AbcGeom::kUniformScope});
    }
    for (ModAttrs::attrs_t::value_type const& modAttr: modAttrs.attrs_)
    {
        std::string const& paramName = materialParamStrip(modAttr.first);
        if (outParams.getPropertyHeader(paramName))
        {
            continue;
        }
        boost::apply_visitor(ParamVisitor(outParams, paramName), modAttr.second);
    }
}

template <typename OGeoSchema>
void putDefUVset(
        ModelData const& modelData,
        OGeoSchema& oGeoSchema,
        typename OGeoSchema::Sample& oGeoSample)
{
    UVsetPtr const& defUVsetPtr = modelData.defUVset_;
    if (!defUVsetPtr)
    {
        return;
    }
    UVset const& defUVset = *defUVsetPtr;
    std::string const& defUVname = defUVset.nm();
    if (!defUVname.empty())
    {
        oGeoSchema.setUVSourceName(defUVname);
    }
    if (defUVset.ndx())
    {
        oGeoSample.setUVs({defUVset.uvs(), Alembic::AbcGeom::kFacevaryingScope});
        return;
    }
    oGeoSample.setUVs({defUVset.uvs(), defUVset.nds(),
            Alembic::AbcGeom::kFacevaryingScope});
}

template void putDefUVset(
        ModelData const& modelData,
        Alembic::AbcGeom::OPolyMeshSchema& oGeoSchema,
        Alembic::AbcGeom::OPolyMeshSchema::Sample& oGeoSample);

template void putDefUVset(
        ModelData const& modelData,
        Alembic::AbcGeom::OSubDSchema& oGeoSchema,
        Alembic::AbcGeom::OSubDSchema::Sample& oGeoSample);

void putUserProps(
        Alembic::Abc::OCompoundProperty const& userProps,
        ModAttrs const& modAttrs)
{
    for (ModAttrs::ss_t::value_type const& m: modAttrs.ss_)
    {
        if (materialParamName != m->nm_)
        {
            continue;
        }
        Alembic::Abc::OStringArrayProperty(userProps, "materials").set(*m->pas_);
        return;
    }
    for (ModAttrs::attrs_t::value_type const& objAttr: modAttrs.attrs_)
    {
        if (materialParamName != objAttr.first)
        {
            continue;
        }
        Alembic::Abc::OStringArrayProperty(userProps,
                "materials").set({&boost::get<std::string>(objAttr.second), 1});
        return;
    }
}

void putRestNormals(
        ModelData const& modelData,
        Alembic::Abc::OCompoundProperty const& arbGeoParams)
{
    std::unique_ptr<Normals> const& nsPtr = modelData.ns_;
    if (!nsPtr)
    {
        return;
    }
    Normals const& ns = *nsPtr;
    std::vector<Alembic::Abc::uint32_t> const& ds = ns.ds_;
    if (ds.empty())
    {
        Alembic::AbcGeom::ON3fGeomParam(arbGeoParams, modConfig.nameNormals,
                false, ns.sc_, 1).set({ns.wns_, ns.sc_});
        return;
    }
    Alembic::AbcGeom::ON3fGeomParam(arbGeoParams, modConfig.nameNormals,
    true, Alembic::AbcGeom::kFacevaryingScope, 1).set({ns.wns_, ns.ds_,
    Alembic::AbcGeom::kFacevaryingScope});
}

void putRestPoints(
        ModelData const& modelData,
        Alembic::Abc::OCompoundProperty const& arbGeoParams)
{
    if (!modConfig.outPositions)
    {
        return;
    }
    Positions::ps_vt const& wps = modelData.ps_->worldSpace();
    Alembic::AbcGeom::OC3fGeomParam(arbGeoParams, modConfig.namePositions,
    false, Alembic::AbcGeom::kVertexScope, 1).set({{
    static_cast<Alembic::AbcGeom::C3fArraySample::value_type const* const>(wps.data()),
    wps.size()}, Alembic::AbcGeom::kVertexScope});
}

void putUVsets(
        ModelData const& modelData,
        Alembic::Abc::OCompoundProperty const& arbGeoParams)
{
    modelData.uvSets_.put(arbGeoParams);
}

ModelData const* findMatch(
        AlembicPath matchPath,
        string_view const modName)
{
    if (multiMod)
    {
        matchPath.emplace(matchPath.cbegin(), modName);
    }
    return findMatchMod(matchPath);
}

ModelData const* findMatch(
        string_view const fullName,
        string_view const modName)
{
    return findMatch(withoutMayaNamespaces(fullName), modName);
}

AnimObj::Geom::PsV const& getPs(ModelData const& modelData)
{
    return modelData.ps_->localSpace();
}

AnimObj::Geom::Topo const& topoExtract(ModelData const& modelData)
{
    return modelData.tp_;
}

ModAttrs const& getModAttrs(ModelData const& modelData)
{
    return modelData.as_;
}

string_view dataLoad(std::vector<std::string> const& modFiles)
{
    if (modFiles.empty())
    {
        return {};
    }
    dataModels = std::make_unique<DataModels>();
    if (modFiles.size() > 1)
    {
        multiMod = true;
    }
    for (std::string const& modFile : modFiles)
    {
        Alembic::Abc::IArchive archMod = animGather::getArchive(modFile);
        if (!archMod.valid())
        {
            return modFile;
        }
        gatherDataModel(archMod.getTop(), extractAssetNameFromFilePath(modFile));
    }
    return {};
}
}

namespace levCheck2
{
using ModFileObjPaths = std::vector<std::pair<string_view,
        std::vector<std::reference_wrapper<AlembicPath const>>>>;

template <bool>
void iterate(
        AnimFiles const& animFiles,
        ModFileObjPaths const& modFileObjPaths,
        std::vector<std::string>& msgs);

static inline std::string const& find(
        std::vector<std::pair<std::string, std::string>> const& modAnimFileNames,
        std::string const& animFileName)
{
    for (std::pair<std::string, std::string> const& modAnimFileName : modAnimFileNames)
    {
        if (modAnimFileName.second == animFileName)
        {
            return modAnimFileName.first;
        }
    }
    return animFileName;
}

void traverse(
        AnimFiles const& animFiles,
        std::vector<std::pair<std::string, std::string>> const& modAnimFileNames,
        std::vector<std::string>& msgs)
{
    ModFileObjPaths modFileObjPaths;
    if (multiMod)
    {
        for (AnimFiles::value_type const& animFile : animFiles)
        {
            string_view const assetName =
                    extractAssetNameFromFilePath(find(modAnimFileNames, animFile.first));
            DataModels::const_iterator fit =
                    findMatch(*dataModels, {std::string(assetName)});
            DataModels::const_iterator const& nd = dataModels->cend();
            if (nd == fit)
            {
                return;
            }
            modFileObjPaths.emplace_back(ModFileObjPaths::value_type());
            ModFileObjPaths::value_type& modFileObjPath = modFileObjPaths.back();
            modFileObjPath.first = assetName;
            ModFileObjPaths::value_type::second_type& modObjPaths = modFileObjPath.second;
            for (; fit != nd; ++fit)
            {
                AlembicPath const& path = fit->first;
                if (path[0] != assetName)
                {
                    break;
                }
                modObjPaths.emplace_back(path);
            }
        }
        iterate<true>(animFiles, modFileObjPaths, msgs);
    }
    else
    {
        modFileObjPaths.emplace_back(ModFileObjPaths::value_type());
        ModFileObjPaths::value_type& modFileObjPath = modFileObjPaths.front();
        modFileObjPath.first = extractAssetNameFromFilePath(modAnimFileNames.front().first);
        ModFileObjPaths::value_type::second_type& modObjPaths = modFileObjPath.second;
        for (DataModels::value_type const& dataModel : *dataModels)
        {
            modObjPaths.emplace_back(dataModel.first);
        }
        iterate<false>(animFiles, modFileObjPaths, msgs);
    }
}

void traverse(
        AnimFiles const& animFiles,
        std::vector<std::string> const& modFileNames,
        std::vector<std::string>& msgs)
{
    ModFileObjPaths modFileObjPaths;
    if (multiMod)
    {
        for (std::string const& modFileName : modFileNames)
        {
            string_view const assetName =
                    extractAssetNameFromFilePath(modFileName);
            DataModels::const_iterator fit =
                    findMatch(*dataModels, {std::string(assetName)});
            DataModels::const_iterator const& nd = dataModels->cend();
            if (nd == fit)
            {
                return;
            }
            modFileObjPaths.emplace_back(ModFileObjPaths::value_type());
            ModFileObjPaths::value_type& modFileObjPath = modFileObjPaths.back();
            modFileObjPath.first = assetName;
            ModFileObjPaths::value_type::second_type& modObjPaths = modFileObjPath.second;
            for (; fit != nd; ++fit)
            {
                AlembicPath const& path = fit->first;
                if (path[0] != assetName)
                {
                    break;
                }
                modObjPaths.emplace_back(path);
            }
        }
        iterate<true>(animFiles, modFileObjPaths, msgs);
    }
    else
    {
        modFileObjPaths.emplace_back(ModFileObjPaths::value_type());
        ModFileObjPaths::value_type& modFileObjPath = modFileObjPaths.front();
        modFileObjPath.first = extractAssetNameFromFilePath(modFileNames.front());
        ModFileObjPaths::value_type::second_type& modObjPaths = modFileObjPath.second;
        for (DataModels::value_type const& dataModel : *dataModels)
        {
            modObjPaths.emplace_back(dataModel.first);
        }
        iterate<false>(animFiles, modFileObjPaths, msgs);
    }
}
}
