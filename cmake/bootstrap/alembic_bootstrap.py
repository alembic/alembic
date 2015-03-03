#!/usr/bin/env python2.6
#-*- mode: python -*-
##-*****************************************************************************
##
## Copyright (c) 2009-2012,
##  Sony Pictures Imageworks Inc. and
##  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
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
import tempfile

try:
    import readline
except ImportError:
    pass

# use our own modules
__self_dir = os.path.split( sys.argv[0] )[0]
__alembic_python_dir = os.path.normpath( os.path.join( __self_dir, os.pardir,
                                                       os.pardir,
                                                       "lib", "python" ) )
sys.path.insert( 0, __alembic_python_dir )
from abcutils import Path, CMakeCache

##-*****************************************************************************
NONDIGITS = re.compile( r'\D+' )
DATABASE_ROOT = None
LOCATEDB = tempfile.mkstemp(suffix=".db", prefix="abc")[1]

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
def getLibExtension( lib ):
    for element in reversed( lib.split( '.' ) ):
        if NONDIGITS.match( element ):
            return element

    return False

##-*****************************************************************************
def configure_dep_root(path):
    """
    Runs the updatedb tool to create a new locate db file with paths
    to all of the lib dependencies. Takes a file system subtree as an
    argument.
    """
    if not os.name == "posix":
        print "Unsupported system:", os.name
    else:
        print "Looking for dependency libs..."
        args = ["updatedb", "--database-root", path, "-o", LOCATEDB, "-l", "0"]
        print " ".join(args)
        global DATABASE_ROOT
        try:
            p = Popen(args, stdout=PIPE)
            p.wait()
            DATABASE_ROOT = path
        except OSError, e:
            print 'Could not generate db file:', e

##-*****************************************************************************
def get_defaults_from_system( magic_file ):
    """
    Uses the `locate` command to find system defaults. The "magic_file" param
    is the name of the lib to locate.
    """
    if os.name == 'posix':
        print "Getting defaults from system..."
        try:
            _args = ["locate"]
            if DATABASE_ROOT is not None:
                _args.extend(["-d", LOCATEDB])
            _args.append(magic_file)
            print " ".join(_args)
            p = Popen(_args, stdout=PIPE )
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
    """
    magic_file is the name of the file we want to find, e.g. lexical_cast.hpp.
    """
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
    if numdefs == 1 and default == None:
        default = defaults[0]

    prompt = "Number of your choice or new value:\n> "

    answer = ""
    while True:
        print
        for i in nums:
            print "%s) %s" % ( i, defaults[i - 1] )
        if default:
            prompt = "Number of your choice, new value, or blank to accept \
the default value:\n[%s]> " % default

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
def ask_to_exit( message ):
    print
    print "Warning!  Something went wrong:\n%s" % message
    print

    while True:
        answer = raw_input( "Would you like to abort the bootstrap and try fixing it manually? Y/n: " )
        if answer == 'Y' or answer == 'y' or answer == '':
            print "OK, exiting the bootstrapping process. Sorry for the difficulty!"
            sys.exit( -1 )
        elif answer == 'n' or answer == 'N':
            print "OK, pressing on regardless. FYI, your build may not succeed."
            break
        else:
            print "Please enter 'y' or 'n'."

    return

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
def configureCMakeBoost(srcdir, 
                        boost_include_dir=None, 
                        thread_libpath=None, 
                        python_libpath=None, 
                        generator=None
                        ):
    
    # cmake args string
    libdir = None
    cmake_extra_args = ''

    # boost include files
    if boost_include_dir is not None:
        cmake_extra_args += ' -D BOOST_INCLUDEDIR:PATH="%s"' % boost_include_dir

    # boost thread lib path
    if thread_libpath is not None:
        thread_libpath = Path(thread_libpath)
        libdir, thread_lib = thread_libpath.split()
        cmake_extra_args += " -D Boost_THREAD_LIBRARY:FILEPATH=%s" % thread_libpath
    
    # boost python lib path
    if python_libpath is not None:
        python_libpath = Path(python_libpath)
        libdir, python_lib = python_libpath.split()
        cmake_extra_args += " -D BOOST_PYTHON_LIBRARY:FILEPATH=%s" % python_libpath

    # boost library root
    if libdir is not None:
        cmake_extra_args += " -D BOOST_LIBRARYDIR:PATH=%s" % libdir
    
    if generator is not None:
        cmake_extra_args += ' -G "%s"' % generator

    cmake_cmd='cmake --debug-trycompile -U BOOTSTRAP_* -D BOOTSTRAP_MODE:INTERNAL=TRUE -D BOOTSTRAP_BOOST:INTERNAL=TRUE -UBoost_* -UBOOST_* -UALEMBIC_BOOST_FOUND %s %s' %\
        (cmake_extra_args, srcdir )

    print "Executing CMake Boost configure command:\n%s" % cmake_cmd

    # execute the cmake command
    cmake_status = Popen(cmake_cmd, shell=True, stdout=PIPE, stderr=PIPE)
    status = cmake_status.wait()

    for line in cmake_status.stdout.readlines():
      print line.strip()

    errors = ''.join(cmake_status.stderr.readlines())
    print errors

    return status, errors

