#!/usr/bin/env python2.6
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

import sys
import os, glob
import re
import logging
from string import Template
from optparse import OptionParser, OptionGroup
import subprocess

# use our own modules
__self_dir = os.path.split( sys.argv[0] )[0]
__alembic_python_dir = os.path.normpath( os.path.join( __self_dir, os.pardir,
                                                       os.pardir,
                                                       "lib", "python" ) )
sys.path.insert( 0, __alembic_python_dir )
from abcutils import Path, CMakeCache

##-*****************************************************************************
PIPE = subprocess.PIPE
Popen = subprocess.Popen

##-*****************************************************************************
def locate(pattern, root=os.curdir):
    for dir, subdir, files in os.walk(root):
        for file in files:
            if glob.fnmatch.fnmatch(file,pattern):
                print "Found!"
                return True
    print "Not found :("
    return False

##-*****************************************************************************
def get_defaults_from_system( magic_file ):
    if os.sep == "/": # are we posix? need better platform test
        try:
            p = Popen( ["locate", magic_file], stdout=PIPE )
            p.wait()
            return map( lambda x: Path( x.strip() ), p.stdout.readlines() )
        except OSError:
            return []
    return []

##-*****************************************************************************
def get_default_from_cmake_cache( cmakevar, cache ):
    default = None
    try:
        default = cache.get( cmakevar ).value()
    except KeyError:
        pass

    return default

##-*****************************************************************************
def get_defaults( magic_file, cmakevar = None, cmakecache = None ):
    cmakedefault = None
    if cmakevar and cmakecache:
        cmakedefault = get_default_from_cmake_cache( cmakevar, cmakecache )

    defaults = get_defaults_from_system( magic_file )

    return cmakedefault, sorted( list( set( defaults ) ) )

##-*****************************************************************************
def choose_defaults( defaults, default = None ):
    print "Enter the number of the choice you'd like to use, or enter"
    print "a different value if none of the choices are valid."

    numdefs = len( defaults )
    nums = map( lambda x: x + 1, range( numdefs ) )

    prompt = "Number of your choice or new value:\n"

    answer = ""
    while True:
        print
        for i in nums:
            print "%s) %s" % ( i, defaults[i - 1] )
        if default:
            prompt = "Number of your choice, new value, or blank to accept \
the default value:\n[%s] " % default

        print
        choice = raw_input( prompt ).strip()
        if default and not choice:
            answer = default
            break
        try:
            idx = int( choice )
            answer = defaults[idx - 1]
            break
        except ValueError:
            answer = choice
            break
        except IndexError:
            print "'%s' is not a valid choice; please try again." % idx

    print "Using value '%s'" % answer

    return answer

##-*****************************************************************************
def validate_path( p, wantdir=False ):
    p = Path( p )
    if wantdir:
        return p.isdir()
    else:
        return p.exists() and not p.isdir()

##-*****************************************************************************
def find_path( magic_file, default=False ):
    input_string = "Enter the path to %s: " % magic_file

    if default:
        print "Checking %s" % default
        p = Path( default )
        if validate_path( p ):
            return p
        else:
            print "Invalid path specified. Please try again."
            print

    while True:
        file_path = Path( raw_input( input_string ) )
        print "Checking '%s'" % file_path

        if validate_path( file_path ):
            print "'%s' exists; awesome!" % file_path
            return file_path
        else:
            print "Invalid path specified. Please try again."

##-*****************************************************************************
def configureCMakeBoost( cmake_args ):
    cmake_extra_args = ''

    srcdir = cmake_args[0]

    try:
        cmake_extra_args+=' -D BOOST_INCLUDEDIR:STRING="%s"' %cmake_args[1]

        cmake_extra_args+=' -D BOOST_LIBRARYDIR:STRING="%s"' %cmake_args[2]

        cmake_extra_args+=' -G "%s"' %cmake_args[3]
    except IndexError:
        pass

    cmake_cmd='cmake -D BOOTSTRAP_MODE:STRING=TRUE "-UBoost_*" "-UBOOST_*" %s %s' %\
        (cmake_extra_args, srcdir )

    print "Executing CMake Boost configure command:\n%s" % cmake_cmd

    cmake_status = Popen( cmake_cmd, shell=True, stdout=PIPE,
                          stderr=PIPE ).wait()

    return cmake_status

