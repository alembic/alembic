-------------------------------------------------------------------------------
- Alembic
-
- Copyright 2009-2011 Sony Pictures Imageworks, Inc. and
- Industrial Light and Magic, a division of Lucasfilm Entertainment Company Ltd.
-------------------------------------------------------------------------------

Installation instructions for Alembic

0) Before Alembic can be built, you will need to satisfy its external
dependencies.  They are, as of May 16th, 2011:

A unix-like OS (Linux, Mac OS X); Windows support is experimental
CMake (2.8.0) www.cmake.org
Boost (1.44) www.boost.org
ilmbase (1.0.1) www.openexr.com
HDF5 (1.8.7) www.hdfgroup.org/HDF5

Optional:
Arnold (3.0)
Autodesk Maya (2011)
Pixar PRMan (15.x)
OpenEXR (1.6.1) www.openexr.com


Note that the versions given parenthetically above are minimum-tested
versions.  You may have good luck with later or earlier versions, but this is
what we've been building Alembic against.

They may be installed in their default system locations (typically somewhere
under /usr/local), or some other centralized directory at
your discretion; it's best to not install your dependencies under the Alembic
source root.  Look in your Alembic source root's "doc" directory for
instructions on building Boost and HDF5; see next step for details.

1) Untar the Alembic source into your desired directory:

$ cd ~/ ; tar xzf Alembic_1_0_0_-xxxxxxxx.tgz

This will create a directory, ~/Alembic_1.0.0, that contains the Alembic
source code (if you're reading this, you've probably already done this).

As alluded to in Step 0, ~/Alembic_1.0.0/doc/ will contain instructional
files for building Boost and HDF5.  Mostly, those packages' libraries just
need a little encouragement to build static archives and with -fPIC.

2) Create a build root directory.  The Alembic build bootstrap script assumes
an out-of-source build.  For purposes of illustration, this document assumes
that your build root is located parallel to your source root, though that is
not required.

$ mkdir ~/ALEMBIC_BUILD

3) Change to the build directory.  This is so you can use `pwd` as part
of the right-hand-side of a setenv command.

$ cd ~/ALEMBIC_BUILD

4) Run the Alembic bootstrap script.  Again, assuming your CWD is still
~/ALEMBIC_BUILD, the following should work:

$ python ../Alembic_1.0.0/build/bootstrap/alembic_bootstrap.py .

You can give it several options and flags; '-h' for a list of them.  If you
don't specify a complete set of options when you run it, it will prompt you
interactively for the information it needs to initialize the build system.

It's worth pointing out that running the bootstrap script is optional; there
is a fairly comprehensive set of CMake files there that might just work for
you "out of the box".  On the other hand, we do strongly recommend running
the bootstrapper; it will make things so much easier for you.

4a) Once the system is bootstrapped, there will be a file called "CMakeCache.txt"
in your build root.  You can examine and manipulate this file with the cmake
commands "ccmake" (curses-based console program), or "cmake-gui" (Qt-based
gui program).  This file is the control file for CMake itself; the main thing
the bootstrapper does is create it and populate it appropriately (the bootstrap
script will also use it to get default values for the things it asks you for).

You can also just edit it directly, if you know what you're doing.  If you
change it, just be sure to run "cmake ." in the same directory as it so that
it regenerates the Makefiles.

5) Run the make command.  Kind of a no-brainer, really.  You can safely run make
with the '-j' flag, for doing multi-process builds.  In general, you can
profitably run as many "make" processes as you have CPUs, so for a dual-proc
machine,

$ make -j2

will build it as quickly as possible.  Once the Alembic project has been built,
you can optionall run:

$ make test

or,

$ make install

each of which does what you'd expect.  Running

$ make help

will give you a list of possible targets.  If you want to make a debug build,
you can either run the bootstrap script again with the option '--debug', or
run ccmake or cmake-gui (depending on what you installed when you installed
cmake, as described in 4a), and change the build type to "Debug".

6) If you'd like, build the example programs:

$ cd ~/ALEMBIC_BUILD/examples
$ make

7) To build the API documentation via Doxygen:

$ cd ../Alembic_1.0.0; doxygen Doxyfile

This will generate html documentation in the doc/html folder.


If you get stuck, contact us on the alembic-discussion mailing list. You
can view the mailing list archives and join the mailing list via
http://groups.google.com/group/alembic-discussion
