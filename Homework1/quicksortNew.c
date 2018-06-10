/*
  Usage under linux:
  gcc quicksort.c -lpthread -DDEBUG
  a.out size NumberOfThreads
*/

#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sched.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum array size */
#define MAXWORKERS 10   /* maximum number of workers */

int size = MAXSIZE;
int arrayToBeSorted[MAXSIZE];
double start_time, end_time;
int stripSize;

int numWorkers;           /* number of workers */
int numArrived = 0;       /* number who have arrived */

void *Quicksort(void *);
void QuicksortP(int, int);

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int main(int argc, char *argv[]) {
  int i, j, threads;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
//  pthread_mutex_init(&mutex, NULL);
//  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;

  srand(time(NULL));


    /* initialize the array */
    for (i = 0; i < size; i++) {
            arrayToBeSorted[i] = rand()%99;
    }

    /* print the matrix */
  #ifdef DEBUG
    for (i = 0; i < size; i++) {
  	  printf("[ ");
  	  printf(" %d", arrayToBeSorted[i]);
  	  printf(" ]\n");
    }
  #endif

    /* do the parallel work: create the workers */
    start_time = read_timer();
    for (l = 0; l < numWorkers; l++)
      pthread_create(&workerid[l], &attr, Quicksort, (void *) l);

    //Barrier();
    QuicksortSeq(0, size-1);

  for(threads = 0; threads < numWorkers; threads++){
      pthread_join(workerid[threads], NULL);
      #ifdef DEBUG // Prints which threads has joined incase the DEBUG commando is issued
          printf("Thread: %d , joined \n", threads);
  //        printf("l: %d", l)
      #endif
  }

  end_time = read_timer();

  for (i = 0; i < size - 1; i++){
    if (arrayToBeSorted[i] > arrayToBeSorted[i + 1]) {
      printf("The array is not sorted!\n");
    }
  }

#ifdef DEBUG
  printf ("The array: \n");
  for (i = 0; i < size; i++)
    printf (" %3d", arrayToBeSorted[i]);
  printf ("\n");
#endif

  printf("It took %g seconds\n", end_time - start_time);
}

void *Quicksort(void *arg) {

}

void QuicksortSeq(int first, int last) {
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


  QuicksortSeq(first, (right - 1));
  QuicksortSeq((right + 1), last);

}
