#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

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
        printf("✅ Found password: %s\n", candidate);
        *found_flag = 1;
    }
}

// Recursive generator function
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

int main() {
    const char *target_hash = "88032ede3980a605a15e42bd577c2584d63f178aa63ba6cd4de315a4e3a41cc0"; // SHA256 of "abc"
    int found = 0;

    int max_length = 4; // Try all passwords from 1 to 3 characters

    for (int len = 1; len <= max_length && !found; len++) {
        printf("Trying passwords of length %d...\n", len);
        char buffer[len + 1];
        buffer[0] = '\0';
        generate_combinations(buffer, 0, len, target_hash, &found);
    }

    return 0;
}
