#!/bin/sh -login
rm results.csv;
touch results.csv;
MAX=960;
MIN=180;
for k in $(seq $1);
do
	echo "Queuing batch $k";
	for i in 1 2 4 8 16;
	do
		for j in 1000 100000 1000000 100000000 500000000 1000000000 5000000000 10000000000;
		do
			qsub -l nodes=1:ppn=$i -F "$j" a2.pbs;
		done
		CALC=$(($MAX/$i));
		Z=$((CALC>=$MIN?$CALC:$MIN));
		echo "Throttling $Z sec for 1 node, $i proc jobs.";
		sleep $Z;
	done
	for i in 2 4;
	do
		for j in 1000 100000 1000000 100000000 500000000 1000000000 5000000000 10000000000;
		do
			qsub -l nodes=$i:ppn=16 -F "$j" a2.pbs;
		done
		echo "Throttling $MIN sec for $i node, 16 proc jobs.";
		sleep $MIN;
	done
	ZZZ=60;
	echo "Waiting $ZZZ sec for results.";
	sleep $ZZZ;
	echo "Compiling results from batch $k";
	echo "Batch $k starts" >> results.csv;
	cat a2.pbs.o* >> results.csv;
	echo "Batch $k ends" >> results.csv
	echo "Cleaning up batch $k";
	rm a2.pbs.o*;
done