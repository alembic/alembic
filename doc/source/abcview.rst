AbcView 1.0.5
=============

.. moduleauthor:: Ryan Galloway <ryang@ilm.com>

AbcView is a graphical PyQt-based Alembic inspection and visualization tool.
It offers a number of widgets to help you inspect and visualize your Alembic
data, as well as assemble hierarchical scenes. 

    * Object, property, sample and value widgets
    * Deferred loading for all Alembic objects
    * Hierarchical scene assembly (with standard JSON output)
    * Python console for closer inspection of Alembic objects
    * Playback or step through animation using the Timeline widget
    * Per-camera render modes (box, fill, line, point)
    * Widgets are embeddable in other PyQt applications

.. image:: abcview.png
   :width: 800
   :align: center

Requirements
------------

The following libs are required to use AbcView:

    * Alembic 1.5
    * PyOpenGL
    * PyAlembic (and PyAbcOpenGL)
    * PyQt4
    * Python 2.6+
    * argparse if using Python 2.6

For your convenience, AbcView comes with a csh wrapper that sets up your environment for running
AbcView from your Alembic build directory. To build the wrapper inside your Alembic build dir, make 
sure the "examples" subdirectory is active in the /python/CMakeLists.txt file ::

    ADD_SUBDIRECTORY( examples )

Basic Usage
-----------

Loading one or more Alembic scenes or AbcView sessions ::

    abcview [file1 .. fileN] [OPTIONS]

    positional arguments:
      filepath              File to open.

    optional arguments:
      -h, --help            show this help message and exit
      -f FRAME, --frame FRAME
                            Frame to load in viewer.
      --first FIRST         Set viewer first frame value.
      --last LAST           Set viewer last frame value.
      --bounds              Force scene bounds display mode.
      --review              Use review display settings.
      --reset               Reset settings.
      -v, --verbose         Verbose output.
      --script SCRIPT       Load and execute Python script.

Using the `--bounds` option tells AbcView to force displaying scenes in bounding box mode.
If the child bounds property (.childBnds) is written at export time, this means that AbcView
can get the bounds information from the archive rather than the GL scene, which usually means
a much faster load time.

Save your session at any time using File->Save/Save As. Loading a saved session is the
same as loading an Alembic scene ::

    $ abcview file.io


Python API
----------

You can also inspect and manipulate session data using the AbcView Python API, for example ::

    >>> from abcview.io import Session
    >>> s = Session()
    >>> s.add_file("file.abc")
    >>> s.save("file.io")

There are a number of properties you can set on session items, such as TRS values, color
and GL rendering mode ::

    >>> from abcview.io import Mode
    >>> item = s.items[0]

GL poylgon draw mode ::

    >>> item.mode = Mode.FILL

GL material color ::

    >>> item.color = (0, 255, 0)

Transation, rotation and scale ::

    >>> item.translate = (10, 0, 100)
    >>> item.rotate = (0, 90, 0, 0)
    >>> item.scale = (1, 1, 1)

AbcView supports both custom (non-animated) cameras and loading ICameras from Alembic
archives. To load an ICamera from the GUI, navigate to the ICamera in the objects 
widget and right-click->view through selected. You can also add it to the session using 
the IO module ::

    >>> s.add_camera(ICamera(get_object("shotcam.abc", 
                                        "ShotCam")
                             loaded=True))
    >>> s.save()

Module Contents
---------------

The AbcView Python API is divided into the IO and GL modules, widgets and utils.

I/O Module
~~~~~~~~~~

.. automodule:: abcview.io
   :members: 

GL Module
~~~~~~~~~

.. automodule:: abcview.gl
   :members: 

Widgets
~~~~~~~

.. automodule:: abcview.widget.tree_widget
   :members: 

.. automodule:: abcview.widget.viewer_widget
   :members: 

Utils
~~~~~

.. automodule:: abcview.utils
   :members: 

