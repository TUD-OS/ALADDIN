SRCS=bb_gemm.c
INCS=bb_gemm.h

ACCEL_NAME = bb_gemm
TEST_BIN = $(ACCEL_NAME)
export TRACE_OUTPUT_DIR=$(ACCEL_NAME)
ifndef WORKLOAD
  export WORKLOAD=bb_gemm
endif

include ../common/Makefile.common
include ../common/Makefile.tracer
