# This test runs the fft benchmark through the gem5 infrastructure and performs
# the output validation to show that the infrastructure is modeling all loads
# and stores correctly.

.PHONY: all trace run

all: fft trace

fft:
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/fft/transpose gem5-accel
	cp $(ALADDIN_HOME)/MachSuite/fft/transpose/fft-transpose-gem5-accel fft

trace:
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/fft/transpose clean-trace
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/fft/transpose trace-binary
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/fft/transpose run-trace || true
	cp $(ALADDIN_HOME)/MachSuite/fft/transpose/dynamic_trace.gz .

run: fft
	@echo off
	rm -f output.data
	sh run.sh


clean:
	rm -rf fft
	rm -rf dynamic_trace.gz
	rm -rf output.data
