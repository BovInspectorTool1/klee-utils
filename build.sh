#!/bin/bash

cmake \
  -DENABLE_SOLVER_STP=OFF \
  -DENABLE_POSIX_RUNTIME=ON \
  -DENABLE_KLEE_UCLIBC=ON \
  -DKLEE_UCLIBC_PATH=/qsp/WIP/klee/klee-uclibc \
  -DGTEST_SRC_DIR=/qsp/WIP/klee/googletest-release-1.7.0 \
  -DENABLE_SYSTEM_TESTS=ON \
  -DENABLE_UNIT_TESTS=ON \
  -DLLVM_CONFIG_BINARY=/usr/lib/llvm-3.8/bin/llvm-config \
  -DLLVMCC=/usr/lib/llvm-3.8/bin/clang \
  -DLLVMCXX=/usr/lib/llvm-3.8/bin/clang++ \
  /qsp/WIP/klee/klee
