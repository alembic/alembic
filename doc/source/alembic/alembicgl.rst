:mod:`alembicgl`
================

The :mod:`alembicgl` module is a set of Boost Python bindings for the AbcOpenGL lib, which allows
you to load an Alembic archive and draw to a GL context, e.g. ::

    >>> import alembicgl as abcgl
    >>> scene = abcgl.SceneWrapper("octopus.abc")
    >>> scene.draw()

.. automodule:: alembicgl
   :members:
   :undoc-members:
