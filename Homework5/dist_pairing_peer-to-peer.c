/*
The problem:
As in the previous problem, we want to assign n students numbered 1 to n to groups of two for a project.
However, the teacher does not want to be involved in forming the groups.
Instead, he asks the students to interact with each other to pair up into groups.
In particular, the students have to come up with a single algorithm that every student executes.
Develop a distributed pairing algorithm for the students. An ideal solution will use only n messages.
The algorithm should use random numbers so that the outcome is non-deterministic.
The teacher starts the algorithm by picking one student at random and sending a message to that student
saying in effect "your turn to pick a partner".

The solution:
My solution for this is to pass an array of integers through MPI Send and Recv. With the help of the array
students can find out who is already taken and ask an not yet taken student to team up. Each student will
talk once so there are max n messages. I call process with rank 0 the teacher and 1 to n is students.

To compile and run :
mpicc -g -Wall dist_pairing_peer-to-peer.c && mpiexec -n NumberOfProcesses ./a.out

*/


#include <stdio.h>
#include <string.h>
#include <mpi.h>


const int MAX_STRING = 100;
#define MAXSIZE 30
int numberOfGroups;
int projectPartners[MAXSIZE];

int main(void){
  int comm_sz;  /* number of processes */
  int my_rank; /* Process rank */
  int count;


  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


  //projectPartners[my_rank] = 0;

  if(my_rank == 0){
          for(int i = 0; i < comm_sz; i++){
            projectPartners[i] = 0;
          }
          int first_student = rand()%comm_sz;
          while(first_student == 0){ //To not get the teacher again
            first_student = rand()%comm_sz;
          }
          printf("Hello student %d you may start off to choose a partner\n", first_student);
          MPI_Send(&projectPartners, comm_sz, MPI_INT, first_student, 0, MPI_COMM_WORLD);
  } else { // This is the teacher that starts of the chain


        MPI_Status status;
        //If form_group is a student id the sender wants to form a group,
        // if 0 it just lets the new receiver know it's their turn to pick a new groupmember
        MPI_Recv(&projectPartners, comm_sz, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        if(projectPartners[0] == (comm_sz - 2) && projectPartners[my_rank] == 0){
          printf("I'm %d, there are no students left to form a group with.. I guess I am by myself :(\n", my_rank);
          MPI_Finalize();
          return 0;
        }



        MPI_Get_count(&status, MPI_INT, &count);

        int sender = status.MPI_SOURCE; // The sender of the message
        srand(time(NULL));
        int next_student = rand()%comm_sz; // next student to message

        if(projectPartners[my_rank] != 0)
          printf("I'm student %d and I got chosen to be in a group with %d\n", my_rank, sender);

        projectPartners[0]++; // stores the number of processes

        if(projectPartners[0] == (comm_sz - 1)){
          MPI_Finalize();
          return 0;
        }

        while (projectPartners[next_student] != 0 || next_student == my_rank || next_student == 0){ // if next student is not available
          next_student = rand()%comm_sz;
        }


        if(projectPartners[my_rank] == 0){
          printf("I'm %d and I choose to be in a group with student %d\n", my_rank, next_student);
          projectPartners[next_student] = my_rank;
          projectPartners[my_rank] = next_student;
          MPI_Send(&projectPartners, comm_sz, MPI_INT, next_student, 0, MPI_COMM_WORLD);
        } else {
          printf("Next student to find a partner is %d\n", next_student);
          MPI_Send(&projectPartners, comm_sz, MPI_INT, next_student, 0, MPI_COMM_WORLD);
        }

  }


  MPI_Finalize();


  return 0;
}
