#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>
#include <omp.h>

#define HASH_LEN 65

const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int charset_size = sizeof(charset) - 1;

void sha256_hash(const char *str, char output[HASH_LEN]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)str, strlen(str), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(output + (i * 2), "%02x", hash[i]);
    output[64] = '\0';
}

void try_password(const char *candidate, const char *target_hash, int *found_flag) {
    char hash[HASH_LEN];
    sha256_hash(candidate, hash);

    if (strcmp(hash, target_hash) == 0) {
        printf("Found password: %s\n", candidate);
        *found_flag = 1;
    }
}

void generate_combinations(char *prefix, int depth, int max_length, const char *target_hash, int *found_flag) {
    if (*found_flag) return;

    if (depth == max_length) {
        try_password(prefix, target_hash, found_flag);
        return;
    }

    for (int i = 0; i < charset_size && !*found_flag; i++) {
        prefix[depth] = charset[i];
        prefix[depth + 1] = '\0';
        generate_combinations(prefix, depth + 1, max_length, target_hash, found_flag);
    }
}

double time_diff_in_seconds(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main() {
    char target_hash[HASH_LEN];
    printf("Enter the SHA-256 hash to crack (64 hex characters): ");
    if (scanf("%64s", target_hash) != 1) {
        fprintf(stderr, "Error reading hash input.\n");
        return 1;
    }

    int found = 0;
    int max_length = 4;

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int len = 1; len <= max_length && !found; len++) {
        printf("Trying passwords of length %d...\n", len);

        
        #pragma omp parallel for shared(found)
        for (int i = 0; i < charset_size; i++) {
            if (found) continue;

            char buffer[len + 1];
            buffer[0] = charset[i];
            buffer[1] = '\0';

            generate_combinations(buffer, 1, len, target_hash, &found);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);

    if (!found) {
        printf("Password not found up to length %d.\n", max_length);
    }

    double elapsed = time_diff_in_seconds(start_time, end_time);
    printf("Time taken: %.6f seconds\n", elapsed);

    return 0;
}



// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project_v4]
// └─$ gcc -fopenmp -o pc2 pc2_omp.c -lcrypto

// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project_v4]
// └─$ ./pc2