.. PyAlemic documentation master file, created by
   sphinx-quickstart on Tue May  1 11:15:16 2012.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Prerequisites
=============

Alembic for Python needs at least `Python 2.5 <http://python.org>`_ 
to run, as well as the `PyImath <http://github.com/openexr/openexr>`_ and 
`Boost::Python <http://boost.org>`_ libraries. 

Alembic Library
---------------

In order to use Alembic in Python, you must first build both Alembic and the
Alembic Python bindings using the bootstrap script from the source root: ::

> build/bootstrap/alembic_bootstrap.py [OPTIONS]


Environment
-----------

Add the path to the same Boost::Python library used to build Alembic 
to LD_LIBRARY_PATH. ::

> setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/usr/local/boost_1_49_0-python2.6/lib

And make sure PyImath is in PYTHONPATH or sys.path because it's imported by Alembic: ::

> setenv PYTHONPATH ${PYTHONPATH}:/usr/local/pyilmbase-1.0.0/lib64/python2.6/site-packages


Usage Basics
============

Importing Alembic is simply, ::

    > import alembic

Namespaces
----------

Each module has a namespace which is similar to the name of the module itself, but
shortened for brevity.

.. toctree::
   :maxdepth: 2

   alembic/index


Reading an Archive
------------------

Reading an Alembic archive, also referred to as an IArchive: ::

    > iarch = alembic.Abc.IArchive('octopus.abc')
    > print "Reading", iarch.getName()
    Reading octopus.abc


Hierarchy Navigation
--------------------

Usually, we want to start by getting the top of the object hierarchy: ::

    > top = iarch.getTop()
    > top.getName()
    'ABC'


Getting the Data
----------------


