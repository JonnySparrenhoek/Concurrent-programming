/* a sequential program for n-body Problem.

   usage under Linux:
     gcc sequentialSlow.c -lm -O
     ./a.out numberofbodies numberofsteps

*/

#include <string.h> /* for memset */
#include <time.h> /* for measuring time */
#include <math.h> /* for mathematical operations */
#include <stdlib.h>
#include <stdio.h> /* for input output */
#include <sys/time.h>
#include <stdbool.h>

#define MAXSIZE 300  /* maximum numbers of bodies */
#define MAXSTEPS 10   /* maximum number of timesteps in the simulation */
#define MAXAREAS 100

/*struct body {
    double m; // mass
    double x, y; // position
    double vx, vy; // velocity
};

struct area {
  sxtruct body *bodies; // Each body
  int num_bodies; // Total number of bodies in the area
}*/

#define DIM 2 // number of dimensions
#define GRAV 6.67 // The gravitational constant
#define X 0
#define Y 1

typedef double vect_t[DIM];

int gnumBodies;
int numSteps;

long double mass[MAXSIZE];
long double position[MAXSIZE][DIM];
long double velocity[MAXSIZE][DIM];
long double force[MAXSIZE][DIM];
int collideX[MAXAREAS];
int collideY[MAXAREAS];


double start_time, end_time; /* start and end times */


/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if(!initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}


/* This init features a good test, no velocity, same mass means
   the body in the middle should not move due to symmetry
*/

void init(int num_bodies){
  srand(time(NULL)); // randomize seed
  for(int i = 0; i < num_bodies; i++){
    mass[i] = 100000000 + 1;
    printf("mass[%d] == %Le \n", i, mass[i]);
    position[i][X] = i+1;//drand48()*100;
    printf("pos[%d][X] == %Le \n", i, position[i][X]);
    position[i][Y] = i+1;//drand48()*100;
    printf("pos[%d][Y] == %Le \n", i, position[i][Y]);
    velocity[i][X] = 0;//(rand()%2 - 0.5)*drand48();//drand48();
    printf("VVELOX [%d]: %Le\n",i, velocity[i][X]);
    velocity[i][Y] = 0;//(rand()%2 - 0.5)*drand48();//rand()%2 - 0.5 to get negative or positive
    printf("VVELOY [%d]: %Le\n",i, velocity[i][Y]);
    force[i][X] = 0;
    force[i][Y] = 0;
    collideX[i] = 0;
    collideY[i] = 0;
  }
}
/*void init(int num_bodies){
  srand(time(NULL)); // randomize seed
  for(int i = 0; i < num_bodies; i++){
    mass[i] = drand48()*100000000 + 1; // + 1 so that no mass is 0
//    printf("mass[%d] == %Le \n", i, mass[i]);
    position[i][X] = drand48()*100;
//    printf("pos[%d][X] == %Le \n", i, position[i][X]);
    position[i][Y] = drand48()*100;
//    printf("pos[%d][Y] == %Le \n", i, position[i][Y]);
    velocity[i][X] = (rand()%2 - 0.5)*drand48();//drand48();
//    printf("VVELOX [%d]: %Le\n",i, velocity[i][X]);
    velocity[i][Y] = (rand()%2 - 0.5)*drand48();//rand()%2 - 0.5 to get negative or positive
//    printf("VVELOY [%d]: %Le\n",i, velocity[i][Y]);
    force[i][X] = 0;
    force[i][Y] = 0;
  }
}*/

void calculate(int num_bodies){
  double grav = 6.67;
  double powt = pow(10,11);
  long double distance, distX, distY, magnitude;
  long double powgrav = grav/powt;
  //printf("%Le gravT \n", powgrav);
  for(int i = 0; i < num_bodies - 1; i++){
    for(int j = i+1; j < num_bodies; j++){
  //    printf("my i j: %d %d \n", i, j);
      distX = fabs(position[i][X] - position[j][X]);
      distY = fabs(position[i][Y] - position[j][Y]);
      distance = sqrt(pow(distX,2)+pow(distY,2));
    //  printf("distance %Le \n", distance);
      magnitude = (powgrav*mass[i]*mass[j])/(pow(distance,2));
  //    printf("magnitude %Le \n", magnitude);
      force[i][X] += (magnitude*(distX))/distance; // -distX?
  //    printf("force[%d][X] == %Le\n", i, force[i][X]);
      force[j][X] -= (magnitude*(distX))/distance; // distX?
  //    printf("force[%d][X] == %Le\n", j, force[j][X]);
      force[i][Y] += (magnitude*(distY))/distance; // -distY?
  //    printf("force[%d][Y] == %Le\n", i, force[i][Y]);
      force[j][Y] -= (magnitude*(distY))/distance; // distY?
  //    printf("force[%d][Y] == %Le\n", j, force[j][Y]);
    }
  }
}