##-*****************************************************************************
def configureCMakeZlib( cmake_args ):
    cmake_extra_args = ''

    srcdir = cmake_args[0]

    try:
        cmake_extra_args+=' -D ZLIB_INCLUDE_DIR:PATH="%s"' % cmake_args[1]

        cmake_extra_args+=' -D ZLIB_LIBRARY:PATH="%s"' % cmake_args[2]

        cmake_extra_args+=' -G "%s"' % cmake_args[3]

        cmake_cmd='cmake --debug-trycompile -U BOOTSTRAP_* -D BOOTSTRAP_MODE:INTERNAL=TRUE -D BOOTSTRAP_ZLIB:INTERNAL=TRUE -UZLIB_* %s %s' % \
            (cmake_extra_args, srcdir )
    except IndexError:
        pass

    print "Executing CMake Zlib trycompile command:\n%s" % cmake_cmd

    cmake_status = Popen( cmake_cmd, shell=True, stdout=PIPE, stderr=PIPE )
    status = cmake_status.wait()

    for line in cmake_status.stdout.readlines():
      print line.strip()

    errors = ''.join( cmake_status.stderr.readlines() )
    print errors

    return status, errors

##-*****************************************************************************
def configureCMakeHDF5( cmake_args ):
    cmake_extra_args = ''

    srcdir = cmake_args[0]
    incdir = Path( cmake_args[1] )


    libNames = ["hdf5_hl", "hdf5"]
    hdf5Libs = ""
    libdir, hdf5Lib = Path( cmake_args[2] ).split()

    hdf5_root = libdir.common( incdir )[0]

    if not "HDF5_ROOT" in os.environ:
        os.environ["HDF5_ROOT"] = str( hdf5_root )

    hdf5LibStr = str( hdf5Lib )
    libext = getLibExtension( hdf5LibStr )
    libPreludeIndex = hdf5LibStr.find( "hdf5" )
    libPrelude = hdf5LibStr[:libPreludeIndex]

    fullPaths = []

    for lib in libNames:
        name = "%s%s.%s" % ( libPrelude, lib, libext )
        libpath = Path( libdir ).join( name )
        cmakeEntry = " -D HDF5_%s_LIBRARY:FILEPATH=%s" % ( lib, libpath )
        cmakeEntry += " -D HDF5_%s_LIBRARY_RELEASE:FILEPATH=%s" % ( lib, libpath )
        cmakeEntry += " -D HDF5_%s_LIBRARY_DEBUG:FILEPATH=%s" % ( lib, libpath )
        hdf5Libs += cmakeEntry
        fullPaths.append( libpath )

    try:
        cmake_extra_args += ' -D HDF5_C_INCLUDE_DIR:PATH="%s"' % incdir
        fullPaths.append( incdir )

        gotHDF5 = reduce( lambda x, y: x and y,
                          map( lambda x: x.exists(), fullPaths ),
                          True )

        if gotHDF5:
            cmake_extra_args += " -D ALEMBIC_HDF5_CONFIGURED:STRING=\"YES\""

        cmake_extra_args += hdf5Libs

        cmake_extra_args += " -D HDF5_LIBRARY_DIR:PATH=%s" % libdir

        cmake_extra_args += ' -G "%s"' % cmake_args[3]

        cmake_cmd='cmake --debug-trycompile -U BOOTSTRAP_* -D BOOTSTRAP_MODE:INTERNAL=TRUE -D BOOTSTRAP_HDF5:INTERNAL=TRUE -UHDF5* %s %s' % \
            (cmake_extra_args, srcdir )
    except IndexError:
        pass

    print "Executing CMake HDF5 trycompile command:\n%s" % cmake_cmd

    cmake_status = Popen( cmake_cmd, shell=True, stdout=PIPE, stderr=PIPE )
    status = cmake_status.wait()

    for line in cmake_status.stdout.readlines():
      print line.strip()

    errors = ''.join( cmake_status.stderr.readlines() )
    print errors

    return status, errors

##-*****************************************************************************
def configureCMakeIlmbase( cmake_args, useRoot = False ):
    cmake_extra_args = ''

    srcdir = cmake_args[0]

    libNames = ["Half", "Iex", "IlmThread", "Imath"]

    ilmbaseLibs = ""
    libdir, imathlib = Path( cmake_args[2] ).split()

    imathlib = str( imathlib )

    libext = getLibExtension( imathlib )
    libPreludeIndex = imathlib.find( "Imath" )
    libPrelude = imathlib[:libPreludeIndex]

    for lib in libNames:
        name = "%s%s.%s" % ( libPrelude, lib, libext )
        cmakeName = lib.upper()
        libpath = Path( libdir ).join( name )
        cmakeEntry = " -D ALEMBIC_ILMBASE_%s_LIB:FILEPATH=%s" % ( cmakeName, libpath )
        ilmbaseLibs += cmakeEntry

    try:
        cmake_extra_args += ' -D ALEMBIC_ILMBASE_INCLUDE_DIRECTORY:PATH="%s"' % cmake_args[1]

        cmake_extra_args += ilmbaseLibs

        cmake_extra_args += ' -G "%s"' % cmake_args[3]

    except IndexError:
        pass

    cmake_cmd = 'cmake --debug-trycompile -U BOOTSTRAP_* -D BOOTSTRAP_MODE:INTERNAL=TRUE -D BOOTSTRAP_ILMBASE:INTERNAL=TRUE -UALEMBIC_ILMBASE* -UILMBASE* %s %s' % \
        (cmake_extra_args, srcdir )

    print "Executing CMake Ilmbase trycompile command:\n%s" % cmake_cmd

    cmake_status = Popen( cmake_cmd, shell=True, stdout=PIPE, stderr=PIPE )
    status = cmake_status.wait()

    for line in cmake_status.stdout.readlines():
      print line.strip()

    errors = ''.join( cmake_status.stderr.readlines() )
    print errors

    return status, errors

