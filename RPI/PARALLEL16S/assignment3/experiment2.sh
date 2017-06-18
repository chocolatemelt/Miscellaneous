#!/bin/bash
#SBATCH --job-name=exp1.0
#SBATCH -t 04:00:00
#SBATCH -D /gpfs/u/home/PCP5/PCP5ngkv/assignment3
#
#SBATCH --mail-type=ALL
#SBATCH --mail-user=zhangk6@rpi.edu

srun -O -o t0.log -N1    --time 60 --ntasks 32   ./assignment3 8192 100 50 &
srun -O -o t1.log -N2    --time 60 --ntasks 64   ./assignment3 8192 100 50 &
srun -O -o t2.log -N4    --time 60 --ntasks 128  ./assignment3 8192 100 50 &
srun -O -o t3.log -N8    --time 60 --ntasks 506  ./assignment3 8192 100 50 &
srun -O -o t4.log -N16   --time 60 --ntasks 512  ./assignment3 8192 100 50 &
srun -O -o t5.log -N32   --time 60 --ntasks 1024 ./assignment3 8192 100 50 &
srun -O -o t6.log -N64   --time 60 --ntasks 2048 ./assignment3 8192 100 50 &
srun -O -o t7.log -N128  --time 60 --ntasks 4096 ./assignment3 8192 100 50 &
wait
