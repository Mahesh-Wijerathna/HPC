#include <stdio.h>

int main() {
    int array1[100], array2[100];
    int sum = 0;

    // Initialize the arrays with some values
    for (int i = 0; i < 100; i++) {
        array1[i] = i ; // Example initialization
        array2[i] = i ; // Example initialization
    }

    // Multiply each element and calculate the sum
    for (int i = 0; i < 100; i++) {
        sum += array1[i] * array2[i];
    }

    printf("The sum of the products of the arrays is: %d\n", sum);

    return 0;
}