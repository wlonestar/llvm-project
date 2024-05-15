SHELL = bash

mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(patsubst %/,%,$(dir $(mkfile_path)))
llvm_build_dir := $(current_dir)/build

.DEFAULT_GOAL := build-llvm

build-llvm:
	@ninja -C $(llvm_build_dir) -j 8

install:
	@ninja -C $(llvm_build_dir) install

clean:
	@rm -rf $(llvm_build_dir)
	@rm -rf install
