#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#define NUM_THREADS 	6
#define ROUNDS		3

int rank, size, sense, parity, tag;
MPI_Status status;

void dissemination_barrier_init()
{
	printf("In init()\n");
	sense = 1;
	tag = 1;
}

void dissemination_barrier()
{
	int round;
	for(round = 0; round < ROUNDS; round++)
	{
		int mysaypartner = (rank + (int) pow((double)2, (double)round)) % NUM_THREADS;
		int mylistenpartner = (rank - (int) pow((double)2, (double)round) + NUM_THREADS) % NUM_THREADS;
		printf("rank = %d round = %d mysaypartner = %d \t mylistenpartner = %d\n", rank, round, mysaypartner, mylistenpartner);
		MPI_Send(&sense, 1, MPI_INT, mysaypartner, tag, MPI_COMM_WORLD);
		int recd_sense;
                MPI_Recv(&recd_sense, 1, MPI_INT, mylistenpartner, tag, MPI_COMM_WORLD, &status);
		printf("rank: %d round= %d listenpartner= %d recd_sense= %d\n", rank,  round, mylistenpartner, recd_sense);
	}
	/*Toggle the sense*/
	sense = (sense + 1) % 2;
}

int main(int argc, char **argv)
{
	dissemination_barrier_init();
	clock_t start = clock();

  	MPI_Init(&argc, &argv);
  	MPI_Comm_size(MPI_COMM_WORLD, &size);
  	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf("Initializing rank %d\n", rank);	
//	dissemination_barrier_init();	
	int i;
	for(i = 0 ; i < 10; i++)
	{
	        printf("Thread No. %d sleeping\n", rank);
	        sleep(rank);
	        printf("%d awake\n", rank);
//		dissemination_barrier();
		MPI_Barrier(MPI_COMM_WORLD);
		printf("Barrier %d completed for %d\n", i+1, rank);
	}
  	printf("Finalizing rank %d\n", rank);
  	MPI_Finalize();
	clock_t end = clock();
	float seconds = (float)(end - start);
	printf("\n");
  	return 0;
}

