#include "xoshiro256starstar.h"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define RESET   "\033[0m"
#define MAGENTA "\x1b[35m"
#define BLUE    "\x1b[34m"

struct node
{
  struct node **l;
  uint32_t *preimage;
};

struct node_attack
{
  struct node_attack **l;
  uint32_t nb_block;
};


void printtab(uint32_t* tab, int size);
void speck48_96(const uint32_t k[4], const uint32_t p[2], uint32_t c[2]);
int test_sp48(void);
void speck48_96_inv(const uint32_t k[4], const uint32_t c[2], uint32_t p[2]);
int test_inv_sp48(void);
uint64_t cs48_dm(const uint32_t m[4], const uint64_t h);
int test_cs48_dm(void);
uint64_t hs48(const uint32_t *m, uint64_t fourlen, int padding, int verbose);
uint64_t get_cs48_dm_fp(uint32_t m[4]);
int test_cs48_dm_fp(void);
void find_exp_mess(uint32_t m1[4], uint32_t m2[4]);
void attack();
void example_attack();
int test_em();
