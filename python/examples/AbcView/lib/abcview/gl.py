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

import imath
import alembic

try:
    import alembicgl
except ImportError:
    print """
    The alembicgl module was not found. Be sure to build AbcOpenGL and
    the PyAbcOpenGL Python bindings.
    """

import abcview
from abcview import log

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

def get_archive(filepath):
    """
    caches alembic archives
    """
    if filepath not in ARCHIVES:
        ARCHIVES[filepath] = alembic.Abc.IArchive(str(filepath))
    return ARCHIVES[filepath]

def get_scene(filepath):
    """
    caches alembicgl scenes
    """
    if filepath not in SCENES:
        SCENES[filepath] = alembicgl.SceneWrapper(str(filepath))
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
        self.glcamera = alembicgl.GLCamera()

    def __repr__(self):
        return "<%s \"%s\">" % (self.__class__.__name__, self.name)

    def dolly(self, dx, dy):
        self.glcamera.dolly(imath.V2d(dx, dy))

    def track(self, dx, dy):
        self.glcamera.track(imath.V2d(dx, dy))

    def rotate(self, dx, dy):
        self.glcamera.rotate(imath.V2d(dx, dy))

    def look_at(self, eye, at):
        self.glcamera.lookAt(eye, at)

    def frame(self, bounds):
        self.glcamera.frame(bounds)

class GLCamera(abcview.io.Camera, GLCameraMixin):
    """
    GL wrapper for Camera objects
    """
    def __init__(self, viewer, name="interactive"):
        """
        :param viewer: GLWidget object
        :param name: camera name
        """
        super(GLCamera, self).__init__(name)
        GLCameraMixin.__init__(self, viewer)

        # set default values
        self.near = 0.1
        self.far = 100000.0
        self.fovx = 45.0
        self.fovy = 45.0
        self.aspect_ratio = 1.85
        self.size = (self.viewer.width(), self.viewer.height())
        
        # apply values
        self.apply()

    def __repr__(self):
        return "<%s \"%s\">" % (self.__class__.__name__, self.name)

    def dolly(self, dx, dy):
        self.glcamera.dolly(imath.V2d(dx, dy))

    def track(self, dx, dy):
        self.glcamera.track(imath.V2d(dx, dy))

    def rotate(self, dx, dy):
        self.glcamera.rotate(imath.V2d(dx, dy))

    def look_at(self, eye, at):
        self.glcamera.lookAt(eye, at)

    def frame(self, bounds):
        if bounds is not None:
            self.glcamera.frame(bounds)

    def _get_translation(self):
        return self.glcamera.translation()

    def _set_translation(self, value):
        self.glcamera.setTranslation(value)

    translation = property(_get_translation, _set_translation, doc="translation (get/set V3d)")

    def _get_rotation(self):
        return self.glcamera.rotation()

    def _set_rotation(self, value):
        self.glcamera.setRotation(value)

    rotation = property(_get_rotation, _set_rotation, doc="rotation (get/set V3d)")

    def _get_scale(self):
        return self.glcamera.scale()

    def _set_scale(self, value):
        self.glcamera.setScale(value)

    scale = property(_get_scale, _set_scale, doc="scale (set/get V3d)")

    def _get_near(self):
        return self.glcamera.clippingPlanes()[0]

    def _set_near(self, value):
        self.glcamera.setClippingPlanes(value, self.far)

    near = property(_get_near, _set_near, doc="get/set near clipping plane")

    def _get_far(self):
        return self.glcamera.clippingPlanes()[1]
   
    def _set_far(self, value):
        self.glcamera.setClippingPlanes(self.near, value)

    far = property(_get_far, _set_far, doc="get/set far clipping plane")

    def _get_fovy(self):
        return self.glcamera.fovy()
   
    def _set_fovy(self, value):
        self.glcamera.setFovy(value)

    fovy = property(_get_fovy, _set_fovy, doc="get/set camera fov Y")

    def _get_aspect_ratio(self):
        if self._aspect_ratio:
            return self._aspect_ratio
        else:
            return self.viewer.aspect_ratio()

    def _set_aspect_ratio(self, value):
        self._aspect_ratio = value

    aspect_ratio = property(_get_aspect_ratio, _set_aspect_ratio, doc="aspect ratio")

    def _get_center(self):
        return self.glcamera.centerOfInterest()

    def _set_center(self, value):
        self.glcamera.setCenterOfInterest(max(value, 0.1))

    center = property(_get_center, _set_center, doc="center of interest")

    def _get_size(self):
        return (self.glcamera.width(), self.glcamera.height())

    def _set_size(self, size):
        """
        :param size: tuple (width, height)
        """
        self.glcamera.setSize(size[0], size[1])

    size = property(_get_size, _set_size, doc="virtual viewport size (w h)")
    
    def apply(self):
        self.glcamera.setClippingPlanes(self.near, self.far)
        self.glcamera.apply()

