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
from functools import wraps

import imath
import alembic

import OpenGL
OpenGL.ERROR_CHECKING = True
from OpenGL.GL import *
from OpenGL.GLU import *

try:
    import alembicgl
except ImportError:
    print """
    The alembicgl module was not found. Be sure to build AbcOpenGL and
    the PyAbcOpenGL Python bindings.
    """

import abcview
from abcview import log
from abcview.io import Mode
from abcview.utils import memoized

__doc__ = """
When loading a Session object into the AbcView GUI, the IO objects are
replaced with corresponding OpenGL objects from the GL module. The GL
module classes provide GL-wrapped methods for viewing and manipulating
Alembic scenes in AbcView GLViewer widgets.
"""

__all__ = ["GLCamera", "GLICamera", "GLScene", ]

# alembic cache index
ARCHIVES = {}
SCENES = {}

def require_loaded(func):
    """
    Load decorator
    """
    @wraps(func)
    def with_wrapped_func(*args, **kwargs):
        klass = args[0]
        klass.load(func.__name__)
        return func(*args, **kwargs)
    return with_wrapped_func

class IArchive(alembic.Abc.IArchive):
    """
    Alembic::IArchive wrapper class that sets some default values.
    """
    def __init__(self, filepath):
        super(IArchive, self).__init__(str(filepath))
        
        # placeholder for top-most xform schema
        self._bounds_cp = None

    def uid(self):
        return id(self)

    def __repr__(self):
        return "<IArchive %s>" % self.uid()

    def bounds(self, seconds=0):
        """
        Attempts to get the top-most childBnds property value for
        a given time in seconds. 

        :param seconds: time in seconds
        :return: Box3d bounds value, or None
        """
        #log.debug("[%s.bounds] %s" % (repr(self), seconds))
        
        if seconds is None:
            seconds = 0

        # limit the levels to walk
        limit = 2

        def walk(obj, l=0):
            if l > limit:
                return
            if alembic.AbcGeom.IXform.matches(obj.getMetaData()):
                x = alembic.AbcGeom.IXform(obj.getParent(), obj.getName())
                xs = x.getSchema()
                cp = xs.getChildBoundsProperty()
                if cp.valid():
                    ts = cp.getTimeSampling()
                    index = ts.getNearIndex(seconds, 
                                    cp.getNumSamples())
                    return cp.getValue(index)

            for i in range(obj.getNumChildren()):
                return walk(obj.getChild(i), l+1)
            return None
        return walk(self.getTop())

class SceneWrapper(alembicgl.SceneWrapper):
    """
    AbcOpenGL::SceneWrapper wrapper class that sets some default values.
    """
    def __init__(self, filepath):
        self.filepath = str(filepath)
        self.loaded = False

    def load(self, func_name="load"):
        """
        Defers actually loading the scene until necessary.
        """
        if not self.loaded:
            log.debug("[%s] reading %s" % (func_name, self.filepath))
            super(SceneWrapper, self).__init__(self.filepath)
            self.loaded = True

    def uid(self):
        return id(self)

    def __repr__(self):
        return "<SceneWrapper %s>" % self.uid()

    @require_loaded
    def draw(self, visible_only=True, bounds_only=False):
        super(SceneWrapper, self).draw(visible_only, bounds_only)

    @require_loaded
    def draw_bounds(self):
        super(SceneWrapper, self).drawBounds()

    @require_loaded
    def bounds(self):
        #log.debug("[%s.bounds]" % (self))
        return super(SceneWrapper, self).bounds()

    @require_loaded
    def get_time(self):
        return self.scene.getCurrentTime()

    @require_loaded
    def set_time(self, value):
        self.setTime(value)

    @require_loaded
    def min_time(self):
        return self.getMinTime()

    @require_loaded
    def max_time(self):
        return self.getMaxTime()

class AbcGLCamera(alembicgl.GLCamera):
    """
    AbcOpenGL::GLCamera wrapper class that sets some default values.
    """
    def __init__(self, parent=None):
        super(AbcGLCamera, self).__init__()
        self.parent = parent
        self.setClippingPlanes(0.1, 100000.0)

    def uid(self):
        return id(self)

    def __repr__(self):
        return "<AbcGLCamera %s>" % self.uid()

