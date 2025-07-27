#include "settings.c"
#include <endian.h>
#include <openssl/md5.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    long lo;
    long hi;
} X;

int search_for_len;
X bedrock_seed;
long checked = 0;
int started;

#define PRINT_X(x)                                                             \
    printf(__FILE__ ":%d " #x " is lo: %li, hi: %li\n", __LINE__, x.lo, x.hi);

X createUnmixedXSeed(long seed) {
    long l = seed ^ 7640891576956012809L;
    long h = l + -7046029254386353131L;
    return (X){.lo = l, .hi = h};
}

long mixStafford13(long sseed) {
    unsigned long seed = sseed;
    seed = (seed ^ seed >> 30) * -4658895280553007687L;
    seed = (seed ^ seed >> 27) * -7723592293110705685L;
    return seed ^ seed >> 31;
}

static inline unsigned long rotl(const unsigned long v, int k) {
    return (v << k) | (v >> (64 - k));
}

long next(X *x) {
    long l = x->lo;
    long h = x->hi;
    long n = rotl(l + h, 17) + l;
    h ^= l;
    x->lo = rotl(l, 49) ^ h ^ h << 21;
    x->hi = rotl(h, 28);
    return n;
}

X split(X *x) { return (X){.lo = next(x), .hi = next(x)}; }

X mixXSeed(X x) {
    return (X){.lo = mixStafford13(x.lo), .hi = mixStafford13(x.hi)};
}

X createXSeed(long seed) { return mixXSeed(createUnmixedXSeed(seed)); }

X str2X(char *s) {
    uint8_t digest[MD5_DIGEST_LENGTH];
    MD5((const unsigned char *)s, strlen(s), digest);
    long l = htobe64(*(long *)digest);
    long h = htobe64(*(long *)&digest[8]);
    return (X){.lo = l, .hi = h};
}

X xorX(X a, X b) { return (X){.lo = a.lo ^ b.lo, .hi = a.hi ^ b.hi}; }

X splitStr(X x, char *s) {
    X a = str2X(s);
    return xorX(x, a);
}

long hashBlockPos(int x, int y, int z) {
    long l = (long)(x * 3129871) ^ (long)z * 116129781L ^ (long)y;
    l = l * l * 42317861L + l * 11L;
    return l >> 16;
}

X splitBlockPos(X a, int x, int y, int z) {
    long l = hashBlockPos(x, y, z);
    return (X){.lo = l ^ a.lo, .hi = a.hi};
}

long nextBits(X *x, int bits) { return (unsigned long)next(x) >> (64 - bits); }

float nextFloat(X *x) { return ((float)(nextBits(x, 24)) * 5.9604645E-8F); }

enum bedrock_state { unkown = '?', bedrock = '#', empty = '.' };

bool isBedrock(int x, int z) {
    X bedrock_random = splitBlockPos(bedrock_seed, x, y, z);
    return nextFloat(&bedrock_random) < 0.2f;
}

void checkPatternRot1(int rx, int rz, int fx, int fz) {
    int x = 0;
    int z = 0;
    for (int i = 0; i < search_for_len; ++i) {
        switch (search_for[i]) {
        case '#':
            if (!isBedrock(rx + x * fx, rz + z * fz)) {
                return;
            }
            break;
        case '.':
            if (isBedrock(rx + x * fx, rz + z * fz)) {
                return;
            }
            break;
        case '\n':
            x = 0;
            ++z;
            continue;
        case '?':
            break;
        default:
            printf("bad char ):");
            exit(1);
        }
        ++x;
    }
    printf("pattern found at x %d z %d\n", rx, rz);
    if (exit_if_found) {
        exit(0);
    }
}

void checkPatternRot2(int rx, int rz, int fx, int fz) {
    int x = 0;
    int z = 0;
    for (int i = 0; i < search_for_len; ++i) {
        switch (search_for[i]) {
        case '#':
            if (!isBedrock(rx + z * fz, rz + x * fx)) {
                return;
            }
            break;
        case '.':
            if (isBedrock(rx + z * fz, rz + x * fx)) {
                return;
            }
            break;
        case '\n':
            x = 0;
            ++z;
            continue;
        case '?':
            break;
        }
        ++x;
    }
    printf("pattern found at x %d z %d\n", rx, rz);
    if (exit_if_found) {
        exit(0);
    }
}
void checkPattern(int x, int z) {
    checkPatternRot1(x, z, 1, 1);
    checkPatternRot2(x, z, 1, -1);
    checkPatternRot1(x, z, -1, -1);
    checkPatternRot2(x, z, -1, 1);
    ++checked;

    if (checked % 1000000 == 0) {
        int now = time(NULL);
        printf("at x %d z %d running for %ds checked %li at %fp/s\n", x, z,
               now - started, checked,
               ((float)checked / (float)(now - started)));
    }
}

int main() {
    X world_random = createXSeed(world_seed);
    PRINT_X(world_random);
    X splitter = split(&world_random);
    PRINT_X(splitter);
    X world_bedrock = splitStr(splitter, random_type);
    PRINT_X(world_bedrock);
    bedrock_seed = split(&world_bedrock);
    PRINT_X(bedrock_seed);
    search_for_len = strlen(search_for);
    started = time(NULL);
    int x = 0;
    int z = 0;
    int steps = 0;
    checkPattern(x, z);
    while (true) {
        for (int i = 0; i < steps; ++i) {
            ++z;
            checkPattern(x, z);
        }
        for (int i = 0; i < steps; ++i) {
            ++x;
            checkPattern(x, z);
        }
        ++steps;
        for (int i = 0; i < steps; ++i) {
            --z;
            checkPattern(x, z);
        }
        for (int i = 0; i < steps; ++i) {
            --x;
            checkPattern(x, z);
        }
        ++steps;
    }
}
