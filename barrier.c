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

void barrier_locksense(){

        int temp = sense;
        if(count == 1) {
                count--;
                if(sense == 1)
                        sense = 0;
                else if(sense == 0)
                        sense = 1;
        }
        else {
		#pragma omp atomic
                	count--;
        }

        while(temp == sense);

}

int main() 
{
	omp_set_num_threads(NUM_THREADS);
		
	count = NUM_THREADS;
	cross_count = 0;
	sense = 0;

	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		printf("Sleeping in %d\n", id);
		sleep((id + 1) * 3);
		printf("%d awake\n", id);
//		barrier_naive();	
		barrier_locksense();
		printf("%d done.\n", id);
	}
	printf("Done\n");
	return 0;
}

