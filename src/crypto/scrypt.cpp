// Copyright (c) 2024 The YukiCoin Core developers
// Copyright (c) 2013-2014 The Litecoin Core developers
// Copyright (c) 2009 Colin Percival
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <crypto/scrypt.h>
#include <crypto/sha256.h>
#include <cstring>
#include <cstdint>

// Scrypt implementation based on Colin Percival's original and Litecoin's adaptation

static inline uint32_t le32dec(const void* pp)
{
    const uint8_t* p = (uint8_t const*)pp;
    return ((uint32_t)(p[0]) | ((uint32_t)(p[1]) << 8) |
            ((uint32_t)(p[2]) << 16) | ((uint32_t)(p[3]) << 24));
}

static inline void le32enc(void* pp, uint32_t x)
{
    uint8_t* p = (uint8_t*)pp;
    p[0] = x & 0xff;
    p[1] = (x >> 8) & 0xff;
    p[2] = (x >> 16) & 0xff;
    p[3] = (x >> 24) & 0xff;
}

static inline uint32_t rotl(uint32_t x, int n)
{
    return (x << n) | (x >> (32 - n));
}

static void salsa20_8(uint32_t B[16])
{
    uint32_t x[16];
    for (int i = 0; i < 16; i++)
        x[i] = B[i];

    for (int i = 0; i < 8; i += 2) {
        x[4] ^= rotl(x[0] + x[12], 7);
        x[8] ^= rotl(x[4] + x[0], 9);
        x[12] ^= rotl(x[8] + x[4], 13);
        x[0] ^= rotl(x[12] + x[8], 18);

        x[9] ^= rotl(x[5] + x[1], 7);
        x[13] ^= rotl(x[9] + x[5], 9);
        x[1] ^= rotl(x[13] + x[9], 13);
        x[5] ^= rotl(x[1] + x[13], 18);

        x[14] ^= rotl(x[10] + x[6], 7);
        x[2] ^= rotl(x[14] + x[10], 9);
        x[6] ^= rotl(x[2] + x[14], 13);
        x[10] ^= rotl(x[6] + x[2], 18);

        x[3] ^= rotl(x[15] + x[11], 7);
        x[7] ^= rotl(x[3] + x[15], 9);
        x[11] ^= rotl(x[7] + x[3], 13);
        x[15] ^= rotl(x[11] + x[7], 18);

        x[1] ^= rotl(x[0] + x[3], 7);
        x[2] ^= rotl(x[1] + x[0], 9);
        x[3] ^= rotl(x[2] + x[1], 13);
        x[0] ^= rotl(x[3] + x[2], 18);

        x[6] ^= rotl(x[5] + x[4], 7);
        x[7] ^= rotl(x[6] + x[5], 9);
        x[4] ^= rotl(x[7] + x[6], 13);
        x[5] ^= rotl(x[4] + x[7], 18);

        x[11] ^= rotl(x[10] + x[9], 7);
        x[8] ^= rotl(x[11] + x[10], 9);
        x[9] ^= rotl(x[8] + x[11], 13);
        x[10] ^= rotl(x[9] + x[8], 18);

        x[12] ^= rotl(x[15] + x[14], 7);
        x[13] ^= rotl(x[12] + x[15], 9);
        x[14] ^= rotl(x[13] + x[12], 13);
        x[15] ^= rotl(x[14] + x[13], 18);
    }

    for (int i = 0; i < 16; i++)
        B[i] += x[i];
}

static void blockmix_salsa8(uint32_t* Bin, uint32_t* Bout, uint32_t* X, int r)
{
    for (int i = 0; i < 16; i++)
        X[i] = Bin[(2 * r - 1) * 16 + i];

    for (int i = 0; i < 2 * r; i++) {
        for (int j = 0; j < 16; j++)
            X[j] ^= Bin[i * 16 + j];
        salsa20_8(X);
        for (int j = 0; j < 16; j++)
            Bout[(i / 2 + (i & 1) * r) * 16 + j] = X[j];
    }
}

static inline uint64_t integerify(uint32_t* B, int r)
{
    uint32_t* X = &B[(2 * r - 1) * 16];
    return ((uint64_t)X[1] << 32) | X[0];
}

