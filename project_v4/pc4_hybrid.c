#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

#define HASH_LEN 65

// const char charset[] = "0123456789"
// const char charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
// const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*0123456789";
const int charset_size = sizeof(charset) - 1;

void sha256_hash(const char *str, char output[HASH_LEN]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)str, strlen(str), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(output + (i * 2), "%02x", hash[i]);
    output[64] = '\0';
}

void try_password(const char *candidate, const char *target_hash, int *found_flag, int rank) {
    char hash[HASH_LEN];
    sha256_hash(candidate, hash);

    if (strcmp(hash, target_hash) == 0) {
        printf("[MPI Rank %d] Found password: %s\n", rank, candidate);
        *found_flag = 1;
    }
}

void generate_combinations(char *prefix, int depth, int max_length, const char *target_hash, int *found_flag, int rank) {
    if (*found_flag) return;

    if (depth == max_length) {
        try_password(prefix, target_hash, found_flag, rank);
        return;
    }

    for (int i = 0; i < charset_size && !*found_flag; i++) {
        prefix[depth] = charset[i];
        prefix[depth + 1] = '\0';
        generate_combinations(prefix, depth + 1, max_length, target_hash, found_flag, rank);
    }
}

double time_diff_in_seconds(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char target_hash[HASH_LEN];
    if (rank == 0) {
        printf("Enter the SHA-256 hash to crack (64 hex characters): ");
        if (scanf("%64s", target_hash) != 1) {
            fprintf(stderr, "Error reading hash input.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Broadcast hash to all processes
    MPI_Bcast(target_hash, HASH_LEN, MPI_CHAR, 0, MPI_COMM_WORLD);

    int found = 0;
    int global_found = 0;
    int max_length = 4;

    struct timespec start_time, end_time;
    if (rank == 0)
        clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int len = 1; len <= max_length; len++) {
        // if (rank == 0) printf("Trying passwords of length %d...\n", len);

        // Parallelize the first character among MPI ranks and rest with OpenMP
        #pragma omp parallel for schedule(dynamic)
        for (int i = rank; i < charset_size; i += size) {
            if (found) continue;

            char buffer[len + 1];
            buffer[0] = charset[i];
            buffer[1] = '\0';

            int local_found = 0;
            generate_combinations(buffer, 1, len, target_hash, &local_found, rank);

            if (local_found) {
                #pragma omp critical
                found = 1;
            }
        }

        // Synchronize across all processes
        MPI_Allreduce(&found, &global_found, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);
        if (global_found) break;
    }

    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        if (!global_found) {
            printf("Password not found up to length %d.\n", max_length);
        }

        double elapsed = time_diff_in_seconds(start_time, end_time);
        printf("Time taken: %.6f seconds\n", elapsed);
    }

    MPI_Finalize();
    return 0;
}



// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project_v4]
// └─$ mpicc -fopenmp -o pc4 pc4_hybrid.c -lcrypto

// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project_v4]
// └─$ mpirun -np 2 ./pc4