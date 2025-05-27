#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <omp.h>

#define MAX_DIGITS 6
#define HASH_LEN 65

void sha256_hash(const char *str, char output[HASH_LEN]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)str, strlen(str), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(output + i * 2, "%02x", hash[i]);
    output[64] = '\0';
}

int main() {
    const char *target_hash = "c0437dd38b1307c9932870d596cbcd8a2e157d08cd7308c25473e11966cf3102"; // SHA-256 of "123456"
    char candidate[16];
    char hash[HASH_LEN];
    int found = 0;

    #pragma omp parallel for private(candidate, hash)
    for (int i = 0; i <= 999999; i++) {
        if (found) continue;

        sprintf(candidate, "%06d", i);  // zero-padded
        sha256_hash(candidate, hash);

        if (strcmp(hash, target_hash) == 0) {
            #pragma omp critical
            {
                printf("✅ Password found: %s\n", candidate);
                found = 1;
            }
        }
    }

    return 0;
}



// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project]
// └─$ gcc -fopenmp password_cracker_openmp.c -lcrypto -o cracker_openmp

// ┌──(mahesh㉿MAHESH-LAP)-[/mnt/g/7_SEM/HPC/project]
// └─$ ./cracker_openmp
// ✅ Password found: 133468
