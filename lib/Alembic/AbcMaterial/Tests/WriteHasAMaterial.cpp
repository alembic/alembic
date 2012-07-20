#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include <Alembic/AbcMaterial/OMaterial.h>
#include <Alembic/AbcMaterial/IMaterial.h>

#include <Alembic/AbcMaterial/MaterialAssignment.h>
#include <Alembic/AbcCoreAbstract/Tests/Assert.h>

#include "PrintMaterial.h"


namespace Abc =  Alembic::Abc;
namespace Mat = Alembic::AbcMaterial;


void write()
{
    Abc::OArchive archive(
            Alembic::AbcCoreHDF5::WriteArchive(), "HasAMaterial.abc" );
    
    Abc::OObject root(archive, Abc::kTop);
    
    
    //BEGIN Manually create and test schemas and compounds {
    Abc::OObject obj(root, "an_object");
    
    Mat::OMaterialSchema mat(Mat::OMaterialSchema(obj.getProperties(), ".byanyothername"));
    mat.setShader("prman", "surface", "ambocc");
    
    {
        Abc::OCompoundProperty compound(obj.getProperties(), "butnotbythisone");
        Abc::OStringProperty sprop(compound, "value");
        sprop.set("hello");
    }
    
    {
        //add a .material but not created with the schema
        Abc::OCompoundProperty compound(obj.getProperties(), ".material");
        Abc::OStringProperty sprop(compound, "value");
        sprop.set("world");
    }
    
    //END Manually create and test schemas and compounds }
    
    //BEGIN Test assignment and has-a mechanisms {
    {
        Abc::OObject anotherObj(root, "another_object");
        Mat::addMaterialAssignment(anotherObj, "/some/material");
        
        Mat::OMaterialSchema mat = Mat::addMaterial(anotherObj);
        
        mat.setShader("prman", "surface", "carpet");
    }
    //END Test assignment and has-a mechanisms }
    
    
    
}


void read()
{
    Abc::IArchive archive(Alembic::AbcCoreHDF5::ReadArchive(),
            "HasAMaterial.abc");
    
    Abc::IObject an_object(archive.getTop(), "an_object");
    
    
    if (const Abc::PropertyHeader * header =
            an_object.getProperties().getPropertyHeader(".byanyothername"))
    {
        TESTING_ASSERT(Mat::OMaterialSchema::matches(*header));
        if (Mat::OMaterialSchema::matches(*header))
        {
            std::cout << ".byanyothername yes.\n";
            
            Mat::IMaterialSchema mat(an_object.getProperties(), ".byanyothername");
            
            printMaterialSchema(mat);
            
        }
        else
        {
            std::cout << ".byanyothername no.\n";
        }
        
    }
    
    if (const Abc::PropertyHeader * header =
            an_object.getProperties().getPropertyHeader("butnotbythisone"))
    {
        TESTING_ASSERT( !Mat::OMaterialSchema::matches(*header) );
        if (Mat::OMaterialSchema::matches(*header))
        {
            std::cout << "butnotbythisone yes.\n";
        }
        else
        {
            std::cout << "butnotbythisone no.\n";
        }
    }
    
    
    if (const Abc::PropertyHeader * header =
            an_object.getProperties().getPropertyHeader(".material"))
    {
        TESTING_ASSERT(!Mat::OMaterialSchema::matches(*header));
        if (Mat::OMaterialSchema::matches(*header))
        {
            std::cout << "manually built .material yes.\n";
        }
        else
        {
            std::cout << "manually built .material no.\n";
        }
    }
    
    std::cout << "-----------\n";
    
    Abc::IObject anotherObj(archive.getTop(), "another_object");
    
    
    std::string assignmentPath;
    if (Mat::getMaterialAssignmentPath(anotherObj, assignmentPath))
    {
        std::cout << "another_object assignment path: " << assignmentPath;
        std::cout << std::endl;
    }
    TESTING_ASSERT(assignmentPath == "/some/material");

    Mat::IMaterialSchema hasMat;
    
    TESTING_ASSERT(Mat::hasMaterial(anotherObj, hasMat));
    if (Mat::hasMaterial(anotherObj, hasMat))
    {
        std::cout << "another_object has local material: " << std::endl;
        
        printMaterialSchema(hasMat);
    }
    
}

int main( int argc, char *argv[] )
{
    write();
    read();
    return 0;
}

