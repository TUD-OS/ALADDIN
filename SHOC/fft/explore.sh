#!/bin/sh

export WORKLOAD=fft1D_512
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

for outer in 2 4 8 16 32; do
    for inner in 2 4 8 16 32; do
        cat >config_$outer-$inner <<EOF
partition,cyclic,work_x,2048,4,$inner
partition,cyclic,work_y,2048,4,$inner
partition,cyclic,data_x,2048,4,$outer
partition,cyclic,data_y,2048,4,$outer
partition,cyclic,DATA_x,2048,4,$inner
partition,cyclic,DATA_y,2048,4,$inner
partition,cyclic,smem,2304,4,$outer
partition,cyclic,reversed,32,4,$outer
partition,cyclic,sin_64,1792,4,8
partition,cyclic,cos_64,1792,4,8
partition,cyclic,sin_512,1792,4,8
partition,cyclic,cos_512,1792,4,8
unrolling,step1,outer,$outer
unrolling,step1,load,$inner
unrolling,step1,twiddles,$inner
unrolling,step1,store,$inner
unrolling,step2,outer,$outer
unrolling,step2,load,$inner
unrolling,step3,outer,$outer
unrolling,step3,load,$inner
unrolling,step3,store,$inner
unrolling,step4,outer,$outer
unrolling,step4,load,$inner
unrolling,step5,outer,$outer
unrolling,step5,load,$inner
unrolling,step5,store,$inner
unrolling,step6,outer,$outer
unrolling,step6,load,$inner
unrolling,step6,twiddles,$inner
unrolling,step6,store,$inner
unrolling,step7,outer,$outer
unrolling,step7,load,$inner
unrolling,step8,outer,$outer
unrolling,step8,load,$inner
unrolling,step8,store,$inner
unrolling,step9,outer,$outer
unrolling,step9,load,$inner
unrolling,step10,outer,$outer
unrolling,step10,load,$inner
unrolling,step10,store,$inner
unrolling,step11,outer,$outer
unrolling,step11,load,$inner
unrolling,step11,store,$inner
pipelining,1
cycle_time,10
EOF

    run config_$outer-$inner
    cp fft_summary summary_$outer-$inner
    done
done

echo "Time AreaPower Name" > plot.data
for outer in 2 4 8 16 32; do
    for inner in 2 4 8 16 32; do
        cycles=`grep 'Cycle :' summary_$outer-$inner | cut -d ' ' -f 3`
        area=`grep 'Total Area:' summary_$outer-$inner | cut -d ' ' -f 3`
        power=`grep 'Avg Power:' summary_$outer-$inner | cut -d ' ' -f 3`
        ap=`echo "$area * $power" | bc`
        echo "$cycles $ap $outer-$inner" >> plot.data
    done
done

Rscript ../explore.plot
