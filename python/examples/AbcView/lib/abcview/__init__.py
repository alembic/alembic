#! /usr/bin/env python
#-******************************************************************************
#
# Copyright (c) 2012-2013,
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
import math
import time
import logging
import traceback
from functools import wraps
from copy import deepcopy

from PyQt4 import QtCore
from PyQt4 import QtGui
from PyQt4 import uic

# alembic imports
import imath
import alembic

from abcview import style, config

# logging handler, imported by most other modules
FORMAT = '%(asctime)-15s %(message)s'
logging.basicConfig(format=FORMAT)
log = logging.getLogger(config.__prog__)
log.setLevel(int(os.environ.get('ABCVIEW_LOG_LEVEL', logging.WARN)))

import abcview
from abcview.io import Session, Scene, Camera, ICamera
from abcview.gl import GLCamera, GLICamera, GLScene
from abcview.widget.console_widget import AbcConsoleWidget
from abcview.widget.viewer_widget import GLWidget
from abcview.widget.viewer_widget import get_final_matrix
from abcview.widget.time_slider import TimeSlider
from abcview.widget.tree_widget import *
from abcview.utils import json

__doc__ = """
What's new:

- OpenGL viewer
- Camera support
- Frame selected object 
- Toggle visible items
- Explicit save layout
- Session management
- Python API

More information:
http://docs.alembic.io/python/abcview.html
"""

"""
TODO:

- GL picking in viewer
- highlight object selection in viewer
- better session cycle checking on load
- more stats (frame rate, poly count, mem usage)
- support for lights and materials
- draggable, pop-out widgets
- share state between processes / remote viewing
- save split window layouts
- support object paths in args, auto-frame on object
- unit tests
"""

def make_dirty(func):
    # make abcview session dirty decorator
    @wraps(func)
    def with_wrapped_func(*args, **kwargs):
        func(*args, **kwargs)
        if args[0].session:
            args[0].session.make_dirty()
    return with_wrapped_func

def make_clean(func):
    # make abcview session clean decorator
    @wraps(func)
    def with_wrapped_func(*args, **kwargs):
        func(*args, **kwargs)
        if args[0].session:
            args[0].session.make_clean()
    return with_wrapped_func

def message(info):
    dialog = QtGui.QMessageBox()
    dialog.setStyleSheet(style.DIALOG)
    dialog.setText(info)
    dialog.exec_()

class QScriptAction(QtGui.QGroupBox):
    def __init__(self, filepath, action):
        super(QScriptAction, self).__init__()
        self.action = action
        self.layout = QtGui.QHBoxLayout()
        self.layout.setMargin(0)
        self.layout.setSpacing(0)
        self.label = QtGui.QPushButton(os.path.basename(filepath))
        self.button = QtGui.QPushButton()
        self.button.setFixedSize(12, 12)
        self.button.setObjectName("edit_button")
        self.button.setIcon(QtGui.QIcon("%s/edit.png" % config.ICON_DIR))
        self.button.setIconSize(self.button.size())
        self.button.setFocusPolicy(QtCore.Qt.NoFocus)
        self.layout.addWidget(self.label)
        self.layout.addWidget(self.button)
        self.filepath = filepath
        self.setLayout(self.layout)
        self.label.pressed.connect(self.handle_clicked)
        self.button.pressed.connect(self.handle_edit)

    def handle_clicked(self):
        self.action.trigger()

    def handle_edit(self):
        os.system("gvim %s" % self.filepath)