class GLICamera(abcview.io.ICamera, GLCameraMixin):
    """
    GL wrapper for ICamera objects
    """
    def __init__(self, viewer, camera):
        """
        :param viewer: GLWidget object
        :param camera: AbcView ICamera IO object
        """
        super(GLICamera, self).__init__(camera)
        GLCameraMixin.__init__(self, viewer)

    def _not_settable(self, value):
        log.debug("GLICamera immutable attribute")

    def _get_translation(self):
        return super(GLICamera, self).translation(self.viewer.current_time)

    translation = property(_get_translation, _not_settable)

    def _get_rotation(self):
        return super(GLICamera, self).rotation(self.viewer.current_time)

    rotation = property(_get_rotation, _not_settable)

    def _get_scale(self):
        return super(GLICamera, self).scale(self.viewer.current_time)

    scale = property(_get_scale, _not_settable)

    def _get_near(self):
        return super(GLICamera, self).near(self.viewer.current_time)

    near = property(_get_near, _not_settable)

    def _get_far(self):
        return super(GLICamera, self).far(self.viewer.current_time)
  
    far = property(_get_far, _not_settable)

    def _get_fovx(self):
        return super(GLICamera, self).fovx(self.viewer.current_time)
   
    fovx = property(_get_fovx, _not_settable)

    def _get_fovy(self):
        return self.fovx / self.aspect_ratio
   
    fovy = property(_get_fovy, _not_settable)

    def _get_aspect_ratio(self):
        return super(GLICamera, self).aspect_ratio(self.viewer.current_time)

    aspect_ratio = property(_get_aspect_ratio, _not_settable, doc="aspect ratio")

    def _get_screen_window(self):
        return super(GLICamera, self).screen_window(self.viewer.current_time)

    screen_window = property(_get_screen_window, _not_settable, doc="screen window")

    def _get_center(self):
        return self.glcamera.centerOfInterest()

    center = property(_get_center, _not_settable, doc="center of interest")

    def _get_size(self):
        return (self.glcamera.width(), self.glcamera.height())

    def _set_size(self, size):
        self.glcamera.setSize(size[0], size[1])

    size = property(_get_size, _set_size)

    def apply(self):
        self.glcamera.setTranslation(self.translation)
        self.glcamera.setRotation(self.rotation)
        self.glcamera.setClippingPlanes(self.near, self.far)
        self.glcamera.setFovy(self.fovy)
        self.glcamera.apply()

class GLScene(abcview.io.Scene):
    """
    GL wrapper for Scene objects.
    """
    def __init__(self, filepath):
        super(GLScene, self).__init__(filepath)
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
        name = self.archive.getName()
        min = self.min_time()
        max = self.max_time()
        log.debug("GLScene.load: %s (min: %s, max %s)" % (name, min, max))
        self.loaded = True
        self.visible = True

    def clear(self):
        self.selected = []
        self.__archive = None
        self.__scene = None

    def draw(self):
        try:
            self.scene.draw()
        except RuntimeError, e:
            log.error(str(e))
   
    def set_time(self, new_time):
        self.scene.setTime(new_time)
   
    def get_time(self):
        return self.scene.getCurrentTime()
   
    def play_forward(self, fps=24):
        self.scene.playForward(fps)
   
    def min_time(self):
        return self.scene.getMinTime()
   
    def max_time(self):
        return self.scene.getMaxTime()
   
    def bounds(self):
        return self.scene.bounds()

    def top(self):
        return self.archive.getTop()
