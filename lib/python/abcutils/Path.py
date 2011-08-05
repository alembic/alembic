#!/usr/bin/env python2.5
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

import os, sys

class Path( object ):
    """The Path class simplifies filesystem path manipulation.  If you wish
    to use a Path object as an argument to standard Python path functions
    such as os.path.*, open(), etc., you must first cast it to a string like
    "str( myPathObject )"."""

    def __init__( self, path=None ):
        if path != None:
            path = str( path )
            self._isabs = os.path.isabs( path )
            self._orig = path
            self._path = os.path.normpath( os.path.expanduser( path ) )
        else:
            self._isabs = False
            self._path = ''
            self._orig = ''

        if self._isabs:
            self._root = os.sep
        else:
            if self._orig == '':
                self._root = None
            else:
                self._root = os.curdir

        self._plist = filter( lambda x: x and x != os.curdir,
                              self._path.split( os.sep ))
        self._len = len( self._plist )
        self._isempty = self._root == None and self._len == 0
        self._maxindex = self._len - 1
        self._maxsliceindex = self._len

    def __reinit__( self, new ):
        self._len = len( new._plist )
        self._plist = new._plist[:]
        self._isempty = 0 == new._len
        self._maxindex = new._len - 1
        self._maxsliceindex = new._len
        self._path = new._path
        self._orig = new._orig
        self._isabs = new._isabs
        self._root = new._root

    def __repr__( self ):
        return self._path

    def __str__( self ):
        return self.__repr__()

    def __contains__( self, other ):
        return other in self._plist

    def __len__( self ):
        return self._len

    def __add__( self, other ):
        return Path( os.path.join( str( self ), str( other ) ) )

    def __radd__( self, other ):
        return Path( other ) + self

    def __iter__( self ):
        self._iterindex = 0
        return self

    def __eq__( self, other ):
        return str( self ) == str( other )

    def __ne__( self, other ):
        return str( self ) != str( other )

    def __cmp__( self, other ):
        _, p1, p2 = self.common( other )
        return len( p1 ) - len( p2 )

    def __nonzero__( self ):
        if not self.isabs() and len( self ) == 0:
            return False
        else:
            return True

    def __hash__( self ):
        return hash( str( self ))

    def __getitem__( self, n ):
        if isinstance( n, slice ):
            path = None
            plist = self._plist[n.start:n.stop:n.step]
            returnabs = self._isabs and n.start < 1

            if len( plist ) > 0:
                path = os.sep.join( plist )
            else:
                path = os.curdir

            path = Path( path )
            if returnabs:
                path = self.root() + path
            else:
                pass

            return path
        else:
            return self._plist[n]

    def __setitem__( self, key, value ):
        try:
            key = int( key )
        except ValueError:
            raise ValueError, "You must use an integer to refer to a path element."

        if key > abs( self._maxindex ):
            raise IndexError, "Maximum index is +/- %s." % self._maxindex

        self._plist[key] = value
        self._path = str( self[:] )

    def __delitem__( self, n ):
        try:
            n = int( n )
        except ValueError:
            raise ValueError, "You must use an integer to refer to a path element."

        try:
            del( self._plist[n] )
            t = Path( self[:] )
            self.__reinit__( t )
        except IndexError:
            raise IndexError, "Maximum index is +/- %s" & self._maxindex

    def rindex( self, val ):
        if val in self:
            return len( self._plist ) - \
                list( reversed( self._plist ) ).index( val ) - 1
        else:
            raise ValueError, "%s is not in path." % val

    def index( self, val ):
        if val in self:
            return self._plist.index( val )
        else:
            raise ValueError, "%s is not in path." % val

    def common( self, other, cmn=None ):
        cmn = Path( cmn )
        other = Path( str( other ) )
        if self.isempty() or other.isempty():
            return cmn, self, other
        elif (self[0] != other[0]) or (self.root() != other.root()):
            return cmn, self, other
        else:
            return self[1:].common( other[1:], self.root() + cmn + self[0] )

    def relative( self, other ):
        cmn, p1, p2 = self.common( other )
        relhead = Path()

        if len( p1 ) > 0:
            relhead = Path( (os.pardir + os.sep) * len( p1 ))

        return relhead + p2

    def join( self, *others ):
        t = self[:]
        for o in others:
            t = t + o
        return t

    def split( self ):
        head = self[:-1]
        tail = None
        if not head.isempty():
            tail = Path( self[-1] )
        else:
            tail = self

        if not head.isabs() and head.isempty():
            head = Path( None )

        if head.isabs() and len( tail ) == 1:
            tail = tail[-1]

        return ( head, tail )

    def splitext( self ):
        head, tail = os.path.splitext( self._path )
        return Path( head ), tail

    def next( self ):
        if self._iterindex > self._maxindex:
            raise StopIteration
        else:
            i = self._iterindex
            self._iterindex += 1
            return self[i]

    def subpaths( self ):
        sliceind = 0
        while sliceind < self._maxsliceindex:
            sliceind += 1
            yield self[:sliceind]

    def append( self, *others ):
        t = self[:]
        for o in others:
            t = t + o
        self.__reinit__( t )

    def root( self ):
        return Path( self._root )

    def elems( self ):
        return self._plist

    def path( self ):
        return self._path

    def exists( self ):
        return os.path.exists( self._path )

    def isempty( self ):
        return self._isempty

    def isabs( self ):
        return self._isabs

    def islink( self ):
        return os.path.islink( self._path )

    def isdir( self ):
        return os.path.isdir( self._path )

    def isfile( self ):
        return os.path.isfile( self._path )

    def readlink( self ):
        if self.islink():
            return Path( os.readlink( self._orig ) )
        else:
            return self

    def dirname( self ):
        return self[:-1]

    def basename( self ):
        return self.dirname()

    def startswith( self, other ):
        return self._path.startswith( other )

    def makeabs( self ):
        t = self[:]
        t._root = os.sep
        t._isabs = True
        t._path = os.path.join( os.sep, self._path )
        self.__reinit__( t )

    def makerel( self ):
        t = self[:]
        t._root = os.curdir
        t._isabs = False
        t._path = os.sep.join( t._plist )
        self.__reinit__( t )

    def toabs( self ):
        return Path( os.path.abspath( self._path ) )

    def torel( self ):
        t = self[:]
        t.makerel()
        return t

