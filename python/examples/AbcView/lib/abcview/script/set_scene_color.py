from PyQt4 import QtGui
from abcview.io import Mode
color = QtGui.QColorDialog(self).getColor()
item = objects.selectedItems()[0]
item.object.properties["color"] = (color.red(), color.green(), color.blue())
self.viewer.updateGL()
