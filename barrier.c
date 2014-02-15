#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_THREADS 2 

int *count, *sense;

void barrier_naive(int *count) {
	int c = *count;
	if(*count == 1) {
		c--;
		*count = c;
		printf("So count is %d.\n", *count);
		*count = NUM_THREADS;
		printf("And now count is %d.\n", *count);
	}
	else {
		c--;
		*count = c;
		while(*count > 0) {
			//printf("Count not zero.\n");
		}
		
		while(*count != NUM_THREADS);
		
	}

		//spinning
}		

void barrier_locksense(int *count, int *sense){
	
	int temp = (int) *sense;
	int c = *count;
	//printf("Count before is %d\n", *count);
	if(*count == 1) {
		c--;
		*count = c;
		if(*sense == 1)
			*sense = 0;
		else if(*sense == 0)
			*sense = 1;
	}
	else {
		c--;
		*count = c;
	}

	//printf("Count is now %d\n", *count);
	while(temp == *sense);
	//	printf("spinning\n");
	printf("Barrier done for %d\n", omp_get_thread_num());
	
}

int main() {
	omp_set_num_threads(NUM_THREADS);
	count = malloc(sizeof(int));
	sense = malloc(sizeof(int));
	
	*count = NUM_THREADS;
	*sense = 1;
	
	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		printf("Sleeping in %d\n", id);
		sleep((id + 1) * 3);
		printf("%d awake\n", id);
		//barrier_locksense(count, sense);
		barrier_naive(count);
		printf("%d done.\n", id);
	}
	printf("Done\n");
	return 0;
}

