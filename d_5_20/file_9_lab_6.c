/* MPI Lab 6, Example Program   */


/* Lab6:

   Create a datatype called submatrix that consists of elements in
   alternate rows and alternate columns of the given original matrix.

   Use MPI_SENDRECV to send the submatrix to itself and print the
   results. To test this program you can run the program on just
   one processor.

   For example, if the given matrix is:

   1  2  3   4  5  6
   7  8  9  10 11 12
   13 14 15 16 17 18
   19 20 21 22 23 23
   24 25 26 27 28 29

   The submatrix created should look like:

   1   3  5
   13 15 17
   24 26 28

*/

#include 
 
int main(argc, argv)
int argc;
char **argv;
{
    int myrank, size ;
    int i, j, mym = 10, myn = 10;
    int a[10][10], b[5][5], c[5][5] ;
    MPI_Datatype subrow, submatrix ;
    MPI_Status status ;
    MPI_Aint sizeofint ;

    MPI_Init (&argc, &argv);
 
    MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);
 
    /* Initialize the local array */
    for (i = 0; i < mym; i++)
      for (j = 0; j < myn; j++)
	a[i][j] = i*myn + j + 1;

    /* Print the local matrix */
    for (i = 0; i < mym; i++){
      for (j = 0; j < myn; j++)
	printf("%d ", a[i][j]) ;
      printf("\n");
    }

    /* Create a submatrix datatype */
    
    /* Create datatype for the sub-row */

    MPI_Type_vector(5,1,2,MPI_INT,&subrow);

    /* Create datatype for the sub-matrix */
    
    MPI_Type_hvector(5,1,20*sizeof(int),subrow,&submatrix);

    /* Commit the datatype created */

    MPI_Type_commit(&submatrix);

    /* Send it to self and print it */

    MPI_Sendrecv(a,1,submatrix,myrank,201,c,25,MPI_INT,myrank,201,MPI_COMM_WORLD,&status);

    /* Print the submatrix */
    for (i = 0; i < 5; i++){
      for (j = 0; j < 5; j++)
	printf("%d ", c[i][j]) ;
      printf("\n");
    }

    MPI_Finalize();
}
 




