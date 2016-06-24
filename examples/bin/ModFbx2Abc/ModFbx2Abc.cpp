#include <array>

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcMaterial/OMaterial.h>
#include <Alembic/AbcMaterial/MaterialAssignment.h>

#include <fbxsdk.h>

namespace {
typedef std::vector<std::array<float, 2>> uvsv;
typedef std::vector<std::array<float, 3>> nrmv;
typedef std::vector<std::pair<nrmv::value_type, int>> nrmnv;
typedef std::vector<std::pair<uvsv::value_type, int>> uvsnv;
}

template <int s>
static inline bool compare(std::pair<std::array<float, s>, int> const& a, std::array<float, s> const& b) {
    return a.first < b;
}

static std::string prefixMaterial;
static bool disableUVopt = false;
static bool alembicMaterial = false;

static inline std::string translateUVname(std::string const& n) {
    std::string::size_type const len = n.length();
    if (len < 11) {
        return n;
    }
    if (n.substr(0, 10) != "UVChannel_") {
        return n;
    }
    char const d = n[10];
    if (d == '1') {
        return "uv";
    }
    return std::string("uv") + d;
}

static inline void getLayerUVs(FbxLayer const* const layer,
                               int const polygonVertexCount,
                               Alembic::Abc::Int32ArraySample::value_vector const& counts,
                               Alembic::Abc::V2fArraySample::value_vector& uvsr,
                               Alembic::Abc::UInt32ArraySample::value_vector& ndsr,
                               std::string& nm,
                               bool const defSet = false,
                               bool const chkSet = true) {
    if (NULL == layer) {
        return;
    }
    FbxLayerElementUV const* const uvSet = layer->GetUVs();
    if (NULL == uvSet) {
        return;
    }
    char const* const uvSetName = uvSet->GetName();
    if (NULL == uvSetName) {
        return;
    }
    nm = translateUVname(uvSetName);
    if (chkSet) {
        if (defSet) {
            if ("uv" != nm) {
                return;
            }
        } else {
            if ("uv" == nm) {
                return;
            }
        }
    }
    FbxLayerElementArrayTemplate<FbxVector2> const& directArray = uvSet->GetDirectArray();
    int const directArrayCount = directArray.GetCount();
    if (directArrayCount < 1) {
        return;
    }
    FbxLayerElementArrayTemplate<int> const& indexArray = uvSet->GetIndexArray();
    int const indexArrayCount = indexArray.GetCount();
    if (indexArrayCount < 1) {
        return;
    }
    if (indexArrayCount != polygonVertexCount) {
        return;
    }
    size_t const polygonCount = counts.size();
    if (disableUVopt) {
        uvsr.reserve(directArrayCount);
        for (int k = 0; k < directArrayCount; ++k) {
            FbxVector2 const& t = directArray[k];
            uvsr.emplace_back(static_cast<float>(t[0]), static_cast<float>(t[1]));
        }
        ndsr.reserve(indexArrayCount);
        int k = 0;
        for (size_t m = 0; m < polygonCount; ++m) {
            int const count = counts[m];
            for (int n = count - 1; 0 <= n; --n) {
                ndsr.emplace_back(indexArray[k + n]);
            }
            k += count;
        }
    } else {
        int x = 0;
        for (; x < indexArrayCount; ++x) {
            if (indexArray[x] != x) {
                break;
            }
        }
        uvsv uvsf;
        uvsf.reserve(indexArrayCount);
        if (x < indexArrayCount) {
            for (int k = 0; k < indexArrayCount; ++k) {
                FbxVector2 const& t = directArray[indexArray[k]];
                uvsv::value_type c = {static_cast<float>(t[0]), static_cast<float>(t[1])};
                uvsf.emplace_back(c);
            }
        } else {
            if (indexArrayCount != directArrayCount) {
                return;
            }
            for (int k = 0; k < directArrayCount; ++k) {
                FbxVector2 const& t = directArray[k];
                uvsv::value_type c = {static_cast<float>(t[0]), static_cast<float>(t[1])};
                uvsf.emplace_back(c);
            }
        }
        uvsv uvss(uvsf);
        std::sort(uvss.begin(), uvss.end());
        std::size_t const d = std::distance(uvss.begin(), std::unique(uvss.begin(), uvss.end()));
        uvsr.reserve(d);
        if (d < polygonVertexCount) {
            uvsnv uvsn;
            for (int m = 0; m < d; ++m) {
                uvsv::value_type const& t = uvss[m];
                uvsn.emplace_back(t, m);
                uvsr.emplace_back(t[0], t[1]);
            }
            ndsr.reserve(polygonVertexCount);
            uvsnv::iterator const& ne = uvsn.end();
            int k = 0;
            for (int m = 0; m < polygonCount; ++m) {
                int const count = counts[m];
                for (int n = count - 1; 0 <= n; --n) {
                    uvsnv::const_iterator const& found = std::lower_bound(uvsn.begin(), ne, uvsf[k + n], compare<2>);
                    ndsr.emplace_back(ne == found ? 0 : found->second);
                }
                k += count;
            }
        } else {
            if (polygonVertexCount != d) {
                return;
            }
            int k = 0;
            for (int m = 0; m < polygonCount; ++m) {
                int const count = counts[m];
                for (int n = count - 1; 0 <= n; --n) {
                    uvsv::value_type const& t = uvsf[k + n];
                    uvsr.emplace_back(t[0], t[1]);
                }
                k += count;
            }
        }
    }
}

