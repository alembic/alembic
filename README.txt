-------------------------------------------------------------------------------
- Alembic
-
- Copyright 2010 Industrial Light and Magic,
-   a division of Lucasfilm Entertainment Company Ltd.
-------------------------------------------------------------------------------

Installation instructions for Alembic

0) Before Alembic can be built, you will need to satisfy its external
dependencies.  They are:

CMake (2.8.2) www.cmake.org
Boost (1.42) www.boost.org
ilmbase (1.0.1) www.openexr.com
OpenEXR (1.6.1) www.openexr.com
HDF5 (1.8.4-patch1) www.hdfgroup.org/HDF5

Optional:
Autodesk Maya (2010)
Pixar PRMan (15.x)

They may be installed in their default system locations (typically somewhere
under /usr/local), or in the Alembic source root's "contrib" directory, at
your discretion (though, CMake probably shouldn't be installed into the contrib
directory).  Look in your Alembic source root's "contrib" directory for
instructions on building Boost and HDF5; see next step for details.

1) Untar the Alembic source into your desired directory:

$ cd ~/ ; tar xzf Alembic_0.1.0.tgz

This will create a directory, ~/Alembic_0.1.0, that contains the Alembic
source code.

As alluded to in Step 0, ~/Alembic_0.1.0/contrib/ will contain instructional
files for building Boost and HDF5.  Mostly, those packages' libraries just
need a little encouragement to build static archives and with -fPIC.

2) Create a build root directory.  The Alembic build bootstrap script assumes
an out-of-source build.  For purposes of illustration, this document assumes
that your build root is located under your source root, though that is not
required.

$ mkdir ~/Alembic_0.1.0/ALEMBIC_BUILD

3) Change to the build directory.  This is so you can use `pwd` as part
of the right-hand-side of a setenv command.

$ cd ~/Alembic_0.1.0/ALEMBIC_BUILD

4) Set the environment varibales so that the Alembic bootstrap script can find
the Alembic dependencies; this document assumes a csh-style syntax.
If you're using bash or other sh-derived shell, obviously change "setenv
FOO bar" to "export FOO=bar".

If you installed the dependent libs into the contrib directory in your source
tree, and your CWD is Alembic_0.1.0/ALEMBIC_BUILD, then a copy-and-paste of
the following will set you up correctly:

setenv HDF5_ROOT "`pwd`/../contrib/hdf5-1.8.4-patch1"
setenv ILMBASE_ROOT "`pwd`/../contrib/ilmbase-1.0.1"
setenv BOOST_ROOT "`pwd`/../contrib/include/boost-1_42"
setenv OPENEXR_ROOT "`pwd`/../contrib/usr"
setenv MAYA_ROOT "`pwd`/../contrib/maya2010"
setenv PRMAN_ROOT "`pwd`/../contrib/prman"

Otherwise set those variables to their respective actual locations.

5) Run the Alembic bootstrap script.  Again, assuming your CWD is still
~/Alembic_0.1.0/ALEMBIC_BUILD, the following should work:

$ ../bin/mk_cmake.py

This will initialize the CMake meta-build system, then run CMake itself to
generate regular Makefiles.  For a list of all the options, invoke it with
'-h' or '--help'.  Go ahead, give it a shot; really: explore the space.

6) Run the make command.  Kind of a no-brainer, really.  You can safely run make
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

will give you a list of possible targets.

5a) If you want to build Alembic in debug mode, you'll need to run mk_cmake.py
again, but with the '--debug' flag.  This will cause debug-build Makefiles to
be generated, at which point, run "make" again.