static void smix(uint32_t* B, int r, uint64_t N, uint32_t* V, uint32_t* XY)
{
    uint32_t* X = XY;
    uint32_t* Y = &XY[32 * r];
    uint32_t* Z = &XY[64 * r];

    for (int k = 0; k < 32 * r; k++)
        X[k] = B[k];

    for (uint64_t i = 0; i < N; i++) {
        for (int k = 0; k < 32 * r; k++)
            V[i * (32 * r) + k] = X[k];
        blockmix_salsa8(X, Y, Z, r);
        for (int k = 0; k < 32 * r; k++)
            X[k] = Y[k];
    }

    for (uint64_t i = 0; i < N; i++) {
        uint64_t j = integerify(X, r) & (N - 1);
        for (int k = 0; k < 32 * r; k++)
            X[k] ^= V[j * (32 * r) + k];
        blockmix_salsa8(X, Y, Z, r);
        for (int k = 0; k < 32 * r; k++)
            X[k] = Y[k];
    }

    for (int k = 0; k < 32 * r; k++)
        B[k] = X[k];
}

// PBKDF2-SHA256 implementation
static void PBKDF2_SHA256(const uint8_t* passwd, size_t passwdlen,
                          const uint8_t* salt, size_t saltlen,
                          uint64_t c, uint8_t* buf, size_t dkLen)
{
    uint8_t U[32];
    uint8_t T[32];
    uint8_t saltbuf[128 + 4];

    if (saltlen > 128) saltlen = 128;
    memcpy(saltbuf, salt, saltlen);

    for (uint32_t i = 1; dkLen > 0; i++) {
        saltbuf[saltlen + 0] = (i >> 24) & 0xff;
        saltbuf[saltlen + 1] = (i >> 16) & 0xff;
        saltbuf[saltlen + 2] = (i >> 8) & 0xff;
        saltbuf[saltlen + 3] = i & 0xff;

        // HMAC-SHA256(passwd, salt || INT(i))
        uint8_t ipad[64], opad[64];
        memset(ipad, 0x36, 64);
        memset(opad, 0x5c, 64);

        uint8_t key[64];
        if (passwdlen > 64) {
            CSHA256 sha;
            sha.Write(passwd, passwdlen);
            uint8_t keyhash[32];
            sha.Finalize(keyhash);
            memcpy(key, keyhash, 32);
            memset(key + 32, 0, 32);
        } else {
            memcpy(key, passwd, passwdlen);
            memset(key + passwdlen, 0, 64 - passwdlen);
        }

        for (int j = 0; j < 64; j++) {
            ipad[j] ^= key[j];
            opad[j] ^= key[j];
        }

        CSHA256 inner, outer;
        inner.Write(ipad, 64);
        inner.Write(saltbuf, saltlen + 4);
        uint8_t innerhash[32];
        inner.Finalize(innerhash);

        outer.Write(opad, 64);
        outer.Write(innerhash, 32);
        outer.Finalize(U);

        memcpy(T, U, 32);

        for (uint64_t j = 1; j < c; j++) {
            // HMAC-SHA256(passwd, U)
            inner.Reset();
            inner.Write(ipad, 64);
            inner.Write(U, 32);
            inner.Finalize(innerhash);

            outer.Reset();
            outer.Write(opad, 64);
            outer.Write(innerhash, 32);
            outer.Finalize(U);

            for (int k = 0; k < 32; k++)
                T[k] ^= U[k];
        }

        size_t clen = (dkLen > 32) ? 32 : dkLen;
        memcpy(buf, T, clen);
        buf += clen;
        dkLen -= clen;
    }
}

void scrypt_1024_1_1_256(const char* input, size_t inputlen, char* output, size_t outputlen)
{
    const int N = 1024;
    const int r = 1;
    const int p = 1;

    uint32_t* V = new uint32_t[32 * r * N];
    uint32_t* XY = new uint32_t[64 * r + 32];
    uint32_t B[32 * r * p];

    // Step 1: PBKDF2(password=input, salt=input, c=1, dkLen=128*r*p)
    PBKDF2_SHA256((const uint8_t*)input, inputlen,
                  (const uint8_t*)input, inputlen,
                  1, (uint8_t*)B, 128 * r * p);

    // Convert to little-endian uint32_t
    for (int k = 0; k < 32 * r * p; k++)
        B[k] = le32dec((uint8_t*)B + 4 * k);

    // Step 2: scrypt ROMix
    for (int i = 0; i < p; i++)
        smix(&B[i * 32 * r], r, N, V, XY);

    // Convert back to bytes
    uint8_t Bbytes[128 * r * p];
    for (int k = 0; k < 32 * r * p; k++)
        le32enc(Bbytes + 4 * k, B[k]);

    // Step 3: PBKDF2(password=input, salt=B, c=1, dkLen=32)
    PBKDF2_SHA256((const uint8_t*)input, inputlen,
                  Bbytes, 128 * r * p,
                  1, (uint8_t*)output, outputlen);

    delete[] V;
    delete[] XY;
}

void scrypt_hash(const char* input, char* output)
{
    scrypt_1024_1_1_256(input, 80, output, 32);
}

