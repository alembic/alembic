#include <Alembic/Abc/All.h>
#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>
#include <Alembic/AbcMaterial/MaterialAssignment.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

namespace Abc =  Alembic::Abc;
namespace Mat = Alembic::AbcMaterial;
namespace Geom = Alembic::AbcGeom;


const size_t g_numVerts = 8;
const Abc::float32_t g_verts[] = { -1.0f, -1.0f, -1.0f,
                                          1.0f, -1.0f, -1.0f,
                                          -1.0f, 1.0f, -1.0f,
                                          1.0f, 1.0f, -1.0f,
                                          -1.0f, -1.0f, 1.0f,
                                          1.0f, -1.0f, 1.0f,
                                          -1.0f, 1.0f, 1.0f,
                                          1.0f, 1.0f, 1.0f };

const size_t g_numIndices = 24;
const Abc::int32_t g_indices[] = {
    // LEFT
    0, 4, 6, 2,
    // RIGHT
    5, 1, 3, 7,
    // BOTTOM
    0, 1, 5, 4,
    // TOP,
    6, 7, 3, 2,
    // BACK
    1, 0, 2, 3,
    // FRONT
    4, 5, 7, 6 };

const Abc::uint32_t g_uindices[] = {
    // LEFT
    0, 4, 6, 2,
    // RIGHT
    5, 1, 3, 7,
    // BOTTOM
    0, 1, 5, 4,
    // TOP,
    6, 7, 3, 2,
    // BACK
    1, 0, 2, 3,
    // FRONT
    4, 5, 7, 6 };

//-*****************************************************************************
// "Face Counts" - number of vertices in each face.
const size_t g_numCounts = 6;
const Abc::int32_t g_counts[] = { 4, 4, 4, 4, 4, 4 };




void setFloatParameter(
        Mat::OMaterialSchema & schema,
        const std::string & target,
        const std::string & shaderType,
        const std::string & paramName, float value)
{
    Abc::OFloatProperty prop(
                schema.getShaderParameters(target, shaderType),
                        paramName);
    prop.set(value);
}

void setStringParameter(
        Mat::OMaterialSchema & schema,
        const std::string & target,
        const std::string & shaderType,
        const std::string & paramName, const std::string & value)
{
    Abc::OStringProperty prop(
                schema.getShaderParameters(target, shaderType),
                        paramName);
    prop.set(value);
}



void write()
{
    Abc::OArchive archive(
            Alembic::AbcCoreHDF5::WriteArchive(), "MeshesWithMaterials.abc" );
    
    Geom::OObject root(archive, Abc::kTop);
    Geom::OXform geometry(root, "geometry");
    Abc::OObject materials(root, "materials");
    
    Mat::OMaterial materialA(materials, "materialA");
    materialA.getSchema().setShader("prman", "surface", "paintedplastic");
    
    setFloatParameter(materialA.getSchema(),
            "prman", "surface", "Kd", 0.5);
    setStringParameter(materialA.getSchema(),
            "prman", "surface", "texturename", "/tmp/mytexture.tx");
    
    
    Mat::OMaterial materialB(materialA, "materialB");
    setStringParameter(materialB.getSchema(),
            "prman", "surface", "texturename", "/tmp/myothertexture.tx");
    
    
    {
        const Abc::float32_t colorArrayData[] = { 1.0f, 0.0f, 0.0f,
                                                  0.5f, 0.5f, 0.5f,
                                                  0.25f, 0.25f, 1.0f};
        
        Abc::OC3fArrayProperty prop(
                materialB.getSchema().getShaderParameters("prman", "surface"),
                        "mycolorarray");
        
        prop.set(Abc::C3fArraySample( ( const Abc::C3f * )colorArrayData, 3));
    }
    
    
    
    Geom::OPolyMesh mesh1(geometry, "mesh1");
    
    
    Geom::OPolyMeshSchema::Sample mesh_samp(
            Abc::V3fArraySample( ( const Abc::V3f * )g_verts, g_numVerts ),
            Abc::Int32ArraySample( g_indices, g_numIndices ),
            Abc::Int32ArraySample( g_counts, g_numCounts ));
    
    mesh1.getSchema().set( mesh_samp );
    
    
    Mat::addMaterialAssignment(mesh1, "/materials/materialA");
    
    
    Geom::OSubD subd1(geometry, "subd1");
    Geom::OSubDSchema::Sample subd_mesh_samp(
            Abc::V3fArraySample( ( const Abc::V3f * )g_verts, g_numVerts ),
            Abc::Int32ArraySample( g_indices, g_numIndices ),
            Abc::Int32ArraySample( g_counts, g_numCounts ));
    subd1.getSchema().set( subd_mesh_samp );
    Mat::addMaterialAssignment(subd1, "/materials/materialA");
    
    
    Geom::OFaceSet faceset1 = subd1.getSchema().createFaceSet("faceset1");
    std::vector<Abc::int32_t> face_nums;
    face_nums.push_back(0);
    face_nums.push_back(3);
    face_nums.push_back(5);
    Geom::OFaceSetSchema::Sample my_face_set_samp(face_nums);
    faceset1.getSchema().set(my_face_set_samp);
    
    Mat::addMaterialAssignment(faceset1, "/materials/materialA/materialB");
    
    
    
}



int main( int argc, char *argv[] )
{
    write();
    return 0;
}
