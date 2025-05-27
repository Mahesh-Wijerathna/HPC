#include <stdio.h>
#include "mpi.h"


int main(argc, argv)
int argc;
char **argv;
{
  MPI_Status status;
  MPI_Request request[2];
  int i, num, msg1, msg2[8192];
  int rank, size, tag, next, prev, index;
  const int tag1 = 201, tag2 = 202;

  /* Start up MPI */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  next = (rank + 1) % size;
  prev = (rank + size - 1) %size;
 
  if (rank == 0) {
    printf("Enter the number of times around the ring: ");
    scanf("%d", &num);

    msg1 = num;
    for (i = 0; i < 8192; i++)
      msg2[i] = num;

    MPI_Send(&msg1, 1, MPI_INT, next, tag1, MPI_COMM_WORLD); 
    MPI_Send(msg2, 8192, MPI_INT, prev, tag2, MPI_COMM_WORLD); 
  } 

  MPI_Irecv(&msg1, 1, MPI_INT, prev, tag1, MPI_COMM_WORLD, &request[0]);
  MPI_Irecv(msg2, 8192, MPI_INT, next, tag2, MPI_COMM_WORLD, &request[1]);

  while (1) {
    MPI_Waitany(2, request, &index, &status);
    printf("%d: %d %d\n", rank, msg1, msg2[0]);
    
    if (rank == 0 && status.MPI_TAG == tag1) {
      msg1--;
      printf("Process 0 decremented msg1\n", msg1);
    } else if (rank == 0 && status.MPI_TAG == tag2) {
      for (i = 0; i < 8192; i++)
	msg2[i]--;
      printf("Process 0 decremented msg2 %d\n", msg2[0]);
    }
    
    if (status.MPI_TAG == tag1) {
      printf("MESSAGE ONE received:  %d --> %d\n", status.MPI_SOURCE, rank);
      MPI_Irecv(&msg1, 1, MPI_INT, prev, tag1, MPI_COMM_WORLD, &request[0]);
      if (msg1 >=0)
	MPI_Send(&msg1, 1, MPI_INT, next, tag1, MPI_COMM_WORLD);
    } else {
      printf("MESSAGE TWO received:  %d --> %d\n", status.MPI_SOURCE, rank);
      MPI_Irecv(msg2, 8192, MPI_INT, next, tag2, MPI_COMM_WORLD, &request[1]); 
      if (msg2[0] >=0)
	MPI_Send(msg2, 8192, MPI_INT, prev, tag2, MPI_COMM_WORLD); 
    }
    
    if (rank == 0) {
      if (msg1 == -1 && msg2[0] == -1) {
	printf("Process %d exiting\n", rank);
	break;
      }
    } else {
      if (msg1 == 0 && msg2[0] == 0) {
	printf("Process %d exiting\n", rank);
	break;
      }
    }
  }

  /* Quit */

  MPI_Finalize();

  return 0;
}