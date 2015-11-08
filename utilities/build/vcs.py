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

# Obtain the revision number from the Git repository.
def revision_id():
    from subprocess import Popen, PIPE
    git = Popen(['git', 'rev-parse', '--short', 'HEAD'], stdout = PIPE, stderr = PIPE)
    revision = git.communicate()[0].strip()

    if git.returncode != 0:
        return None

    return revision