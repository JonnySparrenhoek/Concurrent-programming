/*
The problem:
Assume that a class has n students numbered 1 to n and one teacher. The teacher wants to assign
the students to groups of two for a project. The teacher does so by having every student submit a
request for a partner. The teacher takes the first two requests, forms a group from those students,
and lets each student know his/her partner. The teacher then takes the next two requests, forms a
second group, and so on. If n is odd, the last student "partners" with himself/herself.

Note that the outcome in this problem is and should be deterministic.

The solution:
My solution for this is to make process 0 the teacher. I make Process 1 to n is the students.
Then I Randomize a number between 1 and n and and see if that partner is already taken. A partner
request is sent from the student to the teacher if there is an available partner. If the

To compile and run:
mpicc -g -Wall dist_pairing_client_server.c && mpiexec -n NumberOfProcesses ./a.out

*/


#include <stdio.h>
#include <string.h>
#include <mpi.h>


const int MAX_STRING = 100;
#define MAXSIZE 30
int projectPartners[MAXSIZE];

int main(void){
  char requestAnswer[MAX_STRING];
  int comm_sz;  /* number of processes */
  int my_rank; /* Process rank */
  int partner_rank;
  int count;

  for(int i = 0; i < comm_sz; i++){
    projectPartners[i] = 0;
  }

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if(my_rank != 0){

      int partner_request = my_rank;

        if(my_rank % 2)
        {
          partner_request += 1;
          MPI_Send(&partner_request, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
          MPI_Recv(requestAnswer, MAX_STRING, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          printf("%s\n", requestAnswer);
        } else {
          partner_request -= 1;
          MPI_Send(&partner_request, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
          MPI_Recv(requestAnswer, MAX_STRING, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          printf("%s\n", requestAnswer);
        }


  } else {
    for(int q = 1; q < comm_sz; q++){ // MPI_ANY_SOURCE makes it so that the result is showed in the order the processes finishes

      MPI_Status status;

      MPI_Recv(&partner_rank, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
      MPI_Get_count(&status, MPI_INT, &count);

      int currentSource = status.MPI_SOURCE;

      if(currentSource % 2 && currentSource == comm_sz - 1){
        sprintf(requestAnswer, "Hello student %d unfortunately you are the last student and have to be alone\n", currentSource);
        MPI_Send(requestAnswer, strlen(requestAnswer)+1, MPI_CHAR, currentSource, 0, MPI_COMM_WORLD);
      }

      if(projectPartners[currentSource] != 0){
        sprintf(requestAnswer, "Hello student %d, It seems you already have a group with student %d \n", currentSource, projectPartners[currentSource]);
        MPI_Send(requestAnswer, strlen(requestAnswer)+1, MPI_CHAR, currentSource, 0, MPI_COMM_WORLD);
      } else {

        if(projectPartners[partner_rank] != 0){
          sprintf(requestAnswer, "Hello student %d, unfortunately student %d already has a group member, please request a new one\n", currentSource, partner_rank);
          MPI_Send(requestAnswer, strlen(requestAnswer)+1, MPI_CHAR, currentSource, 0, MPI_COMM_WORLD);
        }else{
          projectPartners[currentSource] = partner_rank;
          projectPartners[partner_rank] = currentSource;
          sprintf(requestAnswer, "Hello student %d you are now in group with student %d\n", currentSource, partner_rank);
          MPI_Send(requestAnswer, strlen(requestAnswer)+1, MPI_CHAR, currentSource, 0, MPI_COMM_WORLD);
        }

      }
    }
  }


  MPI_Finalize();


  return 0;
}
