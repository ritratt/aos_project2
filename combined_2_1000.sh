#!/bin/sh

#PBS -q cs6210 
#PBS -l nodes=2:sixcore
#PBS -l walltime=00:01:00
#PBS -N combined_2_1000
OMPI_MCA_mpi_yield_when_idle=0
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 2 /nethome/rsatpute3/aos_project2/combined_barrier 6 1000