##-*****************************************************************************
def configureCMakeZlib( cmake_args ):
    cmake_extra_args = ''

    srcdir = cmake_args[0]

    try:
        cmake_extra_args+=' -D ZLIB_INCLUDE_DIR:STRING="%s"' % cmake_args[1]

        cmake_extra_args+=' -D ZLIB_LIBRARY:STRING="%s"' % cmake_args[2]

        cmake_extra_args+=' -G "%s"' % cmake_args[3]

        cmake_cmd='cmake -D BOOTSTRAP_MODE:STRING=TRUE "-UZLIB_*" %s %s' % \
            (cmake_extra_args, srcdir )
    except IndexError:
        pass

    print "Executing CMake Zlib trycompile command:\n%s" % cmake_cmd

    cmake_status = Popen( cmake_cmd, shell=True, stdout=PIPE,
                          stderr=PIPE ).wait()

    return cmake_status

##-*****************************************************************************
def configureCMakeHDF5( cmake_args ):
    cmake_extra_args = ''

    srcdir = cmake_args[0]

    try:
        cmake_extra_args += ' -D HDF5_C_INCLUDE_DIR:STRING="%s"' % cmake_args[1]

        cmake_extra_args += ' -D HDF5_hdf5_hl_LIBRARY:STRING="%s"' % cmake_args[2]

        cmake_extra_args += ' -G "%s"' % cmake_args[3]

        cmake_cmd='cmake -D BOOTSTRAP_MODE:STRING=TRUE "-UHDF5*" %s %s' % \
            (cmake_extra_args, srcdir )
    except IndexError:
        pass

    print "Executing CMake HDF5 trycompile command:\n%s" % cmake_cmd

    cmake_status = Popen( cmake_cmd, shell=True, stdout=PIPE,
                          stderr=PIPE ).wait()

    return cmake_status

##-*****************************************************************************
def find_boost_include( cmakecache = None ):
    print "Please enter the full path to the Boost header 'filesystem.hpp'"

    if os.name == "posix":
        print '(eg, "/usr/local/include/boost-1_42/boost/filesystem.hpp")'
    elif os.name == "mac":
        print '(eg, "/usr/local/include/boost-1_42/boost/filesystem.hpp")'
    elif os.name == "nt":
        print '(eg, "C:\Program Files\\Boost\\boost_1_42\\boost\\filesystem.hpp")'
    else:
        # unknown OS - good luck!
        print '(eg, "/usr/local/include/boost-1_42/boost/filesystem.hpp")'
    print

    mf = "filesystem.hpp"
    cmakevar = "Boost_INCLUDE_DIR"

    cmakedefault, defaults = get_defaults( mf, cmakevar, cmakecache )
    if cmakedefault != None:
        cmakedefault = Path( cmakedefault )
        cmakedefault = check_include_from_cmake( mf,
                                                 cmakedefault.join( "boost" ) )

    default = None
    if len( defaults ) > 0 or cmakedefault:
        default = choose_defaults( defaults, cmakedefault )

    boost_include_dir = find_path( mf, default )
    try:
        bid = boost_include_dir[:boost_include_dir.index( "boost" )]
    except ValueError:
        bid = boost_include_dir.dirname()
        print
        print "WARNING!"
        print "'%s' is not under a 'boost' directory;" % boost_include_dir
        print "this may not be good."

    print
    print "Using Boost include directory: %s" % bid
    return bid