##-*****************************************************************************
def configureCMakePyIlmbase( cmake_args, useRoot = False ):
    cmake_extra_args = ''

    srcdir = cmake_args[0]

    libNames = ["PyImath", ]

    pyilmbaseLibs = ""
    libdir, imathlib = Path( cmake_args[2] ).split()
    moduledir, imathmodule = Path( cmake_args[3] ).split()

    imathlib = str( imathlib )
    imathmodule = str( imathmodule )

    libext = getLibExtension( imathlib )
    libPreludeIndex = imathlib.find( "PyImath" )
    libPrelude = imathlib[:libPreludeIndex]

    for lib in libNames:
        name = "%s%s.%s" % ( libPrelude, lib, libext )
        cmakeName = lib.upper()
        libpath = Path( libdir ).join( name )
        cmakeEntry = " -D ALEMBIC_PYILMBASE_%s_LIB:FILEPATH=%s" % ( cmakeName, libpath )
        pyilmbaseLibs += cmakeEntry

    try:
        cmake_extra_args += ' -D ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY:PATH="%s"' % cmake_args[1]
        cmake_extra_args += ' -D ALEMBIC_PYILMBASE_LIBRARY_DIRECTORY:PATH=%s' % libdir
        cmake_extra_args += ' -D ALEMBIC_PYILMBASE_PYIMATH_MODULE_DIRECTORY:PATH=%s' % moduledir
        cmake_extra_args += pyilmbaseLibs
        cmake_extra_args += ' -G "%s"' % cmake_args[4]
    except IndexError:
        pass

    cmake_cmd = 'cmake --debug-trycompile -U BOOTSTRAP_* -D BOOTSTRAP_MODE:INTERNAL=TRUE -D BOOTSTRAP_PYILMBASE:INTERNAL=TRUE -UALEMBIC_PYILMBASE* -UPYILMBASE* %s %s' % \
        (cmake_extra_args, srcdir )

    print "Executing CMake PyIlmbase trycompile command:\n%s" % cmake_cmd

    cmake_status = Popen( cmake_cmd, shell=True, stdout=PIPE, stderr=PIPE )
    status = cmake_status.wait()

    for line in cmake_status.stdout.readlines():
      print line.strip()

    errors = ''.join( cmake_status.stderr.readlines() )
    print errors

    return status, errors

##-*****************************************************************************
def find_boost_include( cmakecache = None ):
    print "\nPlease enter the full path to the Boost header 'lexical_cast.hpp'"

    if os.name == "posix":
        print '(eg, "/usr/local/include/boost-1_42/boost/lexical_cast.hpp")'
    elif os.name == "mac":
        print '(eg, "/usr/local/include/boost-1_42/boost/lexical_cast.hpp")'
    elif os.name == "nt":
        print '(eg, "C:\Program Files\\Boost\\boost_1_42\\boost\\lexical_cast.hpp")'
    else:
        # unknown OS - good luck!
        print '(eg, "/usr/local/include/boost-1_42/boost/lexical_cast.hpp")'
    print

    mf = "lexical_cast.hpp"
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
        bid = boost_include_dir[:boost_include_dir.rindex( "boost" )]
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
def find_boost_thread_lib( cmakecache = None ):
    print "Please enter the full path to the multithreaded,",
    print "versioned Boost thread static library"
    if os.name == "posix":
        print '(eg, "/usr/local/lib/libboost_thread-gcc41-mt-1_42.a")'
    elif os.name == "mac":
        print '(eg, "/usr/local/lib/libboost_thread-gcc41-mt-1_42.a")'
    elif os.name == "nt":
        print '(eg, "C:\Program Files\\Boost\\boost_1_42\\lib\\libboost_thread-vc80-mt-s-1_42.lib")'
    else:
        # unknown OS - good luck!
        print '(eg, "/usr/local/lib/libboost_thread-gcc41-mt-1_42.a")'
    print

    mf = "libboost_thread"
    cmakevar = "Boost_THREAD_LIBRARY"

    cmakedefault, defaults = get_defaults( mf, cmakevar, cmakecache )

    default = None
    if len( defaults ) > 0 or cmakedefault:
        default = choose_defaults( defaults, cmakedefault )

    boost_lib = find_path( mf, default )
    libdir, lib = boost_lib.split()
    print "Using Boost libraries from %s" % libdir
    return boost_lib

