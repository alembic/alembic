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
import logging

from abcview import config
from abcview import style

__doc__ = """
AbcView is a graphical PyQt-based Alembic inspection 
and visualization tool. It offers a number of widgets 
to inspect and visualize Alembic data, as well as 
assemble hierarchical scenes.

What's new:

- fixes performance issue on GeForce cards
- fixes recursion bug when iterating properties

More information:
http://docs.alembic.io/python/abcview.html
"""

__todo__ = """
TODO:

- better object-level selection/framing
- better session cycle checking on load
- contextual session editing
- more stats (poly count, mem usage)
- support for lights and materials
- edit/remove cameras from viewer
- draggable, pop-out widgets
- support object paths in args
- save split window layouts
- better per-time data caching
- socket connections
- unit tests
"""

# logging handler, imported by most other modules
FORMAT = '%(asctime)-15s %(message)s'
logging.basicConfig(format=FORMAT)
log = logging.getLogger(config.__prog__)
log.setLevel(int(os.environ.get('ABCVIEW_LOG_LEVEL', logging.WARN)))

# abcview version info
version_string = config.__version__
version_tuple = tuple(int(v) for v in version_string.split('.'))
