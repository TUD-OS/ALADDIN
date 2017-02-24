#!/bin/sh

if [ $# -ne 2 ]; then
    echo "Usage: $0 <benchmark> <function>" >&2
    exit 1
fi

export LLVM_HOME=/usr/lib/llvm-3.4
export LLVM_VERSION=3.4.2
export ALADDIN_HOME=$(readlink -f ..)
export TRACER_HOME=/usr/local

export WORKLOAD=$2

cd $1 && make run-trace && cd example && $ALADDIN_HOME/common/aladdin $1 ../dynamic_trace.gz config_example
