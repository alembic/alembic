
#include "anim.hpp"
#include "mod.hpp"

extern unsigned char levOpt;

namespace animOut
{
template <typename OGeoSample>
void putGeoTopo(AnimObj::Topo const& topo, OGeoSample& oGeoSample);

bool isSubDShape(string_view const n);

bool hasCreases(ModelData const& modelData);
}

namespace animTran
{
template <typename IObj, typename OObj>
static inline void transferAnimObjectVisible(IObj const& iObj, OObj& oObj)
{
    Alembic::Abc::ICompoundProperty const& props = iObj.getProperties();
    if (props.getPropertyHeader(visiblePropName) == NULL)
    {
        return;
    }
    Alembic::Abc::ICharProperty const iVisibleProp(props, visiblePropName);
    if (!iVisibleProp)
    {
        return;
    }
    size_t const numSamples = iVisibleProp.getNumSamples();
    if (0 == numSamples)
    {
        return;
    }
    Alembic::Abc::OCharProperty oVisibleProp(oObj.getProperties(), visiblePropName);
    oVisibleProp.setTimeSampling(iVisibleProp.getTimeSampling());
    for (size_t k = 0; k < numSamples; ++k)
    {
        oVisibleProp.set(iVisibleProp.getValue(Alembic::Abc::ISampleSelector(static_cast<Alembic::Abc::index_t>(k))));
    }
}

template <typename ParamType>
static inline void transferAnimGeoParamXd(
        Alembic::Abc::ICompoundProperty const& animParams,
        Alembic::Abc::OCompoundProperty const& outParams,
        std::string const& paramName)
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
    if (!iGeomParam.isIndexed())
    {
        return;
    }
    typename Alembic::AbcGeom::OTypedGeomParam<ParamType> oGeomParam(outParams, paramName, true,
            Alembic::AbcGeom::kUniformScope, 1);
    Alembic::AbcGeom::IUInt32ArrayProperty const& iNdxProperty = iGeomParam.getIndexProperty();
    if (!iNdxProperty.valid())
    {
        return;
    }
    oGeomParam.set(typename Alembic::AbcGeom::OTypedGeomParam<ParamType>::Sample(*iValueProperty.getValue(),
            *iNdxProperty.getValue(), Alembic::AbcGeom::kUniformScope));
}

template <typename ParamType>
static inline void transferAnimGeoParam(Alembic::Abc::ICompoundProperty const& animParams,
                                        Alembic::Abc::OCompoundProperty const& outParams,
                                        std::string const& paramName)
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
    typename Alembic::AbcGeom::OTypedGeomParam<ParamType> oGeomParam(outParams, paramName, false,
            Alembic::AbcGeom::kConstantScope, 1);
    oGeomParam.setTimeSampling(iGeomParam.getTimeSampling());
    for (size_t k = 0; k < numSamples; ++k)
    {
        oGeomParam.set(typename Alembic::AbcGeom::OTypedGeomParam<ParamType>::Sample(
            *iValueProperty.getValue(Alembic::Abc::ISampleSelector(static_cast<Alembic::Abc::index_t>(k))),
             Alembic::AbcGeom::kConstantScope));
    }
}

