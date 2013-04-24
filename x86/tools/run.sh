#!/bin/bash
root=$(dirname $0)/../
(cd $root && "/cygdrive/d/Program Files (x86)/qemu/qemu-system-i386.exe" -fda "vm/a.img")
