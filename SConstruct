# -*- coding: utf-8 -*-
# vim: ft=python

import os
from os.path import join

PROJROOT = os.getcwd()
BUILDDIR = 'build'
toolchain_prefix = ''

nasmbuild = Builder(action = 'nasm -o $TARGET -f elf -I `dirname $SOURCES`/ $NASMFLAGS $SOURCES',
                    suffix = '.o', src_suffix = '.asm')

env = Environment(
        PROJROOT = PROJROOT,
        BUILDERS = {'NASMObject': nasmbuild},
        CC = toolchain_prefix + 'gcc',
        AR = toolchain_prefix + 'ar',
        PROGSUFFIX = '.elf',
        CCFLAGS = ['-O0',
            '-m32', '-march=i386', '-msoft-float', '-nostdinc',
            '-fno-common', '-fno-exceptions', '-fno-non-call-exceptions',
            '-ffreestanding', '-fno-stack-protector', '-fno-builtin',
            '-Werror', '-Wall', '-Wno-unused-function', '-Wno-error=unused-variable',
            '-Wno-error=unused-but-set-variable'],
        LINKFLAGS = ['-m32', '-nostdlib'],
        CPPPATH = [join(PROJROOT, 'libc'), join(PROJROOT, 'kernel/uinc')],
        LIBPATH = [join(PROJROOT, 'lib')],
        LIBS = ['gcc', 'c'],
        )

init = SConscript('init/SConscript',
                      dumplicate = 0, exports = {'env': env},
                      variant_dir = join(BUILDDIR, 'init'))
kernel = SConscript('kernel/SConscript',
                      dumplicate = 0, exports = {'env': env},
                      variant_dir = join(BUILDDIR, 'kernel'))
Depends(kernel, init)
