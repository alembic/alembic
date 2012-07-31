:mod:`alembic.AbcCoreAbstract`
==============================

The AbcCoreAbstract library is an almost-pure abstract library that describes the 
interfaces that the storage layer (e.g. AbcCoreHDF5) has to implement for Alembic. 
The AbcCoreAbstract layer specifies the interface for concepts like Objects or 
Properties, and the byte-size of data. More on the Alembic concepts like Objects 
or Properties.

One important piece of Alembic that is defined in this layer is Alembic’s notion of 
time, in the form of the TimeSampling and TimeSamplingType classes. These are 
non-abstract classes, and provide a rich interface for working with the temporal 
component of data samples, and are some of the only classes from this layer that 
are directly and prominently exposed in higher layers.

AbcCoreAbstract is not intended to be the primary human-friendly data-manipulation 
library in Alembic; that distinction is held by the Abc library. 

.. automodule:: alembic.AbcCoreAbstract
   :members:
   :undoc-members:
