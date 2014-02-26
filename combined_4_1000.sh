#!/bin/sh

#PBS -q cs6210 
#PBS -l nodes=4:sixcore
#PBS -l walltime=00:10:00
#PBS -N combined_4_1000
OMPI_MCA_mpi_yield_when_idle=0
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 4 /nethome/rsatpute3/aos_project2/combined_barrier 2 1000
