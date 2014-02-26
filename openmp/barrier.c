#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_THREADS 8 

int count, sense, cross_count;
int numthreads, numbarriers;

void barrier_naive() 
{

	if(count == 1) {
		#pragma omp atomic
			count--;
		printf("So count is %d.\n", count);
		while(cross_count != NUM_THREADS - 1);
		#pragma omp atomic
			count += NUM_THREADS;
		printf("And now count is %d.\n", count);
	}
	else {
		#pragma omp atomic
			count--;
		while(count > 0); 
		#pragma omp atomic
			cross_count++;
		while(count != NUM_THREADS);
	}
}		

void barrier_locksense() {

        int temp = sense;
        if(count == 1) {
		#pragma omp critical
		{
			count--;
			count = NUM_THREADS;
		}
		//printf("Numthreads is %d.\n", numthreads);
                if(sense == 1) {
                        sense = 0;
		}
                else if(sense == 0) {
                        sense = 1;
		}
        }
        else {
		#pragma omp atomic
                	count--;
        }
	
        while(temp == sense);
	
}

int main(int argc, char **argv) 
{
	int i;
	struct timeval tv;
	float total_time = 0;
	if(argc != 3) {
		printf("Usage numthreads numbarriers\n");
		return 1;
	}
	numthreads = atoi(argv[1]);
	numbarriers = atoi(argv[2]);

	omp_set_num_threads(NUM_THREADS);
		
	count = NUM_THREADS;
	cross_count = 0;
	sense = 0;

	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		//printf("Sleeping in %d\n", id);
		//sleep(1);
		//printf("%d awake\n", id);
//		barrier_naive();	
		#pragma omp for
		for(i = 0; i < 1; i++) {
			//sleep(1);
			gettimeofday(&tv, NULL);
			float start_time = (float) tv.tv_usec;
			barrier_locksense();
			gettimeofday(&tv, NULL);
			float end_time = (float) tv.tv_usec;
			#pragma omp critical
			{
				total_time = total_time + (end_time - start_time);
			}
		}
	}
	printf("Time taken = %f\n", total_time);
	return 0;
}

