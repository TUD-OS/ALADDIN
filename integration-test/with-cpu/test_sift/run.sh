#!/usr/bin/env bash

cfg_home=${ALADDIN_HOME}/integration-test/with-cpu/test_sift
gem5_dir=${ALADDIN_HOME}/../..
bmk_home=${CORTEXSUITE_HOME}/vision/benchmarks/sift/src/c

${gem5_dir}/build/X86/gem5.opt \
  --debug-flags=HybridDatapath,Aladdin \
  --outdir=${cfg_home}/outputs \
  ${gem5_dir}/configs/aladdin/aladdin_se.py \
  --enable_prefetchers \
  --num-cpus=1 \
  --mem-size=4GB \
  --mem-type=ddr3_1600_x64  \
  --sys-clock=1GHz \
  --cpu-type=timing \
  --caches \
  --aladdin_cfg_file=${cfg_home}/gem5.cfg \
  -c ${bmk_home}/sift-gem5 \
  -o "${bmk_home}/../../data/sim_fast" \
  > stdout.gz
