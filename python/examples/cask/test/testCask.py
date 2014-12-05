#-******************************************************************************
#
# Copyright (c) 2012-2014,
#  Sony Pictures Imageworks Inc. and
#  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# *       Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# *       Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
# *       Neither the name of Sony Pictures Imageworks, nor
# Industrial Light & Magic, nor the names of their contributors may be used
# to endorse or promote products derived from this software without specific
# prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#-******************************************************************************

import os
import sys
import string
import unittest
import tempfile

import imath
import alembic
import cask
import meshData

kFacevaryingScope = alembic.AbcGeom.GeometryScope.kFacevaryingScope
kConstantScope = alembic.AbcGeom.GeometryScope.kConstantScope

# temporary directory for holding test archives
TEMPDIR = tempfile.mkdtemp()

"""
TODO
 - add new TimeSampling objects to archives (empty and from disk)
 - more tests for getting and setting values/samples
 - test creating prototype schema objects
 - test name collisions when creating new objects and properties
"""

def set_float(schema, target, shaderType, paramName, value):
    prop = alembic.Abc.OFloatProperty(schema.getShaderParameters(target, shaderType),
            paramName)
    prop.setValue(value)
    return prop

def lights_out():
    filename = os.path.join(TEMPDIR, "cask_test_lights.abc")
    if os.path.exists(filename) and cask.is_valid(filename):
        return filename

    os.system("rm -f %s" % filename)
    archive = alembic.Abc.OArchive(filename)
    lightA = alembic.AbcGeom.OLight(archive.getTop(), "lightA")
    lightB = alembic.AbcGeom.OLight(archive.getTop(), "lightB")

    samp = alembic.AbcGeom.CameraSample()
    lightB.getSchema().setCameraSample(samp)

    # camera data
    samp = alembic.AbcGeom.CameraSample(-0.35, 0.75, 0.1, 0.5)
    lightB.getSchema().getChildBoundsProperty().setValue(
        imath.Box3d(imath.V3d(0.0, 0.1, 0.2), imath.V3d(0.3, 0.4, 0.5 )))
    samp.setNearClippingPlane(0.0)
    samp.setFarClippingPlane(1000.0)
    samp.setHorizontalAperture(2.8)
    samp.setVerticalAperture(2.8)
    samp.setFocalLength(50)
    lightB.getSchema().setCameraSample(samp)

    # material data
    mat = alembic.AbcMaterial.addMaterial(lightB, "shader")
    mat.setShader("prman", "light", "materialB")

    exposure = set_float(mat, "prman", "light", "exposure", 1.0)
    spec = set_float(mat, "prman", "light", "specular", 0.1)

    for i in range(10):
        exposure.setValue(1.0)
        spec.setValue(0.1 + i/10.0)

    # user properties
    user = lightB.getSchema().getUserProperties()
    test = alembic.Abc.OFloatProperty(user, "test")
    test.setValue(10)

    return filename

def mesh_out(name="cask_test_mesh.abc", force=False):
    filename = os.path.join(TEMPDIR, name)
    if not force and (os.path.exists(filename) and cask.is_valid(filename)):
        return filename

    oarch = alembic.Abc.OArchive(filename)
    meshyObj = alembic.AbcGeom.OPolyMesh(oarch.getTop(), 'meshy')
    mesh = meshyObj.getSchema()

    uvsamp = alembic.AbcGeom.OV2fGeomParamSample(meshData.uvs, kFacevaryingScope)
    nsamp  = alembic.AbcGeom.ON3fGeomParamSample(meshData.normals, kFacevaryingScope)
    mesh_samp = alembic.AbcGeom.OPolyMeshSchemaSample(
            meshData.verts, meshData.indices, meshData.counts, uvsamp, nsamp)

    cbox = imath.Box3d()
    cbox.extendBy(imath.V3d(1.0, -1.0, 0.0))
    cbox.extendBy(imath.V3d(-1.0, 1.0, 3.0))

    for i in range(10):
        mesh.getChildBoundsProperty().setValue(cbox)
        mesh.set(mesh_samp)

    del oarch
    return filename

