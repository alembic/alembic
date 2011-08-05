#!/usr/bin/env python2.6
#-*- mode: python -*-
##-*****************************************************************************
##
## Copyright (c) 2009-2011,
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
from __future__ import with_statement
import os, sys, re
from Path import Path

##-*****************************************************************************
COMMENT = re.compile( r"//|#" )
WS = re.compile( r"\s" )

##-*****************************************************************************
class CacheEntry( object ):
    def __init__( self, _line ):
        line = WS.sub( "", str( _line ) )

        if not line:
            return None

        elif COMMENT.match( line ):
            return None

        else:
            # get rid of comments at the end of the line
            line = COMMENT.split( line, 1 )[0].strip()
            try:
                name_type, value = line.split( '=' )
                self._value = value.strip()
                if self._value == '':
                    self._value = None
                name, typ = name_type.split( ':' )
                self._name = name.strip()
                self._type = typ.strip()
            except ValueError:
                sys.stderr.write( "Could not parse line '%s'\n" % _line )
                self._value = None
                self._name = None
                self._type = None

    def __str__( self ):
        val = ""
        typ = ""
        if self._value != None:
            val = self._value
        if self._type != None:
            typ = self._type

        if self._name == None:
            return ""
        else:
            s = "%s:%s=%s" % ( self._name, typ, val )
            return s.strip()

    def __eq__( self, other ):
        return str( self ) == str( other )

    def __nonzero__( self ):
        try:
            return self._name != None and self._value != None
        except AttributeError:
            return False

    def name( self ):
        return self._name

    def value( self, newval = None ):
        if newval != None:
            self._value = newval
        else:
            return self._value

    def hint( self ):
        """Return the CMakeCache TYPE of the entry; used as a hint to CMake
        GUIs."""
        return self._type

##-*****************************************************************************
class CMakeCache( object ):
    """This class is used to read in and get programmatic access to the
    variables in a CMakeCache.txt file, manipulate them, and then write the
    cache back out."""

    def __init__( self, path=None ):
        self._cachefile = Path( path )
        _cachefile = str( self._cachefile )
        self._entries = {}
        if self._cachefile.exists():
            with open( _cachefile ) as c:
                entries = filter( None, map( lambda x: CacheEntry( x ),
                                             c.readlines() ) )
                entries = filter( lambda x: x.value() != None, entries )
                for i in entries:
                    self._entries[i.name()] = i

    def __contains__( self, thingy ):
        try:
            return thingy in self.names()
        except TypeError:
            return thingy in self._entries.values()

    def __iter__( self ):
        return self._entries

    def __nonzero__( self ):
        return len( self._entries ) > 0

    def __str__( self ):
        return os.linesep.join( map( lambda x: str( x ), self.entries() ) )

    def add( self, entry ):
        e = CacheEntry( entry )
        if e:
            if not e in self:
                self._entries[e.name()] = e
            else:
                sys.stderr.write( "Entry for '%s' is already in the cache.\n" % \
                                      e.name() )
        else:
            sys.stderr.write( "Could not create cache entry for '%s'\n" % e )

    def update( self, entry ):
        e = CacheEntry( entry )
        if e:
            self._entries[e.name()] = e
        else:
            sys.stderr.write( "Could not create cache entry for '%s'\n" % e )

    def names( self ):
        return self._entries.keys()

    def entries( self ):
        return self._entries.values()

    def get( self, name ):
        return self._entries[name]

    def cachefile( self ):
        return self._cachefile

    def refresh( self ):
        self.__init__( self._cachefile )

    def write( self, newfile = None ):
        if newfile == None:
            newfile = self._cachefile

        with open( newfile, 'w' ) as f:
            for e in self.entries():
                f.write( str( e ) + os.linesep )
