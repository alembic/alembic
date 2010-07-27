#!/usr/bin/env python
#-*- mode: python -*-
##-*****************************************************************************
##
## Copyright (c) 2009-2010, Industrial Light & Magic,
##   a division of Lucasfilm Entertainment Company Ltd.
##
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are
## met:
## *       Redistributions of source code must retain the above copyright
## notice, this list of conditions and the following disclaimer.
## *       Redistributions in binary form must reproduce the above
## copyright notice, this list of conditions and the following disclaimer
## in the documentation and/or other materials provided with the
## distribution.
## *       Neither the name of Industrial Light & Magic nor the names of
## its contributors may be used to endorse or promote products derived
## from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
## A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
## OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
## SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
## LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
## DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
## THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
## OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##-*****************************************************************************

import re
import sys
import logging
import os
from optparse import OptionParser, OptionGroup
from string import Template

# TODO: Download compiled third-party libraries from web and put them in contrib
# TODO: Remove these hard-coded paths

# Set some default paths 
# CMAKE_PREFIX_PATH="/usr/local"
OPENEXR_ROOT="/usr"
HDF5_ROOT="/usr/local/hdf5-1.8.4-patch1"
# BOOST_ROOT="/usr/local/include/boost-1_42"
# MAYA_ROOT="/sww/tools/autodesk/maya2010"
# PRMAN_ROOT="/sww/tools/pixar/rman15"

# This used to be required, but now it is no longer used ...
# export HDF5_ROOT ILMBASE_ROOT BOOST_ROOT OPENEXR_ROOT MAYA_ROOT PRMAN_ROOT

