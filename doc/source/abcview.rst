AbcView
=======

.. moduleauthor:: Ryan Galloway <ryang@ilm.com>

AbcView is a graphical PyQt-based Alembic inspection and visualization tool.
It offers a number of widgets to help you inspect and visualize your Alembic
data, including:

    * Object, property, sample and value widgets,
    * The viewer widget renders the scene using AbcOpenGL,
    * Playback or step through animation using the Timeline widget,
    * And a Python console for closer inspection of Alembic objects.

.. image:: abcview.png
   :width: 640
   :align: center

Requirements
------------

The following libs are required to use AbcView:

    * Alembic
    * OpenGL
    * AbcOpenGL
    * PyAlembic (Alembic Python bindings)
    * PyAbcOpenGL (AbcOpenGL Python bindings)
    * Python 2.6+
    * PyQt4

To build the `abcview` wrapper inside your Alembic build dir, make sure the "examples" 
subdirectory is active in the /python/CMakeLists.txt file ::

    ADD_SUBDIRECTORY( examples )

Basic Usage
-----------

Loading one or more Alembic scenes ::

    > abcview <file1.abc> ... <fileN.abc> [OPTIONS]

Save your session at any time using File->Save/Save As. Loading a saved session is the
same as loading an Alembic scene ::

    > abcview <file.io>

You can also inspect and manipulate session data using the AbcView API, for example ::

    >>> from abcview.io import Session
    >>> s = Session()
    >>> s.add_file("file.abc")
    >>> s.save("file.io")

There are a number of properties you can set on session items, such as TRS values, color
and GL rendering mode ::

    >>> from abcview.io import Mode
    >>> item = s.items[0]
    >>> item.mode = Mode.FILL
    >>> item.color = (0, 255, 0)

Loading ICameras into an AbcView session is easy. You can either navigate to the ICamera
object in the objects widget and right-click->view through selected, or add it to the
session using the API ::

    >>> s.add_camera(ICamera(get_object("shotcam.abc", 
                                        "ShotCam")
                             loaded=True))
    >>> s.save()

Module Contents
---------------

I/O Module
~~~~~~~~~~

.. automodule:: abcview.io
   :members: 

GL Module
~~~~~~~~~

.. automodule:: abcview.gl
   :members: 

Utils
~~~~~

.. automodule:: abcview.utils
   :members: 

