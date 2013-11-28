#!/usr/bin/env python

# Copyright (c) 2012-2013 Hadron Industries, Inc.

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import sys
import optparse
import re
import tempfile
import os

class g_speak_setter ():
    
    def __init__ (self, version):
        self.version = version
        self.gspeak = re.compile ('(g-speak)([0-9][0-9.]+[0-9])')
        self.gs = re.compile ('(gs)([0-9][0-9.]+[0-9])')
        self.either = re.compile ('(g-speak|gs)([0-9][0-9.]+[0-9])')

    def substitute (self, fn):
        f = file (fn, 'r')
        s = f.read ()
        f.close ()

        r = re.subn (self.either, '\g<1>' + self.version, s)
        if r[1] < 1:
            raise ValueError, 'no matching g-speak version information found in ' + fn
        s = r[0]

        handle, path = tempfile.mkstemp()
        with os.fdopen(handle, 'w') as f:
            f.write (s)

        os.rename (path, fn)
        
class g_speak_setter_app ():

    def find_version (self, options, args):
        if options.reset:
            return "3.8"
        if not re.match ('3\.[0-9]+', args[0]):
            raise ValueError, args[0]
        return args[0]

    def __init__ (self, files):

        self.files = files

        parser = optparse.OptionParser (version = '%prog 1.0')

        parser.add_option ("--reset",
                           action="store_true", dest="reset", default=False,
                           help="reset to original g-speak version")

        (options, args) = parser.parse_args ()

        if options.reset and (len(args) != 0):
            parser.error ("no version number should be provided with --reset")

        if (not options.reset) and (len(args) != 1):
            parser.error ("no version number provided")

        self.version = self.find_version (options, args)
        self.setter = g_speak_setter (self.version)

    def substitute (self):
        for x in self.files:
            self.setter.substitute (x)

if __name__ == "__main__":
    s = g_speak_setter_app ([ 'CMakeLists.txt' ])
    s.substitute ()
