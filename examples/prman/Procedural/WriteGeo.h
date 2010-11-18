#ifndef _Alembic_Prman_WriteGeo_h_
#define _Alembic_Prman_WriteGeo_h_

#include <Alembic/AbcGeom/All.h>
using namespace Alembic;

#include "ProcArgs.h"

void WriteIdentifier(const Abc::ObjectHeader & ohead);
void ProcessSimpleTransform(AbcGeom::ISimpleXform xform, ProcArgs & args);
void ProcessPolyMesh(AbcGeom::IPolyMesh polymesh, ProcArgs & args);
void ProcessSubD(AbcGeom::ISubD subd, ProcArgs & args);


#endif
