#!/bin/bash

cd ~/UE1/parmerge-student-1.0.0-Source
make

n=$((($RANDOM * $RANDOM)%1000000))
m=$((($RANDOM * $RANDOM)%1000000))
echo n=${n} m=${m}
srun -t 0:01 -p q_student -N 1 -c 32 ./bin/merge1_tester -n ${n} -m ${m} -p 11 -c
srun -t 0:01 -p q_student -N 1 -c 32 ./bin/merge2_tester -n ${n} -m $((${n} + 4298)) -p 11 -c
srun -t 0:01 -p q_student -N 1 -c 32 ./bin/merge3_tester -n ${n} -m ${m} -p 11 -c

cd ~/UE1/mpimv-student-1.0.0-Source
module load mpi/openmpiS
cmake .
make
n=$(($RANDOM % 4000))
echo n=${n}
srun -p q_student -t 1 -N 3 --ntasks-per-node=7 ./bin/mv1 -n ${n} -c
srun -p q_student -t 1 -N 3 --ntasks-per-node=7 ./bin/mv2 -n ${n} -c
