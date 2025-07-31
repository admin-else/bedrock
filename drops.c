
#include "x.h"

#include <limits.h>

int wither_skeleton_skull(const long seed) {
    X wither_random = mixXSeed(splitStr(createUnmixedXSeed(seed), "minecraft:entities/wither_skeleton"));
    int n = 0;
    while (true) {
        int coal = nextBoundInt(&wither_random, 1);
        int bone = nextBoundInt(&wither_random, 2);
        const float skull = nextFloat(&wither_random);
        if (!(skull < 0.025)) {
            return n;
        }
        ++n;
    }
}

int zombie_iron(const long seed) {
    X random = mixXSeed(splitStr(createUnmixedXSeed(seed), "minecraft:entities/zombie"));
    int n = 0;
    while (true) {
        int rotten_flesh = nextBoundInt(&random, 2);
        float rare_drop = nextFloat(&random);
        if (!(rare_drop < 0.025)) {
            return n;
        }
        int nth = nextBoundInt(&random, 2);
        if (nth != 0) {
            return n;
        }
        ++n;
    }
}

int main() {
    int highest = 0;
    long highest_seed = 0;
    for (long i = 33260000000; i < LONG_MAX; ++i) {
        const int n = zombie_iron(i);
        if (n > highest) {
            highest_seed = i;
            highest = n;
            printf("%ld %d\n", i, n);
        }
        if (i % 10000000 == 0) {
            printf("at %ld best %ld drops %d\n", i, highest_seed, highest);
        }
    }
}