##-*****************************************************************************
def find_boost_python_lib( cmakecache = None ):
    print "Please enter the full path to the ",
    print "Boost Python static library"
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
    cmakevar = "BOOST_PYTHON_LIBRARY"

    cmakedefault, defaults = get_defaults( mf, cmakevar, cmakecache )

    default = None
    if len( defaults ) > 0 or cmakedefault:
        default = choose_defaults( defaults, cmakedefault )

    boost_lib = find_path( mf, default )
    libdir, lib = boost_lib.split()
    print "Using Boost libraries from %s" % libdir
    return boost_lib

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
def find_zlib_lib( cmakecache = None ):
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

    zlib_lib = find_path( mf, default )
    print "Using Zlib library %s" % zlib_lib
    return zlib_lib

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
def find_hdf5_lib( cmakecache = None ):
    print "Please enter the full path to the hdf5 library"
    if os.name == "posix" or os.name == "mac":
        mf = "libhdf5.a"
        print '(eg, "/usr/lib/libhdf5.a")'
    elif os.name == "nt":
        mf = "hdf5.lib"
        print r'(eg, "C:\Program Files\hdf5\lib\hdf5.lib")'
    else:
        # unknown OS - good luck!
        mf = "libhdf5.a"
        print '(eg, "/usr/local/hdf5/lib/libhdf5.a")'
    print

    cmakevar = "HDF5_hdf5_LIBRARY"

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

    hdf5_lib_dir, hdf5_lib = find_path( mf, default ).split()
    print "Using HDF5 libraries from %s" % hdf5_lib_dir
    return hdf5_lib_dir.join( hdf5_lib )

##-*****************************************************************************
def find_ilmbase_include( cmakecache = None ):
    print "\nPlease enter the full path to the ILMBASE header 'ImathMath.h'"

    if os.name == "posix":
        print '(eg, "/usr/include/OpenEXR/ImathMath.h")'
    elif os.name == "mac":
        print '(eg, "/usr/local/include/OpenEXR/ImathMath.h")'
    elif os.name == "nt":
        print r'(eg, "C:\Program Files\ilmbase\include\ImathMath.h")'
    else:
        # unknown OS - good luck!
        print '(eg, "/usr/local/ilmbase-1.0.1/include/ImathMath.h")'
    print

    mf = "ImathMath.h"
    cmakevar = "ALEMBIC_ILMBASE_INCLUDE_DIRECTORY"

    cmakedefault, defaults = get_defaults( mf, cmakevar, cmakecache )
    cmakedefault = Path( cmakedefault )
    cmakedefault = check_include_from_cmake( mf, cmakedefault )

    default = None
    if len( defaults ) > 0 or cmakedefault:
        default = choose_defaults( defaults, cmakedefault )

    ilmbase_include_dir = find_path( mf, default )
    hid = ilmbase_include_dir.dirname()

    print
    print "Using ilmbase include directory: %s" % hid
    return hid

##-*****************************************************************************
def find_ilmbase_imath_lib( cmakecache = None ):
    print "Please enter the full path to the Imath library"
    if os.name == "posix" or os.name == "mac":
        mf = "libImath.a"
        print '(eg, "/usr/lib/libImath.a")'
    elif os.name == "nt":
        mf = "Imath.lib"
        print r'(eg, "C:\Program Files\ilmbase\lib\Imath.lib")'
    else:
        # unknown OS - good luck!
        mf = "libImath.a"
        print '(eg, "/usr/local/ilmbase-1.0.1/lib/libImath.a")'
    print

    cmakevar = "ALEMBIC_ILMBASE_IMATH_LIB"

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

    ilmbase_lib_dir, imathlib = find_path( mf, default ).split()
    print "Using ILMBASE libraries from %s" % ilmbase_lib_dir
    return ilmbase_lib_dir.join( imathlib )

##-*****************************************************************************
def find_pyilmbase_include( cmakecache = None ):
    print "\nPlease enter the full path to the PYILMBASE header 'PyImath.h'"

    if os.name == "posix":
        print '(eg, "/usr/include/OpenEXR/PyImath.h")'
    elif os.name == "mac":
        print '(eg, "/usr/local/include/OpenEXR/PyImath.h")'
    elif os.name == "nt":
        print r'(eg, "C:\Program Files\ilmbase\include\PyImath.h")'
    else:
        # unknown OS - good luck!
        print '(eg, "/usr/local/ilmbase-1.0.1/include/PyImath.h")'
    print

    mf = "PyImath.h"
    cmakevar = "ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY"

    cmakedefault, defaults = get_defaults( mf, cmakevar, cmakecache )
    cmakedefault = Path( cmakedefault )
    cmakedefault = check_include_from_cmake( mf, cmakedefault )

    default = None
    if len( defaults ) > 0 or cmakedefault:
        default = choose_defaults( defaults, cmakedefault )

    pyilmbase_include_dir = find_path( mf, default )
    hid = pyilmbase_include_dir.dirname()

    print
    print "Using Pyilmbase include directory: %s" % hid
    return hid

