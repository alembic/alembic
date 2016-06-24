
#pragma once

#include <experimental/string_view>

using std::experimental::string_view;

string_view extractAssetNameFromFilePath(string_view const filePath);

struct ModelData;

class ModAttrs;

namespace modGather
{
ModelData const* findMatch(
        string_view const fullName,
        string_view const modName);

AnimObj::Geom::PsV const& getPs(ModelData const&);

AnimObj::Geom::Topo const& topoExtract(ModelData const&);

ModAttrs const& getModAttrs(ModelData const& modelData);

std::vector<std::string> getModAttrList(ModAttrs const& modAttrs);

void putUserProps(
        Alembic::Abc::OCompoundProperty const& userProps,
        ModAttrs const& modAttrs);

void putParams(
        Alembic::Abc::OCompoundProperty const& outParams,
        ModAttrs const& modAttrs);

void putUVsets(
        ModelData const& modelData,
        Alembic::Abc::OCompoundProperty const& arbGeoParams);

void putRestPoints(
        ModelData const& modelData,
        Alembic::Abc::OCompoundProperty const& arbGeoParams);

void putRestNormals(
        ModelData const& modelData,
        Alembic::Abc::OCompoundProperty const& arbGeoParams);

template <typename OGeoSample>
void putAnimGeometrySpec(
        ModelData const& modelData,
        const bool setNorms,
        OGeoSample& oGeoSample);

template <typename OGeoSchema>
void putDefUVset(
        ModelData const& modelData,
        OGeoSchema& oGeoSchema,
        typename OGeoSchema::Sample& oGeoSample);
}
