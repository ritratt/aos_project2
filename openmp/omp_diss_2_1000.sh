#!/bin/sh
#PBS -q cs6210
#PBS -l nodes=1:fourcore
#PBS -l walltime=00:01:00
#PBS -N omp_diss_2_1000
/nethome/rsatpute3/aos_project2/openmp/diss_omp 2 1000
