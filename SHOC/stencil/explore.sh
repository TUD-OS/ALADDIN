#!/bin/sh

export WORKLOAD=stencil
export LLVM_HOME=/usr/lib/llvm-3.4
export LLVM_VERSION=3.4.2
export ALADDIN_HOME=$(readlink -f ../..)
export TRACER_HOME=/usr/local

make run-trace

mkdir -p explore
cd explore

run() {
    echo -n "Generating $1..."
    $ALADDIN_HOME/common/aladdin stencil ../dynamic_trace.gz $1 >/dev/null
    echo "done"
}

steps="1 2 4 8 12"
for p1 in $steps; do
    for p2 in $steps; do
        cat >config_$p1-$p2 <<EOF
partition,cyclic,orig,4096,4,$p1
partition,cyclic,sol,4096,4,$p2
partition,cyclic,filter,36,4,$p1
unrolling,stencil,outer,$p1
unrolling,stencil,inner,$p2
pipelining,1
cycle_time,10

EOF

    run config_$p1-$p2
    cp stencil_summary summary_$p1-$p2
    done
done

echo "Time AreaPower Name" > plot.data
for p1 in $steps; do
    for p2 in $steps; do
        cycles=`grep 'Cycle :' summary_$p1-$p2 | cut -d ' ' -f 3`
        area=`grep 'Total Area:' summary_$p1-$p2 | cut -d ' ' -f 3`
        power=`grep 'Avg Power:' summary_$p1-$p2 | cut -d ' ' -f 3`
        ap=`echo "$area * $power" | bc`
        echo "$cycles $ap $p1-$p2" >> plot.data
    done
done

Rscript ../explore.plot
