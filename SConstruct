# coding: utf-8
#
# Copyright (C) 2015 Gil Mendes <gil00mendes@gmail.com>
#
# Este sistema de compilação usa o SCons pois ele é escrito em Python e assim
# fica mais fácil de implmentar, manter e extender o sistema com novas
# funcionalidades.
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

# variaveis para definir no host. Não deve compilar o codigo com as nossas flags
# warning normais, pois alguns componentes não iram compilar.
host_flags = {
    'CCFLAGS': ['-pipe'],
    'CFLAGS': ['-std=gnu11'],
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

# adiciona um target para correr a interface de configuração
env['ENV']['KERNELVERSION'] = version['VER_STRING']
Alias('config', env.ConfigMenu('__config', ['Kconfig']))

# se o ficheiro de configuração não existir, todo o que podemos fazer é
# configurar. Apresenta um erro para notificar o utilizador a informar que
# necessita de configurar para proceder à compilação do sistema.
if not config.configured() or 'config' in COMMAND_LINE_TARGETS:
    RequireTarget('config',
        "Configuração em falta ou desatualizada. Por favor usar a opção 'config'.")
    Return()

# inicializa o gestor do toolchain
toolchain = ToolchainManager(config)
Alias('toolchain', Command('__toolchain', [], Action(toolchain.update, None)))

# se o toolchain estiver desatualizado, precisamos de o criar primeiro
if toolchain.check() or 'toolchain' in COMMAND_LINE_TARGETS:
    RequireTarget('toolchain',
        "O toolchain está desatualizado. Atualize usando a opção 'toolchain'")
    Return()

# agora, define o template para o target
for (k, v) in target_flags.items():
    target_env[k] = v

# o assembler integrado do Clang's não support codigo a 16-bit
target_env['ASFLAGS'] = ['-D__ASM__', '-no-integrated-as']

# define as flags corretas para as bibliotecas partilhadas
target_env['SHCCFLAGS'] = '$CCFLAGS -fPIC -DSHARED'
target_env['SHLINKFLAGS'] = '$LINKFLAGS -shared -Wl,-soname,${TARGET.name}'

# faz o override ao assemlber por defeito - isto usa-o diretamente, nos queremos
# que passe pelo compilador
target_env['ASCOM'] = '$CC $_CCCOMCOM $ASFLAGS -c -o $TARGET $SOURCES'

# adiciona uma action para os ficheiros ASM nas bibliotecas pratilhadas
from SCons.Tool import createObjBuilders
static_obj, shared_obj = createObjBuilders(target_env)
shared_obj.add_action('.S', Action('$CC $_CCCOMCOM $ASFLAGS -DSHARED -c -o $TARGET $SOURCES', '$ASCOMSTR'))

# adiciona mais algumas flags
if config.has_key('ARCH_ASFLAGS'):
    target_env['ASFLAGS'] += config['ARCH_ASFLAGS'].split()
if config.has_key('ARCH_CCFLAGS'):
    target_env['CCFLAGS'] += config['ARCH_CCFLAGS'].split()
if config.has_key('PLATFORM_ASFLAGS'):
    target_env['CCFLAGS'] += config['PLATFORM_ASFLAGS'].split()
if config.has_key('PLATFORM_CCFLAGS'):
    target_env['CCFLAGS'] += config['PLATFORM_CCFLAGS'].split()
target_env['CCFLAGS'] += config['EXTRA_CCFLAGS'].split()
target_env['CFLAGS'] += config['EXTRA_CFLAGS'].split()
target_env['CXXFLAGS'] += config['EXTRA_CXXFLAGS'].split()

# define o caminho para os componentes do toolchain
if os.environ.has_key('CC') and os.path.basename(os.environ['CC']) == 'ccc-analyzer':
    target_env['CC'] = os.environ['CC']
    target_env['ENV']['CCC_CC'] = toolchain.tool_path('clang')

    # Force a rebuild when doing static analysis.
    def decide_if_changed(dependency, target, prev_ni):
        return True
    target_env.Decider(decide_if_changed)
else:
    target_env['CC'] = toolchain.tool_path('clang')
if os.environ.has_key('CXX') and os.path.basename(os.environ['CXX']) == 'c++-analyzer':
    target_env['CXX'] = os.environ['CXX']
    target_env['ENV']['CCC_CXX'] = toolchain.tool_path('clang++')
else:
    target_env['CXX'] = toolchain.tool_path('clang++')
target_env['AS']      = toolchain.tool_path('as')
target_env['OBJDUMP'] = toolchain.tool_path('objdump')
target_env['READELF'] = toolchain.tool_path('readelf')
target_env['NM']      = toolchain.tool_path('nm')
target_env['STRIP']   = toolchain.tool_path('strip')
target_env['AR']      = toolchain.tool_path('ar')
target_env['RANLIB']  = toolchain.tool_path('ranlib')
target_env['OBJCOPY'] = toolchain.tool_path('objcopy')
target_env['LD']      = toolchain.tool_path('ld')

# build the target system
SConscript('source/SConscript', variant_dir = os.path.join('build',
    '%s-%s' % (config['ARCH'], config['PLATFORM'])))

# agora que temos a informação de todas as bibliotecas, atualizamos o sysroot do
# toolchain
toolchain.update_sysroot(manager)
