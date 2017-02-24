#!/bin/sh

export WORKLOAD=sha256_transform
export LLVM_HOME=/usr/lib/llvm-3.4
export LLVM_VERSION=3.4.2
export ALADDIN_HOME=$(readlink -f ../..)
export TRACER_HOME=/usr/local

make run-trace

mkdir -p explore
cd explore

run() {
    echo -n "Generating $1..."
    $ALADDIN_HOME/common/aladdin sha256 ../dynamic_trace.gz $1 >/dev/null
    echo "done"
}

steps="1 2 4 8 12 16"
for par in $steps; do
    for p1 in $steps; do
        for p2 in $steps; do
            for p3 in $steps; do
                cat >config_$par-$p1-$p2-$p3 <<EOF
partition,cyclic,data,64,1,$par
partition,cyclic,m,256,4,$par
partition,cyclic,k,256,4,$par
partition,cyclic,ctx,109,1,$par
unrolling,sha256_transform,loop1,$p1
unrolling,sha256_transform,loop2,$p2
unrolling,sha256_transform,loop3,$p3
pipelining,1
cycle_time,10
EOF

            run config_$par-$p1-$p2-$p3
            cp sha256_summary summary_$par-$p1-$p2-$p3
            done
        done
    done
done

echo "Time AreaPower Name" > plot.data
for par in $steps; do
    for p1 in $steps; do
        for p2 in $steps; do
            for p3 in $steps; do
                cycles=`grep 'Cycle :' summary_$par-$p1-$p2-$p3 | cut -d ' ' -f 3`
                area=`grep 'Total Area:' summary_$par-$p1-$p2-$p3 | cut -d ' ' -f 3`
                power=`grep 'Avg Power:' summary_$par-$p1-$p2-$p3 | cut -d ' ' -f 3`
                ap=`echo "$area * $power" | bc`
                echo "$cycles $ap $par-$p1-$p2-$p3" >> plot.data
            done
        done
    done
done

Rscript ../explore.plot
