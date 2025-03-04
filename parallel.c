#include <stdio.h>
#include <omp.h>

int main() {
    int array1[100], array2[100];
    int sum = 0;
    int partial_sum[4] = {0}; // Assuming 4 threads

    // Initialize the arrays with some values
    for (int i = 0; i < 100; i++) {
        array1[i] = i; // Example initialization
        array2[i] = i; // Example initialization
    }

    // Multiply each element and calculate the sum using OpenMP
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int local_sum = 0;

        #pragma omp for
        for (int i = 0; i < 100; i++) {
            local_sum += array1[i] * array2[i];
        }

        partial_sum[tid] = local_sum;
    }

    // Combine the partial sums
    for (int i = 0; i < 4; i++) {
        sum += partial_sum[i];
    }

    printf("The sum of the products of the arrays is: %d\n", sum);

    return 0;
}