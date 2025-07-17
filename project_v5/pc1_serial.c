#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <time.h>

// === Global Definitions ===
#define HASH_LEN 65

const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const int charset_size = sizeof(charset) - 1;

int found = 0;
int max_length = 4;
// ==========================

void sha256_hash(const char *str, char output[HASH_LEN]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)str, strlen(str), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(output + (i * 2), "%02x", hash[i]);
    output[64] = '\0';
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

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int len = 1; len <= max_length && !found; len++) {
        unsigned long total = 1;
        for (int i = 0; i < len; i++)
            total *= charset_size;

        for (unsigned long n = 0; n < total && !found; n++) {
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
                printf("Found password: %s\n", candidate);
                found = 1;
                break;
            }
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
// └─$ gcc -o pc1 pc1_serial.c -lcrypto

// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project_v4]
// └─$ ./pc1