def cube_out(name="cask_test_cube.abc", force=False):
    filename = os.path.join(TEMPDIR, name)
    if not force and (os.path.exists(filename) and cask.is_valid(filename)):
        return filename

    tvec = alembic.AbcCoreAbstract.TimeVector()
    tvec[:] = [1, 2, 3]

    timePerCycle = 3.0
    numSamplesPerCycle = len(tvec)

    tst = alembic.AbcCoreAbstract.TimeSamplingType(numSamplesPerCycle, timePerCycle)
    ts = alembic.AbcCoreAbstract.TimeSampling(tst, tvec)

    top = alembic.Abc.OArchive(filename).getTop()
    tsidx = top.getArchive().addTimeSampling(ts)

    # create the top xform
    xform = alembic.AbcGeom.OXform(top, 'cube1', tsidx)
    xsamp = alembic.AbcGeom.XformSample()
    xform.getSchema().set(xsamp)

    # the mesh shape
    meshObj = alembic.AbcGeom.OPolyMesh(xform, 'cube1Shape')
    mesh = meshObj.getSchema()
    mesh_samp = alembic.AbcGeom.OPolyMeshSchemaSample(
        meshData.points, meshData.faceIndices, meshData.faceCounts
    )
    mesh_samp.setSelfBounds(meshData.selfBnds)
    mesh.set(mesh_samp)

    return filename

def deep_out():
    filename = os.path.join(TEMPDIR, "cask_test_deep.abc")
    if os.path.exists(filename) and cask.is_valid(filename):
        return filename

    obj = alembic.Abc.OArchive(filename).getTop()
    for i in range(10):
        obj = alembic.AbcGeom.OXform(obj, string.uppercase[i])
        p = obj.getProperties()
        for i in range(3):
            p = alembic.Abc.OCompoundProperty(p, string.lowercase[i])
        p = alembic.Abc.OStringProperty(p, "myprop")
        p.setValue("foo")

    return filename

