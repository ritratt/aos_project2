#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


int *count, *sense;

void barrier(int *count, int *sense){
	
	int temp = (int) *sense;
	int c = *count;
	printf("Count before is %d\n", *count);
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

	printf("Count is now %d\n", *count);
	while(*count > 0 && temp == *sense)
		printf("spinning\n");
	
}

int main() {
	omp_set_num_threads(2);
	count = malloc(sizeof(int));
	sense = malloc(sizeof(int));
	
	*count = 2;
	*sense = 1;
	
	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		printf("Sleeping in %d\n", id);
		sleep((id + 1) * 3);
		barrier(count, sense);
		printf("%d done.\n", id);
	}
	printf("Done\n");
	return 0;
}