class AbcMenuBar(QtGui.QMenuBar):
    def __init__(self, parent, main):
        super(AbcMenuBar, self).__init__(parent)
        self.main = main

        self.file_menu = QtGui.QMenu("File")
        self.widget_menu = QtGui.QMenu("Widgets")
        self.script_menu = QtGui.QMenu("Scripts")
        self.help_menu = QtGui.QMenu("Help")
        
        self.file_menu.setStyleSheet(style.MAIN)
        self.widget_menu.setStyleSheet(style.MAIN)
        self.script_menu.setStyleSheet(style.MAIN)
        self.help_menu.setStyleSheet(style.MAIN)
        
        self.setup_file_menu()
        self.setup_wid_menu()
        self.setup_script_menu()
        self.setup_help_menu()

    def setup_file_menu(self):
        self.file_menu.addAction("New", self.main.handle_new, "Ctrl+N")
        self.file_menu.addAction("Open", self.main.handle_open, "Ctrl+O")
        self.file_menu.addAction("Import", self.main.handle_import, "Ctrl+I")
        self.file_menu.addAction("Reload", self.main.handle_reload, "Ctrl+R")

        self.file_menu.addSeparator()
        self.file_menu.addAction("Save", self.main.handle_save, "Ctrl+S")
        self.file_menu.addAction("Save As..", self.main.handle_save_as, "Ctrl+Shift+S")
        self.file_menu.addSeparator()
        self.file_menu.addAction("Save Layout", self.main.save_settings, "Ctrl+Alt+S")
        self.file_menu.addAction("Reset Layout", self.main.reset_settings)
        self.file_menu.addSeparator()
        self.file_menu.addAction("Quit", self.main.close, "Ctrl+Q") 
        self.addMenu(self.file_menu)
        
    def setup_wid_menu(self):
        self.console_action = QtGui.QAction("Console", self)
        self.console_action.setShortcut("Ctrl+Shift+C")
        self.console_action.setCheckable(True)
        self.console_action.setChecked(True)
        self.connect(self.console_action, QtCore.SIGNAL("toggled (bool)"), self.handle_show_console)
        self.widget_menu.addAction(self.console_action)

        self.object_action = QtGui.QAction("Objects", self)
        self.object_action.setShortcut("Ctrl+Shift+O")
        self.object_action.setCheckable(True)
        self.object_action.setChecked(True)
        self.connect(self.object_action, QtCore.SIGNAL("toggled (bool)"), self.handle_show_objects)
        self.widget_menu.addAction(self.object_action)

        self.props_action = QtGui.QAction("Properties", self)
        self.props_action.setShortcut("Ctrl+Shift+P")
        self.props_action.setCheckable(True)
        self.props_action.setChecked(True)
        self.connect(self.props_action, QtCore.SIGNAL("toggled (bool)"), self.handle_show_props)
        self.widget_menu.addAction(self.props_action)

        self.time_slider_action = QtGui.QAction("Timeline", self)
        self.time_slider_action.setShortcut("Ctrl+Shift+T")
        self.time_slider_action.setCheckable(True)
        self.time_slider_action.setChecked(True)
        self.connect(self.time_slider_action, QtCore.SIGNAL("toggled (bool)"), self.handle_show_timeline)
        self.widget_menu.addAction(self.time_slider_action)

        self.viewer_action = QtGui.QAction("Viewer", self)
        self.viewer_action.setShortcut("Ctrl+Shift+V")
        self.viewer_action.setCheckable(True)
        self.viewer_action.setChecked(True)
        self.connect(self.viewer_action, QtCore.SIGNAL("toggled (bool)"), self.handle_show_viewer)
        self.widget_menu.addAction(self.viewer_action)

        self.addMenu(self.widget_menu)

    def setup_script_menu(self):
        self.addMenu(self.script_menu)
        self.handle_refresh_scripts()

    def setup_help_menu(self):
        self.help_menu.addAction("About Alembic", self.handle_about_abc)
        self.help_menu.addAction("About AbcView", self.handle_about_abcview)
        self.addMenu(self.help_menu)

    def handle_refresh_scripts(self):
        from glob import glob
        self.script_menu.clear()
        self.script_menu.addAction("Refresh", self.handle_refresh_scripts)
        self.script_menu.addSeparator()

        def find_scripts(path):
            #HACK: need script registration
            if not path:
                return
            for f in glob(os.path.join(path, "*.py")):
                name = os.path.basename(f)
                if name == "__init__.py":
                    continue
                
                script_act = QtGui.QWidgetAction(self.script_menu)
                script_act.setDefaultWidget(QScriptAction(f, script_act))
                script_act.setData(f)
                script_act.triggered.connect(self.handle_run_script)
                self.script_menu.addAction(script_act)

        find_scripts(config.SCRIPT_DIR)
        find_scripts(config.USER_SCRIPT_DIR)

    def handle_run_script(self):
        script_path = str(self.sender().data().toString())
        self.main.load_script(script_path)

    def handle_show_console(self, toggled):
        self.main.toggle_widget(self.main.console)

    def handle_show_objects(self):
        self.main.toggle_widget(self.main.objects_group)

    def handle_show_props(self):
        self.main.toggle_widget(self.main.properties_splitter)

    def handle_show_timeline(self):
        self.main.toggle_widget(self.main.time_slider_toolbar)
    
    def handle_show_viewer(self):
        self.main.toggle_widget(self.main.viewer_group)

    def handle_about_abc(self):
        message("Using Alembic %s" % alembic.Abc.GetLibraryVersionShort())

    def handle_about_abcview(self):
        _v = " ".join([config.__prog__, config.__version__])
        message("\n".join([_v, __doc__]))

class FindLineEdit(QtGui.QLineEdit):
    # auto-unfocus line editor
    def __init__(self, parent):
        super(FindLineEdit, self).__init__(parent)
        self._parent = parent
        self.setSizePolicy(QtGui.QSizePolicy.Expanding, 
                                        QtGui.QSizePolicy.Maximum)
        self.setFocusPolicy(QtCore.Qt.ClickFocus)

    def leaveEvent(self, event):
        self._parent.setFocus()
        super(FindLineEdit, self).leaveEvent(event)