def runCMake(options, args):

    if len(args) != 1:
        print "Invalid source directory"
        return None

    cmake_path=args[0]

    # Build type
    if options.build_type == 'Experimental':
        ctest_cmd="ctest -D Experimental"
    elif options.build_type == 'Continuous':
        ctest_cmd="ctest -D Continuous"
    elif options.build_type == 'Nightly':
        ctest_cmd="ctest -D Nightly"
    elif options.build_type == 'NightlyMemoryCheck':
        ctest_cmd="ctest -D NightlyStart; ctest -D NightlyUpdate; ctest -D NightlyConfigure; ctest -D NightlyBuild; ctest -D NightlyTest; ctest -D NightlyCoverage; ctest -D NightlyMemCheck; ctest -D NightlySubmit "
        options.memcheck=True
        options.coverage=True


    # Set the defaults to the contrib directory in the source tree
    if options.use_contrib:
        options.prefix=os.path.realpath(cmake_path + '/contrib')
        options.hdf5=os.path.realpath(cmake_path + "/contrib/hdf5-1.8.4-patch1")
        options.ilmbase=os.path.realpath(cmake_path + "/contrib/ilmbase-1.0.1")
        options.boost=os.path.realpath(cmake_path + "/contrib")
        options.exr=os.path.realpath(cmake_path + "/contrib")
        options.maya=os.path.realpath(cmake_path + "/contrib/autodesk/maya2010")
        options.prman=os.path.realpath(cmake_path + "/contrib/pixar/rman15")

    cmake_extra_args=' -G "Unix Makefiles" -D RUN_FROM_MK:STRING="TRUE"'

    if options.boost_librarydir or options.boost_includedir or options.boost:
        cmake_extra_args+=' -U"Boost_*" -U"BOOST_*"'

    if options.debug:
        cmake_extra_args+=' -D CMAKE_BUILD_TYPE:STRING="Debug"'
    else:
        cmake_extra_args+=' -D CMAKE_BUILD_TYPE:STRING="Release"'

    if options.prefix:
        cmake_extra_args+=' -D CMAKE_PREFIX_PATH:STRING="%s"' %options.prefix 

    if options.quiet:
        cmake_extra_args+=' -D QUIET:STRING="%s"' %options.quiet 

    if options.memcheck:
        cmake_extra_args+=' -D MEMORYCHECK_COMMAND:FILEPATH=/usr/bin/valgrind -D MEMORYCHECK_COMMAND_OPTIONS:STRING="--leak-check=full" -D MEMORYCHECK_SUPPRESSIONS_FILE:FILEPATH=%s' %os.path.realpath(cmake_path + '/build/valgrind-python.supp')

    if options.coverage:
        cmake_coverage_args=r"""-g -O0 -fprofile-arcs -ftest-coverage"""
        cmake_coverage=Template(' -D CMAKE_CXX_FLAGS:STRING="$coverage" -D CMAKE_C_FLAGS:STRING="$coverage" -D CMAKE_EXE_LINKER_FLAGS:STRING="$coverage"')
        cmake_extra_args+=cmake_coverage.substitute(coverage=cmake_coverage_args)

    if options.disable_prman:
        cmake_extra_args+=' -D USE_PRMAN:BOOL="FALSE"'
        options.prman=""

    if options.disable_maya:
        cmake_extra_args+=' -D USE_MAYA:BOOL="FALSE"'
        options.maya=""

    if options.python:
        cmake_extra_args+=' -D PYTHON_ROOT:STRING="%s"' %options.python 

    if options.exr:
        cmake_extra_args+=' -D OPENEXR_ROOT:STRING="%s"' %options.exr 

    if options.hdf5:
        cmake_extra_args+=' -D HDF5_ROOT:STRING="%s"' %options.hdf5 

    if options.maya:
        cmake_extra_args+=' -D MAYA_ROOT:STRING="%s"' %options.maya
        cmake_extra_args+=' -D USE_MAYA:BOOL="TRUE"'

    if options.ilmbase:
        cmake_extra_args+=' -D ILMBASE_ROOT:STRING="%s"' %options.ilmbase 

    if options.prman:
        cmake_extra_args+=' -D PRMAN_ROOT:STRING="%s"' %options.prman 
    
    if options.boost:
        cmake_extra_args+=' -D BOOST_ROOT:STRING="%s"' %options.boost 

    if options.boost_includedir:
        cmake_extra_args+=' -D BOOST_INCLUDEDIR:STRING="%s"' %options.boost_includedir

    if options.boost_librarydir:
        cmake_extra_args+=' -D BOOST_LIBRARYDIR:STRING="%s"' %options.boost_librarydir

    if options.install:
        options.install=os.path.expanduser(options.install)
        cmake_extra_args+=' -D CMAKE_INSTALL_PREFIX:STRING="%s"' %options.install
    if options.cflags:
        cmake_extra_args+=' -D CMAKE_C_FLAGS:STRING="%s"' % options.cflags

    if options.cxxflags:
        cmake_extra_args+=' -D CMAKE_CXX_FLAGS:STRING="%s"' % options.cxxflags

    cmake_cmd='cmake %s %s' %(cmake_extra_args, cmake_path)
 
    print "Executing CMake command: %s" %cmake_cmd

    os.system(cmake_cmd)
    os.system(cmake_cmd)

    make_cmd="make"
    make_install_cmd="make install"

    # Unset http_proxy so that submissions to the CDash dashboard will work at ILM
    os.unsetenv("http_proxy")

    if options.build:
        os.system(make_cmd)
    elif options.install:
        os.system(make_install_cmd)
    elif options.ctest:
        os.system(ctest_cmd)
    
    return cmake_cmd


def runBuild():
    print "Building"
    return None

