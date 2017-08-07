#!/bin/sh

export WORKLOAD=do_bb_gemm
export LLVM_HOME=/usr/lib/llvm-3.4
export LLVM_VERSION=3.4.2
export ALADDIN_HOME=$(readlink -f ../..)
export TRACER_HOME=/usr/local

make run-trace

mkdir -p explore
cd explore

run() {
    echo -n "Generating $1..."
    $ALADDIN_HOME/common/aladdin bb_gemm ../dynamic_trace.gz $1 >/dev/null
    echo "done"
}

steps="1 2 4 8"
for p1 in $steps; do
    for p2 in $steps; do
        for p3 in $steps; do
            max=`echo -e "$p1\n$p2\n$p3" | awk '$0>x{x=$0};END{print x}'`
            cat >config_$p1-$p2-$p3 <<EOF
partition,cyclic,x,4096,4,$max
partition,cyclic,y,4096,4,$max
partition,cyclic,z,4096,4,$max
unrolling,do_bb_gemm,loopi,$p1
unrolling,do_bb_gemm,loopk,$p2
unrolling,do_bb_gemm,loopj,$p3
pipelining,1
cycle_time,10
EOF

        run config_$p1-$p2-$p3
        cp bb_gemm_summary summary_$p1-$p2-$p3
        done
    done
done

echo "Time AreaPower Name" > plot.data
for p1 in $steps; do
    for p2 in $steps; do
        for p3 in $steps; do
            cycles=`grep 'Cycle :' summary_$p1-$p2-$p3 | cut -d ' ' -f 3`
            area=`grep 'Total Area:' summary_$p1-$p2-$p3 | cut -d ' ' -f 3`
            power=`grep 'Avg Power:' summary_$p1-$p2-$p3 | cut -d ' ' -f 3`
            ap=`echo "$area * $power" | bc`
            echo "$cycles $ap $p1-$p2-$p3" >> plot.data
        done
    done
done

Rscript ../explore.plot
