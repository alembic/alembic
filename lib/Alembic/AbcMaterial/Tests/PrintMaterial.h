#ifndef Alembic_AbcMaterialTest_PrintMaterial_h
#define Alembic_AbcMaterialTest_PrintMaterial_h

#include <Alembic/AbcMaterial/MaterialFlatten.h>

void printFlattenedMaterial(Alembic::AbcMaterial::IMaterial & material);

void printMaterialSchema(
        Alembic::AbcMaterial::IMaterialSchema & schema);

void printFlattenedMafla(Alembic::AbcMaterial::MaterialFlatten & mafla);


#endif
