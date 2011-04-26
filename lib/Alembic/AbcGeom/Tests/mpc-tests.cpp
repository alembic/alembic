#include <Alembic/AbcGeom/All.h>
#include <Alembic/AbcCoreHDF5/All.h>

#include <exception>

#include "MeshData.h"

Alembic::Abc::OObject subdCube( Alembic::Abc::OObject parent )
{
    Alembic::AbcGeom::OSubD subdObj( parent, "mySubD",
                                     Alembic::Abc::ErrorHandler::kNoisyNoopPolicy );
    Alembic::AbcGeom::OSubDSchema &schema = subdObj.getSchema();

    Alembic::AbcGeom::OSubDSchema::Sample sample(
        Alembic::Abc::V3fArraySample( ( const Alembic::Abc::V3f* )g_verts, g_numVerts ),
        Alembic::Abc::Int32ArraySample( g_indices, g_numIndices ),
        Alembic::Abc::Int32ArraySample( g_counts, g_numCounts ) );
    schema.set( sample, Alembic::Abc::OSampleSelector( 0 ) );
    return subdObj;
}

int main()
{
    Alembic::Abc::OArchive archive(
        Alembic::AbcCoreHDF5::WriteArchive(),
        "mpc-out.abc"
                                  );
    Alembic::Abc::OObject archiveTop = archive.getTop();

    Alembic::Abc::OObject obj = subdCube( archiveTop );

    //
    // NOW THE FUN BEGINS
    //

    if( Alembic::AbcGeom::OSubD::matches( obj.getHeader() ) )
    {
        //Alembic::AbcGeom::OSubD subdObj( obj, Alembic::Abc::kWrapExisting );

        Alembic::AbcGeom::OCompoundProperty cpschema( obj.getProperties(), ".geom" );


        Alembic::AbcGeom::OSubDSchema::Sample sample;
        sample.setPositions( Alembic::Abc::V3fArraySample( ( const Alembic::Abc::V3f* )g_verts,
                                                           g_numVerts ) );
        //schema().set( sample, Alembic::Abc::OSampleSelector( 1 ) );
    }

    return 0;
}