def get_archive(filepath):
    """
    caches alembic archives
    """
    if filepath not in ARCHIVES:
        log.debug("[get_archive] %s" % filepath)
        ARCHIVES[filepath] = IArchive(str(filepath))
    return ARCHIVES[filepath]

def get_scene(filepath):
    """
    caches alembicgl scenes
    """
    if filepath not in SCENES:
        SCENES[filepath] = SceneWrapper(str(filepath))
    return SCENES[filepath]

class GLCameraMixin(object):
    """
    GL wrapper for Camera objects
    """
    def __init__(self, viewer):
        """
        :param viewer: GLWidget object
        :param name: camera name
        """
        self.viewer = viewer

        # index of views for this camera
        self.views = {}

        # defaults (must be inheritable by subclasses)
        self._translation = imath.V3d(6.85, 1.80, 6.85)
        self._rotation = imath.V3d(-10.5, 45, 0)
        self._scale = imath.V3d(1, 1, 1)
        self._center = 9.85
        self._near = 0.1
        self._far = 10000.0
        self._fovx = 45.0
        self._fovy = 45.0
        self._aspect_ratio = 1.85

        # set default size from viewer
        if viewer is not None:
            self._size = (viewer.width(), viewer.height())

    def dolly(self, dx, dy):
        # implement in subclass
        pass

    def track(self, dx, dy):
        # implement in subclass
        pass

    def rotate(self, dx, dy):
        # implement in subclass
        pass

    def look_at(self, eye, at):
        # implement in subclass
        pass

    def frame(self, bounds):
        # implement in subclass
        pass

    def resize(self):
        for view, camera in self.views.items():
            camera.setSize(view.width(), view.height())

    def add_view(self, viewer):
        """
        Adds a new view for this camera.
        """
        log.debug("[GLCameraMixin.add_view] %s %s" % (self.name, viewer))
        self.views[viewer] = AbcGLCamera(self)
        self.views[viewer].setTranslation(self._translation)
        self.views[viewer].setRotation(self._rotation)
        self.views[viewer].setScale(self._scale)
        self.views[viewer].setCenterOfInterest(self._center)
        self.views[viewer].setClippingPlanes(self._near, self._far)

    def remove_view(self, viewer):
        """
        Removes a view (glcamera) from this camera.
        """
        log.debug("[GLCameraMixin.remove_view] %s %s" % (self.name, viewer))
        if viewer in self.views:
            del self.views[viewer]
        # switch the default viewer for this camera
        for viewer in self.views:
            if viewer != self.viewer:
                self.viewer = viewer

    def _get_viewer(self):
        return self.__viewer

    def _set_viewer(self, viewer):
        self.__viewer = viewer

    viewer = property(_get_viewer, _set_viewer)

    def get_size(self, viewer):
        """
        returns viewer-based size as a tuple
        """
        cam = self.views.get(viewer)
        if cam:
            return (cam.width(), cam.height())
        return self._size

    def set_size(self, size, viewer):
        """
        sets the size for a given view
        """
        self._size = size
        cam = self.views.get(viewer)
        cam.setSize(size[0], size[1])

