:mod:`cask` -- Alembic Python Convenience Wrapper
=================================================

.. moduleauthor:: Ryan Galloway <ryang@ilm.com>

Cask is a high level convenience wrapper for the Alembic Python API. It blurs
the lines between Alembic "I" and "O" objects and properties, abstracting both
into a single class object. It also wraps up a number of lower-level functions
into high level convenience methods. 

Advantages
----------

Arguably one of the biggest conveniences is that cask does all the OObject and
OProperty handle bookkeeping for you, keeping these objects in scope until you
call an explicit "save()" method. Some of the other advantages are listed below.

    * Abstraction layer for I and O object types,
    * Automatically schemify objects on read,
    * Deferred writing / cached reading,
    * Auto convert python data types to alembic data types,
    * Deep dictionary access for objects and properties.

Tutorial
--------

Since cask abstracts "I" and "O" objects, creating a new archive or reading an
existing one from disk uses the same Archive class. ::

    >>> # read an archive from disk
    >>> a = cask.Archive("lights.abc")

    >>> # creates a new empty archive
    >>> b = Archive()

Both objects have internal "iobject" and "oobject" properties that point to the
corresponding Alembic "I" and "O" objects. For "a", there will be an internal 
"iobject" property that points to the IArchive, and for "b" that property returns 
None. In both cases, accessing the Top object is easy, it's just ::

    >>> a.top
    <Top "ABC">

A lot of the accessors in `cask` are properties vs. methods, like `properties`, and
`children`, both with return dictionary-like objects. Walking and modifying the 
archive's hierarchy is as simple as accessing/manipulating these dict-like objects ::

    >>> def walk(obj):
    ...     print obj.name, obj.type()
    ...     for child in obj.children.values():
    ...         walk(child)
    ...
    >>> walk(a.top)

    >>> # add a new xform under the top node
    >>> b.top.children["foo"] = cask.Xform()

Navigation
~~~~~~~~~~

Deep dictionary access and (deferred) auto-schemification are available on the 
dict-like `children` and `properties` accessors. ::

    >>> l = a.top.children["root/world/lgt/gaffer/point/pointShape"]
    >>> l
    <Light "pointShape">

I say deferred because while the the
correct high-level cask class is returned for each object, the underlying
Alembic object is still an IObject until we need to wrap it in its subclass
and/or access its schema class. ::

    >>> l.iobject
    <alembic.Abc.IObject object at 0x1016830>
    >>> l.schema.getNumSamples() # access the schema
    6
    >>> l.iobject
    <alembic.AbcGeom.ILight object at 0x1047838>
    >>> l.schema
    <alembic.AbcGeom.ILightSchema object at 0xfe2670>

To access or create properties, use the `properties` accessor. For example, to
access the value of the `diffuse` light shader property on our light, `l`: ::

    >>> l.properties
    {'.geom': <Property ".geom">, 'shader': <Property "shader">}
    >>> l.properties["shader/arnold.light.params/diffuse"].values[0]
    1.0

The "old" way of doing this via the Alembic Python API would require many additional
steps after finding the light object, including matching and wrapping the IObject in 
the alembic.AbcGeom.ILight class, accessing the schema, then navigating to the property 
through the a series of property accessors. Cask makes this much more straight-forward
and user-friendly. 

Hierarchy Manipulation
~~~~~~~~~~~~~~~~~~~~~~

Adding a new Object or Property is as simple as adding an object to a dictionary. ::

    >>> a.top.children["myObject"] = cask.Xform()
    >>> l.properties["myProperty"] = cask.Property()

Or you can set the `parent` attribute on any object: ::

    >>> x = cask.Xform(name="foo")
    >>> x.parent = a.top

Cask makes it trivial to extract an object by copying it from one archive to a new one: ::

    >>> l.parent = b.top
    >>> b.write_to_file("/var/tmp/point_light.abc")

Check the output: ::

    > abctree /var/tmp/point_light.abc
    ABC
    `-- pointShape

Inserting nodes is also easy: ::

    >>> a = cask.Archive("animatedcube.abc")
    >>> r = cask.Xform()
    >>> x = a.top.children["cube1"]
    >>> a.top.children["root"] = r
    >>> r.children["cube1"] = x
    >>> a.write_to_file("/var/tmp/cask_insert_node.abc")

Check the results: ::

    > abctree /var/tmp/cask_insert_node.abc
    ABC
     `-- root
         `-- cube1
             `-- cube1Shape


Setting Values
~~~~~~~~~~~~~~

Cask automatically converts Python data types to the appropriate Alembic typed property, or
imath type using an internal map. There is no need to know which typed OProperty class to use,
or potentially even which imath type you need as an argument to that property. 

Currently, you can either set a value on a property directly, or create an
Alembic sample object, set values on that, and then set the sample on the cask object. 

Setting values directly on properties: ::

    >>> a = cask.Archive("octopus.abc")
    >>> a.top.children.values()[0].name = "squid_low"
    >>> x = a.top.children.values()[0].properties[".xform/.vals"]
    >>> t = imath.M33d((50, 0, 0), (0, -0, -90), (1, 1, 1))
    >>> for i in range(len(x.values)):
    ...     x.set_value(t, index=i)

Setting values on sample objects: ::

    >>> uvsamp = alembic.AbcGeom.OV2fGeomParamSample(meshData.uvs, kFacevaryingScope)
    >>> nsamp = alembic.AbcGeom.ON3fGeomParamSample(meshData.normals, kFacevaryingScope)
    >>> s = alembic.AbcGeom.OPolyMeshSchemaSample(meshData.verts, meshData.indices,
                   meshData.counts, uvsamp, nsamp)
    >>> p.set_sample(s)

A third way to set values is using cask wrapped methods on objects. Currently, there is
only one method implemented, the `setScale` method on the `cask.Xform` class. These methods
create an internal sample object and cache it, setting it on the object when `save()` is
called. If this seems like the best, most convenient way of setting values, then we can 
implement more of these. ::

    >>> x = cask.Xform()
    >>> x.set_scale(imath.V3d(1, 2, 3))

Caveat: Setting samples on the object will currently only work for new, empty archives, not
archives that have been read in from disk. This is a known limitation and on the to-do list
to resolve. 


Module Contents
---------------

.. automodule:: cask
   :members: 

