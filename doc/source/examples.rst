.. PyAlemic documentation master file, created by
   sphinx-quickstart on Tue May  1 11:15:16 2012.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Introduction
============

Alembic has a completely separate but parallel class hierarchy for input and output. This
maintains the important abstraction that Alembic is for storage, representation, and archival
as opposed to being a dynamic scene manipulation framework.

Importing Alembic is simply: ::

    >>> import alembic

.. note::

    Make sure that PyImath is in your PYTHONPATH, and Boost::Python in LD_LIBRARY_PATH.


Namespaces
----------

Each module has a namespace which is similar to the name of the module itself, but
shortened for brevity.

.. toctree::
   :maxdepth: 2

   alembic/index


Setting & Getting Data
======================

We'll start by targeting the thing you'd most often want to do - write and read
animated, geometric primitives. To do this, we will be using two main
modules: :doc:`alembic.Abc </alembic/abc>`, which provides the basic Alembic Abstractions,
and :doc:`alembic.AbcG </alembic/abcg>`, which implements specific Geometric primitives
on top of alembic.Abc (this example is taken from python/PyAlembic/Tests/testPolyMesh.py).


Archives
--------

An "Archive" is Alembic's term for the actual file on disk containing all of the scene
data.

Our sample output Archive will contain a single animated Transform with a single static PolyMesh 
as its child. Because we're creating an :py:class:`.OArchive`, this is creating (or clobbering) 
the archive file with this filename: ::

    >>> oarch = OArchive('polyMesh1.abc')


Objects
-------

.. TODO: insert something about objects and schemas here.

Create a PolyMesh class object. An :py:class:`.OPolyMesh` is-an OObject that has-an 
:py:class:`.OPolyMeshSchema`. An OPolyMeshSchema is-an :py:class:`.OCompoundProperty`. In this case, 
we're parenting the PolyMesh under the Archive's top node and naming it "meshy". ::

    >>> meshObj = OPolyMesh(oarch.getTop(), "meshy")
    >>> mesh = meshyObj.getSchema()


Samples
-------

UVs and Normals use GeomParams, which can be written or read as indexed or not, as you'd like. 
The typed GeomParams have an inner Sample class that is used for setting and getting data. ::

    >>> uvsamp = OV2fGeomParamSample(uvs, kFacevaryingScope) 

Indexed normals: ::

    >>> nsamp  = ON3fGeomParamSample(normals, kFacevaryingScope)

Create a :py:class:`.OPolyMeshSchemaSample` sample. We're creating the sample inline here, but we could 
create a static sample and leave it around, only modifying the parts that have changed. :: 

    >>> mesh_samp = OPolyMeshSchemaSample(verts, indices, counts, uvsamp, nsamp)

Make up some bounding box data and set it on the sample. ::

    >>> cbox = imath.Box3d()
    >>> cbox.extendBy(imath.V3d(1.0, -1.0, 0.0))
    >>> cbox.extendBy(imath.V3d(-1.0, 1.0, 3.0))
    >>> mesh_samp.setChildBounds(cbox)

Set the sample twice. Because the data is the same in both samples, Alembic will store only one copy, 
but note that two samples have been set. ::

    >>> mesh.set(mesh_samp);
    >>> mesh.set(mesh_samp);

Alembic objects close themselves automatically when they go out of scope, so - we don't have to do
anything to write them to disk. 


Reading an Archive
------------------

Read an Alembic archive, also referred to as an :py:class:`.IArchive`. ::

    >>> iarch = IArchive('polyMesh1.abc')
    >>> print "Reading", iarch.getName()
    Reading polyMesh1.abc


Hierarchy Navigation
--------------------

Usually, we want to start by getting the top node of the scene hierarchy, which
will always be called *ABC*: ::

    >>> top = iarch.getTop()
    >>> top.getName()
    'ABC'

All scene data is parented under this top node. ::

    >>> meshObj = IPolyMesh(top, "meshy")
    >>> mesh = meshObj.getSchema()
    >>> N = mesh.getNormalsParam()
    >>> uv = mesh.getUVsParam()

N and uv are GeomParams, which can be stored as indexed or not. ::

    >>> assert not N.isIndexed()
    >>> assert not uv.isIndexed()

Get the samples. ::

    >>> meshSamp = mesh.getValue()

Check the self bounding box. ::

    >>> assert meshSamp.getSelfBounds().min() == imath.V3d(-1.0, -1.0, -1.0)
    >>> assert meshSamp.getSelfBounds().max() == imath.V3d( 1.0,  1.0,  1.0)

