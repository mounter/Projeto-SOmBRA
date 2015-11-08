#
# Este Build System e baseado no do projeto 'Initium'
# criado pelo Gil Mendes (gil0mendes).
#
# O codigo original pode ser encontrado aqui:
# https://github.com/gil0mendes/Initium
#
# Este sistema usa o SCons que e desenvolvido em
# Python de forma a facilitar a manutencao e de
# forma facil extender a ferramenta com novas
# funcionalidades.
#

import os, SCons.Errors
from SCons.Script import *

# Helper for creating source lists with certain files only enabled by config
# settings.
def FeatureSources(config, files):
    output = []
    for f in files:
        if type(f) == tuple:
            if config[f[0]]:
                output.append(File(f[1]))
        else:
            output.append(File(f))
    return output

# Raise a stop error.
def StopError(str):
    # Don't break if the user is trying to get help.
    if GetOption('help'):
        Return()
    else:
        raise SCons.Errors.StopError(str)

# Test if a program exists by looking it up in the path.
def which(program):
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ['PATH'].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file

    return None

# Raise an error if a certain target is not specified.
def RequireTarget(target, error):
    if GetOption('help') or target in COMMAND_LINE_TARGETS:
        return
    raise SCons.Errors.StopError(error)