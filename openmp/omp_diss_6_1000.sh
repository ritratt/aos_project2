#!/bin/sh
#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=02:00:00
#PBS -N omp_diss_6_1000
/nethome/rsatpute3/aos_project2/openmp/diss_omp 6 100
