:mod:`alembic.Abc`
==================

The Abc library is the human-friendly data-manipulation library 
in Alembic, and provides object-based wrappers around the :py:mod:`.AbcCoreAbstract` 
layer. 

As well, and in contrast with AbcCoreAbstract, the Abc layer 
provides for interpretation of the data manipulated with it, e.g., rather than a pointer 
to a contiguous block of 1024 bytes at the AbcCoreAbstract layer, at the Abc 
layer those bytes can be thought of as a value instance of Imath objects.

.. automodule:: alembic.Abc
   :members:
   :undoc-members:
