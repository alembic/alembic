#ifndef _Alembic_Prman_PathUtil_h_
#define _Alembic_Prman_PathUtil_h_


#include <Alembic/AbcGeom/All.h>
using namespace Alembic;

typedef std::vector<std::string> PathList;

void TokenizePath(const std::string & path, PathList & result);





#endif