struct GeoContext {
    Alembic::Abc::N3fArraySample::value_vector norms;
    Alembic::Abc::UInt32ArraySample::value_vector normNds;
    Alembic::AbcGeom::GeometryScope normScope;
    Alembic::Abc::Int32ArraySample::value_vector creaseIndices;
    Alembic::Abc::Int32ArraySample::value_vector creaseLengths;
    Alembic::Abc::FloatArraySample::value_vector creaseSharpnesses;
};

template <typename OGeoSample>
static inline void convertGeoType(GeoContext const&, OGeoSample&);

template <>
static inline void convertGeoType(GeoContext const& gc, Alembic::AbcGeom::OSubDSchema::Sample& oSubDsample) {
    if (gc.creaseIndices.empty()) {
        return;
    }
    if (gc.creaseLengths.empty()) {
        return;
    }
    if (gc.creaseSharpnesses.empty()) {
        return;
    }
    oSubDsample.setCreases(gc.creaseIndices, gc.creaseLengths, gc.creaseSharpnesses);
}

template <>
static inline void convertGeoType(GeoContext const& gc, Alembic::AbcGeom::OPolyMeshSchema::Sample& oPolyMeshSample) {
    if (gc.norms.empty()) {
        return;
    }
    oPolyMeshSample.setNormals(gc.normNds.empty() ? Alembic::AbcGeom::ON3fGeomParam::Sample(gc.norms, gc.normScope)
                                                  : Alembic::AbcGeom::ON3fGeomParam::Sample(
                                                        gc.norms, gc.normNds, Alembic::AbcGeom::kFacevaryingScope));
}