##-*****************************************************************************
def mapFSTree( root, path, dirs=set(), links={} ):
    """Create a sparse map of the filesystem graph from the root node to the path
    node."""
    root = Path( root )
    path = Path( path )

    for sp in path.subpaths():
        if sp.isabs():
            full = sp
        else:
            full = sp.toabs()
        head = full.dirname()

        if full.islink():
            target = full.readlink()
            if target.isabs():
                newpath = target
            else:
                newpath = head + target
            # make sure there are no cycles
            if full in links:
                continue
            links[full] = newpath
            _dirs, _links = mapFSTree( full, newpath, dirs, links )
            dirs.update( _dirs )
            links.update( _links )
        elif full.isdir():
            if full in dirs:
                continue
            else:
                dirs.add( full )
        elif full.isfile():
            break
            #pass
        else:
            print "QOI??? %s" % full

    return dirs, links

##-*****************************************************************************
def main():
    try:
        arg = Path( sys.argv[1] )
    except IndexError:
        print "Please supply a directory to analyze."
        return 1

    dirs, links = mapFSTree( Path( os.getcwd() ), arg )

    print
    print "Directories traversed to get to %s\n" % arg
    for d in sorted( list( dirs ) ): print d

    print
    print "Symlinks in traversed directories for %s\n" % arg
    for k in links: print "%s: %s" % ( k, links[k] )
    print

    return 0

##-*****************************************************************************
if __name__ == "__main__":
    sys.exit( main() )
