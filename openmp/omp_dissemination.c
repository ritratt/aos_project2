#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#define NUM_THREADS 6
#define ROUNDS 3

typedef struct flags
{
	int myflags[2][ROUNDS];
	int* partnerflags[2][ROUNDS];
}flags;

flags allnodes[NUM_THREADS];

void dissemination_barrier_init()
{
	int i, j, k;
	for(i = 0; i < NUM_THREADS; i++)
        {
        	for(j = 0; j < 2; j++)
                {
                	for(k = 0; k < ROUNDS; k++)
                        {
                        	allnodes[i].myflags[j][k] = 0;
                        }
                }
        }
}

void dissemination_barrier(flags* localflags, int *sense, int * parity)
{
	int round;
	int p = *parity;
	for(round = 0; round < ROUNDS; round++)
	{
		#pragma omp critical
		{
			*localflags->partnerflags[p][round] = *sense;
//			printf("partner flag:%d parity = %d round = %d\n", *localflags->partnerflags[p][round], p, round);
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

int main(int argc, char *args[])
{
	if(argc != 3) {
                printf("Usage ./diss_omp numthreads numbarriers\n");
                return 1;
        }
	struct timeval tv;
	//unsigned long total_time, start_time, end_time;
        omp_set_num_threads(NUM_THREADS);
        int numbarriers = atoi(args[2]);
        printf("Running with %d barriers.\n", numbarriers);

	int size = omp_get_num_threads();
	//printf("Got %d threads\n", size);
	dissemination_barrier_init();
	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		int num_threads = omp_get_num_threads();
		int parity = 0;
		int sense = 1;
		int i, j, k;
		flags* localflags = &allnodes[id];
		double total_time = 0;
		int times;
		for(times = 0; times < numbarriers; times++)
		{	
			#pragma omp critical
			{
				for(i = 0; i < NUM_THREADS; i++)
				{
					for(j = 0; j < ROUNDS; j++)
					{
						for(k = 0; k < 2; k++)//Parity
						{
							if(i == ((id + (int)pow((double)2, (double)j)) % NUM_THREADS))
							{
								allnodes[id].partnerflags[k][j] = &allnodes[i].myflags[k][j];
								//printf("round = %d me = %d partner = %d \n", j, id, i);
							}
						}
					}
				}	
			}
			//printf("%d sleeping.\n", id);
			//sleep(id);
			//printf("%d awake.\n", id);
			int res1 = gettimeofday(&tv, NULL);
			//long start_time = tv.tv_usec;
			double start_time = (double) omp_get_wtime();
			dissemination_barrier(localflags, &sense, &parity);
			double end_time = (double) omp_get_wtime();
			int res2 = gettimeofday(&tv, NULL);
			//long end_time = tv.tv_usec;
			total_time = total_time + (end_time - start_time);
			//printf("Barrier %d done for thread %d.\n", times + 1, id);
		}
		printf("Total time for id = %d is %e \n", id, total_time);
	}	
//	printf("Elapsed time = %u.\n", total_time/NUM_THREADS);
}
