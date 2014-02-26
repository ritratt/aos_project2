#!/bin/sh
#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=02:00:00
#PBS -N omp_cen_8_1000
/nethome/rsatpute3/aos_project2/openmp/diss_omp 8 1000
