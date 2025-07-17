#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>
#include <mpi.h>

#define HASH_LEN 65

const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int charset_size = sizeof(charset) - 1;

void sha256_hash(const char *str, char output[HASH_LEN]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)str, strlen(str), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(output + (i * 2), "%02x", hash[i]);
    output[64] = '\0';
}

double time_diff_in_seconds(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);                 // Start MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // Get current process ID
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // Get total number of processes

    char target_hash[HASH_LEN];

    if (rank == 0) {
        printf("Enter the SHA-256 hash to crack (64 hex characters): ");
        if (scanf("%64s", target_hash) != 1) {
            fprintf(stderr, "Error reading hash input.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Broadcast hash from rank 0 to all processes
    MPI_Bcast(target_hash, HASH_LEN, MPI_CHAR, 0, MPI_COMM_WORLD);

    int max_length = 4;
    int found = 0;
    char found_password[10] = "";

    struct timespec start_time, end_time;
    if (rank == 0)
        clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int len = 1; len <= max_length && !found; len++) {
        unsigned long total = 1;
        for (int i = 0; i < len; i++)
            total *= charset_size;

        // Each process takes a chunk
        for (unsigned long n = rank; n < total; n += size) {
            char candidate[len + 1];
            unsigned long temp = n;

            for (int pos = len - 1; pos >= 0; pos--) {
                candidate[pos] = charset[temp % charset_size];
                temp /= charset_size;
            }
            candidate[len] = '\0';

            char hash[HASH_LEN];
            sha256_hash(candidate, hash);

            if (strcmp(hash, target_hash) == 0) {
                found = 1;
                strncpy(found_password, candidate, sizeof(found_password) - 1);
                break;
            }
        }

        // Check if any process found the password
        int global_found = 0;
        MPI_Allreduce(&found, &global_found, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);
        found = global_found;

        if (found) break;
    }

    // Gather and print result
    if (found) {
        char final_password[10];
        MPI_Reduce(rank == 0 ? found_password : NULL, final_password, 10, MPI_CHAR, MPI_MAX, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            printf("Found password: %s\n", found_password);
            printf("Time taken: %.6f seconds\n", time_diff_in_seconds(start_time, end_time));
        }
    } else {
        if (rank == 0) {
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            printf("Password not found up to length %d.\n", max_length);
            printf("Time taken: %.6f seconds\n", time_diff_in_seconds(start_time, end_time));
        }
    }

    MPI_Finalize(); // End MPI
    return 0;
}


// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project_v4]
// └─$ mpicc pc3_mpi.c -o pc3 -lcrypto

// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project_v4]
// └─$ mpirun -np 2 ./pc3