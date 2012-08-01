
Alembic Module Index
====================

Alembic itself is composed of several layers of libraries, each of which builds extensively 
on the layer below it.

**A note about writing and reading data**

Because Alembic is intended as a caching system, and not a live scenegraph, the API is split into 
two roughly symmetrical halves: one for writing, and one for reading. At the Abc and AbcGeom layers, 
classes that start with ‘O’ are for writing (or “output”), and classes that start with ‘I’ are for 
reading (or “input”). This is analogous to the C++ iostreams conceptual separation of istreams and 
ostreams.


.. toctree::
   :maxdepth: 2

   abc
   abca
   abcc
   abcg
   abcm
   util