template <typename IObjSch>
static inline void transferAnimArb(Alembic::Abc::OCompoundProperty const& outParams, IObjSch const& anim)
{
    Alembic::Abc::ICompoundProperty const& animParams = anim.getArbGeomParams();
    if (!animParams.valid())
    {
        return;
    }
    size_t const numProperties = animParams.getNumProperties();
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
        uint8_t const extent = dataType.getExtent();
        if (extent != 1)
        {
            if (extent)
            {
                continue;
            }
            else
            {
                transferAnimGeoParamXd<Alembic::Abc::StringTPTraits>(animParams, outParams, paramName);
            }
        }
        switch (dataType.getPod())
        {
            case Alembic::Abc::kBooleanPOD:
                transferAnimGeoParam<Alembic::Abc::BooleanTPTraits>(animParams, outParams, paramName);
                break;

            case Alembic::Abc::kUint8POD:
                transferAnimGeoParam<Alembic::Abc::Uint8TPTraits>(animParams, outParams, paramName);
                break;
            case Alembic::Abc::kInt8POD:
                transferAnimGeoParam<Alembic::Abc::Int8TPTraits>(animParams, outParams, paramName);
                break;

            case Alembic::Abc::kUint16POD:
                transferAnimGeoParam<Alembic::Abc::Uint16TPTraits>(animParams, outParams, paramName);
                break;
            case Alembic::Abc::kInt16POD:
                transferAnimGeoParam<Alembic::Abc::Int16TPTraits>(animParams, outParams, paramName);
                break;

            case Alembic::Abc::kUint32POD:
                transferAnimGeoParam<Alembic::Abc::Uint32TPTraits>(animParams, outParams, paramName);
                break;
            case Alembic::Abc::kInt32POD:
                transferAnimGeoParam<Alembic::Abc::Int32TPTraits>(animParams, outParams, paramName);
                break;

            case Alembic::Abc::kUint64POD:
                transferAnimGeoParam<Alembic::Abc::Uint64TPTraits>(animParams, outParams, paramName);
                break;
            case Alembic::Abc::kInt64POD:
                transferAnimGeoParam<Alembic::Abc::Int64TPTraits>(animParams, outParams, paramName);
                break;

            case Alembic::Abc::kFloat16POD:
                transferAnimGeoParam<Alembic::Abc::Float16TPTraits>(animParams, outParams, paramName);
                break;
            case Alembic::Abc::kFloat32POD:
                transferAnimGeoParam<Alembic::Abc::Float32TPTraits>(animParams, outParams, paramName);
                break;
            case Alembic::Abc::kFloat64POD:
                transferAnimGeoParam<Alembic::Abc::Float64TPTraits>(animParams, outParams, paramName);
                break;

            case Alembic::Abc::kStringPOD:
                transferAnimGeoParam<Alembic::Abc::StringTPTraits>(animParams, outParams, paramName);
                break;
            case Alembic::Abc::kWstringPOD:
                transferAnimGeoParam<Alembic::Abc::WstringTPTraits>(animParams, outParams, paramName);
                break;
                
            default:
                break;
        }
    }
}

static inline bool cmprNrml(
        std::pair<std::array<float, 3>, int> const& a,
        std::array<float, 3> const& b)
{
    return a.first < b;
}

static inline Alembic::AbcGeom::GeometryScope processNormals(
        Alembic::AbcGeom::UInt32ArraySample::value_vector& ndfxv,
        Alembic::AbcGeom::N3fArraySample::value_vector& nsv,
        Alembic::AbcGeom::UInt32ArraySample::value_vector& ndxv,
        Alembic::AbcGeom::N3fArraySample::value_type const* const nsp,
        size_t const nsz,
        Alembic::AbcGeom::GeometryScope const gs,
        unsigned char const optLev,
        Alembic::AbcGeom::UInt32ArraySample::value_type const* const ndxp = nullptr,
        size_t const ndSize = 0)
{
    if (Alembic::AbcGeom::kVertexScope == gs)
    {
        putValues(nsp, nsz, nsv);
        return Alembic::AbcGeom::kVertexScope;
    }
    if (ndxp)
    {
        putValues(ndxp, ndSize, ndxv);
        putValues(nsp, nsz, nsv);
        return Alembic::AbcGeom::kFacevaryingScope;
    }
    if (0 == optLev)
    {
        putValues(nsp, nsz, nsv);
        return Alembic::AbcGeom::kFacevaryingScope;
    }
    using nrm_t = Alembic::AbcGeom::N3fArraySample::value_type;
    using nrm_vt = std::vector<std::array<float, 3>>;
    nrm_t* nsvp;
    if (!ndfxv.empty())
    {
        nsvp = nsv.data();
        size_t const fsz = ndfxv.size();
        for (size_t k = 0; k < fsz; ++k)
        {
            nsvp[k] = nsp[ndfxv[k]];
        }
        return Alembic::AbcGeom::kFacevaryingScope;
    }
    nrm_vt nrmf;
    nrmf.reserve(nsz);
    for (size_t k = 0; k < nsz; ++k)
    {
        nrm_t const& v = nsp[k];
        nrm_vt::value_type a = {v[0], v[1], v[2]};
        nrmf.emplace_back(a);
    }
    nrm_vt nrms(nrmf);
    std::sort(nrms.begin(), nrms.end());
    std::size_t const d = std::distance(nrms.begin(), std::unique(nrms.begin(), nrms.end()));
    if (d == nsz)
    {
        putValues(nsp, nsz, nsv);
        return Alembic::AbcGeom::kFacevaryingScope;
    }
    nsv.resize(d);
    nsvp = nsv.data();
    using nrm_nvt = std::vector<std::pair<nrm_vt::value_type, int>>;
    nrm_nvt nrmN;
    for (size_t m = 0; m < d; ++m)
    {
        nrm_vt::value_type const& n = nrms[m];
        nrmN.emplace_back(n, m);
        nsvp[m] = nrm_t(n[0], n[1], n[2]);
    }
    ndxv.resize(nsz);
    nrm_nvt::const_iterator const& nrmNB = nrmN.cbegin();
    nrm_nvt::const_iterator const& nrmNE = nrmN.cend();
    if (1 == optLev)
    {
        for (size_t m = 0; m < nsz; ++m)
        {
            nrm_nvt::const_iterator const& found = std::lower_bound(nrmNB, nrmNE, nrmf[m], cmprNrml);
            ndxv[m] = nrmNE == found ? 0 : found->second;
        }
        return Alembic::AbcGeom::kFacevaryingScope;
    }
    ndfxv.resize(d);
    for (size_t m = 0; m < nsz; ++m)
    {
        nrm_nvt::const_iterator const& found = std::lower_bound(nrmNB, nrmNE, nrmf[m], cmprNrml);
        if (nrmNE != found)
        {
            ndxv[m] = found->second;
            ndfxv[std::distance(nrmNB, found)] = m;
        }
    }
    return Alembic::AbcGeom::kFacevaryingScope;
}

template <typename IGeoSchema, typename OGeoSample>
static inline void passAnimGeometrySpec(
        IGeoSchema const&,
        OGeoSample&,
        Alembic::AbcGeom::UInt32ArraySample::value_vector&,
        Alembic::AbcGeom::N3fArraySample::value_vector&,
        Alembic::AbcGeom::UInt32ArraySample::value_vector&,
        Alembic::AbcGeom::N3fArraySample::value_vector&,
        Alembic::Abc::ISampleSelector const& = Alembic::Abc::ISampleSelector())
{}

static inline bool checkNormals(
        Alembic::AbcGeom::N3fArraySample::value_vector const& nsvr,
        Alembic::AbcGeom::UInt32ArraySample::value_vector const& ndxv,
        size_t const nsz,
        Alembic::AbcGeom::N3fArraySample::value_type const* const nsp,
        Alembic::AbcGeom::N3fArraySample::value_vector& nsvc)
{
    if (nsvc.empty())
    {
        nsvc.resize(nsz);
    }
    for (std::size_t i = 0; i < nsz; ++i)
    {
        nsvc[i] = nsvr[ndxv[i]];
    }
    return 0 == memcmp(nsvc.data(), nsp, sizeof(Alembic::AbcGeom::N3fArraySample::value_type) * nsz);
}

template <>
inline void passAnimGeometrySpec(
        Alembic::AbcGeom::IPolyMeshSchema const& iMeshSchema,
        Alembic::AbcGeom::OPolyMeshSchema::Sample& oPolyMeshSample,
        Alembic::AbcGeom::UInt32ArraySample::value_vector& ndfxv,
        Alembic::AbcGeom::N3fArraySample::value_vector& nsvr,
        Alembic::AbcGeom::UInt32ArraySample::value_vector& ndxv,
        Alembic::AbcGeom::N3fArraySample::value_vector& nsvc,
        Alembic::Abc::ISampleSelector const& iss)
{
    Alembic::AbcGeom::IN3fGeomParam normalParam = iMeshSchema.getNormalsParam();
    if (!normalParam.valid())
    {
        return;
    }
    if (static_cast<Alembic::Abc::index_t>(normalParam.getNumSamples()) <= iss.getRequestedIndex())
    {
        return;
    }
    Alembic::AbcGeom::N3fArraySamplePtr const nsPtr = normalParam.getValueProperty().getValue(iss);
    if (!nsPtr->valid())
    {
        return;
    }
    size_t const nsz = nsPtr->size();
    Alembic::AbcGeom::N3fArraySample::value_type const* const nsp = nsPtr->get();
    Alembic::AbcGeom::GeometryScope gs = Alembic::AbcGeom::kFacevaryingScope;
    bool const indexed = normalParam.isIndexed();
    if (indexed)
    {
        Alembic::AbcGeom::UInt32ArraySamplePtr const nsNdxPtr = normalParam.getIndexProperty().getValue(iss);
        if (!nsNdxPtr->valid())
        {
            return;
        }
        gs = processNormals(ndfxv, nsvr, ndxv, nsp, nsz, Alembic::AbcGeom::kFacevaryingScope, levOpt,
                nsNdxPtr->get(), nsNdxPtr->size());
    }
    else
    {
        gs = processNormals(ndfxv, nsvr, ndxv, nsp, nsz, normalParam.getScope(), levOpt);
    }
    if (nsvr.empty())
    {
        return;
    }
    if (ndxv.empty())
    {
        oPolyMeshSample.setNormals(Alembic::AbcGeom::ON3fGeomParam::Sample(nsvr, gs));
    }
    else
    {
        if (levOpt > 1 && !indexed)
        {
            if (!checkNormals(nsvr, ndxv, nsz, nsp, nsvc))
            {
                ndfxv.clear();
                processNormals(ndfxv, nsvr, ndxv, nsp, nsz, normalParam.getScope(), 1);
            }
        }
        oPolyMeshSample.setNormals(Alembic::AbcGeom::ON3fGeomParam::Sample(
                nsvr, ndxv, Alembic::AbcGeom::kFacevaryingScope));
    }
}