##-*****************************************************************************
def find_pyilmbase_pyimath_lib( cmakecache = None ):
    print "Please enter the full path to the PyImath library"
    if os.name == "posix" or os.name == "mac":
        mf = "libPyImath.so"
        print '(eg, "/usr/lib/libPyImath.so")'
    elif os.name == "nt":
        mf = "PyImath.lib"
        print r'(eg, "C:\Program Files\pyilmbase\lib\PyImath.lib")'
    else:
        # unknown OS - good luck!
        mf = "libPyImath.so"
        print '(eg, "/usr/local/pyilmbase-1.0.1/lib/libPyImath.so")'
    print

    cmakevar = "ALEMBIC_PYILMBASE_PYIMATH_LIB"

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

    pyilmbase_lib_dir, pyimathlib = find_path( mf, default ).split()
    print "Using PYILMBASE libraries from %s" % pyilmbase_lib_dir
    return pyilmbase_lib_dir.join( pyimathlib )

##-*****************************************************************************
def find_pyilmbase_pyimath_mod( cmakecache = None ):
    print "Please enter the full path to the imath python module"
    if os.name == "posix" or os.name == "mac":
        mf = "imathmodule.so"
        print '(eg, "/usr/lib/imathmodule.so")'
    elif os.name == "nt":
        mf = "PyImath.lib"
        print r'(eg, "C:\Program Files\pyilmbase\lib64\python2.6\site-packages\imathmodule.so")'
    else:
        # unknown OS - good luck!
        mf = "libPyImath.so"
        print '(eg, "/usr/local/pyilmbase/lib64/python2.6/imathmodule.so")'
    print

    cmakevar = "ALEMBIC_PYILMBASE_PYIMATH_MODULE"

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

    pyilmbase_module_dir, pyimathmodule = find_path( mf, default ).split()
    print "Using Imath Python module %s" % pyilmbase_module_dir
    return pyilmbase_module_dir.join( pyimathmodule )

##-*****************************************************************************
def check_include_from_cmake( magic_file, cmake_value ):
    result = None
    check = Path( cmake_value ) + magic_file
    if validate_path( check ):
        result = check

    return result

##-*****************************************************************************
def configure_build_root( srcDir, default = None ):
    root = None
    input_string = False
    _default = None

    if not default:
        _default = srcDir[:-1].join( "alembic_build" )

    # check if a cache exists and look for default
    _cache = os.path.join(os.getcwd(), "CMakeCache.txt")
    if os.path.exists(_cache):
        cmakecache = CMakeCache(_cache)
        _default = get_default_from_cmake_cache("ALEMBIC_BINARY_DIR", cmakecache)

    while True:
        input_string = "[%s]: " % _default
        if default:
            file_path = default
        else:
            print
            print "Please enter the location where you would like to build the Alembic"
            print "system (default: %s)" % _default
            file_path = Path( raw_input( input_string ) )

        if file_path.isempty():
            file_path = Path( _default )
        else:
            file_path = file_path.toabs()

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
def configure_boost(options, srcdir, cmakecache):
    print '''
Alembic requires Boost 1.42 or greater to compile. You must have compiled
Boost with STATIC, VERSIONED, and MULTITHREADED options turned on.
'''

    boost_include_dir = None
    boost_thread_library = None
    boost_python_library = None

    if options.boost_include_dir:
        boost_include_dir = options.boost_include_dir
    else:
        boost_include_dir = str(find_boost_include(cmakecache))

    if options.boost_thread_library:
        boost_thread_library = options.boost_thread_library
    else:
        boost_thread_library = str(find_boost_thread_lib(cmakecache))

    if options.boost_python_library:
        boost_python_library = options.boost_python_library
    elif options.enable_pyalembic:
        boost_python_library = str(find_boost_python_lib(cmakecache))

    if options.generator:
        print "Makesystem generator %s: " % (options.generator)

    boost_status, errors = configureCMakeBoost(
                               srcdir=srcdir,
                               boost_include_dir=boost_include_dir,
                               thread_libpath=boost_thread_library,
                               python_libpath=boost_python_library,
                               generator=options.generator
                            )

    if boost_status != 0:
        print "Could not successfully build a Boost test executable!"
        ask_to_exit(errors)

    return boost_status, boost_include_dir, boost_thread_library, boost_python_library

##-*****************************************************************************
def configure_zlib( options, srcdir, cmakecache ):

    if options.zlib_include_dir:
        zlib_include_dir = options.zlib_include_dir
    else:
        zlib_include_dir = str( find_zlib_include( cmakecache ) )

    if options.zlib_library:
        zlib_library = options.zlib_library
    else:
        zlib_library = str( find_zlib_lib( cmakecache ) )

    cmake_args = []

    cmake_args.append( srcdir )
    cmake_args.append( zlib_include_dir )
    cmake_args.append( zlib_library )

    if options.generator:
        print "Makesystem generator %s: " %(options.generator)
        cmake_args.append( options.generator )

    zlib_status, errors = configureCMakeZlib( cmake_args )

    if zlib_status != 0:
        print "Could not successfully build a zlib test executable!"
        ask_to_exit( errors )

    return zlib_status, zlib_include_dir, zlib_library

