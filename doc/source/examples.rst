.. PyAlemic documentation master file, created by
   sphinx-quickstart on Tue May  1 11:15:16 2012.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Introduction
============

The Alembic system is intended to facilitate baked (cached) geometry workflows and 
platform-independent geometry interchange and sharing. It accomplishes this by being, 
at its core, a library for efficiently storing samples of hierarchically related typed 
data. Layered on top of that is a system for interpreting that data as geometry or geometric 
transforms, but it’s important to keep in mind the primacy of data management at its center. 

Major Concepts
--------------

Alembic has a completely separate but parallel class hierarchy for input and output. This
maintains the important abstraction that Alembic is for storage, representation, and archival
as opposed to being a dynamic scene manipulation framework.

An alembic Archive consists of multiple Objects, which may or may not have Properties, in a 
hierarchical parent/child tree that forms an acyclic directed graph. Data are written as Samples 
at specific times, and are read back similarly. Interpolation between these samples is not the 
function of Alembic and, if required, must be done by the plug-in that uses Alembic. However, 
Alembic provides a rich interface for retrieving multiple Samples based on time through the 
TimeSampling class, defined in AbcCoreAbstract. Using that, client code may get all the required 
data for whatever interpolation is desired.

Importing
---------

Importing Alembic is simply: ::

    >>> import alembic

.. note::

    Make sure that PyImath is in your PYTHONPATH, and Boost::Python in LD_LIBRARY_PATH.


Namespaces
----------

Alembic itself is composed of several layers of libraries, each of which builds extensively 
on the layer below it. The lowest level library is an abstract interface layer called
:py:mod:`.AbcCoreAbstract` which all the other user-facing layers are built.

.. toctree::
   :maxdepth: 2

   alembic/index


Setting & Getting Data
======================

We'll start by targeting the thing you'd most often want to do - write and read
animated, geometric primitives. To do this, we will be using two main
modules: :doc:`alembic.Abc </alembic/abc>`, which provides the basic Alembic Abstractions,
and :doc:`alembic.AbcGeom </alembic/abcg>`, which implements specific Geometric primitives
on top of alembic.Abc (this example is taken from ``python/PyAlembic/Tests/testPolyMesh.py``).


Archives
--------

An *Archive* is Alembic's term for the actual file on disk and is the top-level container 
with all of the scene data. Archives contain Objects.

Our sample output Archive will contain a single animated Transform with a single static PolyMesh 
as its child. Because we're creating an :py:class:`.OArchive`, this will create (or clobber) 
the archive file with this filename: ::

    >>> oarch = OArchive('polyMesh1.abc')


Objects
-------

An Alembic Object is the main unit of hierarchy in Alembic. You can think of an Alembic Archive 
as a typical UNIX-style filesystem (eg, ext2fs), and of Alembic Objects as directories in that 
filesystem. The analogy is not perfect, as will be seen shortly, but it’s not bad. Objects don’t 
contain data directly, but instead provide structure for filling with more directly-data-containing 
entities. Alembic Objects are the primary unit of encapsulation. Objects contain Properties.

A Schema is a minimal set of expected properties which implement some complex Object, such as a PolyMesh. 
In the simplest, standard case, there will be a compound property at the top with a certain name, and 
inside the compound property will be some number of additional properties that implement the Object. 
In the case of a PolyMesh, these properties would include a list of vertices (a V3fArray), a list of 
indices (an IntArray), and a list of "per-face counts" (more on this later).

**Create a PolyMesh class object**

An :py:class:`.OPolyMesh` is-an OObject that has-an 
:py:class:`.OPolyMeshSchema`. An OPolyMeshSchema is-an :py:class:`.OCompoundProperty`. In this case, 
we're parenting the PolyMesh under the Archive's top node and naming it "meshy". ::

    >>> meshObj = OPolyMesh(oarch.getTop(), "meshy")
    >>> mesh = meshyObj.getSchema()

Remember that an IPolyMesh is an IObject, and an IPolyMeshSchema *is an* ICompoundProperty (and an IPolyMesh 
*has an* IPolyMeshSchema). So, just like any other Object in Alembic, it can be constructed with its parent 
Object and its name. Every Object in Alembic is the child of some other Object, EXCEPT for the TopObject, 
which is the lone direct child of the Archive.


Properties
----------

Data in Alembic is stored in Samples, which are then stored in Properties.

There are two types of Alembic properties, Simple and Compound. A CompoundProperty is used to hold properties.
It is the main type of container you’ll be working with. Simple Properties hold samples and can be either 
Scalar or Array. These samples are what actually holds your data.

The major semantic difference between scalar and array properties is that to say that a Property is Scalar 
is to say, "There is one single value per Sample for this Property". Because of this, it is meaningless to 
talk about a Scalar Property changing topology (eg, mass will be one floating point value; it won't sometimes 
be two floating point values); array properties have no such restriction. Furthermore, array properties are 
cached and indexed in such a way that repeatedly storing identical samples will not store multiple copies of 
the data. Scalar properties do not cache in this way.

**Scalar Properties**

An Alembic ScalarProperty is a Simple Property that contains Samples whose type and number of elements 
(the extent) is fixed and known prior to writing. Examples of ScalarProperties are FloatProperty (each Sample 
is a 32-bit floating point number; extent = 1), StringProperty (each sample is a single string, of whatever 
size; extent = 1), or an M44dProperty (each Sample is sixteen 64-bit floating point numbers; extent = 16). 
The maximum extent for a ScalarProperty is 256.