static inline void getLayerNormals(FbxLayer const* const layer0,
                                   int const polygonVertexCount,
                                   Alembic::Abc::Int32ArraySample::value_vector const& counts,
                                   Alembic::Abc::N3fArraySample::value_vector& norms,
                                   Alembic::Abc::UInt32ArraySample::value_vector& normNds,
                                   Alembic::AbcGeom::GeometryScope& normScope) {
    FbxLayerElementNormal const* const normals = layer0->GetNormals();
    if (NULL == normals) {
        return;
    }
    normScope = Alembic::AbcGeom::kVertexScope;
    FbxLayerElementArrayTemplate<FbxVector4> const& directArray = normals->GetDirectArray();
    int const directArrayCount = directArray.GetCount();
    if (directArrayCount < 1) {
        return;
    }
    FbxLayerElementNormal::EMappingMode const mappingMode = normals->GetMappingMode();
    if (mappingMode == FbxLayerElementNormal::eByControlPoint) {
        norms.reserve(directArrayCount);
        for (int k = 0; k < directArrayCount; ++k) {
            FbxVector4 const& n = directArray[k];
            norms.emplace_back(static_cast<float>(n[0]), static_cast<float>(n[1]), static_cast<float>(n[2]));
        }
    } else if (mappingMode == FbxLayerElementNormal::eByPolygonVertex) {
        size_t const polygonCount = counts.size();
        FbxLayerElementArrayTemplate<int> const& indexArray = normals->GetIndexArray();
        int const indexArrayCount = indexArray.GetCount();
        if (0 == indexArrayCount) {
            if (directArrayCount != polygonVertexCount) {
                return;
            }
            nrmv nrmf;
            nrmf.reserve(polygonVertexCount);
            for (int k = 0; k < polygonVertexCount; ++k) {
                FbxVector4 const& v = directArray[k];
                nrmv::value_type a = {static_cast<float>(v[0]), static_cast<float>(v[1]), static_cast<float>(v[2])};
                nrmf.emplace_back(a);
            }
            nrmv nrms(nrmf);
            std::sort(nrms.begin(), nrms.end());
            std::size_t const d = std::distance(nrms.begin(), std::unique(nrms.begin(), nrms.end()));
            if (d < polygonVertexCount) {
                norms.reserve(d);
                nrmnv nrmn;
                for (int m = 0; m < d; ++m) {
                    nrmv::value_type const& n = nrms[m];
                    nrmn.emplace_back(n, m);
                    norms.emplace_back(n[0], n[1], n[2]);
                }
                normNds.reserve(polygonVertexCount);
                nrmnv::iterator const& ne = nrmn.end();
                int k = 0;
                for (int m = 0; m < polygonCount; ++m) {
                    int const count = counts[m];
                    for (int n = count - 1; 0 <= n; --n) {
                        nrmnv::const_iterator const& found =
                            std::lower_bound(nrmn.begin(), ne, nrmf[k + n], compare<3>);
                        normNds.emplace_back(ne == found ? 0 : found->second);
                    }
                    k += count;
                }
            } else {
                normScope = Alembic::AbcGeom::kFacevaryingScope;
                norms.reserve(polygonVertexCount);
                int k = 0;
                for (int m = 0; m < polygonCount; ++m) {
                    int const count = counts[m];
                    for (int n = count - 1; 0 <= n; --n) {
                        nrmv::value_type const& l = nrmf[k + n];
                        norms.emplace_back(l[0], l[1], l[2]);
                    }
                    k += count;
                }
            }
        } else {
            if (indexArrayCount < 1) {
                return;
            }
            if (indexArrayCount != polygonVertexCount) {
                return;
            }
            norms.reserve(directArrayCount);
            for (int k = 0; k < directArrayCount; ++k) {
                FbxVector4 const& t = directArray[k];
                norms.emplace_back(static_cast<float>(t[0]), static_cast<float>(t[1]), static_cast<float>(t[2]));
            }
            normNds.reserve(indexArrayCount);
            int k = 0;
            for (size_t m = 0; m < polygonCount; ++m) {
                int const count = counts[m];
                for (int n = count - 1; 0 <= n; --n) {
                    normNds.emplace_back(indexArray[k + n]);
                }
                k += count;
            }
        }
    }
}

static inline bool isSubD(std::string const& n) {
    std::string::size_type const len = n.length();
    if (4 > len) {
        return false;
    }
    return "SubD" == n.substr(len - 4);
}

static inline bool isOut(std::string const& n) {
    std::string::size_type const len = n.length();
    if (4 > len) {
        return false;
    }
    return "_out" == n.substr(len - 4);
}

static inline std::string strip4(std::string const& n) {
    std::string::size_type const len = n.length();
    if (4 > len) {
        return n;
    }
    return n.substr(0, len - 4);
}

static inline std::string stripOut(std::string const& n) {
    return isOut(n) ? strip4(n) : n;
}

static inline std::string stripSubD(std::string const& n) {
    return isSubD(n) ? strip4(n) : n;
}

static inline std::string stripBoth(std::string const& n) {
    return stripSubD(stripOut(n));
}

static inline std::string fxHM(std::string const& n) {
    return stripBoth(n) + "_out";
}

static inline bool isPrefix(std::string const& p, std::string const& n) {
    std::string::size_type const pl = p.length();
    if (pl > n.length()) {
        return false;
    }
    return p == n.substr(0, pl);
}

static std::string const materialParamName("ABC_shop_materialpath");