##-*****************************************************************************
def find_boost_libdir( cmakecache = None ):
    print "Please enter the full path to the multithreaded,",
    print "versioned Boost Python static library"
    if os.name == "posix":
        print '(eg, "/usr/local/lib/libboost_python-gcc41-mt-1_42.a")'
    elif os.name == "mac":
        print '(eg, "/usr/local/lib/libboost_python-gcc41-mt-1_42.a")'
    elif os.name == "nt":
        print '(eg, "C:\Program Files\\Boost\\boost_1_42\\lib\\libboost_python-vc80-mt-s-1_42.lib")'
    else:
        # unknown OS - good luck!
        print '(eg, "/usr/local/lib/libboost_python-gcc41-mt-1_42.a")'
    print

    mf = "libboost_python"
    cmakevar = "Boost_PYTHON_LIBRARY"

    cmakedefault, defaults = get_defaults( mf, cmakevar, cmakecache )

    default = None
    if len( defaults ) > 0 or cmakedefault:
        default = choose_defaults( defaults, cmakedefault )

    boost_lib_dir = find_path( mf, default )
    bld = boost_lib_dir.dirname()
    print "Using Boost libraries from %s" % bld
    return bld

##-*****************************************************************************
def find_zlib_include( cmakecache = None ):
    print "Please enter the full path to the Zlib header 'zlib.h'"

    if os.name == "posix":
        print '(eg, "/usr/include/zlib.h")'
    elif os.name == "mac":
        print '(eg, "/usr/include/zlib.h")'
    elif os.name == "nt":
        print '(eg, "C:\Program Files\\z\\include\zlib.h")'
    else:
        # unknown OS - good luck!
        print '(eg, "/usr/local/z/include/zlib.h")'
    print

    mf = "zlib.h"
    cmakevar = "ZLIB_INCLUDE_DIR"

    cmakedefault, defaults = get_defaults( mf, cmakevar, cmakecache )
    cmakedefault = check_include_from_cmake( mf, cmakedefault )

    default = None
    if len( defaults ) > 0 or cmakedefault:
        default = choose_defaults( defaults, cmakedefault )

    zlib_include_dir = find_path( mf, default )
    zid = zlib_include_dir.dirname()

    print
    print "Using Zlib include directory: %s" % zid
    return zid

##-*****************************************************************************
def find_zlib_libdir( cmakecache = None ):
    print "Please enter the full path to the zlib library"
    if os.name == "posix":
        mf = "libz.a"
        print '(eg, "/usr/lib/libz.a")'
    elif os.name == "mac":
        mf = "libz.a"
        print '(eg, "/usr/lib/libz.a")'
    elif os.name == "nt":
        mf = "zdll.lib"
        print '(eg, "C:\Program Files\\z\\lib\\zdll.lib")'
    else:
        # unknown OS - good luck!
        mf = "libz.a"
        print '(eg, "/usr/local/z/lib/libz.a")'
    print

    cmakevar = "ZLIB_LIBRARY"

    cmakedefault, defaults = get_defaults( mf, cmakevar, cmakecache )
    default = False
    if len( defaults ) > 0 or cmakedefault:
        default = choose_defaults( defaults, cmakedefault )

    zlib_lib_dir = find_path( mf, default )
    print "Using Zlib libraries %s" % zlib_lib_dir
    return zlib_lib_dir

##-*****************************************************************************
def find_hdf5_include( cmakecache = None ):
    print "Please enter the full path to the HDF5 header 'hdf5.h'"

    if os.name == "posix":
        print '(eg, "/usr/local/include/hdf5.h")'
    elif os.name == "mac":
        print '(eg, "/usr/local/include/hdf5.h")'
    elif os.name == "nt":
        print r'(eg, "C:\Program Files\hdf5\include\hdf5.h")'
    else:
        # unknown OS - good luck!
        print '(eg, "/usr/local/include/hdf5.h")'
    print

    mf = "hdf5.h"
    cmakevar = "HDF5_C_INCLUDE_DIR"

    cmakedefault, defaults = get_defaults( mf, cmakevar, cmakecache )
    cmakedefault = Path( cmakedefault )
    cmakedefault = check_include_from_cmake( mf, cmakedefault )

    default = None
    if len( defaults ) > 0 or cmakedefault:
        default = choose_defaults( defaults, cmakedefault )

    hdf5_include_dir = find_path( mf, default )
    hid = hdf5_include_dir.dirname()

    print
    print "Using hdf5 include directory: %s" % hid
    return hid

