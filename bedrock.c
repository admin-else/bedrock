#include "x.h"
#include "bedrock_settings.c"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum bedrock_state { unkown = '?', bedrock = '#', empty = '.' };

size_t search_for_len;
X bedrock_seed;
long checked = 0;
time_t started;

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
            break;
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
        default:
            printf("bad char ):");
            exit(1);
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
        const time_t now = time(NULL);
        printf("at x %d z %d running for %lds checked %li at %fp/s\n", x, z,
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
