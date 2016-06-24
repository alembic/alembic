
#pragma once

#include <Alembic/Abc/All.h>
#include <Alembic/AbcGeom/All.h>
#include <boost/dynamic_bitset.hpp>

struct AnimObj
{
    struct SamplingData
    {
        using Time = Alembic::AbcGeom::chrono_t;
        union
        {
            Time start_;
            bool cyclic_;
        };
        Time cycle_;
        using Times = std::vector<Time>;
        std::unique_ptr<Times> times_;
    };
    using Sampling = std::unique_ptr<SamplingData>;
    struct Visible
    {
        boost::dynamic_bitset<> flags_;
        Sampling sampling_;
    };
    struct Attributes
    {
        template <typename T>
        struct Attribute
        {
            std::string name_;
            Sampling sampling_;
            std::unique_ptr<std::vector<T>> values_;
        };
        template <typename T>
        using AttributeContainer = std::unique_ptr<std::vector<Attribute<T>>>;
        AttributeContainer<int> ints_;
        AttributeContainer<float> floats_;
        AttributeContainer<double> doubles_;
        AttributeContainer<std::string> strings_;
        AttributeContainer<Alembic::AbcGeom::OBoolGeomParam::value_type> bools_;
    };
    std::string name_;
    Sampling sampling_;
    std::unique_ptr<Visible> visible_;
    std::unique_ptr<Attributes> attributes_;
    using Xforms = std::vector<Alembic::Abc::M44d>;
    std::unique_ptr<Xforms> xForms_;
    struct Geom
    {
        struct Topo
        {
            using TpV = Alembic::AbcGeom::Int32ArraySample::value_vector;
            std::unique_ptr<TpV> ds_;
            std::unique_ptr<TpV> cs_;
        };
        using PsV = Alembic::AbcGeom::P3fArraySample::value_vector;
        using Points = std::vector<std::unique_ptr<PsV>>;
        std::unique_ptr<Points> points_;
        using nds_vt = Alembic::AbcGeom::UInt32ArraySample::value_vector;
        struct NormalData
        {
            using NsV = Alembic::AbcGeom::N3fArraySample::value_vector;
            using Normals = std::vector<std::unique_ptr<NsV>>;
            std::unique_ptr<Normals> normals_;
            using Indices = std::vector<std::unique_ptr<nds_vt>>;
            std::unique_ptr<Indices> indices_;
            std::unique_ptr<nds_vt> index_;
        };
        std::unique_ptr<NormalData> normalData_;
        using uvs_vt = Alembic::AbcGeom::V2fArraySample::value_vector;
        using UVset = std::tuple<
                std::unique_ptr<uvs_vt>,
                std::unique_ptr<nds_vt>,
                std::string>;
        std::unique_ptr<UVset> uvSet_;
        Topo topo_;
    };
    std::unique_ptr<Geom> geom_;
    using AnimData = std::unique_ptr<std::vector<std::unique_ptr<AnimObj>>>;
    AnimData anims_;
};

struct AnimConfig
{
    bool groupByInstance = false;
    bool formNamespaces = false;
    bool forceNoMatch = false;
    bool passNoMatch = false;
    bool noReplace = false;
    bool lookDev = false;
};

struct ModConfig
{
    std::string namePositions = "rest";
    std::string nameNormals = "rnml";
    std::string nameProject = "PRJ";
    bool outPositions = false;
    bool outNormals = false;
    bool genNormals = false;
    bool outUVs = true;
};

using AnimFiles = std::vector<std::pair<std::string, AnimObj::AnimData>>;
using AlembicPath = std::vector<std::string>;

extern std::string const visiblePropName;
extern std::string const materialParamName;

template <typename TV>
static inline void putValues(
        typename TV::value_type const* const p,
        size_t const s,
        TV& v)
{
    if (0 == s)
    {
        return;
    }
    v.resize(s);
    memcpy(v.data(), p, sizeof(typename TV::value_type) * s);
}
