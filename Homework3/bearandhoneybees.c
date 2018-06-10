/* The hungry birds problem using pthreads

   usage under Linux:
     gcc hungrybirds.c -lpthread
     a.out CapacityOfHoneyPot NumberOfBees

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
#define HONEYBEES 10  /* maximum number of Honeybees size */
#define MAXCAPACITY 5   /* maximum capacity of the honeypot */


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

sem_t emptyPot, fullPot, lockedPot;

void *HungryBear(void *);
void *HoneyBees(void *);

int numBees, maxHoney, currentHoney;

int main(int argc, char const *argv[]) {
  long l; /* use long incase of 64-bit system */


  /* read command line args if any */
	maxHoney = (argc > 1)? atoi(argv[1]) : MAXCAPACITY;
  numBees = (argc > 2)? atoi(argv[2]) : HONEYBEES;

  if(!(maxHoney && numBees)){
    printf("Something went wrong with the indata");
  }

  pthread_attr_t attr;
  pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  pthread_t workerid[numBees + 1];

  sem_init(&fullPot, 0, 0);
  sem_init(&emptyPot, 0, 0);
  sem_init(&lockedPot, 0, 1);

  /* do the parallel work: create the workers */
  for (l = 0; l < numBees; l++) {
  	pthread_create(&workerid[l], &attr, HoneyBees, (void *)l);
  }

  pthread_create(&workerid[numBees], &attr, HungryBear, NULL);

  for (l = 0; l < numBees; l++) {
  	pthread_join(workerid[l], NULL);
  }

  pthread_exit(NULL);

}

void *HoneyBees(void *arg){
  long myid = (long) arg;

  while(true){
    sleep(rand()%10);
    sem_wait(&lockedPot);
    if(currentHoney != maxHoney){
      currentHoney++;
      printf("Bee with id %ld is putting in honey in the pot, there is %d units of honey in the pot\n", myid, currentHoney);
    } else {
      printf("Bee with id %ld says the honeypot is full with %d units out of %d in it, the bear is awoken \n", myid, currentHoney, maxHoney);
      sem_post(&fullPot);
      sem_wait(&emptyPot);
    }
    sem_post(&lockedPot);
  }

}

void *HungryBear(void *arg){
  //long myid = (long) arg;
  while(true){
    sem_wait(&fullPot);
    printf("The bear eats all of the honeypot\n");
    currentHoney = 0;
    sleep(2);
    srand(time(NULL));
    sem_post(&emptyPot);
  }
  pthread_exit(NULL);
}
