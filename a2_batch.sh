#!/bin/sh -login
MAX=480;
MIN=60;
for k in $(seq $1);
do
	NAME="results_$k.csv";
	touch $NAME;
	echo "Queuing batch $k";

	#one node batches
	for i in 1 2 4 8;
	do
		for j in 78125000 156250000 312500000 625000000 1250000000 2500000000 5000000000 10000000000;
		do
			qsub -l nodes=1:ppn=$i -F "$j" a2.pbs;
		done
		CALC=$(($MAX/$i));
		Z=$((CALC>=$MIN?$CALC:$MIN));
		echo "Throttling $Z sec for 1 node, $i proc jobs.";
		sleep $Z;
	done

	#for some reason, 16 cores causes problems when batched
	#breaking it into two batches with a pause seems to help
	for i in 16;
	do
		for j in 78125000 156250000 312500000 625000000;
		do
			qsub -l nodes=1:ppn=$i -F "$j" a2.pbs;
		done
		CALC=$(($MAX/$i));
		Z=$((CALC>=$MIN?$CALC:$MIN));
		echo "Throttling $Z sec for 1 node, $i proc jobs.";
		sleep $Z;
	done
	for i in 16;
	do
		for j in 1250000000 2500000000 5000000000 10000000000;
		do
			qsub -l nodes=1:ppn=$i -F "$j" a2.pbs;
		done
		CALC=$(($MAX/$i));
		Z=$((CALC>=$MIN?$CALC:$MIN));
		echo "Throttling $Z sec for 1 node, $i proc jobs.";
		sleep $Z;
	done

	#4 node batches
	for i in 2 4;
	do
		for j in 78125000 156250000 312500000 625000000 1250000000 2500000000 5000000000 10000000000;
		do
			qsub -l nodes=$i:ppn=16 -F "$j" a2.pbs;
		done
		echo "Throttling $MIN sec for $i node, 16 proc jobs.";
		sleep $MIN;
	done

	#short pause to make sure everything has settled
	ZZZ=60;
	echo "Waiting $ZZZ sec for results.";
	sleep $ZZZ;
	echo "Compiling results from batch $k";
	echo "Batch $k starts" >> $NAME;
	cat a2.pbs.o* >> $NAME;
	echo "Batch $k ends" >> $NAME;
	echo "Cleaning up batch $k";
	rm a2.pbs.o*;
done