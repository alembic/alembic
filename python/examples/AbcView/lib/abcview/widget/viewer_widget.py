#-******************************************************************************
#
# Copyright (c) 2013,
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
import sys
import math
import traceback
from functools import wraps

from PyQt4 import QtCore
from PyQt4 import QtGui
from PyQt4 import uic
from PyQt4 import QtOpenGL

import numpy
import numpy.linalg as linalg
import OpenGL
OpenGL.ERROR_CHECKING = True
from OpenGL.GL import *
from OpenGL.GLU import *

import imath
import alembic

import abcview
from abcview.io import Mode
from abcview.gl import AbcGLCamera, AbcGLICamera, AbcGLScene
from abcview import log, config

kWrapExisting = alembic.Abc.WrapExistingFlag.kWrapExisting

# GL drawing mode map
GL_MODE_MAP = {
    Mode.FILL: GL_FILL,
    Mode.LINE: GL_LINE,
    Mode.POINT: GL_POINT
}

# scene and camera caches
SCENES = {}
CAMERAS = {}

def accumXform(xf, obj):
    if alembic.AbcGeom.IXform.matches(obj.getHeader()):
        x = alembic.AbcGeom.IXform(obj, kWrapExisting)
        xs = x.getSchema().getValue()
        xf *= xs.getMatrix()

def get_final_matrix(obj):
    xf = imath.M44d()
    xf.makeIdentity()
    parent = obj.getParent()
    while parent:
        accumXform(xf, parent)
        parent = parent.getParent()
    return xf

def update_camera(func):
    """
    GL camera update decorator
    """
    @wraps(func)
    def with_wrapped_func(*args, **kwargs):
        func(*args, **kwargs)
        wid = args[0]
        if hasattr(wid, "apply") and wid.camera is not None:
            wid.camera.apply()
        wid.updateGL()
    return with_wrapped_func

def set_ambient_light():
    glDisable(GL_LIGHT0)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (0.4, 0.4, 0.4, 1.0))

def set_diffuse_light():
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE)
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE)
    glEnable(GL_LIGHTING)
    glEnable(GL_LIGHT0)
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (0.1, 0.1, 0.1, 1.0))

def set_default_materials():
    """
    Hardcoded GL material shaders.
    """
    mat_specular = (1.0, 1.0, 1.0, 1.0)
    mat_shininess = 100.0
    mat_front_emission = (0.0, 0.0, 0.0, 0.0)
    mat_back_emission = (1.0, 0.0, 0.0, 1.0)

    glClearColor(0.12, 0.12, 0.12, 0.0)
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_front_emission)
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular)
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess)

    glMaterialfv(GL_BACK, GL_EMISSION, mat_back_emission)
    glMaterialfv(GL_BACK, GL_SPECULAR, mat_specular)
    glMaterialfv(GL_BACK, GL_SHININESS, mat_shininess)

    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE)
    glEnable(GL_COLOR_MATERIAL)

