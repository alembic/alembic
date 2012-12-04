#! /usr/bin/env python
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

import os
import re
import sys
import copy

from PyQt4 import QtCore
from PyQt4 import QtGui
from PyQt4 import uic

def getSchemaTitleBase(obj):
    """
    Returns a tuple of the shema, title and base strings.

    Example ::
    
        >>> obj.getMetaData().serialize()
        'schema=AbcGeom_Xform_v3;schemaObjTitle=AbcGeom_Xform_v3:.xform'
    
        >>> getSchemaTitleBase(obj)
        ('AbcGeom_Xform_v3', 'AbcGeom_Xform_v3:.xform', None)

    :param obj: Alembic IObject.

    :return: Tuple of strings.
    """
    md = obj.getMetaData()
    return md.get('schema'), md.get('schemaObjTitle'), md.get('schemaBaseType')

def findObjects(obj, name):
    """
    Recursive generator function that yields objects with
    names matching given name regex.

    :param obj: Alembic object
    :param name: Name regular expression to match
    :yeild: Alembic object
    """
    if re.match(name, obj.getName()):
        yield obj
    else:
        for childObject in obj.children:
            for obj in findObjects(childObject, name):
                yield obj

class ArrayThread(QtCore.QThread):
    """
    Simple Qthread that emits values from a (long) array. Takes a
    QWidget parent, an array and a max number of elements to emit
    as arguments (if max is None, get all of them).
    """
    def __init__(self, parent, array, start=0, end=10):
        super(ArrayThread, self).__init__(parent)
        self.array = array
        self._start = start
        self._end = end

    def run(self):
        try:
            for index, value in enumerate(self.array[self._start:]):
                index = index + self._start
                self.emit(QtCore.SIGNAL('arrayValue (PyQt_PyObject)'), (index, value))
                if index >= self._end:
                    break
        except TypeError, e:
            self.emit(QtCore.SIGNAL('arrayValue (PyQt_PyObject)'), (0, str(self.array)))

class AbcTreeWidgetItem(QtGui.QTreeWidgetItem):
    """
    Base class from which all other tree widgets are derived.
    """
    def __init__(self, parent, object=None):
        super(QtGui.QTreeWidgetItem, self).__init__(parent)
        self._seen = False
        self.object = object

    def getObject(self):
        return self.object

    def wasSeen(self):
        return self._seen

    def setSeen(self, seen=True):
        self._seen = seen

class ObjectTreeWidgetItem(AbcTreeWidgetItem):
    def __init__(self, parent, object):
        super(ObjectTreeWidgetItem, self).__init__(parent, object)
        self.object = object
        if self.object.getNumChildren() > 0:
            self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.ShowIndicator)
        else:
            self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.DontShowIndicator)
        self.setExpanded(False)
        self.setText(self.treeWidget().colnum('name'), object.getName())
        schema, title, base = getSchemaTitleBase(self.object)
        self.setText(self.treeWidget().colnum('schema'), schema)
        self.setToolTip(self.treeWidget().colnum('name'), 
                QtCore.QString(object.getFullName()))

    def children(self):
        return self.object.children

    def properties(self):
        props = self.object.getProperties()
        for header in props.propertyheaders:
            yield props.getProperty(header.getName())

class PropertyTreeWidgetItem(AbcTreeWidgetItem):
    def __init__(self, parent, property):
        super(PropertyTreeWidgetItem, self).__init__(parent, property)
        self.property = property
        if self.property.isCompound():
            self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.ShowIndicator)
        else:
            self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.DontShowIndicator)
        self.setText(self.treeWidget().colnum('name'), self.property.getName())
        self.setText(self.treeWidget().colnum('type'), self.getType())
        self.setText(self.treeWidget().colnum('datatype'), str(self.property.getDataType()))
   
    def getObject(self):
        return self.property

    def getType(self):
        if self.property.isCompound():
            return 'compound'
        elif self.property.isScalar():
            return 'scalar'
        elif self.property.isArray():
            return 'array'
        else:
            return 'unknown'

    def isConstant(self):
        return self.property.isConstant

    def properties(self):
        for header in self.property.propertyheaders:
            yield self.property.getProperty(header.getName())

    def samples(self):
        if self.property.isCompound():
            return []
        else:
            return self.property.samples