Examples of ScalarProperties:

    * The mass of a rigid body (extent=1, a single numerical value)
    * A color (for an RGB vector, extent=3, three floating point values)
    * A bounding box (extent=6, two three-vectors of floating point values)

**Array Properties**

An Alembic ArrayProperty is a Simple Property that contains Samples whose type is fixed and known prior to 
writing, but whose extent is variable. Examples of ArrayProperties are DoubleArrayProperty (each Sample is 
an array of varying length, each array element being a single 64-bit floating point number), V3fArrayProperty 
(each Sample is an array of varying length, and each element in the array being an Imath::Vec3f, which is 
three 32-bit floating point numbers), or M44fArrayProperty (each Sample is an array of varying length, and each 
array element is an Imath::M44f, or sixteen 32-bit floating point numbers). 

Examples of Array Properties:

    * A list of the vertices of a polygonal mesh
    * A list of particles in a fluid simulation

**Compound Properties**

Compound properties are a special type of property in that they are not properties per se but rather containers 
for other properties (compound properties included). If an object has any properties, it must have a compound 
property which contains all other properties.


Samples
-------

An Alembic Sample is the container that composes raw data and a time into a single entity. As with Properties, 
Samples are either Scalar or Array. 

UVs and Normals use GeomParams, which can be written or read as indexed or not, as you'd like. 
The typed GeomParams have an inner Sample class that is used for setting and getting data. GeomParams are typed
according to the type of data they will contain, for example the :py:class:`.OV2fGeomParamSample` is
used to store V2f data, such as UVs. Continuing our example: ::

    >>> uvsamp = OV2fGeomParamSample(uvs, kFacevaryingScope) 

Normals have their own type. ::

    >>> nsamp  = ON3fGeomParamSample(normals, kFacevaryingScope)

Create a :py:class:`.OPolyMeshSchemaSample` Sample. We're creating the Sample inline here, but we could 
create a static sample and leave it around, only modifying the parts that have changed. The first sample
should contain at least all of the required data for a PolyMesh. :: 

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

Time Sampling
^^^^^^^^^^^^^

Alembic files consist of a series of Samples of Properties at different times. Every property, therefore, 
has a notion of when it has been 'sampled' (both during write and during read). There are four types of 
time sampling that Alembic supports:

**Uniform**

One Sample per dt where the dt is defined during construction. This is probably the most common 
time sampling type; specifically properties that are sampled every 24th of a second.

**Identity**

Identity time sampling is the default, and is merely a special case of uniform sampling where 
for each sample, i, the time of that sample is i.

**Cyclic**

Some finite number of Samples per dt; for example, shutter open and shutter close.

**Acyclic**

Sample times are arbitrary and follow no particular cycle. This is the least common case and will 
cause the most amount of data to be stored in the Alembic file (explicit values for each sample 
time must be stored, for every acyclic property). 

.. note:: All Samples must be written in strictly increasing temporal order. 


Reading an Archive
------------------

Read an Alembic archive, also referred to as an :py:class:`.IArchive`. ::

    >>> iarch = IArchive('polyMesh1.abc')
    >>> print "Reading", iarch.getName()
    Reading polyMesh1.abc


Hierarchy Navigation
--------------------

Because of its intention to store Samples of scene graphs, Alembic has a notion of parent/child hierarchy. 
At the apex of this hierarchy is the Archive, which can be treated as an Object. Beneath this top-level 
Archive can be many child Objects, each of which can be the parent to any number of other Objects, and so on. 
Ancestry is acyclic -- a descendant of an object cannot be the ancestor of that same object.

As mentioned above, If an object contains any Properties, it contains them within a top-level Compound Property. 
A Compound Property can have any number of child properties of any time (scalar, array or compound). Again, 
this relationship is acyclic. 

Usually, we want to start by getting the top node of the scene hierarchy, which will always be
called *ABC*. So, in our example: ::

    >>> top = iarch.getTop()
    >>> top.getName()
    'ABC'

All scene data is parented under this top node. ::

    >>> meshObj = IPolyMesh(top, "meshy")
    >>> mesh = meshObj.getSchema()
    >>> N = mesh.getNormalsParam()
    >>> uv = mesh.getUVsParam()


.. hint::
    If you don't know the object type of the input data, you can check the Object MetaData
    and match it to a specific type. ::

    >>> obj = top.children[0]
    >>> if IPolyMeshSchema.matches(obj):
    >>> ... meshObj = IPolyMesh(obj, KWrapExisting)

``N`` and ``uv`` are GeomParams, which can be stored as indexed or not. ::

    >>> assert not N.isIndexed()
    >>> assert not uv.isIndexed()

Get the Samples. ::

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

:py:func:`getExpandedValue()` takes an optional ISampleSelector, and returns an typed
:py:class:`.IGeomParamSample`.  :py:func:`.IGeomParamSample.getVals()` returns an 
TypedArraySamplePtr. ::

    >>> nsp = N.getExpandedValue().getVals()

GeomParams and Properties have a method, :py:func:`.isConstant()`, that returns if there are fewer than 
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
            x = IXform(obj, kWrapExisting)
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
            mesh = IPolyMesh(obj, kWrapExisting)
            ms = mesh.getSchema()
            positions = ms.getValue().getPositions()
            numPoints = len(positions)
            for i in range(numPoints):
                bnds.extendBy(imath.V3d(positions[i]))
        bnds.extendBy(transform(bnds, xf))
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

