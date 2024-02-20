#!/bin/bash
#SBATCH --job-name=gpu-job
#SBATCH --output=cuda_out.log
#SBATCH --error=cuda_out.err
#SBATCH --partition=gpu
#SBATCH --gres=gpu:1
#SBATCH --nodes=1
#SBATCH --tasks-per-node=1

module load CUDA
nvcc -arch=sm_37 integral.cu -o exe
srun ./exe 0 8 1000