#pragma once
#include <endian.h>
#include <openssl/md5.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define PRINT_X(x)                                                             \
    printf(__FILE__ ":%d " #x " is lo: %li, hi: %li\n", __LINE__, x.lo, x.hi)

typedef struct {
    long lo;
    long hi;
} X;

static inline X createUnmixedXSeed(long seed) {
    long l = seed ^ 7640891576956012809L;
    long h = l + -7046029254386353131L;
    return (X){.lo = l, .hi = h};
}

static inline long mixStafford13(long sseed) {
    unsigned long seed = sseed;
    seed = (seed ^ seed >> 30) * -4658895280553007687L;
    seed = (seed ^ seed >> 27) * -7723592293110705685L;
    return seed ^ seed >> 31;
}

static inline unsigned long rotl(const unsigned long v, int k) {
    return (v << k) | (v >> (64 - k));
}

static inline long next(X *x) {
    long l = x->lo;
    long h = x->hi;
    long n = rotl(l + h, 17) + l;
    h ^= l;
    x->lo = rotl(l, 49) ^ h ^ h << 21;
    x->hi = rotl(h, 28);
    return n;
}

static inline X split(X *x) { return (X){.lo = next(x), .hi = next(x)}; }
static inline X mixXSeed(X x) {
    return (X){.lo = mixStafford13(x.lo), .hi = mixStafford13(x.hi)};
}
static inline X createXSeed(long seed) {
    return mixXSeed(createUnmixedXSeed(seed));
}
static inline X str2X(char *s) {
    uint8_t digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)s, strlen(s), digest);
    long l = htobe64(*(long *)digest);
    long h = htobe64(*(long *)&digest[8]);
    return (X){.lo = l, .hi = h};
}
static inline X xorX(X a, X b) {
    return (X){.lo = a.lo ^ b.lo, .hi = a.hi ^ b.hi};
}
static inline X splitStr(X x, char *s) {
    X a = str2X(s);
    return xorX(x, a);
}
static inline long hashBlockPos(int x, int y, int z) {
    long l = (long)(x * 3129871) ^ (long)z * 116129781L ^ (long)y;
    l = l * l * 42317861L + l * 11L;
    return l >> 16;
}
static inline X splitBlockPos(X a, int x, int y, int z) {
    long l = hashBlockPos(x, y, z);
    return (X){.lo = l ^ a.lo, .hi = a.hi};
}
static inline long nextBits(X *x, int bits) {
    return (unsigned long)next(x) >> (64 - bits);
}
static inline float nextFloat(X *x) {
    return ((float)(nextBits(x, 24)) * 5.9604645E-8F);
}

static inline int nextInt(X *x) {
    return next(x);
}

static inline int nextBoundInt(X *x, const int bound) {
    if (bound <= 0) {
        return -1;
    }

    uint32_t l = (uint32_t)nextInt(x);
    uint64_t m = (uint64_t)l * bound;
    uint32_t n = (uint32_t)m;  // lower 32 bits

    if (n < (uint32_t)bound) {
        // Calculate remainderUnsigned(~bound + 1, bound)
        const uint32_t remainder = (uint32_t)(~bound + 1) % (uint32_t)bound;

        while (n < remainder) {
            l = (uint32_t)nextInt(x);
            m = (uint64_t)l * bound;
            n = (uint32_t)m;
        }
    }

    return (int)(m >> 32);
}