"ArbGeomParams" are a container that should only contain GeomParams. We didn't set 
any on write, so on read, it should be an invalid container. Invalid Properties of 
any type evaluate as boolean False; valid Properties, even if they are empty, will 
evaluate as True. ::

    >>> arbattrs = mesh.getArbGeomParams()
    >>> assert not arbattrs

:py:func:`getExpandedValue()` takes an optional ISampleSelector, and returns an
:py:class:`.IGeomParamSample`.  IGeomParamSample.getVals() returns an TypedArraySamplePtr. ::

    >>> nsp = N.getExpandedValue().getVals()

GeomParams and Properties have a method, isConstant(), that returns if there are fewer than 
two unique samples in them. ::

    >>> assert N.isConstant()
    >>> assert rv.isConstant()

    >>> for i in range( len( nsp ) ):
    >>>     assert nsp[i] == normals[i]

    >>> uvsamp = uv.getIndexedValue()

    >>> assert uvsamp.getIndices()[1] == 1
    >>> uv2 = uvsamp.getVals()[2]

    >>> assert uv2 == V2f(1.0, 1.0)

    >>> positions = meshSamp.getPositions()
    >>> for i in range(len(positions)):
    >>>     assert positions[i] == verts[i]


Finding a Mesh By Name
======================

In this example, we'll crawl through an IArchive, and find a mesh by a particular name. This example is 
adapted from the previous example and we're only bothering to match against the
:py:class:`.IPolyMesh` and :py:class:`.ISubD` object classes. 

::

    def visitObject(obj, name):
        md = obj.getMetaData()
        if IPolyMesh.matches(md) or ISubD.matches(md):
            if obj.getName() == name:
                print "Found it!", obj.getFullName()
                return
        for childObject in obj.children:
            visitObject(childObject, name)

    arch = IArchive("octopus.abc")
    visitObject(arch.getTop(), "octopus_lowShape")


Accumulating a Transform
========================

In this example, given a node in an Alembic Archive, we'll figure out what the final xform is. We 
proceed from the leaf to the root. We're going to change :py:func:`visitObject()` slightly. Instead of checking the object's 
name, we're going to call a new function, :py:func:`getBounds()`. This example is adapted from

http://code.google.com/p/alembic/wiki/CookingWithAlembic#Accumulating_a_Transform

::
    
    gBounds = imath.Box3d()
    gBounds.makeEmpty()
    kWrapExisting = alembic.Abc.WrapExistingFlag.kWrapExisting

    def accumXform(xf, obj):
        if IXform.matches(obj.getHeader()):
            x = IXform(obj.getParent(), obj.getName())
            xs = x.getSchema().getValue()
            xf *= xs.getMatrix()

    def getFinalMatrix(obj):
        xf = imath.M44d()
        xf.makeIdentity()
        parent = obj.getParent()
        while parent:
            accumXform(xf, parent)
            parent = parent.getParent()
        return xf

    def getBounds(obj):
        bnds = imath.Box3d()
        bnds.makeEmpty()
        md = obj.getMetaData()
        if IPolyMesh.matches(md) or ISubD.matches(md):
            mesh = IPolyMesh(obj.getParent(), obj.getName())
            ms = mesh.getSchema()
            positions = ms.getValue().getPositions()
            numPoints = len(positions)
            for i in range(numPoints):
                bnds.extendBy(imath.V3d(positions[i]))
        bnds.extendBy(imath.transform(bnds, xf))
        gBounds.extendBy(bnds)

    def visitObject(obj):
        md = obj.getMetaData()
        if IPolyMesh.matches(md) or ISubD.matches(md):
            getBounds(obj)
        for childObject in obj.children:
            visitObject(childObject)


Write Non-Standard Data for a PolyMesh
======================================

In this example, we'll store some non-standard data in a PolyMesh. This would only be supported by custom
workflows. Here we need an :py:class:`.OCompoundProperty` to use as the parent of our user Property.

::

    archive = OArchive("crazyPolyMesh1.abc")

    meshObj = OPolyMesh(archive.getTop(), "crazy")
    mesh = meshObj.getSchema()

    myCrazyDataContainer = mesh.getUserProperties()
    mass = ODoubleProperty(myCrazyDataContainer, "mass")
    mesh_samp = OPolyMeshSchemaSample(
            OP3fGeomParamSample(verts, numVerts),
            OInt32GeomParamSample(indices, numIndices),
            OInt32GeomParamSample(counts, numCounts)
        )

    mesh.set(mesh_samp)
    mass.set(2.0)

