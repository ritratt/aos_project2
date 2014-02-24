#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

#define NUM_THREADS 	8
#define LEVELS 		3 /*It is log of NUM_THREADS*/
#define WINNER 		1
#define LOSER		2
#define CHAMP		0 

int rank, size, sense;
MPI_Status status;

struct round_t
{
	int role;
	int opponent;
	int flag;
	
}round_t;

struct round_t rounds[NUM_THREADS][LEVELS];

void tournament_barrier_init()
{
	printf("In init()\n");
	sense = 1;
	int i, j;
	for(i = 0; i < NUM_THREADS; i++)
	{
		for(j = 1; j <= LEVELS; j++)
		{
			/*Initializing the flag to 0*/
			rounds[i][j].flag = 0;

			/*Initializing the roles*/
			/*WINNER*/
			if( ((i % (int)pow((double)2, (double)j)) == 0 ) && 
			    ((i + (int)pow((double)2, (double)(j-1))) < NUM_THREADS ) &&
			    ((int)pow((double)2, (double)j) < NUM_THREADS)
			  )
			{
				rounds[i][j].role = WINNER;
			}
			/*LOSER*/
                        if((i % (int)pow((double)2, (double)j)) == (int)pow((double)2, (double)(j-1)) )
                        {
                                rounds[i][j].role = LOSER;
                        }
			/*CHAMP*/
			if( (i == 0) &&
			    ((int)pow((double)2, (double)j) >= NUM_THREADS)	
			  )
			{
				rounds[i][j].role = CHAMP;	
			}

			/*Initializing the opponents*/
			if(rounds[i][j].role == LOSER)
			{
				int oppo_rank = i - (int)pow((double)2, (double)(j-1));
				rounds[i][j].opponent = oppo_rank;
			}
			else if(rounds[i][j].role == WINNER || rounds[i][j].role == CHAMP)
			{
				int oppo_rank = i + (int)pow((double)2, (double)(j-1));
                                rounds[i][j].opponent = oppo_rank;
			}
			printf("Processor %d Round %d: Role: %d Opponent: %d Flag: %d\n", i, j, rounds[i][j].role, rounds[i][j].opponent, rounds[i][j].flag);
		}
	}
}

void tournament_barrier()
{
	int round;
	/*Arival Loop*/
	for(round = 1; round <= LEVELS; round++)
	{
		int my_role = rounds[rank][round].role;
		int my_opponent = rounds[rank][round].opponent;
		if(my_role == LOSER)
		{
			MPI_Send(&sense, 1, MPI_INT, my_opponent, 123, MPI_COMM_WORLD);
			int recd_sense;
			MPI_Recv(&recd_sense, 1, MPI_INT, my_opponent, 123, MPI_COMM_WORLD, &status);
			rounds[rank][round].flag = recd_sense;
			break;
		}
		else if(my_role == WINNER)
		{
			int recd_sense;
                        MPI_Recv(&recd_sense, 1, MPI_INT, my_opponent, 123, MPI_COMM_WORLD, &status);
			rounds[rank][round].flag = recd_sense;
		}
		else if(my_role == CHAMP)
		{
			int recd_sense;
                        MPI_Recv(&recd_sense, 1, MPI_INT, my_opponent, 123, MPI_COMM_WORLD, &status);
                        rounds[rank][round].flag = recd_sense;
			MPI_Send(&sense, 1, MPI_INT, my_opponent, 123, MPI_COMM_WORLD);
			break;
		}
	}
	/*Wake up Loop*/
	while(round > 0)
	{
		round--;
		int my_role = rounds[rank][round].role;
		int my_opponent = rounds[rank][round].opponent;
		if(my_role == WINNER)
		{
			MPI_Send(&sense, 1, MPI_INT, my_opponent, 123, MPI_COMM_WORLD);
		}
	}
	/*Toggle the sense*/
	sense = (sense + 1) % 2;
	int i,j;
	for(i = 0; i < NUM_THREADS; i++)
	{
		if(rank ==i)
		{
			printf("Rank = %d\n", rank);
			for(j = 1; j <= LEVELS; j++)
			{
				printf("Processor %d Round %d: Role: %d Opponent: %d Flag: %d\n", i, j, rounds[i][j].role, rounds[i][j].opponent, rounds[i][j].flag);
			}
		}
	}
}

int main(int argc, char **argv)
{
	tournament_barrier_init();

  	MPI_Init(&argc, &argv);
  	MPI_Comm_size(MPI_COMM_WORLD, &size);
  	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	printf("Initializing rank %d\n", rank);
	
        printf("Thread No. %d sleeping\n", rank);
        sleep(rank);
        printf("%d awake\n", rank);
	tournament_barrier();
  	printf("Finalizing\n");
  	MPI_Finalize();
	printf("Done\n");
  	return 0;
}

