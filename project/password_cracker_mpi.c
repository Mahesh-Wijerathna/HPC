#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <mpi.h>

#define MAX_DIGITS 6
#define HASH_LEN 65

void sha256_hash(const char *str, char output[HASH_LEN]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)str, strlen(str), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(output + i * 2, "%02x", hash[i]);
    output[64] = '\0';
}

int main(int argc, char **argv) {
    const char *target_hash = "df735d7596b1b99ab2c2abffd2cb883a01e9a9519ed31b19c62c2d2fd03a21f0"; // "123456"
    int rank, size;
    char candidate[16], hash[HASH_LEN];
    int found = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int total = 1000000;
    int chunk = total / size;
    int start = rank * chunk;
    int end = (rank == size - 1) ? total - 1 : start + chunk - 1;

    for (int i = start; i <= end && !found; i++) {
        sprintf(candidate, "%06d", i);
        sha256_hash(candidate, hash);

        if (strcmp(hash, target_hash) == 0) {
            printf("✅ Process %d found the password: %s\n", rank, candidate);
            found = 1;
        }
    }

    MPI_Finalize();
    return 0;
}



// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project]
// └─$ mpicc password_cracker_mpi.c -lcrypto -o cracker_mpi

// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project]
// └─$ mpirun -np 2 ./cracker_mpi
// ✅ Process 0 found the password: 133462
