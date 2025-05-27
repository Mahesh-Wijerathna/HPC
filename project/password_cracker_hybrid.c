#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <mpi.h>
#include <omp.h>

#define MAX_DIGITS 6  // e.g., 000000 to 999999
#define HASH_LEN 65   // SHA-256 = 64 hex chars + null terminator

// Function to hash a string using SHA-256
void sha256_hash(const char *str, char outputBuffer[HASH_LEN]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)str, strlen(str), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);

    outputBuffer[64] = 0;
}

// Main cracking function
void crack_password(int start, int end, const char *target_hash, int *found_flag) {
    char candidate[16];
    char hash[HASH_LEN];

    #pragma omp parallel for private(candidate, hash)
    for (int i = start; i <= end; i++) {
        if (*found_flag) continue;

        sprintf(candidate, "%06d", i);  // zero-padded number
        sha256_hash(candidate, hash);

        if (strcmp(hash, target_hash) == 0) {
            #pragma omp critical
            {
                printf("✅ Password found: %s\n", candidate);
                *found_flag = 1;
            }
        }
    }
}

int main(int argc, char **argv) {
    const char *target_hash = "1ebe9a6cbc88ccbf72b43d7b83cf302684a8fcf54e0f1fdb3d4290495266536f";  // SHA-256 of "123456"

    int world_size, world_rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int range = 1000000;  // 000000 to 999999
    int chunk_size = range / world_size;
    int start = world_rank * chunk_size;
    int end = (world_rank == world_size - 1) ? (range - 1) : (start + chunk_size - 1);

    int found = 0;
    crack_password(start, end, target_hash, &found);

    MPI_Finalize();
    return 0;
}


// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project]
// └─$ mpicc -fopenmp password_cracker.c -lcrypto -o cracker

// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project]
// └─$ mpirun -np 2 ./cracker
// ✅ Password found: 133456
