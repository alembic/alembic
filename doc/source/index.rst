:mod:`alembic` -- Alembic Python Bindings Documentation
=======================================================

Alembic is an open source geometry caching format that promotes interoperability
between authoring tools. From `alembic.io <http://alembic.io>`_:

    *Alembic distills complex, animated scenes into a non-procedural, application-
    independent set of baked geometric results. This ‘distillation' of scenes into 
    baked geometry is exactly analogous to the distillation of lighting and rendering 
    scenes into rendered image data.*

Alembic is focused on efficiently storing the computed results of complex procedural 
geometric constructions. It is specifically NOT concerned with storing the complex 
dependency graph of procedural tools used to create the computed results. For example, 
Alembic will efficiently store the animated vertex positions and animated transforms 
that result from an arbitrarily complex animation and simulation process, but will not
attempt to store a representation of the network of computations (rigs, basically) which
were required to produce the final, animated vertex positions and animated transforms.

This document is intended to serve as a brief introduction and  programmer's guide to
using the Alembic Python bindings. 

Get Alembic
-----------

The Alembic code can be found in a Mercurial repository at 
http://code.google.com/p/alembic.

These docs were built by `sphinx <http://sphinx.pocoo.org>`_. If you built the Python
bindings using the provided bootstrap with the `--enable-pyalembic` flag, then building
these docs is straight-forward. From the build root: ::

    $ make docs

The source code for these docs can be found in the directory `docs/source`.

Getting Started
---------------

The Alembic Python bindings have been tested with 
`Python 2.6 <http://www.python.org/download/releases/2.6.8/>`_, 
and require `PyImath <http://github.com/openexr/openexr>`_ and 
`Boost::Python <http://boost.org>`_ libraries to run. 
In order to use Alembic in Python, you must first build both Alembic and the Alembic 
Python bindings using the bootstrap script: `build/bootstrap/alembic_bootstrap.py`.

**Usage Examples**: Some basic Alembic tasks are explored here.

.. toctree::
   :maxdepth: 2

   examples

Extras
======

The following Python extras are provided as a convenience and are not necessarily officially supported
by the Alembic development team. These items are located under the /python area of the Alembic source
repository.

AbcOpenGL
---------

The :mod:`alembicgl` Python module is a set of Boost Python bindings for the AbcOpenGL lib, which allows
you to load an Alembic archive and draw to a GL context, e.g. from within an existing GL context such as
a PyQt QGLWidget ::

    >>> import alembicgl as abcgl
    >>> scene = abcgl.SceneWrapper("octopus.abc")
    >>> scene.draw()

A good example of the use of AbcOpenGL can be found in the SimpleAbcViewer code, and a good
example of the use of PyAbcOpenGL can be found in the GLWidget code in :doc:`AbcView </abcview>`. 

AbcView
-------

AbcView is a graphical PyQt-based Alembic inspection and visualization tool and offers a 
number of widgets to help you inspect and visualize your Alembic data. It also offers some 
scene assembly features.

.. toctree::
   :maxdepth: 2

   abcview

Cask
----

The :mod:`cask` module is a high level convenience wrapper for the Alembic Python API.
It blurs the lines between Alembic "I" and "O" objects and properties, abstracting both
into a single class object. It also wraps up a number of lower-level functions
into high level convenience methods.  

.. toctree::
   :maxdepth: 3

   cask

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

