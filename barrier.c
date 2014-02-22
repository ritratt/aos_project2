#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_THREADS 8 

int count, sense, cross_count;

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
                count--;
		count = NUM_THREADS;
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

int main() 
{
	int i;
	
	omp_set_num_threads(NUM_THREADS);
		
	count = NUM_THREADS;
	cross_count = 0;
	sense = 0;

	clock_t start = clock();
	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		printf("Sleeping in %d\n", id);
		sleep(id);
		printf("%d awake\n", id);
//		barrier_naive();	
		for(i = 0; i < 3; i++) {
		//	printf("%d\n", i);
			barrier_locksense();
		}
	}
	clock_t end = clock();
	float seconds = (float)(end - start);
	printf("Time taken = %f\n", seconds);
	return 0;
}

