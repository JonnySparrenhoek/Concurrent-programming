/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o quicksort-openmp quicksort-openmp.c
     ./quicksort-openmp size numWorkers

     SIZE 10:
     1 threads - 2.6514e-05 seconds
     2 threads - 0.00017456 seconds, speedup: 0.15
     4 threads - 0.00850949 seconds  speedup: 0.003
     8 threads - 0.00056280 seconds  speedup: 0.047

     SIZE 100:
     1 threads - 9.5202e-05 seconds
     2 threads - 0.00029545 seconds, speedup: 0.322
     4 threads - 0.00046753 seconds, speedup: 0.204
     8 threads - 0.00078156 seconds, speedup: 0.121

     SIZE 1000:
     1 threads - 0.00046915 seconds
     2 threads - 0.00312512 seconds, speedup: 0.150
     4 threads - 0.01352970 seconds, speedup: 0.034
     8 threads - 0.00453513 seconds, speedup: 0.010

     SIZE 10000:
     1 threads - 0.00395584 seconds
     2 threads - 0.01476410 seconds, speedup: 0.031
     4 threads - 0.03715800 seconds, speedup: 0.012
     8 threads - 0.03466750 seconds, speedup: 0.014




*/

#include <omp.h>
#include <stdbool.h>

double start_time, end_time;

#include <stdio.h>
#define MAXSIZE 10000  /* maximum array size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size;
int arrayToBeSorted[MAXSIZE];
void quicksort(int, int);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, total=0;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;


  omp_set_num_threads(numWorkers);

  /* initialize the array */
  for (i = 0; i < size; i++) {
      printf("[ ");
      arrayToBeSorted[i] = rand()%99;
      printf(" %d", arrayToBeSorted[i]);
	    printf(" ]\n");
  }

  start_time = omp_get_wtime();

  #pragma omp parallel
  {
    #pragma omp single nowait
    {
      quicksort(0, size-1);
    }
  }

  end_time = omp_get_wtime();

int sorted = 1;
printf("After sort: \n\n\n");
for (i = 0; i < size; i++) {
    printf("[ ");
    printf(" %d", arrayToBeSorted[i]);
    printf(" ]\n");
    if(arrayToBeSorted[i]<arrayToBeSorted[i-1]){
      printf("not sorted on this row");
      sorted = 0;
    }
}



  if(sorted)
    printf("array is sorted\n");
  printf("it took %g seconds\n", end_time - start_time);

}

void quicksort(int first, int last){
  int pivot, pivotIndex, temp, left, right;
  if (first >= last) return; // if done
  // otherwise middle as pivot
  pivotIndex = first + (last - first) / 2; // middle value to avoid overflow
  pivot = arrayToBeSorted[pivotIndex];
  left = first;
  right = last;
  while (left <= right) {
    if (arrayToBeSorted[left] > pivot) { // swap left element with right element
       temp = arrayToBeSorted[left];
       arrayToBeSorted[left] = arrayToBeSorted[right];
       arrayToBeSorted[right] = temp;
       if (right == pivotIndex) {
        pivotIndex = left;
       }
       right--;
    }
    else {
      left++;
    }
  }
  // place the pivot in its place (i.e. swap with right element)
  temp = arrayToBeSorted[right];
  arrayToBeSorted[right] = pivot;
  arrayToBeSorted[pivotIndex] = temp;


  #pragma omp task
    quicksort(first, (right - 1));
    quicksort((right + 1), last);
}