template <typename OGeometry>
static inline void convertMesh(char const* const childName,
                               FbxNode const* const childNode,
                               FbxMesh const* const mesh,
                               FbxLayer const* const layer0,
                               FbxLayerElementCrease const* const creases,
                               Alembic::AbcGeom::OXform& oXform) {
    Alembic::Abc::P3fArraySample::value_vector points;
    int const pointsCount = mesh->GetControlPointsCount();
    for (int j = 0; j < pointsCount; ++j) {
        FbxVector4 const& p = mesh->GetControlPointAt(j);
        points.emplace_back(Alembic::Abc::P3fArraySample::value_type(static_cast<float>(p[0]), static_cast<float>(p[1]),
                                                                     static_cast<float>(p[2])));
    }
    Alembic::Abc::Int32ArraySample::value_vector faces;
    int const polygonVertexCount = mesh->GetPolygonVertexCount();
    faces.reserve(polygonVertexCount);
    int const polygonCount = mesh->GetPolygonCount();
    Alembic::Abc::Int32ArraySample::value_vector counts;
    counts.reserve(polygonCount);
    for (int j = 0; j < polygonCount; ++j) {
        int const polySize = mesh->GetPolygonSize(j);
        for (int k = polySize - 1; 0 <= k; --k) {
            faces.emplace_back(mesh->GetPolygonVertex(j, k));
        }
        counts.emplace_back(polySize);
    }
    OGeometry oGeometry(oXform, std::string(childName) + "Shape");
    OGeometry::schema_type& oGeoSchema = oGeometry.getSchema();
    Alembic::Abc::V2fArraySample::value_vector defUVs;
    Alembic::Abc::UInt32ArraySample::value_vector defNds;
    std::string defUVname;
    int layerStart = 0;
    int const layerCount = mesh->GetLayerCount();
    if (1 == layerCount) {
        getLayerUVs(layer0, polygonVertexCount, counts, defUVs, defNds, defUVname, true, false);
    } else {
        for (int j = 0; j < layerCount; ++j) {
            getLayerUVs(mesh->GetLayer(j), polygonVertexCount, counts, defUVs, defNds, defUVname, true);
            if (!defUVs.empty()) {
                break;
            }
        }
        if (defUVs.empty()) {
            getLayerUVs(layer0, polygonVertexCount, counts, defUVs, defNds, defUVname, true, false);
            layerStart = 1;
        }
    }
    if (!defUVs.empty()) {
        oGeoSchema.setUVSourceName(defUVname);
    }
    OGeometry::schema_type::Sample oGeoSample(points, faces, counts);
    if (!defUVs.empty()) {
        oGeoSample.setUVs(
            defNds.empty()
                ? Alembic::AbcGeom::OV2fGeomParam::Sample(defUVs, Alembic::AbcGeom::kFacevaryingScope)
                : Alembic::AbcGeom::OV2fGeomParam::Sample(defUVs, defNds, Alembic::AbcGeom::kFacevaryingScope));
    }
    GeoContext gc;
    if (NULL == creases) {
        if (!isSubD(childName)) {
            getLayerNormals(layer0, polygonVertexCount, counts, gc.norms, gc.normNds, gc.normScope);
        }
    } else if (creases->GetReferenceMode() == FbxLayerElementCrease::eDirect) {
        int creaseCount = 0;
        FbxLayerElementArrayTemplate<double> const& directArray = creases->GetDirectArray();
        int const directArrayCount = directArray.GetCount();
        for (int i = 0; i < directArrayCount; ++i) {
            float const s = static_cast<float>(directArray[i]);
            if (s) {
                int a;
                int b;
                mesh->GetMeshEdgeVertices(i, a, b);
                gc.creaseIndices.emplace_back(a);
                gc.creaseIndices.emplace_back(b);
                gc.creaseSharpnesses.emplace_back(10 * s);
                ++creaseCount;
            }
        }
        gc.creaseLengths.resize(creaseCount, 2);
    }
    convertGeoType(gc, oGeoSample);
    oGeoSchema.set(oGeoSample);
    Alembic::Abc::OCompoundProperty const& arbGeomParams = oGeoSchema.getArbGeomParams();
    convertMaterials(childName, childNode, layer0, oGeoSchema, arbGeomParams);
    convertProperties(childNode, arbGeomParams);
    if (layerCount < 2) {
        return;
    }
    for (int j = layerStart; j < layerCount; ++j) {
        Alembic::Abc::V2fArraySample::value_vector uvs;
        Alembic::Abc::UInt32ArraySample::value_vector nds;
        std::string nm;
        getLayerUVs(mesh->GetLayer(j), polygonVertexCount, counts, uvs, nds, nm);
        if (nm.empty()) {
            continue;
        }
        if (uvs.empty()) {
            continue;
        }
        bool const empty = nds.empty();
        Alembic::AbcGeom::OV2fGeomParam(arbGeomParams, nm, !empty, Alembic::AbcGeom::kFacevaryingScope, 1)
            .set(empty ? Alembic::AbcGeom::OV2fGeomParam::Sample(uvs, Alembic::AbcGeom::kFacevaryingScope)
                       : Alembic::AbcGeom::OV2fGeomParam::Sample(uvs, nds, Alembic::AbcGeom::kFacevaryingScope));
    }
}

