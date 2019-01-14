#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>

static int __my_little_init_was_done = 0;
static uint64_t __my_little_xoshiro256starstar__s[4];

static inline uint64_t __my_little_xoshiro256starstar__rotl(const uint64_t x, int k);
void __my_little_xoshiro256starstar_initialization(uint64_t iv[4]);
void __my_little_xoshiro256starstar_unseeded_init();
uint64_t __my_little_xoshiro256starstar__next__unsafe(void);
uint64_t __my_little_xoshiro256starstar__next(void);
void __my_little_xoshiro256starstar__jump(void);
void __my_little_xoshiro256starstar__long_jump(void);
uint64_t xoshiro256starstar_random(void);
uint64_t xoshiro256starstar_random_unsafe(void);
void xoshiro256starstar_random_set(uint64_t seed[4]);
