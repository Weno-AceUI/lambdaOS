#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include <stddef.h>

// SHA-256 hash size in bytes
#define SHA256_HASH_SIZE 32

// Calculate SHA-256 hash of input data
void sha256_hash(const void* data, size_t len, uint8_t hash[SHA256_HASH_SIZE]);

// Calculate SHA-256 hash of a string
void sha256_hash_string(const char* str, uint8_t hash[SHA256_HASH_SIZE]);

// Convert hash to hex string
void hash_to_hex(const uint8_t* hash, size_t len, char* hex);

#endif // CRYPTO_H 