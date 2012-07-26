#!/usr/bin/env python2_zeno

import alembic, sys
from alembic.Abc import *
from alembic.AbcA import *
from alembic.Util import *

def visitSmallArraySamples( iProp, iIndent ):
    indent = " " * iIndent

    for i, s in enumerate( iProp.arraySamples ):
        print "%ssample %d:" % ( indent, i )
        if s is None:
            print indent, "********** Got nothing **********"
        else:
            print indent,
            for j in s:
                print j,
            print

def visitScalarSamples( iProp, iIndent ):
    indent = " " * iIndent

    for i, s in enumerate( iProp.scalarSamples ):
        print "%ssample %d:" % ( indent, i )
        if s is None:
            print indent, "********** Got nothing **********"
        else:
            print "%s%s" % ( indent, s )

def visitArraySamples( iProp, iIndent ):
    indent = " " * iIndent

    for i, s in enumerate( iProp.samples ):
        print "%ssample %d:" % ( indent, i )
        if s is None:
            print indent, "********** Got nothing **********"
        else:
            print indent,
            for j in s:
                print j,
            print 

def visitCompoundProperty( iProp, iIndent ):
    indent = " " * iIndent
    ptype = "CompoundProperty"
    name = "name=%s" % iProp.getName()
    interp = "schema=%s" % iProp.getMetaData().get( "schema" )

    print "%s%s %s; %s" % ( indent, ptype, name, interp )

    visitProperties( iProp, iIndent+2 )

def visitSimpleProperty( iProp, iIndent ):
    indent = " " * iIndent
    ptype = "ScalarProperty" if iProp.isScalar() else "ArrayProperty"
    name = "name=%s" % iProp.getName()
    md = "interpretation=%s" % iProp.getMetaData().get( "interpretation" )
    dtype = "datatype=%s" % iProp.getDataType()
    numsamps = "numsamps=%s" %iProp.getNumSamples()

    print "%s%s %s; %s; %s; %s" % ( indent, ptype, name, md, dtype, numsamps )

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
        print "%s%s %s" % ( indent, ptype, name )

    visitProperties( iObj.getProperties(), iIndent )

    for child in iObj.children:
        visitObject( child, iIndent )

def visitArchive( iArg ):
    iArchive = IArchive( iArg )

    print "AbcEcho for %s" % GetLibraryVersion()

    info = GetArchiveInfo ( iArchive )

    if len( info['appName'] ) > 0:
        print "  file written by : %s" % info['appName']
        print "  using Alembic : %s" % info['libraryVersionString']
        print "  written on : %s" % info['whenWritten']
        print "  user description by: %s" % info['userDescription']
        print
    else:
        print iArg
        print "  (file doesn't have any ArchiveInfo)"
        print

    visitObject( iArchive.getTop() )


map( visitArchive, [ a for a in sys.argv[1:] ] )
