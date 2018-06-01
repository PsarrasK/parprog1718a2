// ... ο κώδικάς σας για την υλοποίηση του quicksort
// με pthreads και thread pool...

// compile with: gcc -O2 -Wall -pthread quicksort.c -o quicksort

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
//#include <stdint.h>
//#include <stddef.h>
//#include <stdbool.h>

#define N 1000000
#define CUTOFF 10
#define LIMIT 50000
#define THREADS 4
/*
// condition variable, signals a put operation (receiver waits on this)
pthread_cond_t msg_in = PTHREAD_COND_INITIALIZER;
// condition variable, signals a get operation (sender waits on this)
pthread_cond_t msg_out = PTHREAD_COND_INITIALIZER;
// mutex protecting common resources
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
*/
void inssort(double *a,int n) {
	int i,j;
	double t;

	for (i=1;i<n;i++) {
		j = i;
		while ((j>0) && (a[j-1]>a[j])) {
			t = a[j-1];  a[j-1] = a[j];  a[j] = t;
			j--;
		}
	}
}

int partition(double *a, int n) {
	int first, middle, last;
	double p,t;
	int i,j;

	first = 0;
	middle = n-1;
	last = n/2;

	// put median-of-3 in the middle
	if (a[middle]<a[first]) { t = a[middle]; a[middle] = a[first]; a[first] = t; }
	if (a[last]<a[middle]) { t = a[last]; a[last] = a[middle]; a[middle] = t; }
	if (a[middle]<a[first]) { t = a[middle]; a[middle] = a[first]; a[first] = t; }

	// partition (first and last are already in correct half)
	p = a[middle]; // pivot
	for (i=1,j=n-2;;i++,j--) {
		while (a[i]<p) i++;
		while (p<a[j]) j--;
		if (i>=j) break;

		t = a[i]; a[i] = a[j]; a[j] = t;
	}
	return i;
}

void quicksort(double *a,int n) {
	int i;

	// check if below cutoff limit
	if (n<=CUTOFF) {
		inssort(a,n);
		return;
	}

	i = partition(a, n);

	// recursively sort halves
	quicksort(a,i);
	quicksort(a+i,n-i);
}

struct thread_params {
	double *a;
	int n;
};

void *work(void *tp) {
	double *a;
	int n;
	int i;
	struct thread_params *tparm;
	tparm = (struct thread_params *)tp;
	a = tparm->a;
	n = tparm->n;

	if(n < LIMIT) {
		quicksort(a, n);
	}
	else {
		i = partition(a, n);
		pthread_t child_0, child_1;
		struct thread_params ch_0, ch_1;
		ch_0.a = a;
		ch_0.n = i;
		pthread_create(&child_0, NULL, work, &ch_0);

		ch_1.a = a + i;
		ch_1.n = n - i;
		pthread_create(&child_1, NULL, work, &ch_1);

		pthread_join(child_0, NULL);
		pthread_join(child_1, NULL);
	}

	pthread_exit(NULL);
}

int main() {
	double *a;
	int i;
	struct thread_params tp;
	pthread_t tid;

	a = (double *)malloc(N*sizeof(double));
	if (a==NULL) {
		printf("error in malloc\n");
		exit(1);
	}

	// fill array with random numbers
	srand(time(NULL));
	for (i=0;i<N;i++) {
		a[i] = (double)rand()/RAND_MAX;
	}

	tp.a = a;
	tp.n = N;
	// thread creation and quicksort function call with array and array length as arguments
	if(pthread_create(&tid, NULL, work, &tp) != 0) {
		printf("Error in thread creation\n");
		exit(1);
	}

	pthread_join(tid, NULL);

	// check sorting
	for (i=0;i<(N-1);i++) {
		//printf("%lf\n", a[i]);
		if (a[i]>a[i+1]) {
			printf("Sort failed!\n");
			break;
		}
	}
	printf("Sorting was successful!\n");

	free(a);
	return 0;
}
