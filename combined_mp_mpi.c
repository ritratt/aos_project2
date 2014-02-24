#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <omp.h>

#define NUM_THREADS 6
#define OMP_ROUNDS 3

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
	//Initialization for MPI
  	MPI_Init(&argc, &argv);
  	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
	printf("Initializing proc rank %d\n", mpi_rank);	
	mpi_dissemination_barrier_init();	

	if(argc != 2) {
		printf("Usage ./combine_barrier numbarriers\n");
		return 1;
	}
	int numbarriers = atoi(argv[1]);
	//Initialization for OMP
	omp_set_num_threads(NUM_THREADS);
	omp_dissemination_barrier_init();

	int i, start_time, elapsed_time;
	start_time = MPI_Wtime();
	for(i = 0; i < numbarriers; i++) {
		#pragma omp parallel
		{
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
			
			printf("MPI rank %d OMP thread %d sleeping.\n", mpi_rank, id);
			sleep(mpi_rank + id);
			printf("MPI rank %d OMP thread %d awake.\n", mpi_rank, id);
			omp_dissemination_barrier(localflags, &sense, &parity);
			printf("OMP_Barrier done for MPI rank %d OMP thread %d.\n", mpi_rank, id);
		}
		
		mpi_dissemination_barrier();
		printf("MPI Barrier completed for MPI rank %d\n", mpi_rank);
	}
	elapsed_time = MPI_Wtime() - start_time;
	printf("Time take by %d = %d\n",mpi_rank, elapsed_time);
	//if(elapsed_time % 10 == 0)
 	printf("Finalizing rank %d\n", mpi_rank);
  	MPI_Finalize();
  	return 0;
}