static inline void setMaterialParam(Alembic::Abc::OCompoundProperty const& arbGeomParams,
                                    std::string const& materialName) {
    Alembic::AbcGeom::OStringGeomParam oGeomStringParam(arbGeomParams, materialParamName, false,
                                                        Alembic::AbcGeom::kConstantScope, 1);
    std::string const materialPrefixed = prefixMaterial + fxHM(materialName);
    oGeomStringParam.set(Alembic::AbcGeom::OStringGeomParam::Sample(
        Alembic::Abc::StringArraySample(&materialPrefixed, 1), Alembic::AbcGeom::kConstantScope));
}

static Alembic::Abc::OObject materialFolder;
static std::string const materialFolderName("materials");

static inline void assignAlembicMaterialObject(Alembic::Abc::OObject const& obj, std::string const& materialName) {
    Alembic::AbcMaterial::addMaterialAssignment(obj, '/' + materialFolderName + '/' + materialName);
    if (materialFolder.getChildHeader(materialName)) {
        return;
    }
    Alembic::AbcMaterial::OMaterial(materialFolder, materialName);
}

template <typename OGeoSchema>
static inline void assignAlembicMaterialSchema(OGeoSchema const& oGeoSchema, std::string const& childName) {
    assignAlembicMaterialObject(oGeoSchema.getObject(), stripBoth(childName));
}

template <typename OGeoSchema>
static inline void convertMaterials(char const* const childName,
                                    FbxNode const* const childNode,
                                    FbxLayer const* const layer0,
                                    OGeoSchema& oGeoSchema,
                                    Alembic::Abc::OCompoundProperty const& arbGeomParams) {
    int const materialCount = childNode->GetMaterialCount();
    if (materialCount == 1) {
        FbxSurfaceMaterial const* const material0 = childNode->GetMaterial(0);
        if (material0) {
            std::string const materialFbx = material0->GetName();
            if (materialFbx == "Default" || materialFbx == "Material" || materialFbx == "Default_Material" ||
                isPrefix("lambert", materialFbx) || isPrefix("Scene_Material", materialFbx) ||
                isPrefix("scene_material", materialFbx)) {
                if (alembicMaterial) {
                    assignAlembicMaterialSchema(oGeoSchema, childName);
                } else {
                    setMaterialParam(arbGeomParams, childName);
                }
            } else {
                if (alembicMaterial) {
                    assignAlembicMaterialSchema(oGeoSchema, materialFbx);
                } else {
                    setMaterialParam(arbGeomParams, materialFbx);
                }
            }
        }
    } else if (materialCount > 0) {
        FbxLayerElementMaterial const* const elementMaterial = layer0->GetMaterials();
        if (elementMaterial) {
            if (elementMaterial->GetMappingMode() == FbxLayerElementNormal::eByPolygon) {
                FbxLayerElementArrayTemplate<int> const& indexArray = elementMaterial->GetIndexArray();
                int const indexArrayCount = indexArray.GetCount();
                if (indexArrayCount > 1) {
                    int const firstIndex = indexArray[0];
                    int n = 1;
                    for (; n < indexArrayCount; ++n) {
                        if (firstIndex != indexArray[n]) {
                            break;
                        }
                    }
                    if (n < indexArrayCount)
                    {
                        typedef Alembic::Abc::Int32ArraySample::value_vector vec_t;
                        std::vector<std::unique_ptr<vec_t>> mis;
                        mis.reserve(materialCount);
                        for (int m = 0; m < materialCount; ++m) {
                            mis.emplace_back(new vec_t);
                        }
                        for (int i = 0; i < indexArrayCount; ++i) {
                            mis[indexArray[i]]->emplace_back(i);
                        }
                        for (int m = 0; m < materialCount; ++m) {
                            FbxSurfaceMaterial const* const material = childNode->GetMaterial(m);
                            if (material) {
                                std::string const materialName = stripBoth(material->GetName());
                                Alembic::AbcGeom::OFaceSet& faceSet = oGeoSchema.createFaceSet(materialName + "SG");
                                vec_t& v = *mis[m];
                                std::sort(v.begin(), v.end());
                                Alembic::AbcGeom::OFaceSetSchema& faceSetSchema = faceSet.getSchema();
                                faceSetSchema.set(Alembic::AbcGeom::OFaceSetSchema::Sample(v));
                                faceSetSchema.setFaceExclusivity(Alembic::AbcGeom::kFaceSetExclusive);
                                if (alembicMaterial) {
                                    assignAlembicMaterialObject(faceSet, materialName);
                                }
                            }
                        }
                    } else {
                        FbxSurfaceMaterial const* const material = childNode->GetMaterial(firstIndex);
                        if (material) {
                            std::string const materialName = material->GetName();
                            if (alembicMaterial) {
                                assignAlembicMaterialSchema(oGeoSchema, materialName);
                            } else {
                                setMaterialParam(arbGeomParams, materialName);
                            }
                        }
                    }
                }
            }
        }
    } else if (materialCount == 0) {
        if (alembicMaterial) {
            assignAlembicMaterialSchema(oGeoSchema, childName);
        } else {
            setMaterialParam(arbGeomParams, childName);
        }
    }
}

