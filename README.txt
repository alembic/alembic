-------------------------------------------------------------------------------
- Alembic
-
- Copyright 2009-2021 Sony Pictures Imageworks, Inc. and
- Industrial Light and Magic, a division of Lucasfilm Entertainment Company Ltd.
-------------------------------------------------------------------------------

Installation instructions for Alembic


0) Before Alembic can be built, you will need to satisfy its external
dependencies:

Required:

    CMake (3.13+) www.cmake.org
    C++ compiler that supports C++11
    Imath 3 https://github.com/AcademySoftwareFoundation/Imath
    OR
    OpenEXR (2.2.0+) www.openexr.com (for Imath)

Optional:

    HDF5 (1.8.9) www.hdfgroup.org/HDF5
    Boost (1.55+) www.boost.org (to build the python bindings)
    PyImath 3 https://github.com/AcademySoftwareFoundation/Imath (to build the python bindings)
    OR
    pyilmbase (1.0.0+) (to build the python bindings)
    Arnold (3.3+)
    Pixar PRMan (15.x)
    Autodesk Maya (2012+)

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

    -G "Visual Studio 15 2017 Win64"  If you want to create the project file for
    the 64 bit build of Alembic with the Visual Studio 2017 Community Edition.

    -DCMAKE_INSTALL_PREFIX=customPath  If you want to install the Alembic into
    an arbitrary location.

    -DUSE_PYALEMBIC=ON Whether you want to build the boost python bindings for Alembic.

    -DPYALEMBIC_PYTHON_MAJOR=2  If you want to look for python 2 (or python 3) via find_package.
    See the [CMake module documentation](https://cmake.org/cmake/help/latest/module/FindPython3.html).

    -DDOCS_PATH=customDocPath  If you have Doxygen installed this will create and install the doxygen
    generated doc (WIP) to customDocPath when you run: cmake doxygen

3)  To build: cmake -build .

4) To test: ctest
   Note:  On Windows you may need to add the path to your Alembic dlls.
   PATH=%PATH%;location of Alembic dlls (and OpenExr if not in a standard place)

If you get stuck, contact us on the alembic-discussion mailing list. You can
view the mailing list archives and join the mailing list:

http://groups.google.com/group/alembic-discussion

