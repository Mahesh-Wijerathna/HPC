#include 
#include "mpi.h"


/*
 * Local function
 */

void Build_Matrix(int x, int y, MPI_Comm comm_in, MPI_Comm *comm_matrix, 
		  MPI_Comm *comm_row, MPI_Comm *comm_col);


/*
 * Global variables to make example simpler
 */

int rank, size, row, col;


int main(int argc, char **argv)
{
  int x, y;
  int sum_row, sum_col;
  MPI_Comm comm_matrix, comm_row, comm_col;
  
  /* Start up MPI */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
 
  /* Invoke for a hard-coded X and Y value. */

  x= 2;
  y= 2;

  /* Print some output on the "console" process */

  if (rank == 0)
    printf("For a %dx%d matrix (size %d):\n", x, y, size);

  /* Call the function to build the communicators */

  Build_Matrix(x, y, MPI_COMM_WORLD, &comm_matrix, &comm_row, &comm_col);
  
  /* Perform the reductions */
  
  MPI_Allreduce(&rank, &sum_row, 1, MPI_INT, MPI_SUM, comm_row);
  MPI_Allreduce(&rank, &sum_col, 1, MPI_INT, MPI_SUM, comm_col);
  printf("Process (%d, %d): Row sum = %d, Col sum = %d\n", row, col,
	 sum_row, sum_col);

  /* Quit */

  MPI_Finalize();
  return 0;
}


/* 
 * Build_Matrix(int, int, MPI_Comm, MPI_Comm *, MPI_Comm *, MPI_Comm *)
 * Build the matrix, return the specified communicators
 */
void Build_Matrix(int x, int y, MPI_Comm comm_in, MPI_Comm *comm_matrix, 
		  MPI_Comm *comm_row, MPI_Comm *comm_col)
{ 
  /* Real (i.e. non-example) programs should obviously have */
  /* have better recovery methods */

  if (x * y != size) {
    if (rank == 0)
      printf("Sorry, program compiled for %dx%d matrix.  Please run with %d processors.\n", x, y, x*y);
    MPI_Finalize();
    exit(-1);
  }

  /* Setup the args for MPI_Comm_split() */

  row= rank / y;
  col= rank % y;

  /* Make the communicators.  First duplicate comm_in (which */
  /* in this case is MPI_COMM_WORLD) into comm_matrix.  Then */
  /* split comm_in again to create communicators for the row */
  /* and column by assigning the color of the splitting to be */
  /* the row and column numbers that we just calculated.  */

  MPI_Comm_dup(comm_in, comm_matrix);
  MPI_Comm_split(comm_in, row, rank, comm_row);
  MPI_Comm_split(comm_in, col, rank, comm_col);
}