static inline void convertProperties(FbxNode const* const childNode, Alembic::Abc::OCompoundProperty const& arbGeomParams) {
    for (FbxProperty prop = childNode->GetFirstProperty(); prop.IsValid(); prop = childNode->GetNextProperty(prop)) {
        std::string const& propName = prop.GetName().Buffer();
        if (propName.length() < 4) {
            continue;
        }
        if (propName.substr(0, 4) != "ABC_") {
            continue;
        }
        FbxDataType const dataType = prop.GetPropertyDataType();
        if (!dataType.Valid()) {
            continue;
        }
        if (arbGeomParams.getPropertyHeader(propName)) {
            continue;
        }
        switch (dataType.GetType()) {
            case eFbxBool: {
                Alembic::AbcGeom::OBoolGeomParam oGeomBoolParam(arbGeomParams, propName, false,
                                                                Alembic::AbcGeom::kConstantScope, 1);
                Alembic::AbcGeom::OBoolGeomParam::value_type const bv(prop.Get<FbxBool>());
                oGeomBoolParam.set(Alembic::AbcGeom::OBoolGeomParam::Sample(Alembic::Abc::BoolArraySample(&bv, 1),
                                                                            Alembic::AbcGeom::kConstantScope));
            } break;
            case eFbxInt: {
                Alembic::AbcGeom::OInt32GeomParam oGeomInt32Param(arbGeomParams, propName, false,
                                                                  Alembic::AbcGeom::kConstantScope, 1);
                int const iv(prop.Get<FbxInt>());
                oGeomInt32Param.set(Alembic::AbcGeom::OInt32GeomParam::Sample(Alembic::Abc::Int32ArraySample(&iv, 1),
                                                                              Alembic::AbcGeom::kConstantScope));
            } break;
            case eFbxFloat: {
                Alembic::AbcGeom::OFloatGeomParam oGeomFloatParam(arbGeomParams, propName, false,
                                                                  Alembic::AbcGeom::kConstantScope, 1);
                float const fv(prop.Get<FbxFloat>());
                oGeomFloatParam.set(Alembic::AbcGeom::OFloatGeomParam::Sample(Alembic::Abc::FloatArraySample(&fv, 1),
                                                                              Alembic::AbcGeom::kConstantScope));
            } break;
            case eFbxDouble: {
                Alembic::AbcGeom::ODoubleGeomParam oGeomDoubleParam(arbGeomParams, propName, false,
                                                                    Alembic::AbcGeom::kConstantScope, 1);
                double const dv(prop.Get<FbxDouble>());
                oGeomDoubleParam.set(Alembic::AbcGeom::ODoubleGeomParam::Sample(Alembic::Abc::DoubleArraySample(&dv, 1),
                                                                                Alembic::AbcGeom::kConstantScope));
            } break;
            case eFbxString: {
                Alembic::AbcGeom::OStringGeomParam oGeomStringParam(arbGeomParams, propName, false,
                                                                    Alembic::AbcGeom::kConstantScope, 1);
                std::string const sv(prop.Get<FbxString>().Buffer());
                oGeomStringParam.set(Alembic::AbcGeom::OStringGeomParam::Sample(Alembic::Abc::StringArraySample(&sv, 1),
                                                                                Alembic::AbcGeom::kConstantScope));
            } break;
        }
    }
}

