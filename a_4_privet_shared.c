#include <stdio.h>
#include <omp.h>
#include <time.h>

int main(int argc, char* argv[]){
    int partial_Sum, total_Sum;
    int array1[100];
    int array2[100];
    int num_threads;
    double start_time, end_time;

    // Initialize array with values from 0 to 99
    for(int i = 0; i < 100; i++) {
        array1[i] = i;
        array2[i] = i;
    }

    start_time = omp_get_wtime();

    #pragma omp parallel private(partial_Sum) shared(total_Sum, num_threads)
    {
        partial_Sum = 0;
        total_Sum = 0;

        #pragma omp single
        {
            num_threads = omp_get_num_threads();
        }

        #pragma omp for
        for(int i = 0; i < 100; i++){
            partial_Sum += array1[i] * array2[i];
        }

        // Create thread safe region.
        #pragma omp critical
        {
            // Add each thread's partial sum to the total sum
            total_Sum += partial_Sum;
        }
    }

    end_time = omp_get_wtime();

    printf("Total Sum: %d\n", total_Sum);
    printf("Number of threads: %d\n", num_threads);
    printf("Execution time: %f seconds\n", end_time - start_time);

    return 0;
}


/*

┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC]
└─$ gcc -fopenmp -o a_4 a_4_privet_shared.c

┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC]
└─$ ./a_4
Total Sum: 328350
Number of threads: 4
Execution time: 0.000314 seconds

*/