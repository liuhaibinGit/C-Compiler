#!/bin/bash

cd ../llvm_build/bin

function getsrc(){
	for file in $1/*.c
	do
	array=(${array[*]} $file)
	done
} 

getsrc ../../lib/musl/src/crypt
getsrc ../../lib/musl/src/ctype
getsrc ../../lib/musl/src/env
getsrc ../../lib/musl/src/errno
getsrc ../../lib/musl/src/exit
#getsrc ../../lib/musl/src/internal
getsrc ../../lib/musl/src/locale
getsrc ../../lib/musl/src/math
getsrc ../../lib/musl/src/multibyte
getsrc ../../lib/musl/src/misc
getsrc ../../lib/musl/src/search
getsrc ../../lib/musl/src/stdio
getsrc ../../lib/musl/src/stdlib
getsrc ../../lib/musl/src/string
getsrc ../../lib/musl/src/thread
#getsrc ../../lib/musl/src/time
getsrc ../../lib/mylib


./clang ${array[@]} -O3 -c -emit-llvm -nostdinc -nostdlib -fno-builtin -isystem ../../lib/musl/src/internal -isystem ../../lib/mylib -I ../../lib/musl/include -Dsyscall=#error -DCURRENT_LOCALE=UTF8_LOCALE -DCURRENT_UTF8=1 -Wno-unknown-pragmas -Wno-shift-op-parentheses -Wno-dangling-else -Wno-bitwise-op-parentheses

./llvm-link *.bc -o ../../lib/lib.bc
rm *.bc
cd ../..

