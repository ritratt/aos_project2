#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <omp.h>

#define NUM_THREADS 2
#define OMP_ROUNDS 1

//int NUM_THREADS;
//int OMP_ROUNDS;

//Global vars reqd for mpi
int mpi_rank, mpi_size, mpi_sense, tag;
MPI_Status status;
int NUM_PROC, MPI_ROUNDS;

//Global vars reqd for omp
typedef struct flags
{
        int myflags[2][OMP_ROUNDS];
        int* partnerflags[2][OMP_ROUNDS];
}flags;

flags allnodes[NUM_THREADS];

void omp_dissemination_barrier_init()
{
        int i, j, k;
        for(i = 0; i < NUM_THREADS; i++)
        {
                for(j = 0; j < 2; j++)
                {
                        for(k = 0; k < OMP_ROUNDS; k++)
                        {
                                allnodes[i].myflags[j][k] = 0;
                        }
                }
        }
}

void omp_dissemination_barrier(flags* localflags, int *sense, int * parity)
{
        int round;
        int p = *parity;
        for(round = 0; round < OMP_ROUNDS; round++)
        {
                #pragma omp critical
                {
                        *localflags->partnerflags[p][round] = *sense;
//                      printf("partner flag:%d parity = %d round = %d\n", *localflags->partnerflags[p][round], p, round);
                }
                while(localflags->myflags[p][round] != *sense)
                {

                }
        }
        if(*parity == 1)
                *sense = (*sense + 1) % 2;
        else
                *parity = 1 - *parity;
}

void mpi_dissemination_barrier_init()
{
	mpi_sense = 1;
	tag = 1;
	NUM_PROC = mpi_size;
	MPI_ROUNDS = ceil(log(NUM_PROC)/log(2));
}

void mpi_dissemination_barrier()
{
	int round;
	for(round = 0; round < MPI_ROUNDS; round++)
	{
		int mysaypartner = (mpi_rank + (int) pow((double)2, (double)round)) % NUM_PROC;
		int mylistenpartner = (mpi_rank - (int) pow((double)2, (double)round) + NUM_PROC) % NUM_PROC;
		MPI_Send(&mpi_sense, 1, MPI_INT, mysaypartner, tag, MPI_COMM_WORLD);
		int recd_sense;
                MPI_Recv(&recd_sense, 1, MPI_INT, mylistenpartner, tag, MPI_COMM_WORLD, &status);
	}
	/*Toggle the sense*/
	mpi_sense = (mpi_sense + 1) % 2;
}

int main(int argc, char **argv)
{
	printf("NUM_THREADS is %d and OMP_ROUND is %d.\n", NUM_THREADS, OMP_ROUNDS);
	//Initialization for MPI
  	MPI_Init(&argc, &argv);
  	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
	//printf("Initializing proc rank %d\n", mpi_rank);	
	mpi_dissemination_barrier_init();	

	struct timeval mpi_tv;
	if(argc != 3) {
		printf("Usage ./combine_barrier numthreads numbarriers\n");
		return 1;
	}
	int numbarriers = atoi(argv[2]);
	//Initialization for OMP
	omp_set_num_threads(NUM_THREADS);
	omp_dissemination_barrier_init();
	double omp_total_time[NUM_THREADS];
	int j;
	for(j = 0; j < NUM_THREADS; j++)
		omp_total_time[j] = 0.0;
	int i, start_time, elapsed_time;
	double total_mpi_time = 0.0;
	for(i = 0; i < numbarriers; i++) {
		#pragma omp parallel
		{
			double total_omp_time = 0.0;
		 	struct timeval omp_tv;
			int id = omp_get_thread_num();
			int num_threads = omp_get_num_threads();
			int parity = 0;
			int sense = 1;
			int i, j, k;
			flags* localflags = &allnodes[id];

			#pragma omp critical
				for(i = 0; i < NUM_THREADS; i++)
					for(j = 0; j < OMP_ROUNDS; j++)
						for(k = 0; k < 2; k++)//Parity
							if(i == ((id + (int)pow((double)2, (double)j)) % NUM_THREADS))
								allnodes[id].partnerflags[k][j] = &allnodes[i].myflags[k][j];
			
			//printf("MPI rank %d OMP thread %d sleeping.\n", mpi_rank, id);
			//sleep(mpi_rank + id);
			//printf("MPI rank %d OMP thread %d awake.\n", mpi_rank, id);
			gettimeofday(&omp_tv, NULL);
			//float time1 = (float) omp_tv.tv_usec;
			double time1 = omp_get_wtime();
			omp_dissemination_barrier(localflags, &sense, &parity);
			double time2 = omp_get_wtime();
			gettimeofday(&omp_tv, NULL);
			//float time2 = (float) omp_tv.tv_usec;
			omp_total_time[id] = omp_total_time[id] + (time2 - time1);
			//printf("OMP_Barrier done for MPI rank %d OMP thread %d.\n", mpi_rank, id);
		}
		gettimeofday(&mpi_tv, NULL);
		//float time1_mpi = (float) mpi_tv.tv_usec;
		double time1_mpi = MPI_Wtime();
		mpi_dissemination_barrier();
		double time2_mpi = MPI_Wtime();
		gettimeofday(&mpi_tv, NULL);
		//float time2_mpi = (float) mpi_tv.tv_usec;
		total_mpi_time = total_mpi_time + (time2_mpi - time1_mpi);
		//printf("MPI Barrier completed for MPI rank %d\n", mpi_rank);
	}
	float omp_final_total_time;
	for(i = 0; i < NUM_THREADS; i++) {
		omp_final_total_time = omp_final_total_time + omp_total_time[i];
	}
	omp_final_total_time = omp_final_total_time/NUM_THREADS;
	printf("OMP Process = %d Time = %e\n", mpi_rank, omp_final_total_time);
	printf("MPI Process = %d Time = %e\n",mpi_rank, total_mpi_time);
	//if(elapsed_time % 10 == 0)
 	//printf("Finalizing rank %d\n", mpi_rank);
  	MPI_Finalize();
  	return 0;
}

