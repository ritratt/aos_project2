#!/bin/sh

#PBS -q cs6210
#PBS -l nodes=8:sixcore
#PBS -l walltime=02:00:00
#PBS -N mpi_tour_8_1000
OMPI_MCA_mpi_yield_when_idle=0
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 8 /nethome/rsatpute3/aos_project2/mpi_tour 1000