##-*****************************************************************************
def find_hdf5_libdir( cmakecache = None ):
    print "Please enter the full path to the hdf5_hl library"
    if os.name == "posix" or os.name == "mac":
        mf = "libhdf5_hl.a"
        print '(eg, "/usr/lib/libhdf5_hl.a")'
    elif os.name == "nt":
        mf = "hdf5_hl.lib"
        print r'(eg, "C:\Program Files\hdf5\lib\hdf5_hl.lib")'
    else:
        # unknown OS - good luck!
        mf = "libhdf5_hl.a"
        print '(eg, "/usr/local/hdf5/lib/libhdf5_hl.a")'
    print

    cmakevar = "HDF5_hdf5_hl_LIBRARY"

    cmakedefault, defaults = get_defaults( mf, cmakevar, cmakecache )
    default = False
    if len( defaults ) > 0 or cmakedefault:
        if cmakedefault:
            hl_list = cmakedefault.split( ';' )
            if len( hl_list ) > 1:
                cmakedefault = Path( hl_list[1] )
            else:
                cmakedefault = Path( cmakedefault )
        default = choose_defaults( defaults, cmakedefault )

    hdf5_lib_dir = find_path( mf, default )
    print "Using HDF5 libraries from %s" % hdf5_lib_dir
    return hdf5_lib_dir

##-*****************************************************************************
def check_include_from_cmake( magic_file, cmake_value ):
    result = None
    check = Path( cmake_value ) + magic_file
    if validate_path( check ):
        result = check

    return result

##-*****************************************************************************
def configure_build_root( default = None ):
    cwd = Path().toabs()
    root = None
    input_string = False

    for i in range( len( cwd ) ):
        if cwd[i].startswith( "alembic" ):
            default = cwd[:i].join( "alembic_build" )
    if not default:
        default = cwd.join( "alembic_build" )

    print
    print "Please enter the location where you would like to build the Alembic"
    print "system (default: %s)" % default
    input_string = "[%s]: " % default
    while True:
        file_path = Path( raw_input( input_string ) )
        if file_path.isempty() and not file_path.isabs():
            file_path = Path( default )

        print "Checking '%s'" % file_path

        if file_path.isdir():
            print "'%s' exists; awesome!" % file_path
            return file_path
        else:
            print "Creating '%s'" % file_path
            try:
                os.makedirs( str( file_path ) )
                root = file_path
                break
            except Exception, e:
                print "Could not create build directory '%s'; got error:" % file_path
                print e

    return root

##-*****************************************************************************
def configure_boost( options, srcdir, cmakecache ):
    print '''
Alembic requires Boost 1.42 or greater to compile. You must have compiled
Boost with STATIC, VERSIONED, and MULTITHREADED options turned on.
'''

    if options.boost_include_dir:
        boost_include_dir = options.boost_include_dir
    else:
        boost_include_dir = str( find_boost_include( cmakecache ) )

    if options.boost_library_dir:
        boost_library_dir = options.boost_library_dir
    else:
        boost_library_dir = str( find_boost_libdir( cmakecache ) )

    cmake_args = []

    cmake_args.append( srcdir )
    cmake_args.append( boost_include_dir )
    cmake_args.append( boost_library_dir )

    if options.generator:
        print "Makesystem generator %s: " %(options.generator)
        cmake_args.append(options.generator)

    boost_status = configureCMakeBoost( cmake_args )

    return boost_status, boost_include_dir, boost_library_dir

