# simple abcview example script
__doc__ = "Changes selected scene color"
from PyQt4 import QtGui
from abcview.io import Mode
color = QtGui.QColorDialog(self).getColor()
item = objects.selectedItems()[0]
item.scene().set_color(color)
self.viewer.updateGL()
