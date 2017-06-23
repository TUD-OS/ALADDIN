SRCS=fft2d.c
INCS=fft2d.h

ACCEL_NAME = fft2d
TEST_BIN = $(ACCEL_NAME)
export TRACE_OUTPUT_DIR=$(ACCEL_NAME)
ifndef WORKLOAD
  export WORKLOAD=fft2D
endif
include ../common/Makefile.common
include ../common/Makefile.tracer
