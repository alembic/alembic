-------------------------------------------------------------------------------
- Alembic
-
- Copyright 2009-2016 Sony Pictures Imageworks, Inc. and
- Industrial Light and Magic, a division of Lucasfilm Entertainment Company Ltd.
-------------------------------------------------------------------------------

Installation instructions for Alembic


0) Before Alembic can be built, you will need to satisfy its external
dependencies:

Required:

    CMake (2.8.11+ newer is better for Windows builds) www.cmake.org
    OpenEXR (2.2.0) www.openexr.com (for ilmbase)

Optional:

    HDF5 (1.8.9) www.hdfgroup.org/HDF5
    Boost (1.55) www.boost.org
    pyilmbase (1.0.0) (to build the python bindings)
    Arnold (3.0)
    Pixar PRMan (15.x)
    Autodesk Maya (2012+)
    zlib

Note that the versions given parenthetically above are minimum-tested
versions.  You may have good luck with later or earlier versions, but this is
what we've been building Alembic against.

They may be installed in their default system locations (typically somewhere
under /usr/local), or some other centralized directory at your discretion; it's
best not to install your dependencies under the Alembic source root.


1) Clone the Alembic repo source into your desired source root:

    $ git clone https://github.com/alembic/alembic [<source root>]

This will create your source root directory that contains the Alembic source
code.


2) Run the cmake command. You should create a separate build root and pass the
source root to cmake:

    $ cd <build root>
    $ cmake  [OPTIONS] <source root>

Some examples of OPTIONS you may want or need to use include:

    -DALEMBIC_SHARED_LIBS=OFF  If you want the primary Alembic library to be
    built as a static library, instead of a dynamic one.

    -DUSE_HDF5=ON
    Specify this if you want to include optional HDF5 support.
    -DHDF_ROOT=HDF5Path may need to be specified if HDF5 is not installed in
    a standard location.

    -DUSE_MAYA=ON If you want to build AbcExport and AbcImport.
    -DMAYA_ROOT=MayaPath may need to be specified to point at a specific
    installation of Maya.

    -DALEMBIC_LIB_USES_TR1=ON or -DALEMBIC_LIB_USES_BOOST=ON if you do not have
    a C++11 capable compiler specify one of these to use TR1, or boost as
    a dependency of the Alembic library.

    -G "Visual Studio 14 2015 Win64"  If you want to create the project file for
    the 64 bit build of Alembic with the Visual Studio 2015 Community Edition.

    -DCMAKE_INSTALL_PREFIX=customPath  If you want to install the Alembic into
    an arbitrary location.

For Unix like operating systems:

3a) Run the make command.  Kind of a no-brainer, really.  You can safely run
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

For Windows:
3b) Open the Visual Studio project file and build the solution. (ALL_BUILD)

Once the Alembic project has been built, you can optionally run the unit tests:
    C:\BUILD_DIR\> PATH=%PATH%;location of Alembic dlls (and OpenExr if not in a standard place)
    C:\BUILD_DIR\> ctest

4) To build the API documentation via Doxygen:

    $ doxygen Doxyfile

This will generate html documentation in the doc/html folder.

If you get stuck, contact us on the alembic-discussion mailing list. You can
view the mailing list archives and join the mailing list via

http://groups.google.com/group/alembic-discussion

