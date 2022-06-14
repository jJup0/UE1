#!/bin/bash

cd ~/UE1/parmerge-student-1.0.0-Source
make
srun -t 0:01 -p q_student -N 1 -c 32 ./bin/merge1_tester -n 1234567 -m 2234567 -p 16 -c
srun -t 0:01 -p q_student -N 1 -c 32 ./bin/merge2_tester -n 1234567 -m 2234567 -p 16 -c
srun -t 0:01 -p q_student -N 1 -c 32 ./bin/merge3_tester -n 1234567 -m 2234567 -p 16 -c

cd ~/UE1/mpimv-student-1.0.0-Source
module load mpi/openmpiS
cmake .
make
srun -p q_student -t 1 -N 3 --ntasks-per-node=7 ./bin/mv1 -n 4123 -c
srun -p q_student -t 1 -N 3 --ntasks-per-node=7 ./bin/mv2 -n 4123 -c
