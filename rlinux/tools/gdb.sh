#!/bin/bash

"/cygdrive/d/Program Files (x86)/qemu/qemu-system-i386.exe" -fda "vm/a.img"   -gdb tcp::1234,nowait,nodelay,server,ipv4 -S
