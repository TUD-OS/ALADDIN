# This test runs the spmv benchmark through the gem5 infrastructure and performs
# the output validation to show that the infrastructure is modeling all loads
# and stores correctly.

.PHONY: all trace run

all: spmv trace

spmv:
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/spmv/crs gem5-accel
	cp $(ALADDIN_HOME)/MachSuite/spmv/crs/spmv-crs-gem5-accel spmv

trace:
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/spmv/crs clean-trace
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/spmv/crs trace-binary
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/spmv/crs run-trace || true
	cp $(ALADDIN_HOME)/MachSuite/spmv/crs/dynamic_trace.gz .

run: spmv
	@echo off
	rm -f output.data
	sh run.sh


clean:
	rm -rf spmv
	rm -rf dynamic_trace.gz
	rm -rf output.data
