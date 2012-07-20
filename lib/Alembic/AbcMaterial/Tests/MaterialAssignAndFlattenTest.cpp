#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include <Alembic/AbcMaterial/MaterialAssignment.h>
#include "PrintMaterial.h"
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

namespace Abc =  Alembic::Abc;
namespace Mat = Alembic::AbcMaterial;

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



void write()
{
    Abc::OArchive archive(
            Alembic::AbcCoreHDF5::WriteArchive(), "MaterialAssignment.abc" );
    
    
    Abc::OObject root(archive, Abc::kTop);
    Abc::OObject materials(root, "materials");
    Abc::OObject geometry(root, "geometry");
    
    
    //parent material
    Mat::OMaterial materialA(materials, "materialA");
    
    materialA.getSchema().setShader("prman", "surface", "paintedplastic");
    
    setFloatParameter(materialA.getSchema(),
            "prman", "surface", "Kd", 0.5);
    setFloatParameter(materialA.getSchema(),
            "prman", "surface", "roughness", 0.1);
    
    //child material
    Mat::OMaterial materialB(materialA, "materialB");
    materialB.getSchema().setShader("prman", "displacement", "knobby");
    setFloatParameter(materialB.getSchema(),
            "prman", "surface", "roughness", 0.2);
    
    
    Abc::OObject geoA(geometry, "geoA");
    Mat::addMaterialAssignment(geoA, "/materials/materialA");
    
    Abc::OObject geoB(geometry, "geoB");
    Mat::addMaterialAssignment(geoB, "/materials/materialA/materialB");
    
    
    Abc::OObject geoC(geometry, "geoC");
    Mat::addMaterialAssignment(geoC, "/materials/materialA/materialB");
    Mat::OMaterialSchema geoCMat = Mat::addMaterial(geoC);
    setFloatParameter(geoCMat, "prman", "surface", "roughness", 0.3);
}



void traverse(Abc::IObject object, bool includeSelf)
{
    if (includeSelf)
    {
        std::cout << "---------------------------------" << std::endl;
        std::cout << object.getFullName() << std::endl;
        
        
        
        
        if (Mat::IMaterial::matches(object.getHeader()))
        {
            std::cout << "(is material, local data shown)\n";
            Mat::IMaterial mat(object, Abc::kWrapExisting);
            printMaterialSchema(mat.getSchema());
            TESTING_ASSERT(object.getName() == "materialA" ||
                object.getName() == "materialB");
        }
        else
        {
            Mat::MaterialFlatten mafla(object);

            std::string name = object.getName();
            std::cout << name << " " << mafla.empty() << std::endl;
            TESTING_ASSERT(
                (!mafla.empty() &&
                    (name == "geoA" || name == "geoB" || name == "geoC")) ||
                (mafla.empty() &&
                    (name == "geometry" || name == "materials")));

            if (!mafla.empty())
            {
                std::cout << "(flattened material via has and/or assigned)\n";
                printFlattenedMafla(mafla);
            }
            else
            {
                std::cout << "(neither is, has or is assigned)\n";
            }
        }
        
        
        
        
        
        
        
        
    }
    
    for (size_t i = 0; i < object.getNumChildren(); ++i)
    {
        traverse(object.getChild(i), true);
    }
    
}


void read()
{
    Abc::IArchive archive(Alembic::AbcCoreHDF5::ReadArchive(),
            "MaterialAssignment.abc");
    
    traverse(archive.getTop(), false);

}


int main( int argc, char *argv[] )
{
    write();
    read();
    return 0;
}