class Test1_Write(unittest.TestCase):
    def test_write_basic(self):
        filename = os.path.join(TEMPDIR, "cask_write_basic.abc")

        # create empty archive
        a = cask.Archive()
        self.assertEqual(len(a.top.children), 0)

        # create xform object named foo and make it a child of top
        f = a.top.children["foo"] = cask.Xform()
        self.assertEqual(len(a.top.children), 1)
        self.assertEqual(a.top.children.values()[0].name, "foo")

        # create some simple properties
        b = f.properties["bar"] = cask.Property()
        a.top.children["foo"].properties["bar"].set_value("hello")
        f.properties["baz"] = cask.Property()
        a.top.children["foo"].properties["baz"].set_value(42.0)

        # check to make sure object() returns the right thing
        self.assertEqual(b.object(), f)

        # write to disk
        a.write_to_file(filename)
        self.assertTrue(os.path.isfile(filename))

    def test_extract_light(self):
        filename = os.path.join(TEMPDIR, "cask_exract_light.abc")

        # open light archive and create new empty archive
        a = cask.Archive(lights_out())
        b = cask.Archive()

        # find a light and assign it to b's hierarchy
        results = cask.find(a.top, "lightB")
        self.assertEqual(len(results), 1)
        b.top.children["lightB"] = results[0]

        # write new archive to disk
        b.write_to_file(filename)
        self.assertTrue(os.path.isfile(filename))

    def test_copy_lights(self):
        filename = os.path.join(TEMPDIR, "cask_copy_lights.abc")

        # walks hierarchy and dupes each object and property as-is
        a = cask.Archive(lights_out())
        a.write_to_file(filename)
        self.assertTrue(os.path.isfile(filename))

    def test_copy_mesh(self):
        filename = os.path.join(TEMPDIR, "cask_copy_mesh.abc")

        # walks hierarchy and dupes each object and property as-is
        a = cask.Archive(mesh_out())
        a.write_to_file(filename)
        self.assertTrue(os.path.isfile(filename))

    def test_write_geom(self):
        filename = os.path.join(TEMPDIR, "cask_write_geom.abc")

        # create empty archive and put some objects in it
        a = cask.Archive()

        # create one of each geom class
        a.top.children["xform"] = cask.Xform()
        a.top.children["polymesh"] = cask.PolyMesh()
        a.top.children["subd"] = cask.SubD()
        a.top.children["faceset"] = cask.FaceSet()
        a.top.children["curve"] = cask.Curve()
        a.top.children["camera"] = cask.Camera()
        a.top.children["nupatch"] = cask.NuPatch()
        a.top.children["material"] = cask.Material()
        a.top.children["light"] = cask.Light()
        a.top.children["points"] = cask.Points()

        # export the archive
        a.write_to_file(filename)
        self.assertTrue(os.path.isfile(filename))

    def test_write_mesh(self):
        filename = os.path.join(TEMPDIR, "cask_write_mesh.abc")

        # create empty archive, xform and polymesh
        a = cask.Archive()
        x = cask.Xform()
        p = cask.PolyMesh()

        # hierarchy assignment
        a.top.children["foo"] = x
        x.children["meshy"] = p

        # set sample values using wrapped sample methods
        x.set_scale(imath.V3d(1, 2, 3))

        # create alembic polymesh sample and set it on our polymesh
        uvsamp = alembic.AbcGeom.OV2fGeomParamSample(meshData.uvs, kFacevaryingScope)
        nsamp = alembic.AbcGeom.ON3fGeomParamSample(meshData.normals, kFacevaryingScope)
        s = alembic.AbcGeom.OPolyMeshSchemaSample(meshData.verts, meshData.indices,
                meshData.counts, uvsamp, nsamp)
        p.set_sample(s)

        # write to disk
        a.write_to_file(filename)
        self.assertTrue(os.path.isfile(filename))

    def test_new_property(self):
        filename = os.path.join(TEMPDIR, "cask_new_property.abc")

        # create a new property
        a = cask.Archive(mesh_out())
        t = a.top

        # create simple property
        f = t.properties["foo"] = cask.Property()
        self.assertEqual(f.name, "foo")
        self.assertEqual(f, t.properties["foo"])
        self.assertEqual(f.parent, t)
        self.assertEqual(f.object(), t)

        # create new object
        l = t.children["spot"] = cask.Light()
        self.assertEqual(a.top.children["spot"], l)
        self.assertEqual(l.name, "spot")
        self.assertEqual(t.children["spot"].name, "spot")
        self.assertEqual(l.parent, t)
        self.assertEqual(l.children, {})

        # change name in place
        t.children["spot"].name = "point"
        self.assertEqual(l.name, "point")

        a.write_to_file(filename)

    def test_selective_update(self):
        filename = os.path.join(TEMPDIR, "cask_selective_update.abc")

        # test setting bounds after reading in from an iarchive
        a = cask.Archive(lights_out())

        # update the name
        l = a.top.children["lightB"]
        l.name = "lightC"

        # update the x position
        p = l.properties["shader/prman.light.params/exposure"]
        p.set_value(0.5, index=5)
        self.assertAlmostEqual(p.values[5], 0.5)

        # write it back out
        a.write_to_file(filename)
        self.assertTrue(os.path.isfile(filename))

    def test_insert_node(self):
        filename = os.path.join(TEMPDIR, "cask_insert_node.abc")

        # test inerting a node into the hierarchy at the top
        a = cask.Archive(mesh_out())

        # insert a new xform between two nodes
        r = cask.Xform()
        m = a.top.children["meshy"]
        a.top.children["root"] = r
        r.children["meshy"] = m

        # write to disk
        a.write_to_file(filename)
        self.assertTrue(os.path.isfile(filename))

    def test_rename(self):
        filename = os.path.join(TEMPDIR, "cask_test_rename.abc")

        a = cask.Archive()
        t = a.top

        # create a new object and property
        t.children["foo"] = cask.Xform()
        t.properties["some"] = cask.Property()
        f = t.children.values()[0]
        p = t.properties.values()[0]
        self.assertEqual(f.name, "foo")
        self.assertEqual(p.name, "some")

        # rename them
        f.name = "bar"
        self.assertEqual(f.name, "bar")
        self.assertEqual(t.children.values()[0].name, "bar")
        p.name = "thing"
        self.assertEqual(p.name, "thing")
        self.assertEqual(t.properties.values()[0].name, "thing")

        # test for accessor updates
        self.assertEqual(t.children["bar"], f)
        self.assertRaises(KeyError, t.children.__getitem__, "foo")
        self.assertEqual(t.properties["thing"], p)
        self.assertRaises(KeyError, t.properties.__getitem__, "some")

        # write to a file
        a.write_to_file(filename)

    def test_reassign(self):
        filename = os.path.join(TEMPDIR, "cask_test_reassign.abc")

        a = cask.Archive()
        t = a.top
        t.children["xform"] = cask.Xform()
        t.properties["prop"] = cask.Property()
        self.assertEqual(t.children["xform"].type(), "Xform")
        self.assertEqual(t.properties["prop"].type(), "Property")

        # reassign object
        t.children["xform"] = cask.PolyMesh()
        self.assertEqual(t.children["xform"].type(), "PolyMesh")

        # rename object
        x = t.children["xform"]
        x.name = "meshy"
        self.assertRaises(KeyError, t.children.__getitem__, "xform")
        self.assertTrue("meshy" in t.children.keys())
        self.assertEqual(t.children["meshy"], x)

        # rename property
        p = t.properties["prop"]
        p.name = "new"
        self.assertRaises(KeyError, t.properties.__getitem__, "prop")
        self.assertEqual(t.properties.values()[0].name, "new")
        self.assertEqual(t.properties["new"], p)

        # another rename test
        x = cask.Xform()
        t.children["foo"] = x
        self.assertTrue("foo" in t.children.keys())
        self.assertEqual(x.path(), "/foo")
        x.name = "bar"
        self.assertEqual(x.path(), "/bar")
        self.assertFalse("foo" in t.children.keys())
        self.assertTrue("bar" in t.children.keys())
        self.assertEqual(x.archive(), a)

        # child of child rename/reassign test
        baz = x.children["baz"] = cask.Xform()
        self.assertEqual(baz.path(), "/bar/baz")
        baz.name = "zap"
        self.assertFalse("zap" in t.children.keys())
        self.assertEqual(baz.path(), "/bar/zap")
        self.assertTrue("zap" in x.children.keys())
        self.assertEqual(baz.type(), "Xform")

        # reassign child obj to PolyMesh
        x.children["zap"] = cask.PolyMesh()
        self.assertEqual(x.children["zap"].type(), "PolyMesh")

        # write to a file
        a.write_to_file(filename)

    def test_equivalency(self):
        # test empty archive equivalency
        a = cask.Archive()
        b = a
        self.assertEqual(a, b)
        self.assertEqual(a.top.parent, a)
        self.assertEqual(a.top.archive(), a)

        # reassign 'b' to a new empty archive
        b = cask.Archive()
        self.assertNotEqual(a, b)

    def test_frame_range(self):
        filename = os.path.join(TEMPDIR, "cask_frame_range.abc")

        # create a new archive and some objects
        a = cask.Archive()
        xf = a.top.children["renderCamXform"] = cask.Xform()

        # set the start frame to 1001
        a.set_start_frame(1001)

        self.assertEqual(a.start_frame(), 1001)
        self.assertEqual(a.start_time(), 1001 / float(a.fps))

        # add some sample data
        for i in range(24):
            samp = alembic.AbcGeom.XformSample()
            samp.setTranslation(imath.V3d(i, 2.0, 3.0))
            xf.set_sample(samp)

        # export it
        a.write_to_file(filename)

    def test_deep_dict(self):
        filename = os.path.join(TEMPDIR, "cask_deep_dict.abc")

        # read in a simple scene with several shapes
        a = cask.Archive(deep_out())
        t = a.top

        # deep dict access
        self.assertEqual(t.children["A"].name, "A")
        self.assertEqual(t.children["A/B/C/D"].name, "D")
        self.assertEqual(t.children["A"].properties["a"].name, "a")
        self.assertEqual(t.children["A/B/C/D"].properties["a/b/c"].name, "c")
        self.assertRaises(KeyError, t.children.__getitem__, "A/B/C/Z")

        # property accessors
        x = t.children.values()[0]
        self.assertEqual(x.name, "A")
        self.assertEqual(x.properties["a/b/c/myprop"].values[0], "foo")

        # test deep set item on leaf node
        p = t.children["A/B/C/D/meshy"] = cask.PolyMesh()
        self.assertEqual(p.name, "meshy")
        self.assertEqual(p.type(), "PolyMesh")
        self.assertEqual(p.parent, t.children["A/B/C/D"])
        self.assertTrue("meshy" in t.children["A/B/C/D"].children.keys())
        self.assertEqual(len(p.children.keys()), 0)

        # another test
        patch = cask.NuPatch()
        t.children["A/B/C/D/patch"] = patch
        self.assertEqual(patch.name, "patch")
        self.assertEqual(patch.parent, t.children["A/B/C/D"])
        self.assertTrue("patch" in t.children["A/B/C/D"].children.keys())
        self.assertFalse("patch" in t.children.keys())
        self.assertEqual(len(patch.children.keys()), 0)

        # rename test
        patch.name = "nurby"
        self.assertFalse("patch" in t.children["A/B/C/D"].children.keys())
        self.assertTrue("nurby" in t.children["A/B/C/D"].children.keys())
        self.assertEqual(patch.parent, t.children["A/B/C/D"])
        self.assertEqual(t.children["A/B/C/D/nurby"], patch)
        self.assertRaises(KeyError, t.children.__getitem__, "A/B/C/D/patch")

        # test deep dict reassignment
        t.children["A/B/C/D/meshy"] = cask.Xform()
        p2 = t.children["A/B/C/D/meshy"]
        self.assertEqual(p2.name, "meshy")
        self.assertEqual(p2.parent, t.children["A/B/C/D"])
        self.assertFalse(p2.name in t.children.keys())
        self.assertEqual(len(p2.children.values()), 0)
        self.assertEqual(t.children["A/B/C/D/meshy"].type(), "Xform")
        self.assertEqual(p2.type(), "Xform")

        # test deep set item when middle node does not exist
        self.assertRaises(KeyError, t.children.__setitem__, "A/foo/C/D/bar", cask.Xform())

        # write the archive
        a.write_to_file(filename)