class GLCamera(abcview.io.Camera, GLCameraMixin):
    """
    Adds OpenGL methods to io.Camera objects. 
    
    Each Camera object contains a number of AbcOpenGL camera children, one for
    each GLWidget viewer object as a separate "view". Each time a new view is
    added, a new AbcOpenGL GLCamera is added to the list of "views", since each
    of these cameras may have a slightly different representation, for example
    the size of the camera corresponds to the viewer size.
    """
    def __init__(self, viewer, name):
        """
        :param viewer: GLWidget object
        :param name: camera name
        """
        super(GLCamera, self).__init__(name)
        self.init(viewer)

    def init(self, viewer):
        GLCameraMixin.__init__(self, viewer)
        self.apply()

    def __repr__(self):
        return "<%s \"%s\">" % (self.__class__.__name__, self.name)

    def dolly(self, dx, dy):
        for cam in self.views.values():
            cam.dolly(imath.V2d(dx, dy))
        self._update()

    def track(self, dx, dy):
        for cam in self.views.values():
            cam.track(imath.V2d(dx, dy))
        self._update()

    def rotate(self, dx, dy):
        for uid, cam in self.views.items():
            cam.rotate(imath.V2d(dx, dy))
        self._update()

    def look_at(self, eye, at):
        for cam in self.views.values():
            cam.lookAt(eye, at)
        self._update()

    def frame(self, bounds):
        if bounds is None:
            return
        for cam in self.views.values():
            cam.frame(bounds)
        self._update()

    def _update(self):
        self._translation = self.views[self.viewer].translation()
        self._rotation = self.views[self.viewer].rotation()
        self._scale = self.views[self.viewer].scale()
        self._center = self.views[self.viewer].centerOfInterest()

    def _get_translation(self):
        return self.views[self.viewer].translation()

    def _set_translation(self, value):
        if type(value) in (list, tuple):
            value = imath.V3d(*value)
        self._translation = value
        for cam in self.views.values():
            cam.setTranslation(value)

    translation = property(_get_translation, _set_translation, 
                           doc="get/set translation (imath.V3d)")

    def _get_rotation(self):
        return self.views[self.viewer].rotation()

    def _set_rotation(self, value):
        if type(value) in (list, tuple):
            value = imath.V3d(*value)
        self._rotation = value
        for cam in self.views.values():
            cam.setRotation(value)

    rotation = property(_get_rotation, _set_rotation, 
                        doc="get/set rotation (imath.V3d)")

    def _get_scale(self):
        return self.views[self.viewer].scale()

    def _set_scale(self, value):
        if type(value) in (list, tuple):
            value = imath.V3d(*value)
        self._scale = value
        for cam in self.views.values():
            cam.setScale(value)

    scale = property(_get_scale, _set_scale, doc="get/set scale (imath.V3d)")

    def _get_near(self):
        return self.views[self.viewer].clippingPlanes()[0]

    def _set_near(self, value):
        self._near = value
        for cam in self.views.values():
            cam.setClippingPlanes(value, self.far)

    near = property(_get_near, _set_near, doc="get/set near clipping plane")

    def _get_far(self):
        return self.views[self.viewer].clippingPlanes()[1]
   
    def _set_far(self, value):
        self._far = value
        for cam in self.views.values():
            cam.setClippingPlanes(self.near, value)

    far = property(_get_far, _set_far, doc="get/set far clipping plane")

    def _get_fovy(self):
        return self.views[self.viewer].fovy()
   
    def _set_fovy(self, value):
        self._fovy = value
        for cam in self.views.values():
            cam.setFovy(value)

    fovy = property(_get_fovy, _set_fovy, doc="get/set camera fov Y")

    def _get_fovx(self):
        return self._fovx
   
    def _set_fovx(self, value):
        self._fovx = value

    fovx = property(_get_fovx, _set_fovx, doc="get/set camera fov X")

    def _get_aspect_ratio(self):
        if self._aspect_ratio:
            return self._aspect_ratio
        else:
            return self.viewer.aspect_ratio()

    def _set_aspect_ratio(self, value):
        self._aspect_ratio = value

    aspect_ratio = property(_get_aspect_ratio, _set_aspect_ratio, doc="aspect ratio")

    def _get_center(self):
        return self.views[self.viewer].centerOfInterest()

    def _set_center(self, value):
        self._center = value
        for cam in self.views.values():
            cam.setCenterOfInterest(max(value, 0.1))

    center = property(_get_center, _set_center, doc="center of interest")

    def get_size(self, viewer):
        cam = self.views.get(viewer)
        if cam:
            return (cam.width(), cam.height())

    def set_size(self, size, viewer):
        cam = self.views.get(viewer)
        cam.setSize(size[0], size[1])

    def apply(self):
        for view, camera in self.views.items():
            camera.setClippingPlanes(self.near, self.far)
            camera.apply()

