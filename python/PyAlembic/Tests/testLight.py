#!/usr/bin/env python2_zeno

#   Copyright (c) 2011-2012 Industrial   Light   and   Magic.
#   All   rights   reserved.    Used   under   authorization.
#   This material contains the confidential  and  proprietary
#   information   of   Industrial   Light   and   Magic   and
#   may not be copied in whole or in part without the express
#   written   permission   of  Industrial Light  and   Magic.
#   This  copyright  notice  does  not   imply   publication.

import imath
import alembic
import traceback

testList = []

kConstantScope = alembic.AbcG.GeometryScope.kConstantScope

def almostEqual(a0, a1, error=0.01):
    return abs(a0 - a1) <= error

def lightOut():
    """write out light archive"""

    archive = alembic.Abc.OArchive("light1.abc")
    emptyLightObj = alembic.AbcG.OLight(archive.getTop(), "emptyLight")
    lightObj = alembic.AbcG.OLight(archive.getTop(), "myLight" )

    samp = alembic.AbcG.CameraSample()
    lightObj.getSchema().setCameraSample( samp )

    samp = alembic.AbcG.CameraSample( -0.35, 0.75, 0.1, 0.5 )
    samp.setChildBounds( imath.Box3d( imath.V3d( 0.0, 0.1, 0.2 ),
                                      imath.V3d( 0.3, 0.4, 0.5 ) ) )

    lightObj.getSchema().setCameraSample( samp )

    arg = lightObj.getSchema().getArbGeomParams()
    param = alembic.AbcG.OFloatGeomParam( arg, "test", False,
                                          kConstantScope, 1 )
    user = lightObj.getSchema().getUserProperties()
    alembic.Abc.OFloatProperty( user, "test" )

def lightIn():
    """read in light archive"""

    archive = alembic.Abc.IArchive("light1.abc")
    emptyLightObj = alembic.AbcG.ILight(archive.getTop(), "emptyLight" )
    lightObj = alembic.AbcG.ILight(archive.getTop(), "myLight" )

    assert not emptyLightObj.getSchema().getArbGeomParams()
    assert not emptyLightObj.getSchema().getUserProperties()
    assert lightObj.getSchema().getArbGeomParams().getNumProperties() == 1
    assert lightObj.getSchema().getUserProperties().getNumProperties() == 1

    samp = lightObj.getSchema().getCameraSchema().getValue( 0 )
    window = samp.getScreenWindow();
    assert almostEqual( window['top'], 0.666666666666667 )
    assert almostEqual( window['bottom'], -0.666666666666667 )
    assert almostEqual( window['left'], -1.0 )
    assert almostEqual( window['right'], 1.0 )

    samp = lightObj.getSchema().getCameraSchema().getValue( 1 )
    window = samp.getScreenWindow();
    assert almostEqual( window['top'], -0.35 )
    assert almostEqual( window['bottom'], 0.75 )
    assert almostEqual( window['left'], 0.1 )
    assert almostEqual( window['right'], 0.5 )

    assert not lightObj.getSchema().getCameraSchema().getChildBoundsProperty()


def testLightBinding():
    lightOut()
    lightIn()

testList.append(('testLightBinding', testLightBinding))

# -------------------------------------------------------------------------
# Main loop

for test in testList:
    funcName = test[0]
    print ""
    print "Running %s" % funcName
    test[1]()
    print "passed"

print ""