class Test2_Read(unittest.TestCase):
    def test_verify_write_basic(self):
        filename = os.path.join(TEMPDIR, "cask_write_basic.abc")
        self.assertTrue(cask.is_valid(filename))

        a = cask.Archive(filename)
        self.assertEqual(len(a.top.children), 1)
        self.assertEqual(a.top.children.values()[0].name, "foo")
        self.assertEqual(type(a.top.children.values()[0]), cask.Xform)
        self.assertEqual(len(a.top.children.values()[0].properties), 3)

        self.assertEqual(a.top.children.values()[0].properties["bar"].get_value(), "hello")
        self.assertEqual(a.top.children.values()[0].properties["baz"].get_value(), 42.0)

    def test_read_lights(self):
        filepath = lights_out()
        a = cask.Archive(filepath)
        self.assertEqual(a.path(), filepath)
        self.assertEqual(a.name, os.path.basename(filepath))

        # test the top node
        t = a.top
        self.assertEqual(type(t), cask.Top)
        self.assertEqual(t.name, "ABC")
        self.assertEqual(t.parent, a)
        self.assertEqual(t.path(), "/")
        self.assertEqual(t.archive(), a)

        # child accessors
        l1 = t.children["lightA"]
        self.assertEqual(type(l1.parent), cask.Top)
        self.assertEqual(l1.name, "lightA")
        self.assertEqual(l1.archive(), a)
        self.assertEqual(type(l1), cask.Light)

        # get next child
        l2 = t.children["lightB"]
        self.assertEqual(type(l2), cask.Light)
        self.assertEqual(l2.name, "lightB")
        self.assertEqual(l2.archive(), a)
        self.assertEqual(type(l2.parent), cask.Top)

        # find lights (w/ deferred schemification)
        self.assertEqual(len(cask.find(a.top, "light.*")), 2)

        # schema accessor (schemifies object on demand)
        self.assertEqual(type(l2.iobject), alembic.Abc.IObject)
        self.assertEqual(len(l2.properties[".geom/.camera/.core"].properties), 0)
        self.assertEqual(len(l2.properties["shader/prman.light.params"].properties), 2)
        self.assertEqual(len(l2.properties["shader/prman.light.params/exposure"].values), 11)
        self.assertEqual(len(l2.properties["shader/prman.light.params/specular"].values), 11)
        self.assertAlmostEqual(l2.properties["shader/prman.light.params/exposure"].values[0], 1.0)
        self.assertAlmostEqual(l2.properties["shader/prman.light.params/specular"].values[0], 0.1)
        self.assertEqual(type(l2.schema), alembic.AbcGeom.ILightSchema)
        self.assertEqual(type(l2.iobject), alembic.AbcGeom.ILight)

    def test_read_mesh(self):
        filepath = mesh_out()
        self.assertTrue(cask.is_valid(filepath))
        a = cask.Archive(filepath)
        t = a.top

    def test_paths(self):
        filepath = lights_out()
        a = cask.Archive(filepath)
        t = a.top

        # get some objects to test
        lightA = t.children["lightA"]
        lightB = t.children["lightB"]

        # test paths on objects
        self.assertEqual(a.path(), filepath)
        self.assertEqual(t.path(), "/")
        self.assertEqual(lightA.path(), "/lightA")
        self.assertEqual(lightB.path(), "/lightB")
        self.assertEqual(t.children[lightA.path()], lightA)

        # test paths on properties
        self.assertEqual(lightB.properties[".geom/.camera/.core"].path(),
                            "/lightB/.geom/.camera/.core")

        # test paths on empty archive
        a = cask.Archive()
        t = a.top
        x = t.children["x"] = cask.Xform()
        y = x.children["y"] = cask.Xform()
        z = y.children["z"] = cask.Xform()
        p = z.properties["p"] = cask.Property()
        self.assertEqual(a.path(), None)
        self.assertEqual(t.path(), "/")
        self.assertEqual(x.path(), "/x")
        self.assertEqual(y.path(), "/x/y")
        self.assertEqual(z.path(), "/x/y/z")
        self.assertEqual(p.path(), "/x/y/z/p")

        # test reparenting
        p.parent = x
        self.assertEqual(p.path(), "/x/p")

    def test_verify_extract_light(self):
        filename = os.path.join(TEMPDIR, "cask_exract_light.abc")
        self.assertTrue(cask.is_valid(filename))

        # open the archive
        a = cask.Archive(filename)
        self.assertEqual(len(a.top.children), 1)

        # verify the hierarchy
        l = a.top.children["lightB"]
        self.assertEqual(type(l), cask.Light)
        self.assertEqual(len(l.properties.keys()), 2)
        self.assertEqual(len(l.properties[".geom/.userProperties"].properties), 1)
        self.assertTrue("specular" in l.properties["shader/prman.light.params"].properties.keys())
        self.assertTrue("exposure" in l.properties["shader/prman.light.params"].properties.keys())

    def test_verify_copy_lights(self):
        filename = os.path.join(TEMPDIR, "cask_copy_lights.abc")
        self.assertTrue(cask.is_valid(filename))

        # open archive
        a = cask.Archive(filename)
        self.assertEqual(len(a.top.children), 2)

        # examine a lights properties
        lightA = a.top.children["lightA"]
        lightB = a.top.children["lightB"]
        self.assertEqual(type(lightA), cask.Light)
        self.assertEqual(type(lightB), cask.Light)

        self.assertEqual(len(lightA.properties), 1)
        self.assertEqual(len(lightB.properties), 2)
        self.assertAlmostEqual(lightB.properties["shader/prman.light.params/exposure"].get_value(), 1.0)
        self.assertAlmostEqual(lightB.properties["shader/prman.light.params/specular"].get_value(), 0.1)

    def test_verify_copy_mesh(self):
        filename = os.path.join(TEMPDIR, "cask_copy_mesh.abc")
        self.assertTrue(cask.is_valid(filename))

        # open the archive
        a = cask.Archive(filename)
        p = a.top.children.values()[0]

        # verify timesamplings were copied
        self.assertEqual(len(a.timesamplings), 1)

        # verify the hierarchy
        self.assertEqual(p.name, "meshy")

        # check the global matrix of the polymesh object
        self.assertEqual(p.global_matrix(),
                imath.M44d((1,0,0,0), (0,1,0,0), (0,0,1,0), (0,0,0,1)))

        geom = p.properties[".geom"]
        self.assertEqual(len(geom.properties[".faceCounts"].values), 10)
        self.assertEqual(geom.properties[".faceCounts"].values[0][0], 4)
        self.assertEqual(geom.properties["P"].values[0][0], imath.V3f(-1, -1, -1))
        self.assertEqual(geom.properties["N"].values[0][0], imath.V3f(-1, 0, 0))

    def test_verify_write_geom(self):
        filename = os.path.join(TEMPDIR, "cask_write_geom.abc")
        self.assertTrue(os.path.isfile(filename))

        # open the test archive
        a = cask.Archive(filename)

        # verify the object names and geom classes are correct
        self.assertEqual(type(a.top.children["xform"]), cask.Xform)
        self.assertEqual(type(a.top.children["polymesh"]), cask.PolyMesh)
        self.assertEqual(type(a.top.children["subd"]), cask.SubD)
        self.assertEqual(type(a.top.children["faceset"]), cask.FaceSet)
        self.assertEqual(type(a.top.children["curve"]), cask.Curve)
        self.assertEqual(type(a.top.children["camera"]), cask.Camera)
        self.assertEqual(type(a.top.children["nupatch"]), cask.NuPatch)
        self.assertEqual(type(a.top.children["material"]), cask.Material)
        self.assertEqual(type(a.top.children["light"]), cask.Light)
        self.assertEqual(type(a.top.children["points"]), cask.Points)

    def test_verify_write_mesh(self):
        filename = os.path.join(TEMPDIR, "cask_write_mesh.abc")
        self.assertTrue(cask.is_valid(filename))

        # get the objects
        a = cask.Archive(filename)
        x = a.top.children.values()[0]
        p = x.children.values()[0]

        # verify the hierarchy
        self.assertEqual(x.name, "foo")
        self.assertEqual(type(x), cask.Xform)
        self.assertEqual(p.name, "meshy")
        self.assertEqual(type(p), cask.PolyMesh)

        # check one of the properties
        vals = x.properties.values()[0].properties[".vals"]
        self.assertEqual(vals.values[0], imath.V3d(1, 2, 3))

    def test_verify_selective_update(self):
        filename = os.path.join(TEMPDIR, "cask_selective_update.abc")
        self.assertTrue(cask.is_valid(filename))

        # verify our name change
        a = cask.Archive(filename)
        self.assertTrue("lightC" in a.top.children.keys())
        l = a.top.children["lightC"]

        # verify our translate update
        p = l.properties["shader/prman.light.params/exposure"]
        self.assertAlmostEqual(p.values[0], 1.0)
        self.assertAlmostEqual(p.values[5], 0.5)

    def test_verify_insert_node(self):
        filename = os.path.join(TEMPDIR, "cask_insert_node.abc")
        self.assertTrue(cask.is_valid(filename))

        # get some objects
        a = cask.Archive(filename)
        r = a.top.children.values()[0]
        m = r.children.values()[0]

        # verify re-parenting
        self.assertEqual(r.name, "root")
        self.assertEqual(type(r), cask.Xform)
        self.assertEqual(m.name, "meshy")
        self.assertEqual(type(m), cask.PolyMesh)
        self.assertEqual(len(m.properties[".geom"].properties), 7)

    def test_verify_frame_range(self):
        filename = os.path.join(TEMPDIR, "cask_frame_range.abc")
        self.assertTrue(cask.is_valid(filename))

        a = cask.Archive(filename)

        # verify the frame range
        self.assertEqual(a.start_time(), 1001 / float(a.fps))
        self.assertEqual(a.start_frame(), 1001)
        self.assertEqual(a.end_frame(), 1024)

    def test_verify_deep_dict(self):
        filename = os.path.join(TEMPDIR, "cask_deep_dict.abc")
        self.assertTrue(cask.is_valid(filename))

        # get some objects
        a = cask.Archive(filename)
        t = a.top
        d = t.children["A/B/C/D"]

        # verify writes
        self.assertTrue("meshy" in d.children.keys())
        self.assertTrue("nurby" in d.children.keys())
        self.assertEqual(type(d.children["meshy"]), cask.Xform)
        self.assertEqual(type(d.children["nurby"]), cask.NuPatch)

