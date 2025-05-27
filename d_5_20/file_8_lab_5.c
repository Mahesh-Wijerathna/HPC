/* MPI Lab 5, Example Program   */


/* Program to illustrate the MPI topology functions */
#include 
#include 

typedef enum{FALSE, TRUE} BOOLEAN;
#define N_DIMS 2

main(argc, argv) 
int argc;
char **argv;
{
    MPI_Comm comm_2d, row_comm, col_comm;
    int myrank, size, P, Q, p, q, reorder;
    int dims[N_DIMS],         /* number of dimensions */
        local[N_DIMS],        /* local row and column positions */
        period[N_DIMS],       /* aperiodic flags */
        remain_dims[N_DIMS];  /* sub-dimension computation flags */
    int left, right, bottom, top;
    int sum_row, sum_col;

    MPI_Init (&argc, &argv);

    MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    /* Generate a new communicator with virtual topology */
    dims[0] = dims[1] = 0;
    MPI_Dims_create( size, N_DIMS, dims );
    P = dims[0];
    Q = dims[1];

    reorder = TRUE;
    period[0] = period[1] = TRUE;
    MPI_Cart_create(MPI_COMM_WORLD, N_DIMS, dims, period, reorder, &comm_2d);

    /* Determine the position in the grid and split grid_comm
       into row and col comms */
    MPI_Cart_coords(comm_2d, myrank, N_DIMS, local);
    p = local[0]; 
    q = local[1];

    /* Get row and column communicators using cartesian sub-topology */
    remain_dims[0] = FALSE; 
    remain_dims[1] = TRUE;
    MPI_Cart_sub(comm_2d, remain_dims, &row_comm);

    remain_dims[0] = TRUE; 
    remain_dims[1] = FALSE;
    MPI_Cart_sub(comm_2d, remain_dims, &col_comm);

    MPI_Cart_shift(comm_2d, 0, 1, &left, &right);
    MPI_Cart_shift(comm_2d, 1, 1, &top, &bottom);

    printf("(%d,%d)[%d] left = %d right = %d top = %d bottom = %d\n",
           p, q, myrank, left, right, top, bottom);

    fflush(stdout);
    
    MPI_Allreduce(&myrank, &sum_row, 1, MPI_INT, MPI_SUM, row_comm);
    MPI_Allreduce(&myrank, &sum_col, 1, MPI_INT, MPI_SUM, col_comm);
    printf("Process (%d, %d): Row sum = %d, Col sum = %d\n", p, q,
	   sum_row, sum_col);
    
    MPI_Finalize();
}

/*

Output with 4 processors

(0,0)[0] left = 2 right = 2 top = 1 bottom = 1
(1,0)[2] left = 0 right = 0 top = 3 bottom = 3
(1,1)[3] left = 1 right = 1 top = 2 bottom = 2
(0,1)[1] left = 3 right = 3 top = 0 bottom = 0
Process (0, 0): Row sum = 1, Col sum = 2
Process (1, 0): Row sum = 5, Col sum = 2
Process (1, 1): Row sum = 5, Col sum = 4
Process (0, 1): Row sum = 1, Col sum = 4


*/