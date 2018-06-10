/* calculate PI using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o calcPi calculatePi.c
     ./calcPi

     To set number of threads change the NUM_THREADS variable.

*/

#include <omp.h>
#include <stdio.h>
static long num_steps = 100000;
double step;
#define MAXWORKERS 8
double start_time, end_time;
int numWorkers;
//#define NUM_THREADS 2
int main(int argc, char *argv[]) {

  numWorkers = (argc > 1)? atoi(argv[1]) : MAXWORKERS;
  if (numWorkers > MAXWORKERS)
    numWorkers = MAXWORKERS;


  int i;
  double x, pi, sum = 0.0;
  step = 1.0/(double) num_steps;

  start_time = omp_get_wtime();
  #pragma omp parallel
  {
    #pragma omp for reduction(+:sum)
      for (i = 0; i<num_steps; i++){
      x = (i+0.5)*step;
        sum += 4.0/(1.0+(x*x));
      }

  }
  pi = step * sum;
  end_time = omp_get_wtime();
  printf("it took %g seconds\n", end_time - start_time);
  printf("PI: %lf \n", pi);
}
