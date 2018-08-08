#!/bin/zsh

export WORKLOAD=convol
export LLVM_HOME=/home/hrniels/Applications/llvm
export LLVM_VERSION=3.4.2
export ALADDIN_HOME=$(readlink -f ../..)
export TRACER_HOME=/usr/local

make run-trace

mkdir -p explore
cd explore

run() {
    echo -n "Generating $1..."
    $ALADDIN_HOME/common/aladdin convol ../dynamic_trace.gz $1 >/dev/null
    echo "done"
}

OUTER="2 4 8 16"
INNER="2 4 8 16"

for outer in 2 4 8 16 32 64; do
    for inner in 2 4 8 16 32 64; do
        max=$([ $inner -ge $outer ] && echo "$inner" || echo "$outer")
        cat >config_$outer-$inner <<EOF
partition,cyclic,data,2048,8,$max
partition,cyclic,kern,2048,8,$max
unrolling,convol,loop1,$outer
unrolling,convol,loop2,$inner
pipelining,1
cycle_time,10
EOF

    run config_$outer-$inner
    cp convol_summary summary_$outer-$inner
    done
done

echo "Time AreaPower Name" > plot.data
for outer in 2 4 8 16 32 64; do
    for inner in 2 4 8 16 32 64; do
        cycles=`grep 'Cycle :' summary_$outer-$inner | cut -d ' ' -f 3`
        area=`grep 'Total Area:' summary_$outer-$inner | cut -d ' ' -f 3`
        power=`grep 'Avg Power:' summary_$outer-$inner | cut -d ' ' -f 3`
        ap=$(($area * $power))
        echo "$cycles $ap $outer-$inner" >> plot.data
    done
done

Rscript ../explore.plot