static inline void convertNode(FbxNode* const childNode,
                               char const* const childName,
                               Alembic::AbcGeom::OXform& oXform,
                               std::vector<std::string>& msgs) {
    FbxMesh* const mesh = childNode->GetMesh();
    if (NULL == mesh) {
        return;
    }
    if (mesh->CheckSamePointTwice()) {
        int const numRem = mesh->RemoveBadPolygons();
        if (numRem == -1) {
            msgs.emplace_back(std::string(childName) + " bad remove skip");
            return;
        }
        msgs.emplace_back(std::to_string(numRem) + " bad polygons removed in " + childName);
    }
    FbxLayer const* const layer0 = mesh->GetLayer(0);
    if (NULL == layer0) {
        return;
    }
    FbxLayerElementCrease const* const creases = layer0->GetEdgeCrease();
    if (creases) {
        convertMesh<Alembic::AbcGeom::OSubD>(childName, childNode, mesh, layer0, creases, oXform);
    } else {
        convertMesh<Alembic::AbcGeom::OPolyMesh>(childName, childNode, mesh, layer0, creases, oXform);
    }
}

static void convertNode(FbxNode* const parentNode, Alembic::Abc::OObject& parentOut, std::vector<std::string>& msgs) {
    if (NULL == parentNode) {
        return;
    }
    int const childCount = parentNode->GetChildCount();
    for (int i = 0; i < childCount; ++i) {
        FbxNode* const childNode = parentNode->GetChild(i);
        if (NULL == childNode) {
            continue;
        }
        FbxNodeAttribute const* const nodeAttr = childNode->GetNodeAttribute();
        if (NULL == nodeAttr) {
            return;
        }
        FbxNodeAttribute::EType const attributeType = nodeAttr->GetAttributeType();
        if (attributeType == FbxNodeAttribute::eNull) {
            if (childNode->GetChildCount() == 0) {
                return;
            }
        } else if (attributeType != FbxNodeAttribute::eMesh) {
            return;
        }
        char const* const childName = childNode->GetName();
        if (parentOut.getChildHeader(childName)) {
            msgs.emplace_back(std::string(childName) + " duplicate skip");
            return;
        }
        Alembic::AbcGeom::OXform oXform(parentOut, childName);
        Alembic::AbcGeom::OXformSchema& oXformSchema = oXform.getSchema();
        Alembic::AbcGeom::XformSample xFormSample;
        xFormSample.setMatrix(childNode->EvaluateLocalTransform().Double44());
        oXformSchema.set(xFormSample);
        convertNode(childNode, childName, oXform, msgs);
        convertNode(childNode, oXform, msgs);
    }
}

static void printHelp() {
    std::cout << "ModFbx2Abc.exe -i fileIn.fbx -o fileOut.abc\n";
    std::cout << "-i --in fileIn.fbx\n";
    std::cout << "-o --out fileOut.abc\n";
    std::cout << "-p --prefix /obj/PRJ_cortana_shaders1/shops/\n";
    std::cout << "-d --disable\n";
    std::cout << "-l --legacy\n";
    std::cout << "-m --material\n";
    std::cout << "-h --help";
}

/**
 * Main function - loads the model fbx file,
 * and converts its contents to alembic.
 */