template <typename IGeometry, typename OGeometry>
static inline void transferAnimGeometryOut(
        ModelData const& modelData,
        Alembic::Abc::IObject const& childIn,
        Alembic::Abc::OObject const& parentOut)
{
    IGeometry const iGeometry(childIn, Alembic::Abc::kWrapExisting);
    if (!iGeometry.valid())
    {
        return;
    }
    typename IGeometry::schema_type const& iGeoSchema = iGeometry.getSchema();
    size_t const numSamples = iGeoSchema.getNumSamples();
    if (0 == numSamples)
    {
        return;
    }
    Alembic::Abc::IP3fArrayProperty const& iPositionsProperty = iGeoSchema.getPositionsProperty();
    Alembic::AbcGeom::P3fArraySamplePtr const ptnPtr =
        iPositionsProperty.getValue(Alembic::Abc::ISampleSelector(static_cast<Alembic::Abc::index_t>(0)));
    if (!ptnPtr->valid())
    {
        return;
    }
    size_t const ptnSize = ptnPtr->size();
    std::string const& childName = parentOut.getName() + "Shape";
    OGeometry oGeometry(parentOut, childName);
    transferAnimObjectVisible(childIn, oGeometry);
    typename OGeometry::schema_type& oGeoSchema = oGeometry.getSchema();
    ModAttrs const& modAttrs = modGather::getModAttrs(modelData);
    modGather::putUserProps(oGeoSchema.getUserProperties(), modAttrs);
    Alembic::Abc::OCompoundProperty const& arbGeoParams = oGeoSchema.getArbGeomParams();
    transferAnimArb(arbGeoParams, iGeoSchema);
    modGather::putParams(arbGeoParams, modAttrs);
    if (1 < numSamples)
    {
        oGeoSchema.setTimeSampling(iGeoSchema.getTimeSampling());
    }
    typename OGeometry::schema_type::Sample oGeoSample;
    AnimObj::PsV const& lps = modGather::getPs(modelData);
    if (1 == numSamples)
    {
        oGeoSample.setPositions(lps);
    }
    else
    {
        if (lps.size() != ptnSize)
        {
            return;
        }
        oGeoSample.setPositions(*ptnPtr);
    }
    animOut::putGeoTopo(modGather::topoExtract(modelData), oGeoSample);
    Alembic::Abc::IV3fArrayProperty const& iVelocitiesProperty = iGeoSchema.getVelocitiesProperty();
    bool const hasVelocities = iVelocitiesProperty.valid();
    if (hasVelocities)
    {
        Alembic::AbcGeom::V3fArraySamplePtr const spdPtr =
            iVelocitiesProperty.getValue(Alembic::Abc::ISampleSelector(static_cast<Alembic::Abc::index_t>(0)));
        if (spdPtr->valid())
        {
            oGeoSample.setVelocities(*spdPtr);
        }
    }
    modGather::putDefUVset(modelData, oGeoSchema, oGeoSample);
    modGather::putUVsets(modelData, arbGeoParams);
    modGather::putRestPoints(modelData, arbGeoParams);
    modGather::putRestNormals(modelData, arbGeoParams);
    modGather::putAnimGeometrySpec(modelData, !animOut::isSubDShape(childName) && 1 == numSamples, oGeoSample);
    Alembic::AbcGeom::N3fArraySample::value_vector nrmls;
    Alembic::AbcGeom::N3fArraySample::value_vector nrmlsChck;
    Alembic::AbcGeom::UInt32ArraySample::value_vector nrmFx;
    Alembic::AbcGeom::UInt32ArraySample::value_vector nrmNdx;
    if (1 < numSamples)
    {
        passAnimGeometrySpec(iGeoSchema, oGeoSample, nrmFx, nrmls, nrmNdx, nrmlsChck);
    }
    oGeoSchema.set(oGeoSample);
    for (size_t i = 1; i < numSamples; ++i)
    {
        Alembic::Abc::ISampleSelector const iss(static_cast<Alembic::Abc::index_t>(i));
        Alembic::AbcGeom::P3fArraySamplePtr const dfrmPsPtr = iPositionsProperty.getValue(iss);
        if (!dfrmPsPtr->valid())
        {
            break;
        }
        if (dfrmPsPtr->size() != ptnSize)
        {
            break;
        }
        typename OGeometry::schema_type::Sample deformSample;
        deformSample.setPositions(*dfrmPsPtr);
        if (hasVelocities)
        {
            Alembic::AbcGeom::V3fArraySamplePtr const spdPtr = iVelocitiesProperty.getValue(iss);
            if (spdPtr->valid())
            {
                deformSample.setVelocities(*spdPtr);
            }
        }
        passAnimGeometrySpec(iGeoSchema, deformSample, nrmFx, nrmls, nrmNdx, nrmlsChck, iss);
        oGeoSchema.set(deformSample);
    }
}