##-*****************************************************************************
def configure_zlib( options, srcdir, cmakecache ):

    if options.zlib_include_dir:
        zlib_include_dir = options.zlib_include_dir
    else:
        zlib_include_dir = str( find_zlib_include( cmakecache ) )

    if options.zlib_library_dir:
        zlib_library_dir = options.zlib_library_dir
    else:
        zlib_library_dir = str( find_zlib_libdir( cmakecache ) )

    cmake_args = []

    cmake_args.append( srcdir )
    cmake_args.append( zlib_include_dir )
    cmake_args.append( zlib_library_dir )

    if options.generator:
        print "Makesystem generator %s: " %(options.generator)
        cmake_args.append( options.generator )

    zlib_status = configureCMakeZlib( cmake_args )

    return zlib_status, zlib_include_dir, zlib_library_dir

##-*****************************************************************************
def configure_hdf5( options, srcdir, cmakecache ):

    if options.hdf5_include_dir:
        hdf5_include_dir = options.hdf5_include_dir
    else:
        hdf5_include_dir = str( find_hdf5_include( cmakecache ) )

    if options.hdf5_library_dir:
        hdf5_library_dir = options.hdf5_library_dir
    else:
        hdf5_library_dir = str( find_hdf5_libdir( cmakecache ) )

    cmake_args = []
    cmake_args.append( srcdir )
    cmake_args.append( hdf5_include_dir )
    cmake_args.append( hdf5_library_dir )

    if options.generator:
        print "Makesystem generator %s: " %(options.generator)
        cmake_args.append(options.generator)

    hdf5_status = configureCMakeHDF5( cmake_args )

    return hdf5_status, hdf5_include_dir, hdf5_library_dir

##-*****************************************************************************
def runBootstrap( options, srcdir, cmakecache = None ):
    bootstrap_options = {}

    bootstrap_options['BOOST'] = configure_boost( options, srcdir, cmakecache )

    bootstrap_options['ZLIB'] = configure_zlib( options, srcdir, cmakecache )

    bootstrap_options['HDF5'] = configure_hdf5( options, srcdir, cmakecache )

    return bootstrap_options

