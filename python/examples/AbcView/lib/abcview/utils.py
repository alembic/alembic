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

import re
import alembic
from functools import partial

# Python 2.5 backwards-compatibility import logic for json
JSON = None
try:
    import json as JSON
except ImportError:
    import simplejson
json = JSON or simplejson

def get_schema_info(obj):
    """
    Returns a tuple of the shema, title and base strings.

    Example ::
    
        >>> obj.getMetaData().serialize()
        'schema=AbcGeom_Xform_v3;schemaObjTitle=AbcGeom_Xform_v3:.xform'
    
        >>> get_schema_info(obj)
        ('AbcGeom_Xform_v3', 'AbcGeom_Xform_v3:.xform', None)

    :param obj: Alembic IObject.

    :return: Tuple of strings.
    """
    md = obj.getMetaData()
    return md.get('schema'), md.get('schemaObjTitle'), md.get('schemaBaseType')

def find_objects(obj, name):
    """
    Recursive generator function that yields objects with
    names matching given name regex.

    :param obj: Alembic object
    :param name: Name regular expression to match
    :yeild: Alembic object
    """
    if re.match(name, obj.getFullName()):
        yield obj
    else:
        for child_object in obj.children:
            for obj in find_objects(child_object, name):
                yield obj

def get_object(filepath, fullname):
    """
    Returns an Alembic object from filepath matching the full path
    name fullname.

    :param filepath: file path to archive
    :param fullname: full path to the object
    """
    arch = alembic.Abc.IArchive(str(filepath))
    obj = arch.getTop()
    for name in str(fullname).split("/"):
        if name:
            obj = obj.getChild(name)
    return obj

class memoized(object):
    """cache the return value of a method
    
    This class is meant to be used as a decorator of methods. The return value
    from a given method invocation will be cached on the instance whose method
    was invoked. All arguments passed to a method decorated with memoize must
    be hashable.
    
    If a memoized method is invoked directly on its class the result will not
    be cached. Instead the method will be invoked like a static method:
    class Obj(object):
        @memoize
        def add_to(self, arg):
            return self + arg
    Obj.add_to(1) # not enough arguments
    Obj.add_to(1, 2) # returns 3, result is not cached
    """
    def __init__(self, func):
        self.func = func

    def __get__(self, obj, objtype=None):
        if obj is None:
            return self.func
        return partial(self, obj)

    def __call__(self, *args, **kw):
        obj = args[0]
        try:
            cache = obj.__cache
        except AttributeError:
            cache = obj.__cache = {}
        key = (self.func, args[1:], frozenset(kw.items()))
        try:
            res = cache[key]
        except KeyError:
            res = cache[key] = self.func(*args, **kw)
        return res
