#!/bin/bash -ex

# Define algumas variaveis
builddir=build/bios
isodir=${builddir}/iso
testdir=utilities/test

# Cria as pastas necessarias
mkdir ${isodir}
mkdir -p ${isodir}/boot/grub

# Copia os ficheiros para os sitios corretos
cp ${testdir}/grub/menu.lst ${isodir}/boot/grub/menu.lst
cp ${testdir}/grub/stage2_eltorito ${isodir}/boot/grub/stage2_eltorito
cp ${testdir}/grub/loader.elf ${isodir}/loader.elf
cp ${builddir}/kernel/kernel.elf ${isodir}/kernel.elf

mkisofs -J -R -l -b boot/grub/stage2_eltorito -V "CDROM" -boot-load-size 4 -boot-info-table -no-emul-boot -o ${builddir}/test.iso ${isodir}
rm -rf ${isodir}
qemu-system-x86_64 -cdrom ${builddir}/test.iso -serial stdio -vga std -boot d -m 512 -monitor vc:1024x768 -s