def draw_bounding_box(bounds):
    """
    Draws a bounding box given a box3d object.

    :param bounds: Imath.box3d
    """
    min_x = bounds.min()[0]
    min_y = bounds.min()[1]
    min_z = bounds.min()[2]
    max_x = bounds.max()[0]
    max_y = bounds.max()[1]
    max_z = bounds.max()[2]
    
    w = max_x - min_x
    h = max_y - min_y
    d = max_z - min_z

    glBegin(GL_LINES)
    glVertex3f(min_x, min_y, min_z)
    glVertex3f(min_x+w, min_y, min_z)
    glVertex3f(min_x, min_y, min_z)
    glVertex3f(min_x, min_y+h, min_z)
    glVertex3f(min_x, min_y, min_z)
    glVertex3f(min_x, min_y, min_z+d)
    glVertex3f(min_x+w, min_y, min_z)
    glVertex3f(min_x+w, min_y+h, min_z)
    glVertex3f(min_x+w, min_y+h, min_z)
    glVertex3f(min_x, min_y+h, min_z)
    glVertex3f(min_x, min_y, min_z+d)
    glVertex3f(min_x+w, min_y, min_z+d)
    glVertex3f(min_x+w, min_y, min_z+d)
    glVertex3f(min_x+w, min_y, min_z)
    glVertex3f(min_x, min_y, min_z+d)
    glVertex3f(min_x, min_y+h, min_z+d)
    glVertex3f(min_x, min_y+h, min_z+d)
    glVertex3f(min_x, min_y+h, min_z)
    glVertex3f(min_x+w, min_y+h, min_z)
    glVertex3f(min_x+w, min_y+h, min_z+d)
    glVertex3f(min_x+w, min_y, min_z+d)
    glVertex3f(min_x+w, min_y+h, min_z+d)
    glVertex3f(min_x, min_y+h, min_z+d)
    glVertex3f(min_x+w, min_y+h, min_z+d)

    cx, cy, cz = bounds.center()
    glVertex3f(cx-1, cy, cz)
    glVertex3f(cx+1, cy, cz)
    glVertex3f(cx, cy-1, cz)
    glVertex3f(cx, cy+1, cz)
    glVertex3f(cx, cy, cz-1)
    glVertex3f(cx, cy, cz+1)

    glEnd()

def create_viewer_app(filepath=None):
    """
    Creates a standalone viewer app. ::

        >>> from abcview.widget.viewer_widget import create_viewer_app
        >>> create_viewer_app("file.abc")

    """
    app = QtGui.QApplication(sys.argv)

    # create the viewer widget
    viewer = AbcGLWidget()
    viewer_group = QtGui.QGroupBox()
    viewer_group.setLayout(QtGui.QVBoxLayout())
    viewer_group.layout().setSpacing(0)
    viewer_group.layout().setMargin(0)
    viewer_group.layout().addWidget(viewer)
    viewer_group.setWindowTitle("AbcGLWidget")

    # set default size
    viewer_group.setMinimumSize(QtCore.QSize(100, 100))
    viewer_group.resize(500, 300)

    # display the viewer app
    viewer_group.show()
    viewer_group.raise_()

    # override key press event handler
    viewer_group.keyPressEvent = viewer.keyPressEvent

    # open a file
    if filepath:
        if os.path.exists(filepath):
            viewer.add_file(filepath)
        else:
            log.warn("file not found: %s" % filepath)

    return app.exec_()

def message(info):
    dialog = QtGui.QMessageBox()
    dialog.setText(info)
    dialog.exec_()

#TODO: support wipes between cameras
class GLSplitter(QtGui.QSplitter):
    def __init__(self, orientation, wipe=False):
        super(GLSplitter, self).__init__(orientation)
        self.wipe = wipe
        #self.splitterMoved.connect(self.handle_splitter_moved)

