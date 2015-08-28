-------------------------------------------------------------------------------
- Alembic
-
- Copyright 2009-2015 Sony Pictures Imageworks, Inc. and
- Industrial Light and Magic, a division of Lucasfilm Entertainment Company Ltd.
-------------------------------------------------------------------------------

Installation instructions for Alembic


0) Before Alembic can be built, you will need to satisfy its external
dependencies:

Required:

    A unix-like OS (Linux, Mac OS X); Windows support is experimental
    CMake (2.8.0) www.cmake.org
    Boost (1.44) www.boost.org
    ilmbase (1.0.3) www.openexr.com
    zlib

Optional:

    HDF5 (1.8.9) www.hdfgroup.org/HDF5
    pyilmbase (1.0.0) # to build the python bindings
    Arnold (3.0)
    Pixar PRMan (15.x)
    Autodesk Maya (2012)
    OpenEXR (1.7.1) www.openexr.com
    OpenGL www.opengl.org # to build AbcOpenGL
    Sphinx (1.1.3) # to build the python documentation

Note that the versions given parenthetically above are minimum-tested
versions.  You may have good luck with later or earlier versions, but this is
what we've been building Alembic against.

They may be installed in their default system locations (typically somewhere
under /usr/local), or some other centralized directory at your discretion; it's 
best not to install your dependencies under the Alembic source root. 

Look in your Alembic source root's "doc" directory for help with building 
Boost and HDF5; see next step for details. Mostly, those packages' libraries
just need a little encouragement to build static archives and with -fPIC.


1) Clone the Alembic repo source into your desired source root:

    $ git clone https://github.com/alembic/alembic [<source root>]

This will create your source root directory that contains the Alembic source
code.


2) Run the cmake command. You can either run this in your source root, or
create a separate build root and pass the source root to cmake:

    $ cd <source root>
    $ cmake <source root> [OPTIONS]


3) Run the make command.  Kind of a no-brainer, really.  You can safely run
make with the '-j' flag, for doing multi-process builds.  In general, you can
profitably run as many "make" processes as you have CPUs, so for a dual-proc
machine,

    $ make -j2

will build it as quickly as possible.  Once the Alembic project has been built,
you can optionally run:

    $ make test

or,

    $ make install

each of which does what you'd expect.  Running

    $ make help

will give you a list of possible targets.  If you want to make a debug build,
run ccmake or cmake-gui (depending on what you installed when you installed
cmake), and change the build type to "Debug".


4) To build the API documentation via Doxygen:

    $ doxygen Doxyfile

This will generate html documentation in the doc/html folder.


If you get stuck, contact us on the alembic-discussion mailing list. You can
view the mailing list archives and join the mailing list via

http://groups.google.com/group/alembic-discussion

