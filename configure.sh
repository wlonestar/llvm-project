#!/bin/bash

llvm_project_dir="$(cd "$(dirname "$0")" && pwd)"
llvm_src_dir="$llvm_project_dir/llvm"
llvm_build_dir="$llvm_project_dir/build"
llvm_install_dir="/home/wjl/miniconda3/envs/kernel"

echo $llvm_project_dir
echo $llvm_src_dir
echo $llvm_build_dir
echo $llvm_install_dir

host_clang="$(which "clang-16")"
host_clangxx="$(which "clang++-16")"
host_linker="$(which "lld-16")"

if [[ -z "$host_clang" ]]; then
  echo "please install clang 16 or higher version"
  exit 1
fi

if [[ -z "$host_linker" ]]; then
  echo "please install lld 16 or higher version"
  exit 1
fi

echo $host_clang
echo $host_clangxx
echo $host_linker

cmake -G Ninja -S $llvm_src_dir -B $llvm_build_dir \
	-DCMAKE_C_COMPILER=$host_clang \
	-DCMAKE_CXX_COMPILER=$host_clangxx \
	-DCMAKE_LINKER=${HOST_LINKER} \
	-DCMAKE_INSTALL_PREFIX=$llvm_install_dir \
	-DCMAKE_BUILD_TYPE=Release \
	-DLLVM_INSTALL_UTILS=ON \
	-DLLVM_ENABLE_RTTI=ON \
	-DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld" \
	-DLLVM_ENABLE_RUNTIMES="compiler-rt" \
	-DLLVM_PARALLEL_COMPILE_JOBS=16 \
	-DLLVM_PARALLEL_LINK_JOBS=2 \
	-DLLVM_TARGETS_TO_BUILD="X86"
