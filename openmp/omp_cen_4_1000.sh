#!/bin/sh
#PBS -q cs6210
#PBS -l nodes=1:sixcore
#PBS -l walltime=02:00:00
#PBS -N omp_cen_4_1000
/nethome/rsatpute3/aos_project2/openmp/barrier 4 1000