class AbcGLWidget(QtOpenGL.QGLWidget):
    """
    AbcView OpenGL Widget.

    Basic usage ::

        >>> viewer = AbcGLWidget()
        >>> viewer.add_file("file.abc")
    """
    signal_scene_opened = QtCore.pyqtSignal(AbcGLScene)
    signal_scene_error = QtCore.pyqtSignal(str)
    signal_scene_drawn = QtCore.pyqtSignal()
    signal_play_fwd = QtCore.pyqtSignal()
    signal_play_stop = QtCore.pyqtSignal()
    signal_current_time = QtCore.pyqtSignal(float)
    signal_current_frame = QtCore.pyqtSignal(int)
    signal_set_camera = QtCore.pyqtSignal(AbcGLCamera)
    signal_new_camera = QtCore.pyqtSignal(AbcGLCamera)

    def __init__(self, parent=None, fps=24):
        """
        :param parent: parent Qt object
        :param fps: frames per second (used by playback timer)
        """
        format = QtOpenGL.QGLFormat()
        format.setDirectRendering(True)
        format.setSampleBuffers(True)
        QtOpenGL.QGLWidget.__init__(self, format, parent)
        self.setAutoBufferSwap(True)
        self.setMouseTracking(True)
        self.setFocusPolicy(QtCore.Qt.StrongFocus)
        self.setCursor(QtCore.Qt.OpenHandCursor)
      
        # save the parent object
        self._main = parent

        # frames per second value
        self.__time = 0
        self.__fps = fps
        self.__playing = False

        # various matrices and vectors
        self.__bounds_default = imath.Box3d((-5,-5,-5), (5,5,5))
        self.__bounds = self.__bounds_default
        self.__radius = 5.0
        self.__last_pok = False
        self.__last_p2d = QtCore.QPoint()
        self.__last_p3d = [1.0, 0.0, 0.0]
        self.__rotating = False

        # for animated scenes
        self.timer = QtCore.QTimer(self)
        
        # camera and scenes dicts
        self.scenes = SCENES
        self.cameras = CAMERAS

        # create default camera
        self.camera = AbcGLCamera(self, "interactive")
        self.camera.aspect_ratio = None
        self.cameras["interactive"] = self.camera
        self.set_camera("interactive")

        # clear alembic scene params
        self.clear()

    def clear(self):
        self.scenes = SCENES
        self.cameras = CAMERAS
        self.frame()
        self.updateGL()

    def add_file(self, filepath):
        self.add_scene(AbcGLScene(filepath))

    def add_scene(self, scene):
        """
        Adds a scene to the viewer session. 

        :param scene: AbcGLScene object
        """
        try:
            if scene.filepath in self.scenes:
                self.scenes[scene.filepath].visible = True
            else:
                self.scenes[scene.filepath] = scene
                self.signal_scene_opened.emit(scene)
            self.updateGL()
        except RuntimeError, e:
            self.signal_scene_error.emit("Error loading scene\n\n%s" 
                                       % str(e))
            log.error(str(e))

    def remove_scene(self, scene):
        """
        Removes a given AbcGLScene object from the master scene.

        :param scene: AbcGLScene to remove.
        """
        scene.visible = False
        self.updateGL()

    def add_camera(self, camera):
        """
        :param camera: AbcGLCamera object
        """
        self.cameras[camera.name] = camera
        self.signal_new_camera.emit(camera)

    @update_camera
    def set_camera(self, camera="interactive"):
        """
        Sets the scene camera from a given camera name string

        :param camera: Name of camera or AbcGLCamera object
        """
        if type(camera) in [str, unicode]:
            if "/" in camera:
                camera = os.path.split("/")[-1]
            if camera not in self.cameras.keys():
                log.warn("camera not found: %s" % camera)
                return
            self.camera = self.cameras[camera]
        else:
            self.camera = camera
        self.resizeGL(self.width(), self.height())
        self.signal_set_camera.emit(self.camera)

    def remove_camera(self, name):
        del self.cameras[name]

    def _get_time(self):
        return self.__time

    def _set_time(self, new_time):
        self.__time = new_time
        self.__frame = new_time * self.fps
        for scene in self.scenes.values():
            if scene.visible:
                scene.set_time(new_time)
        if hasattr(self, "camear") and self.camera.auto_frame:
            self.frame()
        self.signal_current_time.emit(new_time) 
        self.signal_current_frame.emit(int(round(new_time * self.fps)))
        self.updateGL()

    current_time = property(_get_time, _set_time, doc="set/get current time")

    def _get_frame(self):
        return self.__frame

    def _set_frame(self, frame):
        self.current_time = frame / float(self.fps)

    current_frame = property(_get_frame, _set_frame, doc="set/get current frame")

    def is_playing(self):
        return self.__playing

    def play(self):
        """
        plays loaded scenes by activating timer and setting callback
        """
        self.timer.setInterval(1.0 / (float(self.fps)))
        self.connect(self.timer, QtCore.SIGNAL("timeout ()"), self._play_fwd_cb)
        self.timer.start()
        self.signal_play_fwd.emit()

    def _play_fwd_cb(self):
        """
        play callback, sets current time for all scenes
        """
        self.__playing = True
        min_time, max_time = self.time_range()
        self.current_time += 1.0 / float(self.fps)
        if self.current_time > max_time:
            self.current_time = min_time

    def stop(self):
        """
        stops scene playback
        """
        self.__playing = False
        self.disconnect(self.timer,  QtCore.SIGNAL("timeout ()"), self._play_fwd_cb)
        self.connect(self.timer, QtCore.SIGNAL("timeout ()"), self._play_stop_cb)
        self.updateGL()

    def _play_stop_cb(self):
        self.signal_play_stop.emit()

    def _get_fps(self):
        return self.__fps

    def _set_fps(self, fps):
        self.__fps = fps

    fps = property(_get_fps, _set_fps, doc="frames per second value")

    def aspect_ratio(self):
        return self.width() / float(self.height())

    # TODO: prevent scenes w/o animation from updating range
    def time_range(self):
        min = None
        max = None
        if self.scenes:
            for scene in self.scenes.values():
                if min is None or scene.min_time() < min:
                    min = scene.min_time()
                if max is None or scene.max_time() > max:
                    max = scene.max_time()
        else:
            min, max = 0, 0
        return (min, max)

    def frame_range(self):
        (min, max) = self.time_range()
        return (min * self.fps, max * self.fps)
    
    def _get_bounds(self):
        if self.__bounds:
            return self.__bounds
        elif self.scenes:
            bounds = None
            for scene in self.scenes.values():
                if bounds is None or scene.bounds().max() > bounds.max():
                    bounds = scene.bounds()
            self.__bounds = bounds
            return self.__bounds
        else:
            return self.__bounds_default
        return self.__bounds

    def _set_bounds(self, bounds):
        self.__bounds = bounds

    bounds = property(_get_bounds, _set_bounds, doc="scene bounding box")

    @update_camera
    def frame(self, bounds=None):
        if bounds is None:
            bounds = self.bounds
        self.camera.frame(bounds)

    def split(self, orientation=QtCore.Qt.Vertical,
                    wipe=False):
        """
        Splist the viewer into two separate widgets according
        to the orientation param.
        """
        if not self.parent():
            return

        item = self.parent().layout().itemAt(0)

        splitter = GLSplitter(orientation, wipe)
        self.parent().layout().addWidget(splitter)

        group1 = QtGui.QGroupBox()
        group1.setLayout(QtGui.QVBoxLayout())
        group1.layout().setSpacing(0)
        group1.layout().setMargin(0)
        group1.layout().addWidget(item.widget())

        group2 = QtGui.QGroupBox()
        group2.setLayout(QtGui.QVBoxLayout())
        group2.layout().setSpacing(0)
        group2.layout().setMargin(0)
        new_viewer = AbcGLWidget(self._main)
        group2.layout().addWidget(new_viewer)

        splitter.addWidget(group1)
        splitter.addWidget(group2)

    def split_vert(self):
        self.split(QtCore.Qt.Horizontal)

    def split_horz(self):
        self.split(QtCore.Qt.Vertical)

    def wipe_horz(self):
        self.split(QtCore.Qt.Horizontal, wipe=True)

    def unsplit(self):
        if not self.parent():
            return

        splitter = self.parent().parent()
        group = self.parent().parent().parent()

        for i in range(splitter.count()):
            widget = splitter.widget(i)
            if splitter.indexOf(widget) == splitter.indexOf(self):
                del widget

        group1 = QtGui.QGroupBox()
        group1.setLayout(QtGui.QVBoxLayout())
        group1.layout().setSpacing(0)
        group1.layout().setMargin(0)
        group1.layout().addWidget(widget)

        #self.parent().layout().removeWidget(group)
        #self.parent().layout().addWidget(group1)

    def _paint_normals(self):
        glColor3f(1, 1, 1)
        def _draw(obj):
            md = obj.getMetaData()
            if alembic.AbcGeom.IPolyMesh.matches(md) or alembic.AbcGeom.ISubD.matches(md):
                meshObj = alembic.AbcGeom.IPolyMesh(obj.getParent(), obj.getName())
                mesh = meshObj.getSchema()
                
                xf = get_final_matrix(meshObj)
                iss = alembic.Abc.ISampleSelector(0)

                facesProp = mesh.getFaceIndicesProperty()
                if not facesProp.valid():
                    return
                pointsProp = mesh.getPositionsProperty()
                if not pointsProp.valid():
                    return
                normalsProp = mesh.getNormalsParam().getValueProperty()
                if not normalsProp.valid():
                    return
                boundsProp = mesh.getSelfBoundsProperty()
                if not boundsProp.valid():
                    return
                
                faces = facesProp.getValue(iss)
                points = pointsProp.getValue(iss)
                normals = normalsProp.getValue(iss)
                bounds = boundsProp.getValue(iss)

                for i, fi in enumerate(faces):
                    p = points[fi] * xf
                    n = normals[i]
                    v = p + n
                    glBegin(GL_LINES)
                    glColor3f(0, 1, 0)
                    glVertex3f(p[0], p[1], p[2])
                    glVertex3f(v[0], v[1], v[2])
                    glEnd()
        
            for child in obj.children:
                try:
                    _draw(child)
                except Exception, e:
                    log.warn("unhandled exception: %s" % e)

        for scene in self.scenes.values():
            _draw(scene.top())

    def _paint_bounds(self):
        glColor3f(1, 1, 1)
        set_ambient_light()
        
        for scene in self.scenes.values():
            draw_bounding_box(scene.scene.bounds())

        set_diffuse_light()

    def _paint_grid(self):
        glColor3f(1, 1, 1)
        set_ambient_light() 
        for x in range(-10, 11):
            if x == 0:
                continue
            glBegin(GL_LINES)
            glVertex3f(x, 0, -10)
            glVertex3f(x, 0, 10)
            glVertex3f(-10, 0, x)
            glVertex3f(10, 0, x)
            glEnd()
        
        glBegin(GL_LINES)
        glVertex3f(0, 0, -10)
        glVertex3f(0, 0, 10)
        glVertex3f(-10, 0, 0)
        glVertex3f(10, 0, 0)
        glEnd()

        set_diffuse_light()
    
    def _paint_hud(self):
        glColor3f(1, 1, 1)
        def _format(array):
            return " ".join(["%.2f" %f for f in array])
        
        glColor3f(0.5, 1, 0.5)
        self.renderText(15, 20, "%s (AR %.02f)" 
                % (self.camera.name, self.camera.aspect_ratio))

        glColor3f(1, 1, 0.5)
        self.renderText(15, 40, _format(self.camera.translation))
        self.renderText(15, 60, _format(self.camera.rotation))
    
    def _paint_fixed(self):
        """
        Changes the GL viewport according to camera's fixed aspect ratio.
        """
        if self.aspect_ratio() > self.camera.aspect_ratio:
            w = int(self.width() / (self.aspect_ratio() \
                    / self.camera.aspect_ratio))
            h = self.camera.size[1]
        else:
            w = self.camera.size[0]
            h = int(self.height() * (self.aspect_ratio() \
                    / self.camera.aspect_ratio))

        # left and bottom viewport positions
        x = int(abs(w-self.width()) / 2.0)
        y = int(abs(h-self.height()) / 2.0)
        
        glViewport(x, y, w, h)
        self.makeCurrent()
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(self.camera.fovy, self.camera.aspect_ratio,
                       self.camera.near, self.camera.far)

    def set_camera_clipping_planes(self, near=None, far=None):
        print "AbcGLWidget.set_camera_clipping_planes"
        self.camera.set_clipping_planes(self.camera.near, self.camera.far)

    def map_to_sphere(self, v2d):
        v3d = [0.0, 0.0, 0.0]
        if ((v2d.x() >= 0) and (v2d.x() <= self.width()) and
            (v2d.y() >= 0) and (v2d.y() <= self.height())):
            x = float(v2d.x() - 0.5 * self.width())  / self.width()
            y = float(0.5 * self.height() - v2d.y()) / self.height()
            v3d[0] = x
            v3d[1] = y
            z2 = 2.0 * 0.5 * 0.5 - x * x - y * y
            v3d[2] = math.sqrt(max( z2, 0.0 ))
            n = linalg.norm(v3d)
            v3d = numpy.array(v3d) / float(n)
            return True, v3d
        else:
            return False, v3d

    def handle_set_camera(self, action):
        """
        Sets camera from name derived from the text of a QAction.

        :param action: QAction
        """
        self.set_camera(str(action.text().toAscii()))

    def handle_set_mode(self, mode):
        """
        Set camera drawing mode menu handler.
        """
        if self.sender():
            mode = self.sender().data().toInt()[0]
        if mode not in GL_MODE_MAP.keys():
            raise Exception("Invalid drawing mode: %s" % mode)
        self.camera.mode = mode

    def handle_camera_action(self, action):
        """
        New camera menu handler.
        """
        action_name = str(action.text().toAscii())
        if action_name == "New":
            text, ok = QtGui.QInputDialog.getText(self, "New Camera",
                                  "Camera Name:", QtGui.QLineEdit.Normal)
            if ok and not text.isEmpty():
                name = str(text.toAscii())
                if name in self.cameras.keys():
                    message("Camera name already in use.")
                else:
                    camera = AbcGLCamera(self, name)
                    self.add_camera(camera)
                    self.set_camera(name)

    ## base class overrides

    def initializeGL(self):
        self.makeCurrent()
        glPointSize(3.0)
        glEnable(GL_BLEND)
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_AUTO_NORMAL)
        glEnable(GL_COLOR_MATERIAL)
        glEnable(GL_NORMALIZE)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
        glDisable(GL_CULL_FACE)
        glShadeModel(GL_SMOOTH)
        glClearColor(0.15, 0.15, 0.15, 0.0)
        set_diffuse_light()

    def paintGL(self):
        if self.isHidden():
            return

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glDrawBuffer( GL_BACK )

        # HACK: camera may not have been set at this point
        if hasattr(self, "camera") and self.camera is not None:
            self.camera.apply()

        # order is important here
        if hasattr(self, "camera") and self.camera.draw_hud:
            self._paint_hud()
        if hasattr(self, "camera") and self.camera.fixed:
            self._paint_fixed()
        if hasattr(self, "camera") and self.camera.draw_grid:
            self._paint_grid()
        if hasattr(self, "camera") and self.camera.draw_normals:
            self._paint_normals()
        if hasattr(self, "camera") and self.camera.draw_bounds:
            self._paint_bounds()

        # draw each scene
        for scene in self.scenes.values():
            if not scene.visible:
                continue

            # color override
            glEnable(GL_COLOR_MATERIAL)
            glMaterialfv(GL_BACK, GL_EMISSION, scene.properties.get("color", 
                         (1, 0, 0)))

            # draw mode override
            mode = GL_MODE_MAP.get(scene.properties.get("mode"), 
                   GL_MODE_MAP.get(self.camera.mode, GL_LINE)
                   )
            glPolygonMode(GL_FRONT_AND_BACK, mode)

            # trs overrides
            glTranslatef(*scene.properties.get("translate", (0, 0, 0)))
            glRotatef(*scene.properties.get("rotate", (0, 0, 0, 0)))
            glScalef(*scene.properties.get("scale", (1, 1, 1)))

            scene.draw()
            self.signal_scene_drawn.emit()

    @update_camera
    def resizeGL(self, width, height):
        if hasattr(self, "camera") and self.camera is not None:
            self.camera.size = (width, height)

    def keyPressEvent(self, event):
        key = event.key()
        mod = event.modifiers()

        if mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_F:
            self.camera._set_fixed()

        elif mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_B:
            self.camera._set_draw_bounds()

        elif mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_N:
            self.camera._set_draw_normals()

        elif mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_G:
            self.camera._set_draw_grid()

        elif mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_H:
            self.camera._set_draw_hud()

        elif mod == QtCore.Qt.ShiftModifier and key == QtCore.Qt.Key_Bar:
            self.split_vert()

        elif mod == QtCore.Qt.ShiftModifier and key == QtCore.Qt.Key_Underscore:
            self.split_horz()
            
        elif mod == QtCore.Qt.AltModifier and key == QtCore.Qt.Key_Minus:
            self.unsplit()

        elif key == QtCore.Qt.Key_F:
            self.frame()

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.RightButton and \
           event.modifiers() == QtCore.Qt.NoModifier:
            self.setCursor(QtCore.Qt.ArrowCursor)
            
            menu = QtGui.QMenu(self)

            # splits
            layout_menu = QtGui.QMenu("Layout", self)
            self.splitHAct = QtGui.QAction(QtGui.QIcon("%s/split_vert.png" % config.ICON_DIR),
                                          "Split Vertical (Shift+|)", self)
            self.connect(self.splitHAct, QtCore.SIGNAL("triggered (bool)"), self.split_vert)
            layout_menu.addAction(self.splitHAct)

            self.splitVAct = QtGui.QAction(QtGui.QIcon("%s/split_horz.png" % config.ICON_DIR),
                                          "Split Horizontal (Shift+_)", self)
            self.connect(self.splitVAct, QtCore.SIGNAL("triggered (bool)"), self.split_horz)
            layout_menu.addAction(self.splitVAct)
            
            self.closeAct = QtGui.QAction("Close (Alt+-)", self)
            self.connect(self.closeAct, QtCore.SIGNAL("triggered (bool)"), self.unsplit)
            layout_menu.addAction(self.closeAct)
            
            menu.addMenu(layout_menu)

            # cameras
            camera_menu = QtGui.QMenu("Cameras", self)
            camera_group = QtGui.QActionGroup(camera_menu) 

            for name, camera in self.cameras.items():
                camera_action = QtGui.QAction(name, self)
                camera_action.setCheckable(True)
                if name == self.camera.name:
                    camera_action.setChecked(True)
                camera_action.setActionGroup(camera_group)
                camera_menu.addAction(camera_action)
            self.connect(camera_group, QtCore.SIGNAL("triggered (QAction *)"), 
                        self.handle_set_camera)
            self.connect(camera_menu, QtCore.SIGNAL("triggered (QAction *)"), 
                        self.handle_camera_action)
            camera_menu.addSeparator()
            camera_menu.addAction("New")

            menu.addMenu(camera_menu)
          
            options_menu = QtGui.QMenu("Options", self)

            # bounds toggle menu item
            self.aframeAct = QtGui.QAction("Autoframe", self)
            self.aframeAct.setCheckable(True)
            self.aframeAct.setChecked(self.camera.auto_frame)
            self.connect(self.aframeAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_auto_frame)
            options_menu.addAction(self.aframeAct)

            # bounds toggle menu item
            self.boundsAct = QtGui.QAction("Bounds (Alt+B)", self)
            self.boundsAct.setCheckable(True)
            self.boundsAct.setChecked(self.camera.draw_bounds)
            self.connect(self.boundsAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_draw_bounds)
            options_menu.addAction(self.boundsAct)
 
            # fixed aspect ratio toggle menu item
            self.fixedAct = QtGui.QAction("Fixed Aspect Ratio (Alt+F)", self)
            self.fixedAct.setCheckable(True)
            self.fixedAct.setChecked(self.camera.fixed)
            self.connect(self.fixedAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_fixed)
            options_menu.addAction(self.fixedAct)

            # heads-up-display menu item
            self.hudAct = QtGui.QAction("HUD (Alt+H)", self)
            self.hudAct.setCheckable(True)
            self.hudAct.setChecked(self.camera.draw_hud)
            self.connect(self.hudAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_draw_hud)
            options_menu.addAction(self.hudAct)

            # grid toggle menu item
            self.gridAct = QtGui.QAction("Grid (Alt+G)", self)
            self.gridAct.setCheckable(True)
            self.gridAct.setChecked(self.camera.draw_grid)
            self.connect(self.gridAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_draw_grid)
            options_menu.addAction(self.gridAct)
          
            # normals toggle menu item
            self.normalsAct = QtGui.QAction("Normals (Alt+N)", self)
            self.normalsAct.setCheckable(True)
            self.normalsAct.setChecked(self.camera.draw_normals)
            self.connect(self.normalsAct, QtCore.SIGNAL("toggled (bool)"), self.camera._set_draw_normals)
            options_menu.addAction(self.normalsAct)

            # shading toggle menu item
            self.shading_menu = QtGui.QMenu("Shading", self)
            shading_group = QtGui.QActionGroup(self.shading_menu)

            self.fillAct = QtGui.QAction("Fill", self)
            self.fillAct.setCheckable(True)
            self.fillAct.setActionGroup(shading_group)
            self.fillAct.setData(Mode.FILL)
            self.fillAct.setChecked(self.camera.mode == Mode.FILL)
            self.fillAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.fillAct)
            
            self.lineAct = QtGui.QAction("Line", self)
            self.lineAct.setCheckable(True)
            self.lineAct.setActionGroup(shading_group)
            self.lineAct.setData(Mode.LINE)
            self.lineAct.setChecked(self.camera.mode == Mode.LINE)
            self.lineAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.lineAct)

            self.pointAct = QtGui.QAction("Point", self)
            self.pointAct.setCheckable(True)
            self.pointAct.setActionGroup(shading_group)
            self.pointAct.setData(Mode.POINT)
            self.pointAct.setChecked(self.camera.mode == Mode.POINT)
            self.pointAct.toggled.connect(self.handle_set_mode)
            self.shading_menu.addAction(self.pointAct)
            
            options_menu.addMenu(self.shading_menu)

            menu.addMenu(options_menu)
            menu.popup(QtCore.QPoint(event.globalX(), event.globalY()))

        else:
            self.__last_p2d = event.pos()
            self.__last_pok, self.__last_p3d = self.map_to_sphere(self.__last_p2d)
        
        self.setCursor(QtCore.Qt.OpenHandCursor)

    @update_camera
    def mouseMoveEvent(self, event):
        newPoint2D = event.pos()
        if ((newPoint2D.x() < 0) or (newPoint2D.x() > self.width()) or
            (newPoint2D.y() < 0) or (newPoint2D.y() > self.height())):
            return
        value_y = 0
        newPoint_hitSphere, newPoint3D = self.map_to_sphere(newPoint2D)
        dx = float(newPoint2D.x() - self.__last_p2d.x())
        dy = float(newPoint2D.y() - self.__last_p2d.y())
        w = float(self.width())
        h = float(self.height())
        self.makeCurrent()

        if (((event.buttons() & QtCore.Qt.LeftButton) and (event.buttons() & QtCore.Qt.MidButton))
            or (event.buttons() & QtCore.Qt.LeftButton and event.modifiers() & QtCore.Qt.ControlModifier)
            or (event.buttons() & QtCore.Qt.RightButton and event.modifiers() & QtCore.Qt.AltModifier)):
            self.camera.dolly(dx, dy)
        
        elif (event.buttons() & QtCore.Qt.MidButton
              or (event.buttons() & QtCore.Qt.LeftButton and event.modifiers() & QtCore.Qt.ShiftModifier)):
            self.camera.track(dx, dy)

        elif event.buttons() & QtCore.Qt.LeftButton:
            self.__rotating = True
            self.camera.rotate(dx, dy)
            
        # TODO: replace with global state (with these as attrs)
        self.__last_p2d = newPoint2D
        self.__last_p3d = newPoint3D
        self.__last_pok = newPoint_hitSphere
       
    def mouseReleaseEvent(self, event):
        self.__rotating = False
        self.__last_pok = False
        super(AbcGLWidget, self).mouseReleaseEvent(event)

    @update_camera
    def wheelEvent(self, event):
        dx = float(event.delta()) / 10
        self.camera.dolly(dx, 0)
        event.accept()
