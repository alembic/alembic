//-*****************************************************************************
//
// Copyright (c) 2009-2012,
//  Sony Pictures Imageworks Inc. and
//  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// *       Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// *       Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
// *       Neither the name of Sony Pictures Imageworks, nor
// Industrial Light & Magic, nor the names of their contributors may be used
// to endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//-*****************************************************************************

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreOgawa/All.h>

#include <Alembic/AbcMaterial/OMaterial.h>
#include <Alembic/AbcMaterial/IMaterial.h>
#include <Alembic/AbcMaterial/MaterialFlatten.h>

#include "PrintMaterial.h"

namespace Abc =  Alembic::Abc;
namespace Mat = Alembic::AbcMaterial;


void write()
{
    Abc::OArchive archive(
            Alembic::AbcCoreOgawa::WriteArchive(), "FlattenMaterial.abc" );

    Abc::OObject root(archive, Abc::kTop);

    //make a dummy enclosing object
    Abc::OObject materials(root, "materials");

    Mat::OMaterial parentMaterial(materials, "parentMaterial");

    parentMaterial.getSchema().setShader("prman", "surface", "paintedplastic");
    parentMaterial.getSchema().setShader("prman", "displacement", "knobby");

    {
        Abc::OFloatProperty prop(
                parentMaterial.getSchema().getShaderParameters(
                        "prman", "surface"),
                        "Kd");
        prop.set(0.5);
    }

    {
        Abc::OStringProperty prop(
                parentMaterial.getSchema().getShaderParameters(
                        "prman", "surface"), "texname");
        prop.set("taco");
    }

    {
        Abc::OFloatProperty prop(
                parentMaterial.getSchema().getShaderParameters(
                        "prman", "displacement"),
                        "height");
        prop.set(0.75);
    }

    Mat::OMaterial childMaterial(parentMaterial, "childMaterial");

    childMaterial.getSchema().setShader("prman", "surface",
            "betterpaintedplastic");

    {
        Abc::OStringProperty prop(
                childMaterial.getSchema().getShaderParameters(
                        "prman", "surface"), "texname");
        prop.set("cheese");
    }

}



void read()
{
    Abc::IArchive archive(Alembic::AbcCoreOgawa::ReadArchive(),
            "FlattenMaterial.abc");


    Abc::IObject materialsObject(archive.getTop(), "materials");
    Mat::IMaterial parentMaterial(materialsObject, "parentMaterial");
    Mat::IMaterial childMaterial(parentMaterial, "childMaterial");

    std::cout << "----------------------------------------\n";
    std::cout << "LOCAL\n";


    std::cout << "---------------------------\n";
    std::cout << "Local parent material\n";
    std::cout << "---------------------------\n";

    printMaterialSchema(parentMaterial.getSchema());

    std::cout << "---------------------------\n";
    std::cout << "Local child material\n";
    std::cout << "---------------------------\n";

    printMaterialSchema(childMaterial.getSchema());

    std::cout << "----------------------------------------\n";
    std::cout << "FLATTENED\n";

    printFlattenedMaterial(parentMaterial);
    printFlattenedMaterial(childMaterial);

}




int main( int argc, char *argv[] )
{
    write();
    read();


    return 0;
}
