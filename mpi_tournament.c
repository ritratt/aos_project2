#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>

#define WINNER          1
#define LOSER           2
#define CHAMP           0 
#define DROPOUT         3
#define BYE             4

int rank, size, sense;
MPI_Status status;
int NUM_THREADS, LEVELS;
struct round_t
{
        int role;
        int opponent;

}round_t;

void tournament_barrier(struct round_t rounds[NUM_THREADS][LEVELS])
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
                        break;
                }
                else if(my_role == WINNER)
  		{
                        int recd_sense;
                        MPI_Recv(&recd_sense, 1, MPI_INT, my_opponent, 123, MPI_COMM_WORLD, &status);
                }
                else if(my_role == CHAMP)
                {
                        int recd_sense;
                        MPI_Recv(&recd_sense, 1, MPI_INT, my_opponent, 123, MPI_COMM_WORLD, &status);
                        MPI_Send(&sense, 1, MPI_INT, my_opponent, 123, MPI_COMM_WORLD);
                        break;
                }
        }
//      printf("Value of round before wake up starts = %d \n ", round);
        /*Wake up Loop*/
        while(round > 0)
        {
                int my_role = rounds[rank][round].role;
                int my_opponent = rounds[rank][round].opponent;
                if(my_role == WINNER)
                {
                        MPI_Send(&sense, 1, MPI_INT, my_opponent, 123, MPI_COMM_WORLD);
                }
                else if(my_role == DROPOUT)
                        break;
                round--;
        }
        /*Toggle the sense*/
        sense = (sense + 1) % 2;
}

int main(int argc, char **argv)
{
        MPI_Init(&argc, &argv);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        printf("Initializing rank %d\n", rank);
        struct timeval bar_time;
        //struct timezone tz = NULL;
	//Init
        NUM_THREADS = size;
        LEVELS = ceil(log(NUM_THREADS)/log(2));
        struct round_t rounds[NUM_THREADS][LEVELS];
        sense = 1;
        int i, j;
        int num_bar = atoi(argv[1]);
        for(i = 1; i <= LEVELS; i++)
        {
                rounds[rank][i].role = -1;
                rounds[rank][i].opponent = -1;
        }
        for(i = 0; i < NUM_THREADS; i++)
        {
                for(j = 1; j <= LEVELS; j++)
                {
                        /*Initializing the roles*/
                        /*WINNER*/
                        if( (j > 0) &&
                            ((i % (int)pow((double)2, (double)j)) == 0 ) &&
                            ((i + (int)pow((double)2, (double)(j-1))) < NUM_THREADS ) &&
                            ((int)pow((double)2, (double)j) < NUM_THREADS)
                          )
                        {
                                rounds[i][j].role = WINNER;
                        }
                        /*BYE*/
                        else if((j > 0) &&
                                ((i % (int)pow((double)2, (double)j)) == 0 ) &&
                                ((i + (int)pow((double)2, (double)(j-1))) >= NUM_THREADS )
                               )
                        {
                                rounds[i][j].role = BYE;
                        }
                        /*LOSER*/
                        else if((j > 0) &&
                                (i % (int)pow((double)2, (double)j)) == (int)pow((double)2, (double)(j-1))
                               )
                        {
                                rounds[i][j].role = LOSER;
                        }
                        /*CHAMP*/
                        else if((j > 0) &&
                                (i == 0) &&
                                ((int)pow((double)2, (double)j) >= NUM_THREADS)
                               )
                        {
                                rounds[i][j].role = CHAMP;
                        }
                        /*DROPOUT*/
                        else if(j == 0)
                        {
                                rounds[i][j].role = DROPOUT;
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
                }
        }
        /*Initialization done*/

        float total_time = 0.0;
	for(i = 0; i < num_bar; i++)
        {
//              printf("Thread No. %d sleeping\n", rank);
                sleep(rank);
//              printf("%d awake\n", rank);
                int res1 = gettimeofday(&bar_time, NULL);
                float time1 = (float)bar_time.tv_usec;
                printf("time1 = %f\n", time1);
                tournament_barrier(rounds);
                int res2 = gettimeofday(&bar_time, NULL);
                float time2 = (float)bar_time.tv_usec;
                printf("time2 = %f\n", time2);
                total_time = total_time + (time2 - time1);
                printf("BArrier %d  completed for rank %d \n", i+1, rank);
        }
        printf("Total time = %f\n", total_time);
  //    printf("Finalizing\n");
        MPI_Finalize();
        printf("Done\n");
        return 0;
}

