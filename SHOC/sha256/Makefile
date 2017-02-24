SRCS=sha256.c
INCS=sha256.h

ACCEL_NAME = sha256
TEST_BIN = $(ACCEL_NAME)
export TRACE_OUTPUT_DIR=$(ACCEL_NAME)
ifndef WORKLOAD
  export WORKLOAD=sha256
endif
include ../common/Makefile.common
include ../common/Makefile.tracer
