#!/bin/sh

set -eu

# $1: thread to block ratio
# $2: starting thread
# $3: ending thread

for i in $(seq $2 $3)
do
  export OMP_NUM_THREADS=$i;
  ./hw2 520 $(( $i*$1  )) $(( $i*$1 )) | tail -1 >> time_val.txt;
done

echo " " >> time_val.txt; 
