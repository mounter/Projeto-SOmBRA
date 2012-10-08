#!/bin/sh

make all SOmBRA.iso

cd bin
bochs -f sombra.bochsrc
