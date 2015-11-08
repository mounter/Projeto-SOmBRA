# coding: utf-8
#
# Copyright (C) 2015 Gil Mendes <gil00mendes@gmail.com>
#
# Este sistema de compilação usa o SCons pois ele é escrito em Python e assim fica mais fácil de implmentar, manter e extender o sistema com novas funcionalidades.
#

# informação da release
version = {
    'VER_RELEASE':  1,
    'VER_UPDATE':   0,
    'VER_REVISION': 0
}

# warning flags do C/C++
cc_warning_flags = [
    '-Wall', '-Wextra', '-Wno-variadic-macros', '-Wno-unused-parameter',
    '-Wwrite-strings', '-Wmissing-declarations', '-Wredundant-decls',
    '-Wno-format', '-Werror', '-Wno-error=unused',
]

# warning flags especificas para o C++
cxx_warning_flags = [
    '-Wold-style-cast', '-Wsign-promo',
]

# variaveis para o target
target_flags = {
    'CCFLAGS': cc_warning_flags + ['-gdwarf-2', '-pipe', '-fno-omit-frame-pointer'],
    'CFLAGS': ['-std=gnu11'],
    'CXXFLAGS': cxx_warning_flags + ['-std=c++11'],
    'ASFLAGS': ['-D__ASM__'],
}

# variaveis para definir no host. Não deve compilar o codigo com as nossas flags warning normais, pois alguns componentes não iram compilar.
host_flags = {
    'CCFLAGS': ['-pipe'],
    'CFLAGS': ['-std=gnu99'],
    'CXXFLAGS': filter(lambda f: f not in [
        '-Wmissing-declarations', '-Wno-variadic-macros',
        '-Wno-unused-but-set-variable'], cc_warning_flags),
    'YACCFLAGS': ['-d'],
}

##########################################
# Configuração do ambiente de construção #
##########################################

import os, sys, SCons.Errors

# adiciona o caminho da pasta utilities ao path
sys.path = [os.path.abspath(os.path.join('utilities', 'build'))] + sys.path

from manager import BuildManager
from kconfig import ConfigParser
from toolchain import ToolchainManager
from util import RequireTarget
import vcs

# define a versão da compilação
version['VER_STRING'] = '%d.%d' % (
    version['VER_RELEASE'],
    version['VER_UPDATE']
)

if version['VER_REVISION']:
    version['VER_STRING'] += '.%d' % (version['VER_REVISION'])

revision = vcs.revision_id()
if revision:
    version['VER_STRING'] += '-%s' % (revision)

# altera o Decider para MD5-timestamp para aumentar a velocidade de compilação
Decider('MD5-timestamp')

host_env = Environment(ENV = os.environ)
target_env = Environment(platform = 'posix', ENV = os.environ)
manager = BuildManager(host_env, target_env)

# carrega as configurações (se já existirem)
config = ConfigParser('.config')
manager.AddVariable('_CONFIG', config)

# export o Config, Manager e a variavél Version para alem deste Script
Export('config', 'manager', 'version')

# define o template de ambiente do host
for (k, v) in host_flags.items():
    host_env[k] = v

# os hosts Darwin possivelmente têm as bibliotecas em /opt, não é Gil? ;)
if os.uname()[0] == 'Darwin':
    host_env['CPPPATH'] = ['/opt/local/include']
    host_env['LIBPATH'] = ['/opt/local/lib']

# cria o ambiente para o host e para os utilitários
env = manager.CreateHost(name = 'host')
SConscript('utilities/SConscript', variant_dir = os.path.join('build', 'host'), exports = ['env'])
