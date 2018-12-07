#!/bin/bash
mkdir llvm_build
cd llvm_build

cmake -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD="" -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly -DLLVM_DEFAULT_TARGET_TRIPLE=wasm32-unknown-unknown -DCMAKE_EXE_LINKER_FLAGS=-static-libgcc -static-libstdc++ ../llvm

make clang llvm-link llc -j4
cd ..

mkdir binaryen_build
cd binaryen_build

cmake -DCMAKE_BUILD_TYPE=Release ../binaryen

make s2wasm wasm-opt wasm-as wasm-dis wasm-merge -j4
cd ..
