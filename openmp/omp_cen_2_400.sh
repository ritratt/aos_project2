#!/bin/sh
#PBS -q cs6210
#PBS -l nodes=1:fourcore
#PBS -l walltime=00:01:00
#PBS -N omp_cen_2_400
/nethome/rsatpute3/aos_project2/openmp/barrier 2 400