class SampleTreeWidgetItem(AbcTreeWidgetItem):
    def __init__(self, parent, index, sample, property=None):
        super(SampleTreeWidgetItem, self).__init__(parent, sample)
        self.property = property
        self.sample = sample
        self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.DontShowIndicator)
        self.setText(self.treeWidget().colnum('index'), str(index))

        if property and property.getObject().isArray():
            size = len(sample)
            if size <= 5:
                value = ", ".join([str(v) for v in sample])
            else:
                value = str(sample)
        else:
            size = 1
            value = str(sample)
        self.setText(self.treeWidget().colnum('value'), value)

        try:
           self.setText(self.treeWidget().colnum('size'), str(size))
        except TypeError:
            self.setText(self.treeWidget().colnum('size'), "1")

    def getObject(self):
        return self.sample

class ArrayTreeWidgetItem(AbcTreeWidgetItem):
    def __init__(self, parent, index, value, array=None):
        super(ArrayTreeWidgetItem, self).__init__(parent, array)
        self.setChildIndicatorPolicy(QtGui.QTreeWidgetItem.DontShowIndicator)
        self.setText(self.treeWidget().colnum('index'), str(index))
        self.setText(self.treeWidget().colnum('value'), str(value))

class AbcTreeWidget(QtGui.QTreeWidget):

    DEFAULT_COLUMN_NAMES = ['name', 'schema', ]
    DEFAULT_COLUMNS = dict(enumerate(DEFAULT_COLUMN_NAMES))
    DEFAULT_COLUMNS.update(dict(zip(DEFAULT_COLUMN_NAMES, range(len(DEFAULT_COLUMN_NAMES)))))
    COLUMNS = copy.copy(DEFAULT_COLUMNS)

    def colnum(self, name):
        return self.COLUMNS.get(name, -1)

    @property
    def columnNames(self):
        return [self.COLUMNS[elm] for elm in sorted(elm for elm in self.COLUMNS if type(elm) == int)]

    def __init__(self, parent=None):
        super(QtGui.QTreeWidget, self).__init__(parent)
        self.setIconSize(QtCore.QSize(20, 20))
        self.setAllColumnsShowFocus(True)
        self.setAnimated(False)
        self.setAutoScroll(False)
        self.setUniformRowHeights(True)
        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.setDragDropMode(QtGui.QAbstractItemView.NoDragDrop)

        self.initHeader()

        self.connect(self, QtCore.SIGNAL("itemSelectionChanged ()"), 
                self.handleItemSelected)
        self.connect(self, QtCore.SIGNAL("itemClicked (QTreeWidgetItem *, int)"), 
                self.handleClick)
        self.connect(self, QtCore.SIGNAL("itemDoubleClicked (QTreeWidgetItem *, int)"), 
                self.handleDoubleClick)
        self.connect(self, QtCore.SIGNAL("itemExpanded (QTreeWidgetItem *)"), 
                self.handleExpand)

    def initHeader(self):
        self.COLUMNS = copy.copy(self.DEFAULT_COLUMNS)
        self.setupHeader()
        self.setSortingEnabled(True)
        self.sortByColumn(self.colnum('date'), QtCore.Qt.DescendingOrder)

    def setupHeader(self):
        self.setColumnCount(len(self.COLUMNS)/2)
        self.setHeaderLabels(self.columnNames)
        self.header().resizeSection(self.colnum('index'), 50)
        self.header().resizeSection(self.colnum('size'), 75)
        self.header().resizeSection(self.colnum('name'), 300)

    def rowHeight(self):
        return 24

    def handleItemSelected(self):
        items = self.selectedItems()
        if not items:
            return
        self.handleClick(items[0])

    def handleClick(self, item):
        self.scrollToItem(item, QtGui.QAbstractItemView.EnsureVisible)
        self.emit(QtCore.SIGNAL('itemClicked (PyQt_PyObject)'), item)

    def handleDoubleClick(self, item):
        self.emit(QtCore.SIGNAL('itemDoubleClicked (PyQt_PyObject)'), item)

    def handleExpand(self, item):
        raise NotImplementedError

    def selected(self):
        if self.selectedItems():
            selected = [item.getObject() for item in self.selectedItems()]
            return selected[0]
        return None

    def find(self, name):
        """
        Searches each top level item for name, starting with
        nodes under /ABC, the top
        """
        if name.startswith('/'):
            name = name[1:]
        item = self.topLevelItem(0)
        found = False
        for object in findObjects(item.getObject(), name):
            parts = [p for p in object.getFullName().split('/') if p]
            item.setExpanded(True)
            while parts:
                part = parts.pop(0)
                for i in range(0, item.childCount()):
                    child = item.child(i)
                    check = str(child.text(self.colnum('name')))
                    if part == check:
                        child.setExpanded(True)
                        item = child
                        found = True
                        break
                    else:
                        child.setExpanded(False)
                        found = False

        if found:
            self.scrollToItem(item, QtGui.QAbstractItemView.PositionAtCenter)
            item.setExpanded(False)
            self.setItemSelected(item, True)

