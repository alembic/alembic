#-******************************************************************************
#
# Copyright (c) 2012,
#  Sony Pictures Imageworks Inc. and
#  Industrial Light & Magic, a division of Lucasfilm Entertainment Company Ltd.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# *       Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# *       Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
# *       Neither the name of Sony Pictures Imageworks, nor
# Industrial Light & Magic, nor the names of their contributors may be used
# to endorse or promote products derived from this software without specific
# prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#-******************************************************************************

import sys
from alembic.AbcCoreAbstract import *
from alembic.Abc import *
from alembic.Util import *

def visitSmallArraySamples( iProp, iIndent ):
    indent = " " * iIndent

    for i, s in enumerate( iProp.arraySamples ):
        print ("%ssample %d:" % ( indent, i ))
        if s is None:
            print (indent, "********** Got nothing **********")
        else:
            print (indent,)
            for j in s:
                print (j,)
            print

def visitScalarSamples( iProp, iIndent ):
    indent = " " * iIndent

    for i, s in enumerate( iProp.scalarSamples ):
        print ("%ssample %d:" % ( indent, i ))
        if s is None:
            print (indent, "********** Got nothing **********")
        else:
            print ("%s%s" % ( indent, s ))

def visitArraySamples( iProp, iIndent ):
    indent = " " * iIndent

    for i, s in enumerate( iProp.samples ):
        print ("%ssample %d:" % ( indent, i ))
        if s is None:
            print (indent, "********** Got nothing **********")
        else:
            print (indent,)
            for j in s:
                print (j,)
            print 

def visitCompoundProperty( iProp, iIndent ):
    indent = " " * iIndent
    ptype = "CompoundProperty"
    name = "name=%s" % iProp.getName()
    interp = "schema=%s" % iProp.getMetaData().get( "schema" )

    print ("%s%s %s; %s" % ( indent, ptype, name, interp ))

    visitProperties( iProp, iIndent+2 )

def visitSimpleProperty( iProp, iIndent ):
    indent = " " * iIndent
    ptype = "ScalarProperty" if iProp.isScalar() else "ArrayProperty"
    name = "name=%s" % iProp.getName()
    md = "interpretation=%s" % iProp.getMetaData().get( "interpretation" )
    dtype = "datatype=%s" % iProp.getDataType()
    numsamps = "numsamps=%s" %iProp.getNumSamples()

    print ("%s%s %s; %s; %s; %s" % ( indent, ptype, name, md, dtype, numsamps ))

    if iProp.isScalar():
        if iProp.getDataType().getExtent() == 1:
            visitScalarSamples( iProp, iIndent+2 )
        elif len( iProp.getMetaData().get( "interpretation" ) ) > 0:
            visitScalarSamples( iProp, iIndent+2 )
        else:
            visitSmallArraySamples( iProp, iIndent+2 )
    else:
        visitArraySamples( iProp, iIndent+2 )


def visitProperties( iParent, iIndent ):
    for header in iParent.propertyheaders:
        prop = iParent.getProperty( header.getName() )

        if header.isCompound():
            visitCompoundProperty( prop, iIndent+2 )
        elif header.isScalar() or header.isArray():
            visitSimpleProperty( prop, iIndent+2 )

def visitObject( iObj, iIndent = 0 ):

    path = iObj;
    indent = " " * iIndent
    ptype = "Object"
    name = "name=%s" % path

    if path != "/":
        iIndent += 2
        print ("%s%s %s" % ( indent, ptype, name ))

    visitProperties( iObj.getProperties(), iIndent )

    for child in iObj.children:
        visitObject( child, iIndent )

def visitArchive( iArg ):
    iArchive = IArchive( iArg )

    print ("AbcEcho for %s" % GetLibraryVersion())

    info = GetArchiveInfo ( iArchive )

    if len( info['appName'] ) > 0:
        print ("  file written by : %s" % info['appName'])
        print ("  using Alembic : %s" % info['libraryVersionString'])
        print ("  written on : %s" % info['whenWritten'])
        print ("  user description by: %s" % info['userDescription'])
        print
    else:
        print (iArg)
        print ("  (file doesn't have any ArchiveInfo)")
        print

    visitObject( iArchive.getTop() )


map( visitArchive, [ a for a in sys.argv[1:] ] )
