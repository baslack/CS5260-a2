#!/bin/sh -login
for i in 1 2 4 8 16;
do
	for j in 1024 1048576 1073741824;
	do
		qsub -l nodes=1:ppn=$i:student -F "$j" a2.pbs
	done
done