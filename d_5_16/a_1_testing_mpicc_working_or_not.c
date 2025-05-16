#include "mpi.h"
#include <stdio.h>
int main(argc,argv)
int argc;
char **argv;
{
    int rank, size ;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Hello World ! I am process %d of %d\n", rank, size);
    MPI_Finalize();
    return 0;
}


//  to compile 
//  mpicc -w -o a_1 a_1_test.c

//  to run
//  ./a_1

// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/d_5_16]
// └─$ ./a_1
// Hello World ! I am process 0 of 1


// to run with 2 processes
//  mpirun -np 2 a_1.out
//  mpiexec -np 2 a_1.out
//  srun -n 48 a_1.out
// 48 was not enough, so I used 2

// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/d_5_16]
// └─$ mpirun -np 2 a_1
// Hello World ! I am process 1 of 2
// Hello World ! I am process 0 of 2

