.. PyAlemic documentation master file, created by
   sphinx-quickstart on Tue May  1 11:15:16 2012.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

:mod:`alembic` -- Alembic Python Bindings Documentation
=======================================================

Alembic is an open source geometry caching format that promotes interoperability
between authoring tools. From `alembic.io <http://alembic.io>`_:

*Alembic distills complex, animated scenes into a non-procedural, application-independent 
set of baked geometric results. This ‘distillation' of scenes into baked geometry is 
exactly analogous to the distillation of lighting and rendering scenes into rendered 
image data.*

This module provides Python bindings for Alembic.


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
   :maxdepth: 1

   examples


Get Alembic
-----------

The Alembic code can be found in a Mercurial repository at 
http://code.google.com/p/alembic.


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