class ObjectTreeWidget(AbcTreeWidget):
    def __init__(self, parent, main):
        super(ObjectTreeWidget, self).__init__(parent)
        self.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)

    def handleExpand(self, item):
        if not item.wasSeen():
            for child in item.children():
                item.addChild(ObjectTreeWidgetItem(item, child))
            item.setSeen(True)

class PropertyTreeWidget(AbcTreeWidget):
    
    DEFAULT_COLUMN_NAMES = ['name', 'type', 'datatype', ]
    DEFAULT_COLUMNS = dict(enumerate(DEFAULT_COLUMN_NAMES))
    DEFAULT_COLUMNS.update(dict(zip(DEFAULT_COLUMN_NAMES, range(len(DEFAULT_COLUMN_NAMES)))))
    COLUMNS = copy.copy(DEFAULT_COLUMNS)

    def __init__(self, parent):
        super(PropertyTreeWidget, self).__init__(parent)

    def setupHeader(self):
        self.setColumnCount(len(self.COLUMNS)/2)
        self.setHeaderLabels(self.columnNames)
        self.header().resizeSection(self.colnum('name'), 150)

    def showProps(self, item):
        self.clear()
        for property in item.properties():    
            self.addTopLevelItem(PropertyTreeWidgetItem(self, property))

    def handleExpand(self, item):
        if not item.wasSeen():
            for property in item.properties():
                item.addChild(PropertyTreeWidgetItem(item, property))
            item.setSeen(True)

class SampleTreeWidget(AbcTreeWidget):
    
    DEFAULT_COLUMN_NAMES = ['index', 'size', 'value', ]
    DEFAULT_COLUMNS = dict(enumerate(DEFAULT_COLUMN_NAMES))
    DEFAULT_COLUMNS.update(dict(zip(DEFAULT_COLUMN_NAMES, range(len(DEFAULT_COLUMN_NAMES)))))
    COLUMNS = copy.copy(DEFAULT_COLUMNS)

    def __init__(self, parent):
        super(SampleTreeWidget, self).__init__(parent)
        self.setRootIsDecorated(False)

    def showSamples(self, item):
        self.clear()
        for index, sample in enumerate(item.samples()):
            self.addTopLevelItem(SampleTreeWidgetItem(self, index, sample, item))

class ArrayTreeWidget(AbcTreeWidget):
    
    DEFAULT_COLUMN_NAMES = ['index', 'value', ]
    DEFAULT_COLUMNS = dict(enumerate(DEFAULT_COLUMN_NAMES))
    DEFAULT_COLUMNS.update(dict(zip(DEFAULT_COLUMN_NAMES, range(len(DEFAULT_COLUMN_NAMES)))))
    COLUMNS = copy.copy(DEFAULT_COLUMNS)

    def __init__(self, parent):
        super(ArrayTreeWidget, self).__init__(parent)
        self.setRootIsDecorated(False)

    def numRows(self):
        return (self.size().height() / self.rowHeight()) + 20

    def wheelEvent(self, event):
        index = self.indexAt(self.viewport().rect().bottomLeft())
        lastRow = self.itemFromIndex(index)
        if lastRow:
            lastIndex = lastRow.text(self.colnum("index"))
            if self.index >= (len(self.item.getObject()) - 1):
                pass
            elif (int(lastIndex) + 10) >= int(self.index):
                self.getRows(self.index, self.index + self.numRows())
        super(AbcTreeWidget, self).wheelEvent(event)

    def getRows(self, start, end):
        thread = ArrayThread(self, self.item.getObject(), start=start, end=end)
        self.connect(thread, QtCore.SIGNAL("arrayValue (PyQt_PyObject)"), self.handleAddValue)
        thread.start()

    def showValues(self, item):
        self.clear()
        self.item = item
        self.getRows(0, self.numRows())

    def handleAddValue(self, indexValue):
        self.index, value = indexValue
        self.addTopLevelItem(ArrayTreeWidgetItem(self, self.index, value))

