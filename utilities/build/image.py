#
# Copyright (C) 2009-2013 Gil Mendes
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

from SCons.Script import *
import tarfile, glob, os, tempfile, shutil, time

# Create a TAR archive containing the filesystem tree.
def fs_image_func(target, source, env):
    config = env['_CONFIG']

    # Create the TAR file.
    tar = tarfile.open(str(target[0]), 'w')

    def make_dir(name):
        if len(name) == 0:
            return
        try:
            tar.getmember(name)
        except KeyError:
            make_dir(os.path.dirname(name))
            tarinfo = tarfile.TarInfo(name)
            tarinfo.type = tarfile.DIRTYPE
            tarinfo.mtime = int(time.time())
            tar.addfile(tarinfo)

    # Copy everything into it.
    for (path, target) in env['FILES']:
        while path[0] == '/':
            path = path[1:]
        make_dir(os.path.dirname(path))
        tarinfo = tar.gettarinfo(str(target), path)
        tarinfo.uid = 0
        tarinfo.gid = 0
        tarinfo.uname = "root"
        tarinfo.gname = "root"
        tar.addfile(tarinfo, file(str(target)))
    for (path, target) in env['LINKS']:
        while path[0] == '/':
            path = path[1:]
        make_dir(os.path.dirname(path))
        tarinfo = tarfile.TarInfo(path)
        tarinfo.type = tarfile.SYMTYPE
        tarinfo.linkname = target
        tarinfo.mtime = int(time.time())
        tar.addfile(tarinfo)

    # Add in extra stuff from the directory specified in the configuration.
    if len(config['EXTRA_FSIMAGE']) > 0:
        cwd = os.getcwd()
        os.chdir(config['EXTRA_FSIMAGE'])
        for f in glob.glob('*'):
            tar.add(f)
        os.chdir(cwd)

    tar.close()
    return 0
def fs_image_emitter(target, source, env):
    # We must depend on every file that goes into the image.
    deps = [f for (p, f) in env['FILES']]
    return (target, source + deps)
fs_image_builder = Builder(action = Action(fs_image_func, '$GENCOMSTR'), emitter = fs_image_emitter)

# Function to generate an ISO image.
def iso_image_func(target, source, env):
    config = env['_CONFIG']

    fsimage = str(source[-1])
    cdboot = str(env['CDBOOT'])
    loader = str(env['LOADER'])
    kernel = str(env['KERNEL'])

    # Create the work directory.
    tmpdir = tempfile.mkdtemp('.pulsariso')
    os.makedirs(os.path.join(tmpdir, 'boot'))
    os.makedirs(os.path.join(tmpdir, 'pulsar', 'modules'))

    # Copy stuff into it.
    shutil.copy(kernel, os.path.join(tmpdir, 'pulsar'))
    shutil.copy(fsimage, os.path.join(tmpdir, 'pulsar', 'modules'))
    #for mod in env['MODULES']:
    #    shutil.copy(str(mod), os.path.join(tmpdir, 'pulsar', 'modules'))

    # Write the configuration.
    f = open(os.path.join(tmpdir, 'boot', 'loader.cfg'), 'w')
    f.write('set "timeout" 5\n')
    f.write('entry "SOmBRA" {\n')
    if len(config['FORCE_VIDEO_MODE']) > 0:
        f.write('   set "video_mode" "%s"\n' % (config['FORCE_VIDEO_MODE']))
    #f.write('   laos "/pulsar/kernel" "/pulsar/modules"\n')
    f.write('   initium "/pulsar/kernel"\n')
    f.write('}\n')
    f.close()

    # Create the loader by concatenating the CD boot sector and the loader
    # together.
    f = open(os.path.join(tmpdir, 'boot', 'cdboot.img'), 'w')
    f.write(open(cdboot, 'r').read())
    f.write(open(loader, 'r').read())
    f.close()

    # Create the ISO.
    verbose = (ARGUMENTS.get('V') == '1') and '' or '>> /dev/null 2>&1'
    if os.system('mkisofs -J -R -l -b boot/cdboot.img -V "Infinity CDROM" ' + \
            '-boot-load-size 4 -boot-info-table -no-emul-boot ' + \
            '-o %s %s %s' % (target[0], tmpdir, verbose)) != 0:
        print "Could not find mkisofs! Please ensure that it is installed."
        shutil.rmtree(tmpdir)
        return 1

    # Clean up.
    shutil.rmtree(tmpdir)
    return 0
def iso_image_emitter(target, source, env):
    assert len(source) == 1
    #return (target, [env['KERNEL'], env['LOADER'], env['CDBOOT']] + env['MODULES'] + source)
    return (target, [env['KERNEL'], env['LOADER'], env['CDBOOT']] + source)
iso_image_builder = Builder(action = Action(iso_image_func, '$GENCOMSTR'), emitter = iso_image_emitter)