##-*****************************************************************************
def configure_hdf5( options, srcdir, cmakecache ):

    if options.hdf5_include_dir:
        hdf5_include_dir = options.hdf5_include_dir
    else:
        hdf5_include_dir = str( find_hdf5_include( cmakecache ) )

    if options.hdf5_hdf5_library:
        hdf5_hdf5_library = options.hdf5_hdf5_library
    else:
        hdf5_hdf5_library = str( find_hdf5_lib( cmakecache ) )

    cmake_args = []
    cmake_args.append( srcdir )
    cmake_args.append( hdf5_include_dir )
    cmake_args.append( hdf5_hdf5_library )

    if options.generator:
        print "Makesystem generator %s: " %(options.generator)
        cmake_args.append(options.generator)

    hdf5_status, errors = configureCMakeHDF5( cmake_args )

    if hdf5_status != 0:
        print "Could not successfully build an HDF5 test executable!"
        ask_to_exit( errors )

    return hdf5_status, hdf5_include_dir, hdf5_hdf5_library

##-*****************************************************************************
def configure_ilmbase( options, srcdir, cmakecache ):

    if options.ilmbase_include_dir:
        ilmbase_include_dir = options.ilmbase_include_dir
    else:
        ilmbase_include_dir = str( find_ilmbase_include( cmakecache ) )

    if options.ilmbase_imath_library:
        ilmbase_imath_library = options.ilmbase_imath_library
    else:
        ilmbase_imath_library = str( find_ilmbase_imath_lib( cmakecache ) )

    cmake_args = []
    cmake_args.append( srcdir )
    cmake_args.append( ilmbase_include_dir )
    cmake_args.append( ilmbase_imath_library )

    if options.generator:
        print "Makesystem generator %s: " %(options.generator)
        cmake_args.append(options.generator)

    ilmbase_status, errors = configureCMakeIlmbase( cmake_args )

    if ilmbase_status != 0:
        print "Could not successfully build an ilmbase test executable!"
        ask_to_exit( errors )

    return ilmbase_status, ilmbase_include_dir, ilmbase_imath_library

##-*****************************************************************************
def configure_pyilmbase( options, srcdir, cmakecache ):

    # pyimath include dir
    if options.pyilmbase_include_dir:
        pyilmbase_include_dir = options.pyilmbase_include_dir
    else:
        pyilmbase_include_dir = str( find_pyilmbase_include( cmakecache ) )

    # pyimath lib
    if options.pyilmbase_pyimath_library:
        pyilmbase_pyimath_library = options.pyilmbase_pyimath_library
    else:
        pyilmbase_pyimath_library = str( find_pyilmbase_pyimath_lib( cmakecache ) )

    # pyimath python module
    if options.pyilmbase_pyimath_module:
        pyilmbase_pyimath_module = options.pyilmbase_pyimath_module
    else:
        pyilmbase_pyimath_module = str( find_pyilmbase_pyimath_mod( cmakecache ) )

    cmake_args = []
    cmake_args.append( srcdir )
    cmake_args.append( pyilmbase_include_dir )
    cmake_args.append( pyilmbase_pyimath_library )
    cmake_args.append( pyilmbase_pyimath_module )

    if options.generator:
        print "Makesystem generator %s: " %(options.generator)
        cmake_args.append(options.generator)

    pyilmbase_status, errors = configureCMakePyIlmbase( cmake_args )

    if pyilmbase_status != 0:
        print "Could not successfully build an pyilmbase test executable!"
        ask_to_exit( errors )

    return pyilmbase_status, \
           pyilmbase_include_dir, \
           pyilmbase_pyimath_library, \
           pyilmbase_pyimath_module

##-*****************************************************************************
def runBootstrap( options, srcdir, cmakecache = None ):
    """
    config various libs
    """
    configure_boost( options, srcdir, cmakecache )
    print
    print

    configure_zlib( options, srcdir, cmakecache )
    print
    print

    configure_hdf5( options, srcdir, cmakecache )
    print
    print

    configure_ilmbase( options, srcdir, cmakecache )
    print
    print

    # we only need pyilmbase/pyimath if we're building pyalembic
    if options.enable_pyalembic:
        configure_pyilmbase( options, srcdir, cmakecache )
        print
        print

    return True

