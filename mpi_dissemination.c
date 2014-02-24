#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

int rank, size, sense, parity, tag;
MPI_Status status;
int NUM_THREADS, ROUNDS;

void dissemination_barrier_init()
{
	sense = 1;
	tag = 1;
	NUM_THREADS = size;
	ROUNDS = ceil(log(NUM_THREADS)/log(2));
}

void dissemination_barrier()
{
	int round;
	for(round = 0; round < ROUNDS; round++)
	{
		int mysaypartner = (rank + (int) pow((double)2, (double)round)) % NUM_THREADS;
		int mylistenpartner = (rank - (int) pow((double)2, (double)round) + NUM_THREADS) % NUM_THREADS;
		MPI_Send(&sense, 1, MPI_INT, mysaypartner, tag, MPI_COMM_WORLD);
		int recd_sense;
                MPI_Recv(&recd_sense, 1, MPI_INT, mylistenpartner, tag, MPI_COMM_WORLD, &status);
	}
	/*Toggle the sense*/
	sense = (sense + 1) % 2;
}

int main(int argc, char **argv)
{
	clock_t start = clock();
	int start_time, elapsed_time;
	if(argc != 2) {
		printf("Usage ./mpi_diss numbarriers\n");
		return 1;
	}
	int numbarriers = atoi(argv[1]); 
  	MPI_Init(&argc, &argv);
  	MPI_Comm_size(MPI_COMM_WORLD, &size);
  	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf("Initializing rank %d\n", rank);	
	dissemination_barrier_init();	
	int i;
	start_time = MPI_Wtime();
	for(i = 0 ; i < numbarriers; i++)
	{
	        printf("Thread No. %d sleeping\n", rank);
	        sleep(rank);
	        printf("%d awake\n", rank);
/*		if(rank == 0)
		{
			while(1)
			{

			}
		}
*/		dissemination_barrier();
		printf("Barrier %d completed for %d\n", i+1, rank);
	}
	elapsed_time = MPI_Wtime() - start_time;
	printf("Time take by %d = %d.\n", rank, elapsed_time);
 	printf("Finalizing rank %d\n", rank);
  	MPI_Finalize();
	clock_t end = clock();
	float seconds = (float)(end - start);
	printf("\n");
  	return 0;
}

