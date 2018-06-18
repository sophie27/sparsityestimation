#!/bin/bash

T_zero="$(($(date +%s)))"
RUNS=$(seq 1 1000)
MS=$(seq 500 500 12000)
KS=$(seq 500 500 12000)
N_PARALLEL_TASKS=10 #Attention! Number of parallel disk; to be set according the used CPU
RUN_SESSION=0

for run in $RUNS; do
	if [ $RUN_SESSION -eq $N_PARALLEL_TASKS ]; then	
		while [ 1 -eq 1 ]; do
			sleep 1
			RUNNING_PROCESSES=$(ps aux | grep gofree | grep -v grep | wc -l)
			if [ $RUNNING_PROCESSES -lt $N_PARALLEL_TASKS ]; then
				RUN_SESSION=$(($RUNNING_PROCESSES-1))
				break
			fi
		done
	fi
	nohup ./gofree $run > nonoise_out_${run}.txt &
	RUN_SESSION=$(($RUN_SESSION + 1))
done


while [ 1 -eq 1 ]; do
	sleep 1
	RUNNING_PROCESSES=$(ps aux | grep gofree | grep -v grep | wc -l)
		if [ $RUNNING_PROCESSES -eq 0 ]; then
			break
		fi
done

rm G.txt
rm km.txt

paste nonoise_out_*.txt | awk '{ s=$1; for(i=2; i<=NF; i+=1) s+=$i; print s/NF}' > G.txt

for k in $KS; do
	for m in $MS; do
		echo "$k $m" >> km.txt
	done
done

paste km.txt G.txt > kmG.txt	


T="$(($(date +%s)-T_zero))"
echo $T >> Time_Elapsed.txt

gnuplot Gnonoise.gnu
ps2pdf G.eps