##-*****************************************************************************
def runCMake( opts, srcdir, bootstrap_options = {} ):
    if not Path( srcdir ).isdir():
        print "Invalid source directory"
        return None

    # Set the defaults to the contrib directory in the source tree
    if opts.use_contrib:
        opts.prefix=os.path.realpath(cmake_path + '/contrib')
        opts.hdf5=os.path.realpath(cmake_path + "/contrib/hdf5-1.8.4-patch1")
        opts.ilmbase=os.path.realpath(cmake_path + "/contrib/ilmbase-1.0.1")
        opts.boost=os.path.realpath(cmake_path + "/contrib")
        opts.maya=os.path.realpath(cmake_path + "/contrib/autodesk/maya2010")
        opts.prman=os.path.realpath(cmake_path + "/contrib/pixar/rman15")

    # Create the cmake command line
    cmake_extra_args=' -D RUN_FROM_MK:STRING="TRUE"'
    cmake_bootstrap_string = " -D BOOTSTRAP_MODE:STRING=TRUE"

    if bootstrap_options != {}:
        ( opts.boost_status, opts.boost_include_dir, opts.boost_library_dir) = \
            bootstrap_options['BOOST']

        ( opts.zlib_status, opts.zlib_include_dir, opts.zlib_library_dir ) = \
            bootstrap_options['ZLIB']

        ( opts.hdf5_status, opts.hdf5_include_dir, opts.hdf5_library_dir ) = \
            bootstrap_options['HDF5']

        cmake_bootstrap_string = " -D BOOTSTRAP_MODE:STRING=FALSE"
    else:
        pass

    if opts.boost_library_dir or opts.boost_include_dir or opts.boost:
        cmake_extra_args+=' -U"Boost_*" -U"BOOST_*"'

    if opts.generator:
        cmake_extra_args+=' -G "%s" ' %(opts.generator)

    if opts.debug:
        cmake_extra_args+=' -D CMAKE_BUILD_TYPE:STRING="Debug"'
    else:
        cmake_extra_args+=' -D CMAKE_BUILD_TYPE:STRING="Release"'

    if opts.prefix:
        cmake_extra_args+=' -D CMAKE_PREFIX_PATH:STRING="%s"' %opts.prefix

    if opts.quiet:
        cmake_extra_args+=' -D QUIET:STRING="%s"' %opts.quiet
    else:
        cmake_extra_args += " -U QUIET"

    if opts.disable_prman:
        cmake_extra_args+=' -D USE_PRMAN:BOOL="FALSE"'
        opts.prman=""

    if opts.disable_maya:
        cmake_extra_args+=' -D USE_MAYA:BOOL="FALSE"'
        opts.maya=""

    if opts.python:
        cmake_extra_args+=' -D PYTHON_ROOT:STRING="%s"' %opts.python

    if opts.hdf5:
        cmake_extra_args+=' -D HDF5_ROOT:STRING="%s"' %opts.hdf5

    if opts.maya:
        cmake_extra_args+=' -D MAYA_ROOT:STRING="%s"' %opts.maya
        cmake_extra_args+=' -D USE_MAYA:BOOL="TRUE"'

    if opts.ilmbase:
        cmake_extra_args+=' -D ILMBASE_ROOT:STRING="%s"' %opts.ilmbase

    if opts.prman:
        cmake_extra_args+=' -D PRMAN_ROOT:STRING="%s"' %opts.prman

    if opts.boost:
        cmake_extra_args+=' -D BOOST_ROOT:STRING="%s"' %opts.boost

    # TODO: add the remainder of these
    # Custom INCLUDE and LIBRARY Directories from bootstrap script
    #    if python_include_dir:
    #        cmake_extra_args+=' -D PYTHON_INCLUDE_DIR:STRING="%s"' %python_include_dir
    #
    #    if python_library_dir:
    #        cmake_extra_args+=' -D PYTHON_LIBRARY_DIR:STRING="%s"' %python_library_dir
    #
    if opts.hdf5_include_dir:
        cmake_extra_args+=' -D HDF5_INCLUDE_DIR:STRING="%s"' % opts.hdf5_include_dir

    if opts.hdf5_library_dir:
        cmake_extra_args+=' -D HDF5_LIBRARY_DIR:STRING="%s"' % opts.hdf5_library_dir

    #
    #    if maya_include_dir:
    #        cmake_extra_args+=' -D MAYA_INCLUDE_DIR:STRING="%s"' %maya_include_dir
    #
    #    if maya_library_dir:
    #        cmake_extra_args+=' -D MAYA_LIBRARY_DIR:STRING="%s"' %maya_library_dir
    #
    #    if ilmbase_include_dir:
    #        cmake_extra_args+=' -D ILMBASE_INCLUDE_DIR:STRING="%s"' %ilmbase_include_dir
    #
    #    if ilmbase_library_dir:
    #        cmake_extra_args+=' -D ILMBASE_LIBRARY_DIR:STRING="%s"' %ilmbase_library_dir
    #
    #    if prman_include_dir:
    #        cmake_extra_args+=' -D PRMAN_INCLUDE_DIR:STRING="%s"' %prman_include_dir
    #
    #    if prman_library_dir:
    #        cmake_extra_args+=' -D PRMAN_LIBRARY_DIR:STRING="%s"' %prman_library_dir
    #
    if opts.boost_include_dir:
        cmake_extra_args+=' -D BOOST_INCLUDEDIR:STRING="%s"' % opts.boost_include_dir

    if opts.boost_library_dir:
        cmake_extra_args+=' -D BOOST_LIBRARYDIR:STRING="%s"' % opts.boost_library_dir

    if opts.zlib_include_dir:
        cmake_extra_args+=' -D ZLIB_INCLUDE_DIR:STRING="%s"' % opts.zlib_include_dir

    if opts.zlib_library_dir:
        cmake_extra_args+=' -D ZLIB_LIBRARY:STRING="%s"' % opts.zlib_library_dir

    if opts.cflags:
        cmake_extra_args+=' -D CMAKE_C_FLAGS:STRING="%s"' % opts.cflags

    if opts.cxxflags:
        cmake_extra_args+=' -D CMAKE_CXX_FLAGS:STRING="%s"' % opts.cxxflags

    cmake_cmd='cmake %s %s %s' % ( cmake_bootstrap_string, cmake_extra_args,
                                   srcdir )

    if bootstrap_options != {}:
        # Run CMake twice to generate the correct build system files (this is an
        # undocumented feature of CMake)
        print "Executing CMake command: %s" % cmake_cmd
        Popen( cmake_cmd, shell=True ).wait()
        Popen( cmake_cmd, shell=True ).wait()
    else:
        # don't output anything; we're just running to get some values in the
        # cmakecache for later.
        Popen( cmake_cmd, shell=True, stdout=PIPE, stdin=PIPE,
               stderr=PIPE ).wait()

    return cmake_cmd