void movebodies(int num_bodies, int dTime){ // dTime is deltatime
  long double deltavx, deltavy, deltapx, deltapy;
  for(int i = 0; i < num_bodies; i++){
    deltavx = (force[i][X]/mass[i])*dTime; //change of velocity in x direction
    deltavy = (force[i][Y]/mass[i])*dTime; //change of velocity in y direction
    printf("deltavx %Le, deltavy %Le for %d\n", deltavx, deltavy,i);
    deltapx = (velocity[i][X] + (deltavx/2))*dTime; //change of position in x direction
    deltapy = (velocity[i][Y] + (deltavy/2))*dTime; //change of position in y direction
  //  printf("deltavx : %Le, deltavy %Le, deltapx %Le, deltapy%Le\n", deltavx, deltavy, deltapx, deltapy);
    velocity[i][X] += deltavx; //changing velocity in x direction
    velocity[i][Y] += deltavy; //changing velocity in Y direction
    position[i][X] += deltapx; //changing position in x direction
    int posX = (int) position[i][X];
  //  printf("posX: %d\n", posX);
    if(collideX[posX] != 0 && collideX[posX] != i){
      if((velocity[i][X] > 0 && velocity[collideX[posX]][X] < 0) || (velocity[i][X] < 0 && velocity[collideX[posX]][X] > 0)){
        printf("already different directions from %d and %d\n", i, collideX[posX]);
      } else {
        printf("BOUNCE i X av %d pga värde %d som delas med %d\n", i, posX, collideX[posX]);
        velocity[i][X] = -velocity[i][X];
        velocity[i][Y] = -velocity[i][Y];
        velocity[collideX[posX]][X] = -velocity[collideX[posX]][X];
        velocity[collideX[posX]][Y] = -velocity[collideX[posX]][Y];
        collideX[posX] = 0;
      }
    } else {
      collideX[posX] = i;
    }
    position[i][Y] += deltapy; //changing position in y direction
    int posY = (int) position[i][Y];
  //  printf("posX: %d\n", posX);
    if(collideY[posY] != 0 && collideY[posY] != i){
      if((velocity[i][Y] > 0 && velocity[collideY[posY]][Y] < 0) || (velocity[i][Y] < 0 && velocity[collideY[posY]][Y] > 0)){
        printf("already different directions from %d and %d\n", i, collideY[posY]);
      } else {
        printf("BOUNCE i Y av %d pga värde %d som delas med %d\n", i, posY, collideY[posY]);
        velocity[i][X] = -velocity[i][X];
        velocity[i][Y] = -velocity[i][Y];
        velocity[collideY[posY]][X] = -velocity[collideY[posY]][X];
        velocity[collideY[posY]][Y] = -velocity[collideY[posY]][Y];
        collideY[posY] = 0;
      }
    } else {
      collideY[posY] = i;
     }
    force[i][X] = 0; //Reset if calculation is to be done again
    force[i][Y] = 0; //Reset if calculation is to be done again
  }
}

int main(int argc, char *argv[]) {



  /* read command line args if any */
  gnumBodies = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numSteps = (argc > 2)? atoi(argv[2]) : MAXSTEPS;
  if (gnumBodies > MAXSIZE) gnumBodies = MAXSIZE;
  if (numSteps > MAXSTEPS) numSteps = MAXSTEPS;

  init(gnumBodies);
  for(int i = 0; i < gnumBodies; i++)
    printf("particle %d is positioned at x = %Le and y =%Le\n", i, position[i][X], position[i][Y]);
  for(int i = 1; i <= numSteps; i++){
    start_time = read_timer();
    calculate(gnumBodies);
    movebodies(gnumBodies,i);
    end_time = read_timer();
    printf("a calculation has been done in %lf seconds\n", (end_time - start_time));
  }
  for(int i = 0; i < gnumBodies; i++)
    printf("particle %d is positioned at x = %Le and y =%Le\n", i, position[i][X], position[i][Y]);

  return 0;
}
