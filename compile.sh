#!/bin/bash

cd llvm_build/bin
./clang ../../$1 -O -c --target=wasm32-unknown-unknown -emit-llvm -nostdinc -nostdlib -D WEBASSEMBLY  -I ../../lib/musl/include -I ../../lib/mylib -o file.bc 

./llvm-link file.bc ../../lib/lib.bc -only-needed -o sum.bc
./llc sum.bc -march=wasm32 -filetype=asm -asm-verbose=false -thread-model=single -data-sections -function-sections -o file.o

mv file.o ../../binaryen_build/bin
rm file.bc sum.bc 

cd ../../binaryen_build/bin

./s2wasm file.o --allocate-stack 20480 --validate wasm -o out
./wasm-opt out -o ../../$2.wasm
./wasm-dis ../../$2.wasm -o ../../$2.wast
rm file.o out

cd ../..

