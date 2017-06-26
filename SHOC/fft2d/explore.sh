#!/bin/sh

export WORKLOAD=FFT2D
export LLVM_HOME=/usr/lib/llvm-3.4
export LLVM_VERSION=3.4.2
export ALADDIN_HOME=$(readlink -f ../..)
export TRACER_HOME=/usr/local

make run-trace

mkdir -p explore
cd explore

run() {
    echo -n "Generating $1..."
    $ALADDIN_HOME/common/aladdin fft ../dynamic_trace.gz $1 >/dev/null
    echo "done"
}

OUTER="2 4 8 16 32 64 128"
INNER="2 4 8 16 32"

for outer in $OUTER; do
    for inner in $INNER; do
        cat >config_$outer-$inner <<EOF
partition,cyclic,rbuf,64,4,$inner
partition,cyclic,ibuf,64,4,$inner
partition,cyclic,c,2048,8,$inner
unrolling,FFT,loop1,$outer
unrolling,FFT,loop2,$inner
unrolling,FFT,loop3,$outer
unrolling,FFT,loop4,$outer
unrolling,FFT,loop5,$outer
unrolling,FFT,loop6,$inner
unrolling,FFT,loop7,$inner
pipelining,1
cycle_time,10
EOF

    run config_$outer-$inner
    cp fft_summary summary_$outer-$inner
    done
done

echo "Time AreaPower Name" > plot.data
for outer in $OUTER; do
    for inner in $INNER; do
        cycles=`grep 'Cycle :' summary_$outer-$inner | cut -d ' ' -f 3`
        area=`grep 'Total Area:' summary_$outer-$inner | cut -d ' ' -f 3`
        power=`grep 'Avg Power:' summary_$outer-$inner | cut -d ' ' -f 3`
        ap=`echo "$area * $power" | bc`
        echo "$cycles $ap $outer-$inner" >> plot.data
    done
done

Rscript ../explore.plot