##-*****************************************************************************
def runCMake( opts, srcdir, ranBootstrap = False ):
    if not Path( srcdir ).isdir():
        print "Invalid source directory"
        return None

    # Create the cmake command line
    cmake_extra_args=' -D RUN_FROM_MK:STRING="TRUE"'
    cmake_bootstrap_string = " -D BOOTSTRAP_MODE:INTERNAL=FALSE"

    if not ranBootstrap:
        cmake_bootstrap_string = " -D BOOTSTRAP_MODE:INTERNAL=TRUE"

        if opts.generator:
            cmake_extra_args += ' -G "%s" ' % opts.generator

        if opts.debug:
            cmake_extra_args += ' -D CMAKE_BUILD_TYPE:STRING="Debug"'
            cmake_extra_args += ' -D CMAKE_TRY_COMPILE_CONFIGURATION:STRING="Debug"'
        else:
            cmake_extra_args += ' -D CMAKE_BUILD_TYPE:STRING="Release"'
            cmake_extra_args += ' -D CMAKE_TRY_COMPILE_CONFIGURATION:STRING="Release"'

        if opts.sharedLibs:
            cmake_extra_args += ' -D BUILD_SHARED_LIBS:BOOL="TRUE"'
        else:
            cmake_extra_args += ' -D BUILD_STATIC_LIBS:BOOL="TRUE"'

        if opts.quiet:
            cmake_extra_args += ' -D QUIET:STRING="%s"' % opts.quiet
        else:
            cmake_extra_args += " -U QUIET"

        if opts.prman:
            cmake_extra_args += ' -D USE_PRMAN:BOOL="TRUE"'
        else:
            cmake_extra_args += ' -D USE_PRMAN:BOOL="FALSE"'
            opts.prman = None

        if opts.arnold:
            cmake_extra_args += ' -D USE_ARNOLD:BOOL="TRUE"'
        else:
            cmake_extra_args += ' -D USE_ARNOLD:BOOL="FALSE"'
            opts.arnold = None

        if opts.maya:
            cmake_extra_args += ' -D USE_MAYA:BOOL="TRUE"'
        else:
            cmake_extra_args += ' -D USE_MAYA:BOOL="FALSE"'
            opts.maya = None

        if opts.enable_pyalembic:
            cmake_extra_args += ' -D USE_PYALEMBIC:BOOL="TRUE"'
        else:
            cmake_extra_args += ' -D USE_PYALEMBIC:BOOL="FALSE"'
            opts.pyalembic = None

        if opts.maya:
            cmake_extra_args += ' -D MAYA_ROOT:STRING="%s"' %opts.maya
            cmake_extra_args += ' -D USE_MAYA:BOOL="TRUE"'

        if opts.prman:
            cmake_extra_args += ' -D PRMAN_ROOT:STRING="%s"' %opts.prman
        
        if opts.arnold:
            cmake_extra_args += ' -D ARNOLD_ROOT:STRING="%s"' %opts.arnold

        if opts.hdf5_include_dir:
            cmake_extra_args += ' -D HDF5_C_INCLUDE_DIR:PATH="%s"' % \
                opts.hdf5_include_dir
        if opts.hdf5_hdf5_library:
            cmake_extra_args += ' -D HDF5_hdf5_LIBRARY:FILEPATH="%s"' % \
                opts.hdf5_hdf5_library

        if opts.ilmbase_include_dir:
            cmake_extra_args += ' -D ALEMBIC_ILMBASE_INCLUDE_DIRECTORY:PATH="%s"' % \
                opts.ilmbase_include_dir

        if opts.ilmbase_imath_library:
            cmake_extra_args += ' -D ALEMBIC_ILMBASE_IMATH_LIB:FILEPATH=%s' % \
                opts.ilmbase_imath_library

        if opts.pyilmbase_include_dir:
            cmake_extra_args += ' -D ALEMBIC_PYILMBASE_INCLUDE_DIRECTORY:PATH="%s"' % \
                opts.pyilmbase_include_dir

        if opts.pyilmbase_pyimath_library:
            cmake_extra_args += ' -D ALEMBIC_PYILMBASE_PYIMATH_LIB:FILEPATH="%s"' % \
                opts.pyilmbase_pyimath_library

        if opts.pyilmbase_pyimath_module:
            cmake_extra_args += ' -D ALEMBIC_PYILMBASE_PYIMATH_MODULE:FILEPATH="%s"' % \
                opts.pyilmbase_pyimath_module

        if opts.boost_include_dir:
            cmake_extra_args += ' -D BOOST_INCLUDEDIR:PATH="%s"' % \
                opts.boost_include_dir

        if opts.boost_thread_library:
            cmake_extra_args += ' -D Boost_THREAD_LIBRARY:FILEPATH="%s"' % \
                opts.boost_thread_library

        if opts.boost_python_library:
            cmake_extra_args += ' -D BOOST_PYTHON_LIBRARY:FILEPATH=%s' % \
                opts.boost_python_library

        if opts.zlib_include_dir:
            cmake_extra_args += ' -D ZLIB_INCLUDE_DIR:PATH="%s"' % \
                opts.zlib_include_dir

        if opts.zlib_library:
            cmake_extra_args += ' -D ZLIB_LIBRARY:FILEPATH=%s' % opts.zlib_library

        if opts.cflags:
            cmake_extra_args += ' -D CMAKE_C_FLAGS:STRING="%s"' % opts.cflags

        if opts.cxxflags:
            cmake_extra_args += ' -D CMAKE_CXX_FLAGS:STRING="%s"' % opts.cxxflags


    cmake_cmd = 'cmake %s %s %s' % ( cmake_bootstrap_string, cmake_extra_args,
                                     srcdir )

    if ranBootstrap:
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
    parser.set_usage( mk_cmake_basename + " [--options] <build directory>\n" + \
                          "Use '--help' for list of options" )

    configOptions = OptionGroup( parser, "Config" )

    configOptions.add_option("--dependency-install-root", dest="deproot", type="string",
                             default=None, help="Filesystem subtree location of "
                             "library dependencies")

    # App roots / triggers plugin builds
    configOptions.add_option( "--with-maya", dest="maya", type="string",
                              default=None, help="Maya location, build Maya plugins",
                              metavar="MAYA_ROOT" )
    configOptions.add_option( "--with-prman", dest="prman", type="string",
                              default=None, help="PRMAN location, bulid prman procedural",
                              metavar="PRMAN_ROOT" )
    configOptions.add_option( "--with-arnold", dest="arnold", type="string",
                              default=None, help="ARNOLD location, build arnold plugin",
                              metavar="ARNOLD_ROOT" )

    # HDF5 lib
    configOptions.add_option( "--hdf5_include_dir", dest="hdf5_include_dir",
                              type="string", default=None,
                              help="hdf5_include_dir location",
                              metavar="HDF5_INCLUDE_DIR" )
    configOptions.add_option( "--hdf5_hdf5_library", dest="hdf5_hdf5_library",
                              type="string", default=None,
                              help="hdf5_library location",
                              metavar="HDF5_HDF5_LIBRARY" )

    # IlmBase lib
    configOptions.add_option( "--ilmbase_include_dir", dest="ilmbase_include_dir",
                              type="string", default=None,
                              help="ilmbase_include_dir location",
                              metavar="ILMBASE_INCLUDE_DIR")
    configOptions.add_option( "--ilmbase_imath_library",
                              dest="ilmbase_imath_library",
                              type="string", default=None,
                              help="ilmbase_library_dir location",
                              metavar="ILMBASE_LIBRARY_DIR" )

    # PyIlmBase lib
    configOptions.add_option( "--pyilmbase_include_dir", dest="pyilmbase_include_dir",
                              type="string", default=None,
                              help="pyilmbase_include_dir location",
                              metavar="PYILMBASE_INCLUDE_DIR")
    configOptions.add_option( "--pyilmbase_pyimath_library",
                              dest="pyilmbase_pyimath_library",
                              type="string", default=None,
                              help="pyilmbase_library_dir location",
                              metavar="PYILMBASE_LIBRARY_DIR" )
    configOptions.add_option( "--pyilmbase_pyimath_module",
                              dest="pyilmbase_pyimath_module",
                              type="string", default=None,
                              help="pyilmbase_module location",
                              metavar="PYILMBASE_MODULE_DIR" )

    # Boost lib
    configOptions.add_option( "--boost_include_dir", dest="boost_include_dir",
                              type="string", default=None,
                              help="boost_include_dir location",
                              metavar="Boost_INCLUDE_DIR" )
    configOptions.add_option( "--boost_thread_library",
                              dest="boost_thread_library",
                              type="string", default=None,
                              help="libboost_thread library filepath",
                              metavar="Boost_THREAD_LIBRARY" )
    configOptions.add_option( "--boost_python_library",
                              dest="boost_python_library",
                              type="string", default=None,
                              help="libboost_python library filepath",
                              metavar="BOOST_PYTHON_LIBRARY" )

    # Zlib
    configOptions.add_option( "--zlib_include_dir", dest="zlib_include_dir",
                              type="string", default=None,
                              help="zlib_include_dir location",
                              metavar="ZLIB_INCLUDE_DIR" )
    configOptions.add_option( "--zlib_library", dest="zlib_library",
                              type="string", default=None,
                              help="zlib library",
                              metavar="ZLIB_LIBRARY" )

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

    configOptions.add_option( "--shared", dest="sharedLibs",
                              action="store_true", default=False,
                              help="Build shared libraries" )

    configOptions.add_option( "--cflags", dest="cflags", type="string",
                              default=None, help="CFLAGS to pass to the compiler",
                              metavar="CFLAGS" )

    configOptions.add_option( "--cxxflags", dest="cxxflags", type="string",
                              default=None, help="CXXFLAGS to pass to the compiler",
                              metavar="CXXFLAGS" )

    # Alembic Python bindings
    configOptions.add_option( "--enable-pyalembic", dest="enable_pyalembic",
                              action="store_true", default=False,
                              help="Build the Alembic Python bindings" )

    parser.add_option_group(configOptions)

    return parser

