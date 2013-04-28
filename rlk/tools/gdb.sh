#!/bin/bash

root=$(dirname $0)/../
(cd $root && "/cygdrive/d/Tools/qemu/qemu-system-i386.exe" -m 512 -fda "vm/a.img"   -gdb tcp::1234,nowait,nodelay,server,ipv4 -S)