def main(argv=None):

    mk_cmake_path=sys.argv[0]
    mk_cmake_basename=os.path.basename(mk_cmake_path)
    mk_cmake_path=os.path.dirname(mk_cmake_path) + '/../'
    mk_cmake_path=os.path.normpath(mk_cmake_path)

    parser = OptionParser()
    parser.set_usage(mk_cmake_basename + " [--options] <directory>\nUse '--help' for list of options") 

    configOptions = OptionGroup(parser, "Config")
    configOptions.add_option("--with-python", dest="python", type="string", default=None, help="Python location", metavar="PYTHON_ROOT")
    configOptions.add_option("--with-exr", dest="exr", type="string", default=OPENEXR_ROOT, help="EXR location", metavar="EXR_ROOT")
    configOptions.add_option("--with-hdf5", dest="hdf5", type="string", default=HDF5_ROOT, help="HDF5 location", metavar="HDF5_ROOT")
    configOptions.add_option("--with-maya", dest="maya", type="string", default=None, help="Maya location", metavar="MAYA_ROOT")
    configOptions.add_option("--with-ilmbase", dest="ilmbase", type="string", default=None, help="ILMbase location", metavar="ILMBASE_ROOT")
    configOptions.add_option("--with-prman", dest="prman", type="string", default=None, help="PRMAN location", metavar="PRMAN_ROOT")
    configOptions.add_option("--with-boost", dest="boost", type="string", default=None, help="boost_root location", metavar="BOOST_ROOT")
    configOptions.add_option("--boost_includedir", dest="boost_includedir", type="string", default=None, help="boost_includedir location", metavar="Boost_INCLUDE_DIR")
    configOptions.add_option("--boost_librarydir", dest="boost_librarydir", type="string", default=None, help="boost_librarydir location", metavar="Boost_LIBRARY_DIR")

    configOptions.add_option("--prefix", dest="prefix", type="string", default=None, help="Prefix for location to search for contrib libraries", metavar="PREFIX")

    configOptions.add_option("--quiet", dest="quiet", action="store_true", default=False, help="Generate quiet Makefiles")
    configOptions.add_option("--debug", dest="debug", action="store_true", default=False, help="Generate debug Makefiles")
    configOptions.add_option("--coverage", dest="coverage", action="store_true", default=False, help="Generate coverage testing Makefiles")
    configOptions.add_option("--memcheck", dest="memcheck", action="store_true", default=False, help="Peform memory checking using valgrind")

    configOptions.add_option("--disable-prman", dest="disable_prman", action="store_true", default=False, help="Disable PRMAN")
    configOptions.add_option("--disable-maya", dest="disable_maya", action="store_true", default=False, help="Disable Maya")
    configOptions.add_option("--use-contrib", dest="use_contrib", action="store_true", default=False, help="Use contrib directory for third-party dependencies")

    configOptions.add_option("--build-type", dest="build_type", type="choice", default="Experimental", choices=('Experimental', 'Continuous', 'Nightly', 'NightlyMemoryCheck'), help="Build type: one of 'Experimental' [default], 'Continuous', 'Nightly', 'NightlyMemoryCheck'")

    configOptions.add_option("--cflags", dest="cflags", type="string", default=None, help="CFLAGS to pass to the compiler", metavar="CFLAGS")
    configOptions.add_option("--cxxflags", dest="cxxflags", type="string", default=None, help="CXXFLAGS to pass to the compiler", metavar="CXXFLAGS")

    actionOptions = OptionGroup(parser, "Actions")

    actionOptions.add_option("--build", dest="build", action="store_true", default=False, help="build alembic and run tests")
    actionOptions.add_option("--ctest", dest="ctest", action="store_true", default=False, help="build alembic, run tests and submit to CDash")
    actionOptions.add_option("--install", dest="install", type="string", default=None, help="build, test, then install Alembic into TARGET", metavar="TARGET")

    parser.add_option_group(configOptions)
    parser.add_option_group(actionOptions)

    (options, args) = parser.parse_args()

    if args == []:
        print "mk_cmake path to source: %s" %(mk_cmake_path)
        args.append(mk_cmake_path)

    runCMake(options, args)

    if options.build:
        runBuild()

# this sys.exit(main()) idiom is intentional.
# it allows the Python program to return a result code that can be interpreted
# by other processes.

if __name__ == "__main__":
    sys.exit(main())


