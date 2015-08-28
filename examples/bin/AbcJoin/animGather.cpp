
#include "anim.hpp"
#include <fstream>
#include <experimental/string_view>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

using std::experimental::string_view;

std::string const visiblePropName("visible");
std::string const materialParamName("ABC_shop_materialpath");

namespace animGather
{
template <typename IGeoSample>
AnimObj::Topo get(IGeoSample const& iGeoSample)
{
    AnimObj::Topo topo;
    Alembic::AbcGeom::Int32ArraySamplePtr const faceIndicesPtr = iGeoSample.getFaceIndices();
    std::unique_ptr<AnimObj::TpV> dsPtr = std::make_unique<AnimObj::TpV>();
    putValues(faceIndicesPtr->get(), faceIndicesPtr->size(), *dsPtr);
    topo.ds_ = std::move(dsPtr);
    Alembic::AbcGeom::Int32ArraySamplePtr const faceCountsPtr = iGeoSample.getFaceCounts();
    std::unique_ptr<AnimObj::TpV> csPtr = std::make_unique<AnimObj::TpV>();
    putValues(faceCountsPtr->get(), faceCountsPtr->size(), *csPtr);
    topo.cs_ = std::move(csPtr);
    return topo;
}

template AnimObj::Topo get(Alembic::AbcGeom::ISubDSchema::Sample const&);

template <typename IGeometry>
static inline std::unique_ptr<AnimObj> common(Alembic::Abc::IObject const& child);

static inline AnimObj::AnimData children(Alembic::Abc::IObject const& parent)
{
    AnimObj::AnimData animData = std::make_unique<AnimObj::AnimData::element_type>();
    AnimObj::AnimData::element_type& children = *animData;
    bool geo = false;
    size_t const numChildren = parent.getNumChildren();
    for (size_t i = 0; i < numChildren; ++i)
    {
        Alembic::Abc::IObject const& child = parent.getChild(i);
        Alembic::Abc::ObjectHeader const& childHeader = child.getHeader();
        if (Alembic::AbcGeom::IXform::matches(childHeader))
        {
            children.emplace_back(common<Alembic::AbcGeom::IXform>(child));
        }
        else
        {
            if (geo)
            {
                continue;
            }
            if (Alembic::AbcGeom::IPolyMesh::matches(childHeader))
            {
                children.emplace_back(common<Alembic::AbcGeom::IPolyMesh>(child));
            }
            else if (Alembic::AbcGeom::ISubD::matches(childHeader))
            {
                children.emplace_back(common<Alembic::AbcGeom::ISubD>(child));
            }
            geo = true;
        }
    }
    return animData;
}

namespace geo
{
template <typename IGeoSchema>
static inline void specific(
        size_t const,
        IGeoSchema const&,
        AnimObj&)
{}

template <>
inline void specific(
        size_t const numSamples,
        Alembic::AbcGeom::IPolyMeshSchema const& iPolyMeshSchema,
        AnimObj& animObj)
{
    Alembic::AbcGeom::IN3fGeomParam normalParam = iPolyMeshSchema.getNormalsParam();
    if (!normalParam.valid())
    {
        return;
    }
    if (normalParam.isIndexed())
    {
        return;
    }
    if (normalParam.getNumSamples() != numSamples)
    {
        return;
    }
    animObj.geom_->normalData_ = std::make_unique<AnimObj::NormalData>();
    AnimObj::NormalData& normalData = *animObj.geom_->normalData_;
    normalData.normals_ = std::make_unique<AnimObj::NormalData::Normals>();
    AnimObj::NormalData::Normals& normals = *normalData.normals_;
    normals.reserve(numSamples);
    Alembic::Abc::IN3fArrayProperty const& iNormalsProperty = normalParam.getValueProperty();
    for (size_t j = 0; j < numSamples; ++j)
    {
        Alembic::AbcGeom::N3fArraySamplePtr const nsPtr = iNormalsProperty.getValue(
                Alembic::Abc::ISampleSelector(static_cast<Alembic::Abc::index_t>(j)));
        std::unique_ptr<AnimObj::NormalData::NsV> smpPtr = std::make_unique<AnimObj::NormalData::NsV>();
        putValues(nsPtr->get(), nsPtr->size(), *smpPtr);
        normals.emplace_back(std::move(smpPtr));
    }
}
}

template <typename IGeoSchema>
static inline void specific(
        size_t const numSamples,
        IGeoSchema const& iGeoSchema,
        AnimObj& animObj)
{
    animObj.geom_ = std::make_unique<AnimObj::Geom>();
    AnimObj::Geom& geom = *animObj.geom_;
    geom.points_ = std::make_unique<AnimObj::Points>();
    AnimObj::Points& points = *geom.points_;
    points.reserve(numSamples);
    Alembic::Abc::IP3fArrayProperty const& iPositionsProperty = iGeoSchema.getPositionsProperty();
    for (size_t j = 0; j < numSamples; ++j)
    {
        Alembic::AbcGeom::P3fArraySamplePtr const ptnPtr = iPositionsProperty.getValue(
                Alembic::Abc::ISampleSelector(static_cast<Alembic::Abc::index_t>(j)));
        std::unique_ptr<AnimObj::PsV> smpPtr = std::make_unique<AnimObj::PsV>();
        putValues(ptnPtr->get(), ptnPtr->size(), *smpPtr);
        points.emplace_back(std::move(smpPtr));
    }
    geom.topo_ = get(iGeoSchema.getValue());
    geo::specific(numSamples, iGeoSchema, animObj);
}

template <>
inline void specific(
        size_t const numSamples,
        Alembic::AbcGeom::IXformSchema const& iXformSchema,
        AnimObj& animObj)
{
    animObj.xForms_ = std::make_unique<AnimObj::Xforms>();
    AnimObj::Xforms& xForms = *animObj.xForms_;
    xForms.reserve(numSamples);
    for (size_t j = 0; j < numSamples; ++j)
    {
        Alembic::AbcGeom::XformSample xFormSample =
	    iXformSchema.getValue(Alembic::Abc::ISampleSelector(static_cast<Alembic::Abc::index_t>(j)));
        xForms.emplace_back(xFormSample.getMatrix());
    }
}

template <typename Gather>
static inline AnimObj::Sampling getSampling(Gather const& gather)
{
    AnimObj::Sampling samplingPtr = std::make_unique<AnimObj::SamplingData>();
    AnimObj::SamplingData& sampling = *samplingPtr;
    Alembic::Abc::TimeSamplingPtr const timeSamplingPtr = gather.getTimeSampling();
    Alembic::Abc::TimeSamplingType const timeSamplingType = timeSamplingPtr->getTimeSamplingType();
    if (timeSamplingType.isUniform())
    {
        AnimObj::SamplingData::Time& start = sampling.start_;
        start = timeSamplingPtr->getSampleTime(0);
        sampling.cycle_ = timeSamplingPtr->getSampleTime(1) - start;
    }
    else
    {
        sampling.times_ = std::make_unique<AnimObj::SamplingData::Times>(timeSamplingPtr->getStoredTimes());
        sampling.cyclic_ = false;
        if (timeSamplingType.isCyclic())
        {
            sampling.cyclic_ = true;
            sampling.cycle_ = timeSamplingType.getTimePerCycle();
        }
    }
    return samplingPtr;
}

template <typename ParamType>
static inline void geoParam(
        Alembic::Abc::ICompoundProperty const& animParams,
        std::string const& paramName,
        std::unique_ptr<std::vector<AnimObj::Attributes::Attribute<typename ParamType::value_type>>>& typAttrPtr)
{
    typename Alembic::AbcGeom::ITypedGeomParam<ParamType> iGeomParam(animParams, paramName);
    if (!iGeomParam.valid())
    {
        return;
    }
    size_t const numSamples = iGeomParam.getNumSamples();
    if (0 == numSamples)
    {
        return;
    }
    typename Alembic::AbcGeom::ITypedArrayProperty<ParamType> const& iValueProperty = iGeomParam.getValueProperty();
    if (!iValueProperty.valid())
    {
        return;
    }
    if (!typAttrPtr)
    {
        typAttrPtr = std::make_unique<std::vector<AnimObj::Attributes::Attribute<typename ParamType::value_type>>>();
    }
    std::vector<AnimObj::Attributes::Attribute<typename ParamType::value_type>>& typAttrs = *typAttrPtr;
    AnimObj::Attributes::Attribute<typename ParamType::value_type> attribute;
    attribute.name_ = paramName;
    if (1 < numSamples)
    {
        attribute.sampling_ = getSampling(iGeomParam);
    }
    attribute.values_ = std::make_unique<std::vector<typename ParamType::value_type>>();
    std::vector<typename ParamType::value_type>& values = *attribute.values_;
    values.reserve(numSamples);
    for (size_t k = 0; k < numSamples; ++k)
    {
        Alembic::Util::shared_ptr<Alembic::Abc::TypedArraySample<ParamType>> const arraySamplePtr =
                iValueProperty.getValue(Alembic::Abc::ISampleSelector(static_cast<Alembic::Abc::index_t>(k)));
        values.emplace_back(1 == arraySamplePtr->size() ? arraySamplePtr->get()[0] : typename ParamType::value_type());
    }
    typAttrs.emplace_back(std::move(attribute));
}

template <typename IGeoSchema>
static inline void arb(IGeoSchema const& iGeoSchema, std::unique_ptr<AnimObj::Attributes>& attributes)
{
    Alembic::Abc::ICompoundProperty const& animParams = iGeoSchema.getArbGeomParams();
    if (!animParams.valid())
    {
        return;
    }
    size_t const numProperties = animParams.getNumProperties();
    if (0 == numProperties)
    {
        return;
    }
    attributes = std::make_unique<AnimObj::Attributes>();
    for (size_t i = 0; i < numProperties; ++i)
    {
        Alembic::Abc::PropertyHeader const& propertyHeader = animParams.getPropertyHeader(i);
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
        if (dataType.getExtent() != 1)
        {
            continue;
        }
        switch (dataType.getPod())
        {
            case Alembic::Abc::kStringPOD:
                geoParam<Alembic::Abc::StringTPTraits>(animParams, paramName, attributes->strings_);
                break;
            case Alembic::Abc::kFloat64POD:
                geoParam<Alembic::Abc::Float64TPTraits>(animParams, paramName, attributes->doubles_);
                break;
            case Alembic::Abc::kFloat32POD:
                geoParam<Alembic::Abc::Float32TPTraits>(animParams, paramName, attributes->floats_);
                break;
            case Alembic::Abc::kInt32POD:
                geoParam<Alembic::Abc::Int32TPTraits>(animParams, paramName, attributes->ints_);
                break;
            case Alembic::Abc::kBooleanPOD:
                geoParam<Alembic::Abc::BooleanTPTraits>(animParams, paramName, attributes->bools_);
                break;
            default:
                break;
        }
    }
}

template <typename IGeometry>
static inline std::unique_ptr<AnimObj> common(Alembic::Abc::IObject const& child)
{
    std::unique_ptr<AnimObj> animObjPtr = std::make_unique<AnimObj>();
    AnimObj& animObj = *animObjPtr;
    Alembic::Abc::ICompoundProperty const& animProps = child.getProperties();
    if (animProps.getPropertyHeader(visiblePropName))
    {
        Alembic::Abc::ICharProperty const iVisibleProp(animProps, visiblePropName);
        if (iVisibleProp)
        {
            size_t const numSamples = iVisibleProp.getNumSamples();
            if (numSamples)
            {
                animObj.visible_ = std::make_unique<AnimObj::Visible>();
                AnimObj::Visible& visible = *animObj.visible_;
                if (1 < numSamples)
                {
                    visible.sampling_ = getSampling(iVisibleProp);
                }
                boost::dynamic_bitset<>& flags = visible.flags_;
                flags.resize(numSamples);
                for (size_t k = 0; k < numSamples; ++k)
                {
                    flags.set(k, iVisibleProp.getValue(Alembic::Abc::ISampleSelector(
                            static_cast<Alembic::Abc::index_t>(k))));
                }
            }
        }
    }
    animObj.name_ = child.getName();
    IGeometry iGeometry(child, Alembic::Abc::kWrapExisting);
    typename IGeometry::schema_type const& iGeoSchema = iGeometry.getSchema();
    arb(iGeoSchema, animObj.attributes_);
    size_t const numSamples = iGeoSchema.getNumSamples();
    if (numSamples)
    {
        if (1 < numSamples)
        {
            animObj.sampling_ = getSampling(iGeoSchema);
        }
        specific(numSamples, iGeoSchema, animObj);
    }
    animObj.anims_ = children(child);
    return animObjPtr;
}

static inline Alembic::Abc::IArchive readArchive(std::string const& animFileName)
{
    Alembic::Abc::IArchive animArch(Alembic::AbcCoreHDF5::ReadArchive(),
            animFileName, Alembic::Abc::ErrorHandler::kQuietNoopPolicy);
    if (animArch.valid())
    {
        return animArch;
    }
    return {};
}

static inline Alembic::Abc::IArchive getArchive(
        std::string const& animFileName,
        std::istringstream& archInStream)
{
    std::allocator<char> charAllocator;
    std::ifstream archInFile;
    archInFile.open(animFileName);
    if (!archInFile.good())
    {
        return {};
    }
    archInFile.seekg(0, archInFile.end);
    using sz_tp = unsigned long long int;
    sz_tp fs = archInFile.tellg();
    if (0 == fs)
    {
        return {};
    }
    archInFile.seekg(0, archInFile.beg);
    char* tmpBfr = charAllocator.allocate(fs);
    sz_tp const bs = 1 << 30;
    if (fs > bs)
    {
        sz_tp const c = fs / bs;
        for (sz_tp i = 0; i < c; ++i)
        {
            archInFile.read(tmpBfr + i * bs, bs);
        }
        sz_tp const r = bs * c;
        archInFile.read(tmpBfr + r, fs - r);
    }
    else
    {
        archInFile.read(tmpBfr, fs);
    }
    archInFile.close();
    {
        std::string const tmpStr(tmpBfr, fs);
        charAllocator.deallocate(tmpBfr, fs);
        archInStream.str(tmpStr);
    }
    std::vector<std::istream*> isv(1, &archInStream);
    Alembic::Abc::IArchive animArch(
            Alembic::AbcCoreOgawa::ReadArchive(isv), {},
            Alembic::Abc::ErrorHandler::kQuietNoopPolicy);
    if (animArch.valid())
    {
        return animArch;
    }
    archInStream.str({});
    return readArchive(animFileName);
}

Alembic::Abc::IArchive getArchive(std::string const& animFileName)
{
    Alembic::Abc::IArchive animArch(
            Alembic::AbcCoreOgawa::ReadArchive(), animFileName,
            Alembic::Abc::ErrorHandler::kQuietNoopPolicy);
    if (animArch.valid())
    {
        return animArch;
    }
    return readArchive(animFileName);
}

template <bool noLoadOpt>
class Loader {};

template <>
class Loader<true>
{
   public:
    Alembic::Abc::IArchive load(std::string const& animFileName)
    {
        return getArchive(animFileName);
    }
};

template <>
class Loader<false>
{
   public:
    Alembic::Abc::IArchive load(std::string const& animFileName)
    {
        return getArchive(animFileName, archInStream);
    }
   private:
    std::istringstream archInStream;
};

template <bool noLoadOpt>
static inline string_view traverse(
        std::vector<std::string> const& animFileNames, AnimFiles& animFiles)
{
    for (std::string const& animFileName : animFileNames)
    {
        Loader<noLoadOpt> loader;
        Alembic::Abc::IArchive animArch = loader.load(animFileName);
        if (!animArch.valid())
        {
            return animFileName;
        }
        animFiles.emplace_back(animFileName, children(animArch.getTop()));
    }
    return {};
}

string_view load(
        bool const noLoadOpt,
        std::vector<std::string> const& animFileNames,
        AnimFiles& animFiles)
{
    if (noLoadOpt)
    {
        return traverse<true>(animFileNames, animFiles);
    }
    return traverse<false>(animFileNames, animFiles);
}
}
