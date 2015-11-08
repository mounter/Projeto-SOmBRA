#
# Copyright (C) 2012-2014 Designture
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

from SCons.Script import *
import os

# Builder to pre-process a linker script
ld_script_builder = Builder(action = Action(
    '$CC $_CCCOMCOM $ASFLAGS -E -x c $SOURCE | grep -v "^\#" > $TARGET',
    '$GENCOMSTR'))

# Custom method to build a Pulsar application.
def pulsar_application_method(env, name, sources, **kwargs):
    flags = kwargs['flags'] if 'flags' in kwargs else {}

    target = File(name)

    # Add the application to the image.
    dist = env['_MANAGER']['dist']
    dist.AddFile(target, 'system/bin/%s' % (name))

    # Build the application.
    return env.Program(target, sources, **flags)

def pulsar_service_method(env, name, sources, **kwargs):
  # Prepare extra args
  flags = kwargs['flags'] if 'flags' in kwargs else {}

  target = File(name)

  # Add the service to the distribution environment
  env['_MANAGER']['dist'].AddFile(target, 'system/services/%s' % (name))

  # Build the service
  return env.Program(target, sources, **flags)

# Custom method to build a Infi library.
def pulsar_library_method(env, name, sources, **kwargs):
    manager = env['_MANAGER']

    build_libraries = kwargs['build_libraries'] if 'build_libraries' in kwargs else []
    include_paths = kwargs['include_paths'] if 'include_paths' in kwargs else []
    flags = kwargs['flags'] if 'flags' in kwargs else {}

    # Register this library with the build manager.
    manager.AddLibrary(name, build_libraries, include_paths)

    # Modify the target path so that libraries all get placed in the build
    # library directory.
    target = File('%s/lib%s.so' % (str(env['_LIBOUTDIR']), name))

    # Add the library to the distribution environment.
    dist = manager['dist']
    dist.AddFile(target, 'system/lib/lib%s.so' % (name))

    # Build the library.
    return env.SharedLibrary(target, sources, **flags)
