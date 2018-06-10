/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
     ./matrixSum-openmp size numWorkers

     I combined exercise a with exercise b since I noticed my solution to a would also sufficient for exercise b

     Information from lscpu:
     CPU(s): 4
     On-line CPU(s) list: 0-3
     Thread(s) per core: 2
     Core(s) per socket: 2

     SIZE 2:
     1 threads  -  1.3411e-05  seconds
     2 threads  -  0.000162456 seconds, speedup: 0.0826
     4 threads  -  0.00140227  seconds, speedup: 0.0096
     8 threads  -  0.000458123 seconds, speedup: 0.0293

     SIZE 4:
     1 threads  -  1.11388e-05 seconds
     2 threads  -  0.000139234 seconds, speedup: 0.0800
     4 threads  -  0.00026099  seconds, speedup: 0.0427
     8 threads  -  0.00055285  seconds, speedup: 0.0201

     SIZE 16:
     1 threads  -  1.3812e-05  seconds,
     2 threads  -  9.385e-05   seconds, speedup: 0.1471
     4 threads  -  0.000233435 seconds, speedup: 0.0592
     8 threads  -  0.000423735 seconds, speedup: 0.0326

     SIZE 512:
     1 threads  -  0.000828764 seconds
     2 threads  -  0.000505857 seconds, speedup: 1.6383
     4 threads  -  0.0121703   seconds, speedup: 0.0680
     8 threads  -  0.000883598 seconds, speedup: 0.9379

     SIZE 1024:
     1 threads  -  0.00235903  seconds
     2 threads  -  0.00168097  seconds, speedup: 1.4033
     4 threads  -  0.00134747  seconds, speedup: 1.7507
     8 threads  -  0.00161451  seconds, speedup: 1.4611


*/

#include <omp.h>

double start_time, end_time;

#include <stdio.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size;
void *Worker(void *);
int matrix[MAXSIZE][MAXSIZE]; //Previously outside main

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, indexMaxRow, indexMaxCol, indexMinRow, indexMinCol;
  int total=0;
  int min = 101;
  int max = -1;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;


  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
      printf("[ ");
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      	  printf(" %d", matrix[i][j]);
	  }
	  	  printf(" ]\n");
  }

  start_time = omp_get_wtime();
#pragma omp parallel for reduction (+:total) private(j)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++){
      total += matrix[i][j];

      //if we find a new max
      if(matrix[i][j] > max){
        #pragma omp critical
        {
            if(matrix[i][j] > max){
              max = matrix[i][j];
              indexMaxRow = i;
              indexMaxCol = j;
            }
        }
      }

      //if we find a new min
      if(matrix[i][j] < min){
        #pragma omp critical
        {
          if(matrix[i][j] < min){
            min = matrix[i][j];
            indexMinRow = i;
            indexMinCol = j;
          }
        }
      }


    }
// implicit barrier

  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("the maximum value %d was found at row %d and column %d\n", max, indexMaxRow, indexMaxCol);
  printf("the minimum value %d was found at row %d and column %d\n", min, indexMinRow, indexMinCol);
  printf("it took %g seconds\n", end_time - start_time);

}