##-*****************************************************************************
def makeParser( mk_cmake_basename ):
    parser = OptionParser()
    parser.set_usage( mk_cmake_basename + " [--options] <directory>\n" + \
                          "Use '--help' for list of options" )

    configOptions = OptionGroup( parser, "Config" )
    configOptions.add_option( "--with-python", dest="python", type="string",
                             default=None, help="Python location",
                             metavar="PYTHON_ROOT")
    configOptions.add_option( "--with-hdf5", dest="hdf5", type="string",
                              default=None, help="HDF5 location",
                              metavar="HDF5_ROOT" )
    configOptions.add_option( "--with-maya", dest="maya", type="string",
                              default=None, help="Maya location",
                              metavar="MAYA_ROOT" )
    configOptions.add_option( "--with-ilmbase", dest="ilmbase", type="string",
                              default=None, help="ILMbase location",
                              metavar="ILMBASE_ROOT" )
    configOptions.add_option( "--with-prman", dest="prman", type="string",
                              default=None, help="PRMAN location",
                              metavar="PRMAN_ROOT" )
    configOptions.add_option( "--with-boost", dest="boost", type="string",
                              default=None, help="boost_root location",
                              metavar="BOOST_ROOT" )

    # Include and library file locations
    configOptions.add_option( "--python_include_dir", dest="python_include_dir",
                              type="string", default=None,
                              help="python_include_dir location",
                              metavar="PYTHON_INCLUDE_DIR" )
    configOptions.add_option( "--python_library_dir", dest="python_library_dir",
                              type="string", default=None,
                              help="python_library_dir location",
                              metavar="PYTHON_LIBRARY_DIR" )
    configOptions.add_option( "--hdf5_include_dir", dest="hdf5_include_dir",
                              type="string", default=None,
                              help="hdf5_include_dir location",
                              metavar="HDF5_INCLUDE_DIR" )
    configOptions.add_option( "--hdf5_library_dir", dest="hdf5_library_dir",
                              type="string", default=None,
                              help="hdf5_library_dir location",
                              metavar="HDF5_LIBRARY_DIR" )
    configOptions.add_option( "--maya_include_dir", dest="maya_include_dir",
                              type="string", default=None,
                              help="maya_include_dir location",
                              metavar="MAYA_INCLUDE_DIR" )
    configOptions.add_option( "--maya_library_dir", dest="maya_library_dir",
                              type="string", default=None,
                              help="maya_library_dir location",
                              metavar="MAYA_LIBRARY_DIR" )
    configOptions.add_option( "--ilmbase_include_dir", dest="ilmbase_include_dir",
                              type="string", default=None,
                              help="ilmbase_include_dir location",
                              metavar="ILMBASE_INCLUDE_DIR")
    configOptions.add_option( "--ilmbase_library_dir", dest="ilmbase_library_dir",
                              type="string", default=None,
                              help="ilmbase_library_dir location",
                              metavar="ILMBASE_LIBRARY_DIR" )
    configOptions.add_option( "--prman_include_dir", dest="prman_include_dir",
                              type="string", default=None,
                              help="prman_include_dir location",
                              metavar="PRMAN_INCLUDE_DIR" )
    configOptions.add_option( "--prman_library_dir", dest="prman_library_dir",
                              type="string", default=None,
                              help="prman_library_dir location",
                              metavar="PRMAN_LIBRARY_DIR" )
    configOptions.add_option( "--boost_include_dir", dest="boost_include_dir",
                              type="string", default=None,
                              help="boost_include_dir location",
                              metavar="Boost_INCLUDE_DIR" )
    configOptions.add_option( "--boost_library_dir", dest="boost_library_dir",
                              type="string", default=None,
                              help="boost_library_dir location",
                              metavar="Boost_LIBRARY_DIR" )

    configOptions.add_option( "--zlib_include_dir", dest="zlib_include_dir",
                              type="string", default=None,
                              help="zlib_include_dir location",
                              metavar="ZLIB_INCLUDE_DIR" )
    configOptions.add_option( "--zlib_library_dir", dest="zlib_library_dir",
                              type="string", default=None,
                              help="zlib_library_dir location",
                              metavar="ZLIB_LIBRARY_DIR" )

    # Prefix if using a single location under which the above libraries are
    # installed
    configOptions.add_option( "--prefix", dest="prefix", type="string",
                              default=None,
                              help="Prefix for location to search for contrib libraries",
                              metavar="PREFIX" )

    configOptions.add_option( "--use-contrib", dest="use_contrib",
                              action="store_true", default=False,
                              help="Use contrib directory for third-party dependencies" )

    # Set the build system that is generated
    configOptions.add_option( "--generator", dest="generator", type="string",
                              default="Unix Makefiles", help="Default generator",
                              metavar="GENERATOR" )

    # Options that affect the build
    configOptions.add_option( "--quiet", dest="quiet", action="store_true",
                              default=False, help="Generate quiet Makefiles" )

    configOptions.add_option( "--debug", dest="debug",
                              action="store_true", default=False,
                              help="Generate debug Makefiles" )

    configOptions.add_option( "--cflags", dest="cflags", type="string",
                              default=None, help="CFLAGS to pass to the compiler",
                              metavar="CFLAGS" )

    configOptions.add_option( "--cxxflags", dest="cxxflags", type="string",
                              default=None, help="CXXFLAGS to pass to the compiler",
                              metavar="CXXFLAGS" )

    configOptions.add_option( "--disable-prman", dest="disable_prman",
                              action="store_true", default=False,
                              help="Disable PRMAN" )

    configOptions.add_option( "--disable-maya", dest="disable_maya",
                              action="store_true", default=False,
                              help="Disable Maya" )

    parser.add_option_group(configOptions)

    return parser

##-*****************************************************************************
def main(argv=None):

    bootstrap_script_path = Path( sys.argv[0] ).toabs()
    alembic_src_dir = bootstrap_script_path.dirname().join( os.pardir,
                                                            os.pardir )
    # bootstrap_dir = alembic_src_dir.join( "build", "bootstrap" )

    parser = makeParser( str( bootstrap_script_path ) )

    options, args = parser.parse_args()

    build_root = configure_build_root()

    cmakecache = CMakeCache( build_root + "CMakeCache.txt" )

    os.chdir( str( build_root ) )
    print
    print "Now running in %s" % build_root

    if args:
        alembic_src_dir = Path( args[0] ).toabs()

    print "%s\n" % ( "-" * 80 )
    print "Using Alembic source from %s\n" % alembic_src_dir

    ### RUN BOOTSTRAP ####
    # initial silent run to init build dir and cmake cache
    runCMake( options, alembic_src_dir )

    bootstrap_options = runBootstrap( options, alembic_src_dir, cmakecache )

    ### RUN CMAKE ####

    # args passed from command line
    runCMake( options, alembic_src_dir, bootstrap_options )

    return 0

##-*****************************************************************************
if __name__ == "__main__":
    sys.exit( main() )