##-*****************************************************************************
def main(argv=None):

    bootstrap_script_path = Path( sys.argv[0] ).toabs()
    alembic_src_dir = bootstrap_script_path.dirname().join( os.pardir,
                                                            os.pardir )
    parser = makeParser( str( bootstrap_script_path ) )

    options, args = parser.parse_args()

    build_root = None

    if args:
        build_root = Path( args[0] ).toabs()

    build_root = configure_build_root( alembic_src_dir, build_root )

    # update locate db value used for finding libs from system
    if options.deproot:
        configure_dep_root(options.deproot)

    cmakecache = CMakeCache( build_root + "CMakeCache.txt" )

    os.chdir( str( build_root ) )
    print
    print "Now running in %s" % build_root

    print "%s\n" % ( "-" * 80 )
    print "Using Alembic source from %s\n" % alembic_src_dir

    ### RUN BOOTSTRAP ####
    # initial silent run to init build dir and cmake cache
    runCMake( options, alembic_src_dir )

    ranBootstrap = runBootstrap( options, alembic_src_dir, cmakecache )

    ### RUN CMAKE ####

    # args passed from command line
    runCMake( options, alembic_src_dir, ranBootstrap )

    return 0

##-*****************************************************************************
if __name__ == "__main__":
    sys.exit( main() )

