SRCS=convol.c
INCS=convol.h

ACCEL_NAME = convol
TEST_BIN = $(ACCEL_NAME)
export TRACE_OUTPUT_DIR=$(ACCEL_NAME)
ifndef WORKLOAD
  export WORKLOAD=convol
endif
include ../common/Makefile.common
include ../common/Makefile.tracer