static inline string_view extractAssetNameFromXformName(string_view const xFormName)
{
    string_view::size_type const firstColon = xFormName.find_first_of(':');
    string_view const firstNameSpace = xFormName.substr(0, firstColon);
    string_view::size_type const lastUnderscore = firstNameSpace.find_last_of('_');
    return firstNameSpace.substr(lastUnderscore == string_view::npos ? 0 : lastUnderscore + 1);            
}

template <typename IGeometry>
static inline void transferAnimGeometry(Alembic::Abc::IObject const& parentAnim,
                                        Alembic::Abc::OObject const& parentOut,
                                        size_t const i)
{
    ModelData const* const mdPtr = modGather::findMatch(parentAnim.getFullName(),
            extractAssetNameFromXformName(parentAnim.getName()));
    if (nullptr == mdPtr)
    {
        return;
    }
    Alembic::Abc::IObject const& childIn = parentAnim.getChild(i);
    ModelData const& modelData = *mdPtr;
    if (animOut::hasCreases(modelData))
    {
        transferAnimGeometryOut<IGeometry, Alembic::AbcGeom::OSubD>(modelData, childIn, parentOut);
    }
    else
    {
        transferAnimGeometryOut<IGeometry, Alembic::AbcGeom::OPolyMesh>(modelData, childIn, parentOut);
    }
}

static inline void transferAnimXform(Alembic::Abc::IObject const& childIn, Alembic::AbcGeom::OXform& oXform)
{
    transferAnimObjectVisible(childIn, oXform);
    Alembic::AbcGeom::IXform const iXform(childIn, Alembic::Abc::kWrapExisting);
    if (!iXform)
    {
        return;
    }
    Alembic::AbcGeom::IXformSchema const& iXformSchema = iXform.getSchema();
    Alembic::AbcGeom::OXformSchema& oXformSchema = oXform.getSchema();
    transferAnimArb(oXformSchema.getArbGeomParams(), iXformSchema);
    size_t const numSamples = iXformSchema.getNumSamples();
    if (0 == numSamples)
    {
        return;
    }
    oXformSchema.setTimeSampling(iXformSchema.getTimeSampling());
    for (size_t j = 0; j < numSamples; ++j)
    {
        Alembic::AbcGeom::XformSample xformSample =
	    iXformSchema.getValue(Alembic::Abc::ISampleSelector(static_cast<Alembic::Abc::index_t>(j)));
        oXformSchema.set(xformSample);
    }
}

void transferAnimObject(
        Alembic::Abc::IObject const& parentAnim,
        Alembic::Abc::OObject const& parentOut)
{
    size_t const numChildren = parentAnim.getNumChildren();
    for (size_t i = 0; i < numChildren; ++i)
    {
        Alembic::Abc::ObjectHeader const& childHeader = parentAnim.getChildHeader(i);
        if (Alembic::AbcGeom::IXform::matches(childHeader))
        {
            Alembic::Abc::IObject const& childIn = parentAnim.getChild(i);
            Alembic::AbcGeom::OXform oXform(parentOut, childIn.getName());
            transferAnimXform(childIn, oXform);
            transferAnimObject(childIn, oXform);
        }
        else if (Alembic::AbcGeom::IPolyMesh::matches(childHeader))
        {
            transferAnimGeometry<Alembic::AbcGeom::IPolyMesh>(parentAnim, parentOut, i);
        }
        else if (Alembic::AbcGeom::ISubD::matches(childHeader))
        {
            transferAnimGeometry<Alembic::AbcGeom::ISubD>(parentAnim, parentOut, i);
        }
        else if (Alembic::AbcGeom::IPoints::matches(childHeader))
        {
            transferAnimGeometry<Alembic::AbcGeom::IPoints>(parentAnim, parentOut, i);
        }
    }
}


}
