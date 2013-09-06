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
import sys
import argparse
import traceback
import abcview

def create_parser():
    """
    Creates and returns the option parser.
    """
    parser = argparse.ArgumentParser(description=abcview.__doc__,
                   formatter_class=argparse.RawTextHelpFormatter
                  )
    parser.add_argument('filepath', type=str, nargs='*',
            help='File to open.')

    parser.add_argument('-f', '--frame', default=None, type=int,
            help='Frame to load in viewer.')
    parser.add_argument('--first', default=None, type=int,
            help='Set viewer first frame value.')
    parser.add_argument('--last', default=None, type=int,
            help='Set viewer last frame value.')
    parser.add_argument('--bounds', action='store_true',
            help='Force scene bounds display mode.')
    parser.add_argument('--review', action='store_true',
            help='Use review mode layout settings.')
    parser.add_argument('--reset', action='store_true',
            help='Restore default layout settings.')
    parser.add_argument('-v', '--verbose', action='store_true',
            help='Verbose standard output.')
    parser.add_argument('--script', 
            help='Load and execute Python script.')
    return parser

if __name__ == "__main__":
    try:
        from abcview.app import create_app
        args = create_parser().parse_args()
        sys.exit(create_app(files=args.filepath,
                            first_frame=args.first, 
                            last_frame=args.last, 
                            current_frame=args.frame,
                            script=args.script, 
                            bounds=args.bounds, 
                            review=args.review, 
                            reset=args.reset, 
                            verbose=args.verbose
                           )
                )
    except ImportError, e:
        print "abcview import error", e
    except KeyboardInterrupt:
        print "stopping..."
    except Exception, e:
        traceback.print_exc()
    sys.exit(1)
