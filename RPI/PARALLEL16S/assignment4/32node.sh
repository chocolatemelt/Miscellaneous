#!/bin/bash
#SBATCH --job-name=exp32n
#SBATCH -t 04:00:00
#SBATCH -D /gpfs/u/home/PCP5/PCP5ngkv/scratch/assignment4

srun  -o 64_32_1.log -N32 --time 60 --ntasks 2048 ./a4 131072 1 1 out1.dat
srun  -o 64_32_4.log -N32 --time 60 --ntasks 2048 ./a4 131072 4 1 out2.dat
srun  -o 64_32_8.log -N32 --time 60 --ntasks 2048 ./a4 131072 8 1 out3.dat
srun  -o 64_32_32.log -N32 --time 60 --ntasks 2048 ./a4 131072 32 1 out4.dat
srun  -o 16_32_1.log -N32 --time 60 --ntasks 512 ./a4 131072 1 1 out1.dat
srun  -o 16_32_4.log -N32 --time 60 --ntasks 512 ./a4 131072 4 1 out2.dat
srun  -o 16_32_8.log -N32 --time 60 --ntasks 512 ./a4 131072 8 1 out3.dat
srun  -o 16_32_32.log -N32 --time 60 --ntasks 512 ./a4 131072 32 1 out4.dat
srun  -o 4_32_1.log -N32 --time 60 --ntasks 128 ./a4 131072 1 1 out1.dat
srun  -o 4_32_4.log -N32 --time 60 --ntasks 128 ./a4 131072 4 1 out2.dat
srun  -o 4_32_8.log -N32 --time 60 --ntasks 128 ./a4 131072 8 1 out3.dat
srun  -o 4_32_32.log -N32 --time 60 --ntasks 128 ./a4 131072 32 1 out4.dat
srun  -o 1_32_1.log -N32 --time 60 --ntasks 32 ./a4 131072 1 1 out1.dat
srun  -o 1_32_4.log -N32 --time 60 --ntasks 32 ./a4 131072 4 1 out2.dat
srun  -o 1_32_8.log -N32 --time 60 --ntasks 32 ./a4 131072 8 1 out3.dat
srun  -o 1_32_32.log -N32 --time 60 --ntasks 32 ./a4 131072 32 1 out4.dat
