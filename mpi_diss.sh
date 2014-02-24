#!/bin/sh

#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=02:00:00
#PBS -N combined
OMPI_MCA_mpi_yield_when_idle=0
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 2 /nethome/rsatpute3/aos_project2/mpi_diss 1000