class Test3_Issues(unittest.TestCase):
    def test_issue_318(self):
        filename = "cask_test_issue_318.abc"

        # create a test file
        test_file_1 = mesh_out(filename)

        # access an object in the file, then close it
        a = cask.Archive(test_file_1)
        self.assertEqual(a.top.children.keys(), ['meshy'])
        a.close()

        # try to write to the same file path
        #  the error that's being tested for is this:
        #   hdf5-1.8.9/src/H5F.c line 1255 in H5F_open():
        #   unable to truncate a file which is already open
        test_file_2 = mesh_out(filename, force=True)

        self.assertEqual(test_file_1, test_file_2)

    def test_issue_345(self):
        filename_1 = "cask_test_issue_345_1.abc"
        filename_2 = "cask_test_issue_345_2.abc"
        filename_3 = "cask_test_issue_345_3.abc"

        test_file_1 = mesh_out(filename_1)
        test_file_2 = os.path.join(TEMPDIR, filename_2)
        test_file_3 = os.path.join(TEMPDIR, filename_3)
        test_file_geom = os.path.join(TEMPDIR, "cask_write_geom.abc")

        # round trip test some metadata
        a = cask.Archive(test_file_1)
        childBnds = a.top.children["meshy"].properties[".geom/.childBnds"]
        uvs = a.top.children["meshy"].properties[".geom/uv"]
        self.assertEqual(uvs.metadata.get("geoScope"), "fvr")
        self.assertEqual(childBnds.metadata.get("interpretation"), "box")
        a.write_to_file(test_file_2)
        a.close()
        
        a = cask.Archive(test_file_2)
        childBnds = a.top.children["meshy"].properties[".geom/.childBnds"]
        uvs = a.top.children["meshy"].properties[".geom/uv"]
        self.assertEqual(uvs.metadata.get("geoScope"), "fvr")
        self.assertEqual(childBnds.metadata.get("interpretation"), "box")
        a.close()

        # round trip test some pod values
        a = cask.Archive(test_file_geom)
        points = a.top.children.get("points")
        curve = a.top.children.get("curve")
        faceset = a.top.children.get("faceset")
        polymesh = a.top.children.get("polymesh")
        pod1 = points.properties[".geom/P"].pod()
        pod2 = points.properties[".geom/.pointIds"].pod()
        pod3 = curve.properties[".geom/curveBasisAndType"].pod()
        pod4 = faceset.properties[".faceset"].pod()
        pod5 = polymesh.properties[".geom/.selfBnds"].pod()
        a.write_to_file(test_file_3)
        a.close()

        a = cask.Archive(test_file_3)
        points = a.top.children.get("points")
        curve = a.top.children.get("curve")
        faceset = a.top.children.get("faceset")
        polymesh = a.top.children.get("polymesh")
        self.assertEqual(points.properties[".geom/P"].pod(), pod1)
        self.assertEqual(points.properties[".geom/.pointIds"].pod(), pod2)
        self.assertEqual(curve.properties[".geom/curveBasisAndType"].pod(), pod3)
        self.assertEqual(faceset.properties[".faceset"].pod(), pod4)
        self.assertEqual(polymesh.properties[".geom/.selfBnds"].pod(), pod5)
        a.close()

    def test_issue_346(self):
        filename_1 = "cask_test_issue_346_1.abc"
        filename_2 = "cask_test_issue_346_2.abc"

        # create a test file with 1 time sampling object
        test_file_1 = mesh_out(filename_1)
        test_file_2 = os.path.join(TEMPDIR, filename_2)

        a = cask.Archive(test_file_1)
        a.write_to_file(test_file_2)
        b = cask.Archive(test_file_2)

        # compare test1 and test2
        self.assertEqual(len(a.timesamplings), len(b.timesamplings))
        self.assertEqual(a.time_range(), b.time_range())
        tst_1 = a.timesamplings[0].getTimeSamplingType()
        tst_2 = b.timesamplings[0].getTimeSamplingType()
        self.assertEqual(str(tst_1), str(tst_2))

        filename_3 = "cask_test_issue_346_3.abc"
        filename_4 = "cask_test_issue_346_4.abc"

        # create another test with 2 time sampling objects
        test_file_3 = cube_out(filename_3)
        test_file_4 = os.path.join(TEMPDIR, filename_4)

        c = cask.Archive(test_file_3)
        c.write_to_file(test_file_4)
        d = cask.Archive(test_file_4)

        # compare test3 and test4
        self.assertEqual(len(c.timesamplings), len(d.timesamplings))
        self.assertEqual(c.time_range(), d.time_range())
        tst_3 = c.timesamplings[0].getTimeSamplingType()
        tst_4 = d.timesamplings[0].getTimeSamplingType()
        self.assertEqual(str(tst_3), str(tst_4))

    def test_issue_349(self):
        test_file = os.path.join(TEMPDIR, "cask_test_issue_349.abc")

        # create a new archive and some objects
        a = cask.Archive()
        xf = a.top.children["renderCamXform"] = cask.Xform()
        cam = xf.children["renderCamShape"] = cask.Camera()

        # add some sample data
        for i in range(24):
            samp = alembic.AbcGeom.XformSample()
            samp.setTranslation(imath.V3d(i, 2.0, 3.0))
            xf.set_sample(samp)

        # export it
        a.write_to_file(test_file)
        a.close()

        # read the test archive back in and verify results
        a = cask.Archive(test_file)
        xform = a.top.children["renderCamXform"]
        self.assertEqual(len(a.timesamplings), 2)
        self.assertEqual(xform.time_sampling_id, 1)
        self.assertEqual(len(xform.samples), 24)
        self.assertEqual(a.start_frame(), 0)
        self.assertEqual(a.end_frame(), 23)

if __name__ == '__main__':
    unittest.main()
