#!/bin/bash

export PATH=$PATH:/cygdrive/g/Projetos/SOmBRA/SDK/nasm:/cygdrive/g/Projetos/SOmBRA/SDK/x86_64-pc-elf/bin

make -C src/loader
cp ./src/loader/loader.elf ./bin
make clean -C src/loader
make all SOmBRA.iso
