#include <stdio.h>
#include "mpi.h"

int main(argc, argv)
int argc;
char **argv;
{
   int rank, size;
   MPI_Init(&argc,&argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   printf("Hello world! I am %d of %d\n",rank,size);
   MPI_Finalize();
   return 0;
}



// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/d_5_20]
// └─$ mpicc file_3_lab_1.c -o file_3

// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/d_5_20]
// └─$ ./file_3
// Hello world! I am 0 of 1
