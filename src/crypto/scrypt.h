// Copyright (c) 2024 The YukiCoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef YUKICOIN_CRYPTO_SCRYPT_H
#define YUKICOIN_CRYPTO_SCRYPT_H

#include <cstdint>
#include <cstdlib>

/**
 * Scrypt hash function for proof-of-work.
 * Uses Litecoin parameters: N=1024, r=1, p=1
 * 
 * @param input     The input data to hash (typically 80 bytes block header)
 * @param inputlen  Length of input data
 * @param output    Output buffer (must be at least 32 bytes)
 * @param outputlen Length of output (typically 32 for 256-bit hash)
 */
void scrypt_1024_1_1_256(const char* input, size_t inputlen, char* output, size_t outputlen);

/**
 * Simplified interface for block header hashing
 * @param input  80-byte block header
 * @param output 32-byte hash output
 */
void scrypt_hash(const char* input, char* output);

#endif // YUKICOIN_CRYPTO_SCRYPT_H

