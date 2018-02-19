# This test runs the stencil benchmark through the gem5 infrastructure and performs
# the output validation to show that the infrastructure is modeling all loads
# and stores correctly.

.PHONY: all trace run

all: stencil trace

stencil:
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/stencil/stencil3d gem5-accel
	cp $(ALADDIN_HOME)/MachSuite/stencil/stencil3d/stencil-stencil3d-gem5-accel stencil

trace:
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/stencil/stencil3d clean-trace
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/stencil/stencil3d trace-binary
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/stencil/stencil3d run-trace || true
	cp $(ALADDIN_HOME)/MachSuite/stencil/stencil3d/dynamic_trace.gz .

run: stencil
	@echo off
	rm -f output.data
	sh run.sh


clean:
	rm -rf stencil
	rm -rf dynamic_trace.gz
	rm -rf output.data
