# This test runs the md benchmark through the gem5 infrastructure and performs
# the output validation to show that the infrastructure is modeling all loads
# and stores correctly.

.PHONY: all trace run

all: md trace

md:
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/md/knn gem5-accel
	cp $(ALADDIN_HOME)/MachSuite/md/knn/md-knn-gem5-accel md

trace:
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/md/knn clean-trace
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/md/knn trace-binary
	$(MAKE) -C $(ALADDIN_HOME)/MachSuite/md/knn run-trace || true
	cp $(ALADDIN_HOME)/MachSuite/md/knn/dynamic_trace.gz .

run: md
	@echo off
	rm -f output.data
	sh run.sh


clean:
	rm -rf md
	rm -rf dynamic_trace.gz
	rm -rf output.data
