#!/bin/bash

cd ~/UE1/parmerge-student-1.0.0-Source
make
for i in {1..10}
do
    m=$(($RANDOM % 353))
    n=$(($RANDOM % 311))
    echo -n ${n} -m ${m}
    srun -t 0:01 -p q_student -N 1 -c 32 ./bin/merge2_tester -n ${n} -m ${m} -p 16 -c
done

# srun -t 0:01 -p q_student -N 1 -c 32 ./bin/merge1_tester -n 10 -m 20 -p 16
# srun -t 0:01 -p q_student -N 1 -c 32 ./bin/merge1_tester -n 100 -m 20 -p 16
# srun -t 0:01 -p q_student -N 1 -c 32 ./bin/merge1_tester -n 10 -m 200 -p 16
srun -t 0:01 -p q_student -N 1 -c 32 ~/UE1/parmerge-student-1.0.0-Source/bin/merge2_tester -n 6015 -m 1577 -p 16 -c
~/UE1/parmerge-student-1.0.0-Source/bin/merge2_tester -n 6015 -m 1577 -p 16 -c
 