class Splash(QtGui.QSplashScreen):
    """
    AbcView splash screen.
    """
    def __init__(self, parent):
        super(Splash, self).__init__(parent)
        self._parent = parent
        self.setStyleSheet(style.SPLASH)
        self.setWindowFlags(QtCore.Qt.WindowStaysOnTopHint)

        # logo
        self.logo = QtGui.QLabel()
        self.logo.setPixmap(QtGui.QPixmap("%s/logo.png" % config.ICON_DIR))
       
        self.resize(600, 350)
        self.move(0, 0) 

        # layout
        layout = QtGui.QVBoxLayout()
        layout.setSpacing(0)
        layout.setMargin(0)
        layout.addWidget(self.logo)
        self.text = QtGui.QLineEdit()
        self.progress = QtGui.QProgressBar()
        self.progress.setMaximum(100)
        self.progress.setMinimum(0)
        self.progress.setValue(0)
        self.progress.setTextVisible(False)
        layout.addWidget(self.progress)
        layout.addWidget(self.text)
        self.setLayout(layout)

    def updateProgress(self, value):
        self.progress.setValue(value)

    def setMessage(self, message):
        self.text.setText(message)

## MAIN -----------------------------------------------------------------------
class AbcView(QtGui.QMainWindow):
    TITLE = " ".join([config.__prog__, config.__version__])
    def __init__(self, filepath=None, first_frame=None, last_frame=None):
        """
        Creates an instance of the AbcView Main Window.

        :param filepath: file to load (.io or .abc)
        :param first_frame: set default first frame value, if None then
               derive the first frame from filepath
        :param last_frame: set default last frame value, if None then
               derive the first frame from filepath
        """
        QtGui.QMainWindow.__init__(self)
        self.setWindowState(QtCore.Qt.WindowActive)
        self.setWindowFlags(QtCore.Qt.Window)
        self.setWindowTitle(self.TITLE)
        self.setStyle(QtGui.QStyleFactory.create('cleanlooks'))
        self.setStyleSheet(style.MAIN)
        self.setMinimumSize(200, 200)
        self.setFocusPolicy(QtCore.Qt.StrongFocus)

        # deferred load list
        self._load_list = []

        # setting these should trigger updates
        self.first_frame = first_frame
        self.last_frame = last_frame

        # session data
        self.settings = QtCore.QSettings("Alembic", 
                           "-".join([config.__prog__, config.__version__]))
        self.session = Session(filepath)

        # main menu
        self.main_menu = AbcMenuBar(self, main=self)
        self.setMenuBar(self.main_menu)

        self.objects_group = QtGui.QGroupBox(self)
        self.objects_group.setLayout(QtGui.QVBoxLayout())
        self.objects_tree = ObjectTreeWidget(self, main=self)
        self.objects_tree.signal_view_camera.connect(self.handle_view_camera)
        self.find_line_edit = FindLineEdit(self)
        self.objects_group.layout().setSpacing(0)
        self.objects_group.layout().setMargin(0)
        self.objects_group.layout().addWidget(self.find_line_edit)
        self.objects_group.layout().addWidget(self.objects_tree)

        # some tree widgets
        self.properties_tree = PropertyTreeWidget(self, main=self)
        self.samples_tree = SampleTreeWidget(self, main=self)
        self.array_tree = ArrayTreeWidget(self, main=self)

        # console widget
        self.console = AbcConsoleWidget(self)
        self.console.updateNamespace({
            'exit': self.console.exit,
            'find': self.find,
            'self': self,
            'objects': self.objects_tree,
            'properties': self.properties_tree,
            'samples': self.samples_tree,
            'alembic': alembic,
            'abcview': abcview
            })

        #HACK: better way to do this? 
        # remap some session methods for GL wrapping
        self.session.add_file = self.import_file

        # viewer
        self.viewer = GLWidget(self)
        self.viewer_group = QtGui.QGroupBox(self)
        self.viewer_group.setLayout(QtGui.QVBoxLayout())
        self.viewer_group.layout().setSpacing(0)
        self.viewer_group.layout().setMargin(0)
        self.viewer_group.layout().addWidget(self.viewer)
        
        # viewer/state connections
        self.viewer.signal_scene_error.connect(self.handle_viewer_error)
        self.viewer.signal_scene_opened.connect(self.handle_scene_opened)
        self.viewer.signal_set_camera.connect(self.handle_set_camera)
        self.viewer.signal_new_camera.connect(self.handle_new_camera)
        self.viewer.state.signal_current_frame.connect(self.handle_update_frame)
        self.viewer.state.signal_play_fwd.connect(self.handle_state_play_fwd)
        self.viewer.state.signal_play_stop.connect(self.handle_state_play_stop)
        self.viewer.signal_object_selected.connect(self.handle_object_selected)
        
        # time slider
        self.time_slider = TimeSlider(self)
        self.time_slider.setFocus(True)
        self.time_slider.signal_play_fwd.connect(self.handle_play)
        self.time_slider.signal_play_stop.connect(self.handle_stop)
        self.time_slider.signal_frame_changed.connect(self.handle_time_slider_change)
        self.time_slider.signal_first_frame_changed.connect(self.handle_first_frame_change)
        self.time_slider.signal_last_frame_changed.connect(self.handle_last_frame_change)
        self.time_slider_toolbar = QtGui.QToolBar(self)
        self.time_slider_toolbar.setObjectName("time_slider_toolbar")
        self.time_slider_toolbar.addWidget(self.time_slider)
        self.time_slider_toolbar.setMovable(False)
        self.addToolBar(QtCore.Qt.BottomToolBarArea, self.time_slider_toolbar)

        # splitters
        self.main_splitter = QtGui.QSplitter(QtCore.Qt.Vertical, self)
        self.console_splitter = QtGui.QSplitter(QtCore.Qt.Vertical, self)
        self.objects_splitter = QtGui.QSplitter(QtCore.Qt.Horizontal, self)
        self.properties_splitter = QtGui.QSplitter(QtCore.Qt.Vertical, self)
        self.properties_splitter.addWidget(self.properties_tree)
        self.properties_splitter.addWidget(self.samples_tree)
        self.properties_splitter.addWidget(self.array_tree)
        self.objects_splitter.addWidget(self.objects_group)
        self.objects_splitter.addWidget(self.viewer_group)
        self.objects_splitter.addWidget(self.properties_splitter)
        self.main_splitter.addWidget(self.objects_splitter)
        self.console_splitter.addWidget(self.main_splitter)
        self.console_splitter.addWidget(self.console)
        self.setCentralWidget(self.console_splitter)

        # TODO: refactor signals to new signal object method
        self.properties_tree.connect(self.objects_tree, QtCore.SIGNAL("itemSelectionChanged()"),
                self.properties_tree.clear)
        self.samples_tree.connect(self.properties_tree, QtCore.SIGNAL("itemSelectionChanged()"),
                self.samples_tree.clear)
        self.array_tree.connect(self.samples_tree, QtCore.SIGNAL("itemSelectionChanged()"),
                self.array_tree.clear)
        self.connect(self.objects_tree, QtCore.SIGNAL("itemLoaded (PyQt_PyObject)"), 
                self.handle_item_loaded)
        self.connect(self.objects_tree, QtCore.SIGNAL("itemUnloaded (PyQt_PyObject)"), 
                self.handle_item_unloaded)
        self.properties_tree.connect(self.objects_tree, QtCore.SIGNAL("itemClicked (PyQt_PyObject)"), 
                self.handle_object_clicked)
        self.samples_tree.connect(self.properties_tree, QtCore.SIGNAL("itemClicked (PyQt_PyObject)"), 
                self.handle_property_clicked)
        self.array_tree.connect(self.samples_tree, QtCore.SIGNAL("itemClicked (PyQt_PyObject)"), 
                self.array_tree.show_values)
        self.connect(self.find_line_edit, QtCore.SIGNAL("returnPressed ()"), 
                self.handle_find)
   
        # wait for main event loop to start
        QtGui.QApplication.instance().signal_starting_up.connect(self._start)
       
        # create the splash screen
        self.splash = Splash(self)

        # open a session
        if filepath and filepath.endswith(Session.EXT):
            self.open_session(filepath)

    def set_load_list(self, filepaths):
        self._load_list = deepcopy(filepaths)
        self.splash.progress.setMaximum(len(filepaths))

    def _start(self):
        """
        This is the startup callback function for when the QApplication starts its
        event loop. This handles deferred file loading.
        """
        start = time.time()
        self.splash.show()

        # load session files
        if len(self._load_list) == 1 and self._load_list[0].endswith(Session.EXT):
            try:
                self._open_session(self._load_list[0])
            except Exception, e:
                log.error("Error loading %s" % self._load_list[0])
                traceback.print_exc()

        # load scene files
        else:
            try:
                for filepath in self._load_list:
                    self.import_file(filepath)
                if len(self._load_list) > 0:
                    self.viewer.frame()

            except Exception, e:
                log.error("Error loading %s" % filepath)
                traceback.print_exc()

        log.debug("session loaded in %.2fs"  % (time.time() - start))
        self.splash.close()

    @make_clean
    def clear(self):
        """
        Clears session and viewer.
        """
        self.session.clear()
        self.viewer.clear()
        self.objects_tree.clear()
        self.properties_tree.clear()
        self.samples_tree.clear()
        self.array_tree.clear()

    def confirm_close(self, message):
        """
        Window close confirmation.

        :param message: text to display
        """
        msg = QtGui.QMessageBox()
        msg.setStyleSheet(style.DIALOG)
        msg.setText(message);
        msg.setInformativeText("Do you want to save your changes?");
        msg.setStandardButtons(QtGui.QMessageBox.Save | QtGui.QMessageBox.Discard | QtGui.QMessageBox.Cancel);
        msg.setDefaultButton(QtGui.QMessageBox.Save);
        return msg.exec_()

    ## settings

    def reset_settings(self):
        """
        Resets the AbcView layout settings to default values.
        """
        self.settings.clear()
        
        # default position settings
        self.setGeometry(400, 250, 550, 500)

        # default splitter locations
        self.objects_splitter.setSizes([200, 200, 200])

        # default widgets settings
        self.main_menu.object_action.setChecked(False)
        self.main_menu.props_action.setChecked(False)
        self.main_menu.console_action.setChecked(False)
        self.main_menu.viewer_action.setChecked(True)
        self.main_menu.time_slider_action.setChecked(True)

        # default viewer settings
        self.viewer.resize(self.width(), self.height())
        self.viewer.draw_hud = False

    def load_settings(self):
        """
        Loads AbcView layout settings from a PyQt settings file.
        """
        geom = self.settings.value('geometry')
        if geom.isNull() or not geom.isValid():
            self.reset_settings()
            return

        # general settings
        self.restoreGeometry(geom.toByteArray())
        self.restoreState(self.settings.value('window_state').toByteArray())
       
        # layout settings
        self.settings.beginGroup("layout")
        self.main_splitter.restoreState(
                self.settings.value('main_splitter').toByteArray())
        self.objects_splitter.restoreState(
                self.settings.value('objects_splitter').toByteArray())
        self.properties_splitter.restoreState(
                self.settings.value('properties_splitter').toByteArray())
        self.console_splitter.restoreState(
                self.settings.value('console_splitter').toByteArray())
        self.main_menu.object_action.setChecked(not self.settings.value('objects_hidden').toBool())
        self.objects_group.setHidden(self.settings.value('objects_hidden').toBool())
        self.main_menu.props_action.setChecked(not self.settings.value('properties_hidden').toBool())
        self.properties_splitter.setHidden(self.settings.value('properties_hidden').toBool())
        self.main_menu.console_action.setChecked(not self.settings.value('console_hidden').toBool())
        self.console.setHidden(self.settings.value('console_hidden').toBool())
        self.main_menu.time_slider_action.setChecked(not self.settings.value('time_slider_hidden').toBool())
        self.time_slider_toolbar.setHidden(self.settings.value('time_slider_hidden').toBool())
        self.main_menu.viewer_action.setChecked(not self.settings.value('viewer_hidden').toBool())
        self.viewer_group.setHidden(self.settings.value('viewer_hidden').toBool())
        self.settings.endGroup()

        # restore viewer settings
        self.settings.beginGroup("viewer")
        self.viewer.restoreGeometry(self.settings.value("geometry").toByteArray())

        # drawing mode
        mode, found = self.settings.value("draw_mode").toInt()
        if found:
            self.viewer.mode = mode

        self.settings.endGroup()

    def save_settings(self, settings=None):
        """
        Saves layout settings to a PyQt settings file.
        """
        if settings is None:
            settings = self.settings

        # general settings
        settings.setValue("geometry", self.saveGeometry())
        settings.setValue("window_state", self.saveState())

        # layout settings
        settings.beginGroup("layout")
        settings.setValue("main_splitter", self.main_splitter.saveState())
        settings.setValue("objects_splitter", self.objects_splitter.saveState())
        settings.setValue("properties_splitter", self.properties_splitter.saveState())
        settings.setValue("console_splitter", self.console_splitter.saveState())
        settings.setValue("objects_hidden", self.objects_group.isHidden())
        settings.setValue("properties_hidden", self.properties_splitter.isHidden())
        settings.setValue("console_hidden", self.console.isHidden())
        settings.setValue("time_slider_hidden", self.time_slider_toolbar.isHidden())
        settings.setValue("viewer_hidden", self.viewer_group.isHidden())
        settings.endGroup()

        # save viewer settings
        settings.beginGroup("viewer")
        settings.setValue("geometry", self.saveGeometry())

        # save timeslider settings
        settings.endGroup()

    @make_dirty
    def toggle_widget(self, widget):
        """
        Toggles visibilty of given widget.

        :param widget: AbcView widget object.
        """
        if widget.isHidden():
            widget.show()
        else:
            widget.hide()

    ## file io

    def _wait(self):
        """
        Waits for the window to be drawn before proceeding.
        """
        return
        app = QtGui.QApplication.instance()
        while not app.ok:
            pass
        while QtGui.QApplication.instance().startingUp():
            pass
        while not self.isVisible():
            pass

    def open_session(self, filepath):
        self.set_load_list([filepath])
        self._start()

    @make_clean
    def _open_session(self, filepath):
        """
        Loads a session file and swaps out IO objects for GL objects.
        """
        log.debug("[AbcView._open_session] %s" % filepath)
        self.splash.setMessage("loading")

        # validate window state
        self._wait()

        if not filepath or not os.path.isfile(filepath):
            message("invalid file: %s" % filepath)
            raise

        # clear session
        self.clear()

        # load an alembic file
        if filepath.endswith(Scene.EXT):
            self.import_file(filepath)
            self.viewer.frame()

        # load an abcview file
        elif filepath.endswith(Session.EXT):
            self.splash.setMessage("loading %s" % filepath)
            
            self.session = Session(filepath)

            # set frame range
            if self.first_frame is not None:
                self.viewer.state.min_frame = self.first_frame
            else:
                self.viewer.state.min_time = self.session.min_time
            if self.last_frame is not None:
                self.viewer.state.max_frame = self.last_frame
            else:
                self.viewer.state.max_time = self.session.max_time

            #HACK: transform io.Cameras to gl.Cameras
            for camera in self.session.cameras:
                self.session.remove_camera(camera)
                if camera.type() == Camera.type():
                    _camera = GLCamera(self.viewer, camera.name)
                elif camera.type() == ICamera.type():
                    _camera = GLICamera(self.viewer, camera.icamera)
                for attr in _camera.SERIALIZE:
                    setattr(_camera, attr, getattr(camera, attr))
                self.viewer.add_camera(_camera)
                if camera.loaded:
                    self.viewer.set_camera(_camera)

            self.splash.progress.setMaximum(len(self.session.items))

            #HACK: transform io.Scenes to gl.Scenes
            for index in range(len(self.session.items)):
                item = self.session.items[0]
                if item.filepath.endswith(Scene.EXT):
                    _item = GLScene(item.filepath)
                    _item.properties = deepcopy(item.properties)
                    _item.loaded = item.loaded
                    _item.visible = item.loaded
                    tree_item = SceneTreeWidgetItem(self.objects_tree, _item)
                    self.session.remove_item(item)
                    self.session.add_item(_item)
                else:
                    tree_item = SessionTreeWidgetItem(self.objects_tree, item)

                if item.loaded:
                    tree_item.load()

                self.splash.setMessage("loading %s" % item.name)
                self.splash.updateProgress(self.splash.progress.value()+1)
           
        # restore current frame after loading everything
        self.viewer.state.frames_per_second = self.session.frames_per_second
        self.viewer.state.current_frame = self.session.current_time * \
                                     self.session.frames_per_second

        # update window title
        self.setWindowTitle(" - ".join([self.TITLE, filepath]))

    @make_clean
    def save_session(self, filepath=None):
        """
        Saves the current AbcView session to  a given filepath, or
        to the current filepath if filepath is None.

        :param filepath: save to filepath (passing no args or None
                         overrwrites current session file.
        """
        try:
            self.session.min_time = self.viewer.state.min_time
            self.session.max_time = self.viewer.state.max_time
            self.session.current_time = self.viewer.state.current_time
            self.session.frames_per_second = self.viewer.state.frames_per_second
            self.session.save(filepath)
        except Exception, e:
            traceback.print_exc()
            message("Error saving file:\n\n%s" % str(e))

    def load_script(self, script):
        """
        Loads and executes a python AbcView script.

        :param script: code or path to file containing python code.
        """
        log.debug("[AbcView.load_script] %s" % script)
        if os.path.exists(script):
            lines = open(script, "r").readlines()
            for line in lines:
                self.console.setCommand(line.strip())
                self.console.runCommand()
        else:
            self.console.setCommand(script)
            self.console.runCommand()

    @make_dirty
    def import_file(self, filepath):
        """
        Imports a file into the current session.

        :param filepath: path to scene or session file.
        """
        # validate window state
        self._wait()

        if not filepath or not os.path.isfile(filepath):
            message("invalid file: %s" % filepath)
            raise
        
        self.splash.setMessage("loading %s" % filepath)
    
        # add the item to the session
        item = GLScene(filepath)
        self.session.add_item(item)

        if self.viewer_group.isHidden():
            item.loaded = False
        else:
            item.loaded = True
        if filepath.endswith(Scene.EXT):
            tree_item = SceneTreeWidgetItem(self.objects_tree, item)
        elif filepath.endswith(Session.EXT):
            tree_item = SessionTreeWidgetItem(self.objects_tree, item)

        # load the item into the viewer
        if item.loaded:
            tree_item.load()

    ## event handlers

    def handle_play(self):
        """
        Playback handler, starts playing the viewer.
        """
        self.viewer.state.play()

    def handle_stop(self):
        """
        Viewer playback stop handler, stops the viewer playback.
        """
        self.viewer.state.stop()

    @make_dirty
    def handle_delete(self):
        log.info("remove item")

    def handle_scene_opened(self, scene):
        """
        Scene open signal handler.
        """
        frame_range = self.viewer.state.frame_range()
        self.time_slider.set_minimum(frame_range[0])
        self.time_slider.set_maximum(frame_range[1])

    def handle_viewer_error(self, msg):
        message(msg)

    def handle_time_slider_change(self, frame):
        """
        handles frame changes coming from time slider

        :param frame: frame number
        """
        self.viewer.state.current_frame = frame

    def handle_first_frame_change(self, frame):
        """
        handles first frame change from time slider

        :param frame: frame number
        """
        self.viewer.state.min_frame = frame
        if self.session.frames_per_second > 0:
            self.session.min_time = frame / self.session.frames_per_second

    def handle_last_frame_change(self, frame):
        """
        handles last frame change from time slider

        :param frame: frame number
        """
        self.viewer.state.max_frame = frame
        if self.session.frames_per_second > 0:
            self.session.max_time = frame / self.session.frames_per_second

    def handle_state_play_fwd(self):
        """
        viewer state play foward signal handler
        """
        self.time_slider.playing = True

    def handle_state_play_stop(self):
        """
        viewer state play stop signal handler
        """
        self.time_slider.playing = False

    def handle_update_frame(self, frame):
        """
        handles frame changes coming from viewer
        """
        self.time_slider.set_value(frame)

        # update the samples tree selected item
        if not self.properties_splitter.isHidden() and \
            self.samples_tree.topLevelItemCount() > 0:
            p = self.properties_tree.selected()
            ts = p.getTimeSampling()
            t = self.time_slider.value() \
                    / float(self.viewer.state.frames_per_second)
            index = ts.getNearIndex(t, len(p.samples))    
            item = self.samples_tree.topLevelItem(index)
            self.samples_tree.clearSelection()
            self.samples_tree.setItemSelected(item, True)
            self.samples_tree.scrollToItem(item)
   
    @make_dirty 
    def handle_new_camera(self, camera):
        """
        handles adding a new camera to the session

        :param camera: GLCamera
        """
        log.debug("[AbcView.handle_new_camera] %s" % camera)
        if camera.name != "interactive":
            self.session.add_camera(camera)

    @make_dirty
    def handle_set_camera(self, camera):
        """
        handles setting the camera name on the session

        :param camera: GLCamera
        """
        if camera.name != "interactive":
            self.session.set_camera(camera)

    @make_dirty
    def handle_view_camera(self, item):
        """
        object tree 'view through selected' menu handler

        :param item: CameraTreeWidgetItem
        """
        icamera = item.camera()
        if icamera.getName() not in [c.name for c in self.viewer.state.cameras]:
            camera = GLICamera(self.viewer, icamera)
            self.viewer.add_camera(camera)
            self.viewer.set_camera(camera.name)
        else:
            self.viewer.set_camera(icamera.getName())

    def handle_object_selected(self, name):
        self.objects_tree.find(str(name.toAscii()))

    def find(self, name):
        """
        searches for objects in the tree matching name
        """
        self.setFocus()
        self.objects_tree.find(name)

    def handle_find(self, text=None):
        """
        handles input from the search box
        """
        if text is None:
            text = self.find_line_edit.text()
        if text:
            self.find(str(text.toAscii()))

    def handle_item_loaded(self, item):
        """
        load checkbox click handler

        :param item: SceneTreeWidgetItem, SessionTreeWidgetItem
        """
        def load(item):
            if type(item) == SceneTreeWidgetItem:
                self.viewer.add_scene(item.object)
            elif type(item) == SessionTreeWidgetItem:
                for child in item.children():
                    load(child)
        log.debug("[AbcView.handle_item_loaded] %s" % item.object)
        load(item)

    def handle_item_unloaded(self, item):
        """
        Item unloaded handler.

        :param item: ObjectTreeWidgetItem
        """
        self.viewer.remove_scene(item.object)

    def handle_object_clicked(self, item):
        """
        Object tree item clicked handler.

        :param item: ObjectTreeWidgetItem
        """
        self.samples_tree.clear()
        self.array_tree.clear()
        self.properties_tree.show_properties(item)
        
    def handle_property_clicked(self, item):
        """
        Property tree item clicked handler.

        :param item: PropertyTreeWidgetItem
        """
        self.array_tree.clear()
        self.samples_tree.show_samples(item)

    def handle_new(self):
        """
        File->New menu handler
        """
        self.clear()
        self.setWindowTitle(self.TITLE)

    def handle_open(self):
        """
        File->Open menud handler
        """
        filepath = QtGui.QFileDialog.getOpenFileName(self, "Open File", 
                    os.getcwd(), ("Alembic Files (*.%s *.%s)" 
                        % (Scene.EXT, Session.EXT)))
        if filepath:
            self.open_session(str(filepath.toAscii()))

    def handle_import(self):
        """
        File->Import menud handler
        """
        filepath = QtGui.QFileDialog.getOpenFileName(self, "Open File", 
                    os.getcwd(), ("Alembic Files (*.%s *.%s)" 
                        % (Scene.EXT, Session.EXT)))
        if filepath:
            self.import_file(str(filepath.toAscii()))

    def handle_reload(self):
        """
        File->Reload menud handler
        """
        self.open_session(self.session.filepath)

    def handle_frame_scene(self):
        """
        Scene level framing handler
        """
        self.viewer.frame()

    def handle_frame_object(self, obj=None):
        """
        Object level framing handler
        """
        if obj is None:
            obj = self.objects_tree.selected()
        if obj.getFullName() == "/":
            self.handle_frame_scene()
        md = obj.getMetaData()
        if alembic.AbcGeom.IPolyMesh.matches(md) or \
           alembic.AbcGeom.ISubD.matches(md):
            meshObj = alembic.AbcGeom.IPolyMesh(obj.getParent(), obj.getName())
            mesh = meshObj.getSchema()
            bounds_prop = mesh.getSelfBoundsProperty()
        elif alembic.AbcGeom.IXform.matches(md):
            meshObj = alembic.AbcGeom.IXform(obj.getParent(), obj.getName())
            mesh = meshObj.getSchema()
            bounds_prop = mesh.getChildBoundsProperty()
        else:
            return
        xf = get_final_matrix(meshObj)
        iss = alembic.Abc.ISampleSelector(0)
        if not bounds_prop.valid():
            message("Object has invalid or no bounds set.")
            return

        bounds = bounds_prop.getValue(iss)
  
        # apply local transforms
        if 0:
            min = bounds.min()
            max = bounds.max()
            item = self.objects_tree.selectedItems()[0]
            scene = item.scene().object
            if scene.properties.get("translate"):
                max = max * imath.V3d(*scene.translate)
                min = min * imath.V3d(*scene.translate)
            if scene.properties.get("scale"):
                max = max * imath.V3d(*scene.scale)
                min = min * imath.V3d(*scene.scale)
            bounds = imath.Box3d(min, max)

        self.viewer.frame(bounds * xf)

    def handle_save(self):
        if not self.session.filepath:
            self.handle_save_as()
        else:
            self.save_session()

    def handle_save_as(self):
        filepath = QtGui.QFileDialog.getSaveFileName(self, "Save File", 
                    os.getcwd(), ("Alembic Files (*.%s)" % Session.EXT))
        if filepath:
            self.save_session(str(filepath.toAscii()))

    ## base class overrides

    def resizeEvent(self, event):
        self.splash.move((event.size().width() / 2.0) - (self.splash.width() / 2.0), 
                         (event.size().height() / 2.0) - (self.splash.height() / 2.0))
        super(AbcView, self).resizeEvent(event)

    def keyPressEvent(self, event):
        """
        Handles key press events for the main application. If nothing
        matches, it defers to the viewer.
        """
        if event.key() == QtCore.Qt.Key_F:
            if len(self.objects_tree.selectedItems()) == 0:
                self.handle_frame_scene()
            else:
                self.handle_frame_object()
        elif event.key() == QtCore.Qt.Key_Space:
            self.handle_play()
        elif event.key() == QtCore.Qt.Key_Backspace:
            self.handle_delete()
        else:
            self.viewer.keyPressEvent(event)

    def closeEvent(self, event):
        if self.session and self.session.is_dirty():
            resp = self.confirm_close("This session has changed.")
            if resp == QtGui.QMessageBox.Cancel:
                event.ignore()
                return
            elif resp == QtGui.QMessageBox.Save:
                self.handle_save()
        super(AbcView, self).closeEvent(event)

class App(QtGui.QApplication):
    """
    QApplication subclass that emits a "startup" signal after
    the event loop has started. This may trigger deferred
    file loading, among other things.
    """
    signal_starting_up = QtCore.pyqtSignal()

    def __init__(self, args):
        super(App, self).__init__(args)
        # trigger the callback after the main event loop starts
        QtCore.QTimer.singleShot(0, self.starting)

    def starting(self):
        """
        Callback function for Timer timeout.
        """
        self.signal_starting_up.emit()