class GLICamera(abcview.io.ICamera, GLCameraMixin):
    """
    GL wrapper for ICamera objects
    """
    def __init__(self, viewer, camera):
        """
        :param viewer: GLWidget object
        :param camera: Alembic ICamera object
        """
        super(GLICamera, self).__init__(camera)
        self.init(viewer)

    def init(self, viewer):
        GLCameraMixin.__init__(self, viewer)

    def __repr__(self):
        return "<%s \"%s\">" % (self.__class__.__name__, self.name)

    def _not_settable(self, value):
        log.debug("GLICamera immutable attribute")

    def _get_translation(self):
        return super(GLICamera, self).translation(self.viewer.state.current_time)

    translation = property(_get_translation, _not_settable)

    def _get_rotation(self):
        return super(GLICamera, self).rotation(self.viewer.state.current_time)

    rotation = property(_get_rotation, _not_settable)

    def _get_scale(self):
        return super(GLICamera, self).scale(self.viewer.state.current_time)

    scale = property(_get_scale, _not_settable)

    def _get_near(self):
        return super(GLICamera, self).near(self.viewer.state.current_time)

    near = property(_get_near, _not_settable)

    def _get_far(self):
        return super(GLICamera, self).far(self.viewer.state.current_time)
  
    far = property(_get_far, _not_settable)

    def _get_fovx(self):
        return super(GLICamera, self).fovx(self.viewer.state.current_time)
   
    fovx = property(_get_fovx, _not_settable)

    def _get_fovy(self):
        return self.fovx / self.aspect_ratio
   
    fovy = property(_get_fovy, _not_settable)

    def _get_aspect_ratio(self):
        return super(GLICamera, self).aspect_ratio(self.viewer.state.current_time)

    aspect_ratio = property(_get_aspect_ratio, _not_settable, doc="aspect ratio")

    def _get_screen_window(self):
        return super(GLICamera, self).screen_window(self.viewer.state.current_time)

    screen_window = property(_get_screen_window, _not_settable, doc="screen window")

    def _get_center(self):
        return self.views[self.viewer].centerOfInterest()

    center = property(_get_center, _not_settable, doc="center of interest")

    def apply(self):
        for view, camera in self.views.items():
            camera.setTranslation(self.translation)
            camera.setRotation(self.rotation)
            camera.setClippingPlanes(self.near, self.far)
            camera.setFovy(self.fovy)
            camera.apply()

class GLScene(abcview.io.Scene):
    """
    GL wrapper for Scene objects.
    """
    def __init__(self, filepath):
        super(GLScene, self).__init__(filepath)
        self.init()
  
    def init(self):
        self.visible = True
        self.clear()

    @property
    def archive(self):
        if self.__archive is None and self.filepath:
            self.__archive = get_archive(self.filepath)
        return self.__archive

    @property
    def scene(self):
        if self.__scene is None and self.filepath:
            self.__scene = get_scene(self.filepath)
        return self.__scene

    def aspect_ratio(self):
        return self.width() / float(self.height())

    def load(self):
        # by accessing the archive/min/max time we load the scene
        name = self.archive.getName()
        min = self.min_time()
        max = self.max_time()
        self.loaded = True
        self.visible = True

    def clear(self):
        self.selected = []
        self.__archive = None
        self.__scene = None

    def draw(self, visible_only=True, bounds_only=False):
        try:
            self.scene.draw(visible_only, bounds_only)
        except RuntimeError, e:
            log.error(str(e))
    
    def draw_bounds(self, seconds=0):
        """
        Draw scene bounding boxes.
        """
        glPushName(0)
        
        # try to get top-most bounds from archive first...
        bounds = self.archive.bounds(seconds)

        if bounds is not None:
            alembicgl.drawBounds(bounds)
        
        # because instantiating the SceneWrapper is slow
        else:
            try:
                self.scene.draw_bounds()
            except RuntimeError, e:
                log.error(str(e))
        
        glPopName()

    def selection(self, x, y, camera):
        log.debug("[%s.selection] %s %s %s" % (self, x, y, camera))
        print "state:", self.state, "visible:", self.visible, "loaded:", self.loaded
        return self.scene.selection(x, y, camera)

    def set_time(self, value):
        if self.visible and self.mode != Mode.OFF:
            self.scene.set_time(value)
   
    def get_time(self):
        return self.scene.get_time()
   
    def play_forward(self, fps=24):
        if self.visible:
            self.scene.playForward(fps)
   
    def min_time(self):
        return self.scene.min_time()
   
    def max_time(self):
        return self.scene.max_time()
 
    @memoized
    def bounds(self, seconds=0):
        #log.debug("[%s.bounds] %s" % (self, seconds))
        bounds = self.archive.bounds(seconds)
        if bounds is None:
            bounds = self.scene.bounds()
        return bounds

    def top(self):
        return self.archive.getTop()
