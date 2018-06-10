/* The hungry birds problem using pthreads

   usage under Linux:
     gcc hungrybirds.c -lpthread
     a.out birds worms

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
#include <semaphore.h>
#include <unistd.h>
#define HUNGRYBIRDS 10  /* maximum number of birdchildren size */
#define MAXWORMS 15   /* maximum number of worms for eating */


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

double start_time, end_time; /* start and end times */

sem_t emptyDish, fullDish, lockedDish;

void *HungryBird(void *);
void *ParentBird(void *);

int numBirds, wormsLeft;

int main(int argc, char const *argv[]) {
  long l; /* use long incase of 64-bit system */


  /* read command line args if any */
	wormsLeft = (argc > 1)? atoi(argv[1]) : MAXWORMS;
  numBirds = (argc > 2)? atoi(argv[2]) : HUNGRYBIRDS;

  if(!(wormsLeft && numBirds)){
    printf("Something went wrong with the indata");
  }

  pthread_attr_t attr;
  pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  pthread_t workerid[numBirds + 1];

  sem_init(&fullDish, 0, 0);
  sem_init(&emptyDish, 0, 0);
  sem_init(&lockedDish, 0, 1);

  /* do the parallel work: create the workers */
  for (l = 0; l < numBirds; l++) {
  	pthread_create(&workerid[l], &attr, HungryBird, (void *)l);
  }

  pthread_create(&workerid[numBirds], &attr, ParentBird, NULL);

  for (l = 0; l < numBirds; l++) {
  	pthread_join(workerid[l], NULL);
  }

  pthread_exit(NULL);

}

void *HungryBird(void *arg){
  long myid = (long) arg;

  while(true){
    sem_wait(&lockedDish);
    if(wormsLeft){
      wormsLeft--;
      printf("Child with id %ld is eating, there is %d worms left\n", myid, wormsLeft);
    } else {
      printf("Child with id %ld says cries because are %d worms left to eat \n", myid, wormsLeft);
      sem_post(&emptyDish);
      sem_wait(&fullDish);
    }
    sem_post(&lockedDish);
    printf("Child with id %ld is no longer hungry\n", myid);
    sleep(rand()%10);
  }

}

void *ParentBird(void *arg){
  //long myid = (long) arg;
  while(true){
    sem_wait(&emptyDish);
    printf("parentbird is on the way to get more food\n");
    sleep(2);
    srand(time(NULL));
    wormsLeft = rand()%MAXWORMS + 1;
    printf("parentbird got %d new worms\n", wormsLeft);
    sem_post(&fullDish);
  }
  pthread_exit(NULL);
}