int main(int argc, char** argv) {
    if (argc == 1) {
        printHelp();
        return EXIT_FAILURE;
    }
    std::string nameFileIn;
    std::string nameFileOut;
    bool legacy = false;
    for (int argi = 1; argi < argc; ++argi) {
        std::string const flag(argv[argi]);
        if (flag == "-d" || flag == "--disable") {
            disableUVopt = true;
        } else if (flag == "-l" || flag == "--legacy") {
            legacy = true;
        } else if (flag == "-m" || flag == "--material") {
            alembicMaterial = true;
        } else if (flag == "-i" || flag == "--in") {
            ++argi;
            if (argi < argc) {
                nameFileIn = argv[argi];
            } else {
                std::cerr << "no in file";
                return EXIT_FAILURE;
            }
        } else if (flag == "-o" || flag == "--out") {
            ++argi;
            if (argi < argc) {
                nameFileOut = argv[argi];
            } else {
                std::cerr << "no out file";
                return EXIT_FAILURE;
            }
        } else if (flag == "-p" || flag == "--prefix") {
            ++argi;
            if (argi < argc) {
                prefixMaterial = argv[argi];
            } else {
                std::cerr << "no prefix";
                return EXIT_FAILURE;
            }
        } else if (flag == "-h" || flag == "--help") {
            printHelp();
            return 0;
        } else {
            if (flag[0] == '-') {
                std::size_t const fl = flag.length();
                if (fl == 2 && flag[1] == '-' || fl == 1) {
                    std::cerr << "empty flag";
                } else {
                    std::cerr << "unknown flag " << flag;
                }
            } else {
                std::cerr << "flags should begin with -";
            }
            return EXIT_FAILURE;
        }
    }
    if (nameFileIn.empty()) {
        std::cerr << "no in file" << std::endl;
        printHelp();
        return EXIT_FAILURE;
    }
    if (nameFileOut.empty()) {
        std::cerr << "no out file" << std::endl;
        printHelp();
        return EXIT_FAILURE;
    }
    if (nameFileOut == nameFileIn) {
        std::cerr << "in file and out file cannot be the same" << std::endl;
        return EXIT_FAILURE;
    }
    if (prefixMaterial.empty() && !alembicMaterial) {
        std::cerr << "no prefix material" << std::endl;
        printHelp();
        return EXIT_FAILURE;
    }

    // Initialize the SDK manager. This object handles all our memory management.
    FbxManager* lSdkManager = FbxManager::Create();
    
    // Create the IO settings object.
    FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
    lSdkManager->SetIOSettings(ios);

    // Create an importer using the SDK manager.
    FbxImporter* lImporter = FbxImporter::Create(lSdkManager,"");
    
    // Use the first argument as the filename for the importer.
    if (!lImporter->Initialize(nameFileIn.c_str(), -1, lSdkManager->GetIOSettings())) {
        std::cerr << "cannot use " << nameFileIn << std::endl;
        return EXIT_FAILURE;
    }
    
    // Create a new scene so that it can be populated by the imported file.
    FbxScene* lScene = FbxScene::Create(lSdkManager, "newScene");

    // Import the contents of the file into the scene.
    lImporter->Import(lScene);

    // Convert the scene back to centimeters in case it was not.
    FbxSystemUnit::cm.ConvertScene(lScene);

    // The file is imported; so get rid of the importer.
    lImporter->Destroy();

    Alembic::Abc::OArchive archOut;
    if (legacy) {
        archOut = Alembic::Abc::OArchive(Alembic::AbcCoreHDF5::WriteArchive(), nameFileOut);
    } else {
        archOut = Alembic::Abc::OArchive(Alembic::AbcCoreOgawa::WriteArchive(), nameFileOut);
    }
    Alembic::Abc::OObject top = archOut.getTop();
    if (alembicMaterial) {
        materialFolder = Alembic::Abc::OObject(top, materialFolderName);
    }
    std::vector<std::string> msgs;
    convertNode(lScene->GetRootNode(), top, msgs);

    // Destroy the SDK manager and all the other objects it was handling either way.
    lSdkManager->Destroy();

    if (msgs.empty()) {
        return 0;
    }
    std::cerr << msgs.front();
    std::size_t const msgCount = msgs.size();
    for (int m = 1; m < msgCount; ++m) {
        std::cerr << "\n";
        std::cerr << msgs[m];
    }
    return EXIT_FAILURE;
}
