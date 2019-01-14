#include "second_preim_48_fillme.h"

#define ROTL24_16(x) ((((x) << 16) ^ ((x) >> 8)) & 0xFFFFFF)
#define ROTL24_3(x) ((((x) << 3) ^ ((x) >> 21)) & 0xFFFFFF)

#define ROTL24_8(x) ((((x) << 8) ^ ((x) >> 16)) & 0xFFFFFF)
#define ROTL24_21(x) ((((x) << 21) ^ ((x) >> 3)) & 0xFFFFFF)

#define IV 0x010203040506ULL

void printtab(uint32_t* tab, int size){
  int i;
  for(i=0;i<size;i++){
    printf("\t%08x",tab[i]);
  }
  printf("\n");
}

void free_tree(struct node* n){
  int i = 0;
  if(n){
    if(n->l){
      while(i<16){
        free_tree(n->l[i]);
        i++;
      }
      free(n->l);
    }
    free(n);
  }
}

void speck48_96(const uint32_t k[4], const uint32_t p[2], uint32_t c[2])
{
	uint32_t rk[23];
	uint32_t ell[3] = {k[1], k[2], k[3]};

	rk[0] = k[0];

	c[0] = p[0];
	c[1] = p[1];

	/* full key schedule */
	for (unsigned i = 0; i < 22; i++)
	{
		uint32_t new_ell = ((ROTL24_16(ell[0]) + rk[i]) ^ i) & 0xFFFFFF;
		rk[i+1] = ROTL24_3(rk[i]) ^ new_ell;
		ell[0] = ell[1];
		ell[1] = ell[2];
		ell[2] = new_ell;
	}

	for (unsigned i = 0; i < 23; i++)
	{
		c[1]=((ROTL24_16(c[1])+c[0])^rk[i]) & 0xFFFFFF;
		c[0]=ROTL24_3(c[0])^c[1];
	}

	return;
}

int test_sp48(void){
	uint32_t key[4]= {0x020100, 0x0a0908 , 0x121110, 0x1a1918};
	uint32_t plaintext[2] = {0x696874, 0x6d2073};
  uint32_t ciphertext[2] = {0xb6445d, 0x735e10};
	uint32_t result[2];
	speck48_96(key, plaintext, result);
	printf("\tResult\n");
	printtab(result, 2);
	printf("\n\tExpected\n");
	printtab(ciphertext, 2);
	if(result[0]==ciphertext[0] && result[1]==ciphertext[1])
		return 1;
	return 0;
}

/* the inverse cipher */
void speck48_96_inv(const uint32_t k[4], const uint32_t c[2], uint32_t p[2])
{
	uint32_t rk[23];
	uint32_t ell[3] = {k[1], k[2], k[3]};

	rk[0] = k[0];

	p[0] = c[0];
	p[1] = c[1];

	/* full key schedule */
	for (unsigned i = 0; i < 22; i++)
	{
		uint32_t new_ell = ((ROTL24_16(ell[0]) + rk[i]) ^ i) & 0xFFFFFF;
		rk[i+1] = ROTL24_3(rk[i]) ^ new_ell;
		ell[0] = ell[1];
		ell[1] = ell[2];
		ell[2] = new_ell;
	}

	for (int i = 22; i >= 0; i--)
	{
		p[0]=ROTL24_21(p[0]^p[1]);
		p[1]=ROTL24_8(((p[1]^rk[i])-p[0])&0xFFFFFF);
	}

	return;
}

int test_inv_sp48(void){
	uint32_t key[4]= {0x020100, 0x0a0908 , 0x121110, 0x1a1918};
	uint32_t plaintext[2] = {0x696874, 0x6d2073};
  uint32_t ciphertext[2] = {0xb6445d, 0x735e10};
	uint32_t result[2];
	speck48_96(key, plaintext, result);
	printf("\tPlaintext\n");
	printtab(plaintext, 2);
	printf("\n\tCiphered\n");
	printtab(result, 2);
	printf("\n\tExpected\n");
	printtab(ciphertext, 2);
	printf("\n\tInversion\n");
	speck48_96_inv(key, ciphertext, result);
	printtab(result, 2);
	if(plaintext[0]==result[0] && plaintext[1]==result[1])
		return 1;
	return 0;
}

/* The Davies-Meyer compression function based on speck48_96,
 * using an XOR feedforward
 * The input/output chaining value is given on a single 64-bit word, whose
 * low bits are set to the low half of the "plaintext"/"ciphertext" (p[0]/c[0])
 */
uint64_t cs48_dm(const uint32_t m[4], const uint64_t h)
{
	uint32_t p[2]={(uint32_t)h&0xFFFFFF, (uint32_t)(h>>24)&0xFFFFFF};
	uint32_t c[2];
	uint64_t res;
	speck48_96(m, p, c);
	res=c[0];
	res=res<<24;
	res+=c[1];
	return res^h;
}

int test_cs48_dm(void){
	uint64_t expected = 0x7FDD5A6EB248ULL;
	uint32_t m[4] = {0x00, 0x00, 0x00, 0x00};
	uint64_t h = 0;
	uint64_t result =  cs48_dm(m, h);
	printf("\tResult\n");
	printf("\t%" PRIx64 "\n", result);
	printf("\n\tExpected\n");
	printf("\t%" PRIx64 "\n", expected);
	if(result==expected)
		return 1;
	return 0;
}

/* assumes message length is fourlen * four blocks of 24 bits store over 32
 * fourlen is on 48 bitscs48_dm(m,fp) == fp
 * simply add one block of padding with fourlen and zeros on higher pos */
uint64_t hs48(const uint32_t *m, uint64_t fourlen, int padding, int verbose)
{
	uint64_t h = IV;
	const uint32_t *mp = m;

	for (uint64_t i = 0; i < fourlen; i++)
	{
		h = cs48_dm(mp, h);
		if (verbose)
			printf("@%lu : %06X %06X %06X %06X => %06lX\n", i, mp[0], mp[1], mp[2], mp[3], h);
		mp += 4;
	}
	if (padding)
	{
		uint32_t pad[4];
		pad[0] = fourlen & 0xFFFFFF;
		pad[1] = (fourlen >> 24) & 0xFFFFFF;
		pad[2] = 0;
		pad[3] = 0;
		h = cs48_dm(pad, h);
		if (verbose)
			printf("@%lu : %06X %06X %06X %06X => %06lX\n", fourlen, pad[0], pad[1], pad[2], pad[3], h);
	}

	return h;
}

/* Computes the unique fixed-point for cs48_dm for the message m */
uint64_t get_cs48_dm_fp(uint32_t m[4]){
	uint32_t fixed_point[2];
	uint32_t cipher[2]={0x00, 0x00};
	speck48_96_inv(m, cipher, fixed_point);
	uint64_t h;
	h=fixed_point[1];
	h=h<<24;
	h+=fixed_point[0];
	return h;
}

int test_cs48_dm_fp(void){
	uint32_t m[4] = {0x12, 0x34, 0x56, 0x78};
	uint64_t fixed_point=get_cs48_dm_fp(m);
	printf("\tFIXED POINT: \n");
	printf("\t%" PRIx64 "\n", fixed_point);
	uint64_t res=cs48_dm(m, fixed_point);
	printf("\tRES CS48DM: \n");
	printf("\t%" PRIx64 "\n", res);
	if(res==fixed_point)
		return 1;
	return 0;
}

/* Finds a two-block expandable message for hs48, using a fixed-point
 * That is, computes m1, m2 s.t. hs48_nopad(m1||m2) = hs48_nopad(m1||m2^*),
 * where hs48_nopad is hs48 with no padding */
void find_exp_mess(uint32_t m1[4], uint32_t m2[4], int ram){
  struct node* root = calloc(1,sizeof(struct node*));
  struct node* f;

  uint64_t hin = 0;
  uint32_t m1r[4], m2r[4];
  uint64_t m2_0, m2_1, m1_0, m1_1, hash;
  __my_little_xoshiro256starstar_unseeded_init();
  for (uint64_t i = 0; i< 5800000; i++){
    m1_0 = xoshiro256starstar_random();
    m1_1 = xoshiro256starstar_random();
    m1r[0] =m1_0&0xFFFFFF;
    m1r[1] =(m1_0>>24)&0xFFFFFF;
    m1r[2] =m1_1&0xFFFFFF;
    m1r[3] =(m1_1>>24)&0xFFFFFF;

    hash  = hs48(m1r,1,0,0);
    f = root;
    for(size_t j = 0; j<12; j++){
      uint8_t valHexa = (((hash<<4*j))>>44)&0xF;
      if(f->l==NULL)
        f->l= calloc(16,sizeof(struct node*));

      if(f->l[valHexa]==NULL)
        f->l[valHexa] = calloc(1,sizeof(struct node*));

      f = f->l[valHexa];
      if(j==11){
        f->preimage = calloc(4,sizeof(uint32_t));
        f->preimage[0]=m1r[0];
        f->preimage[1]=m1r[1];
        f->preimage[2]=m1r[2];
        f->preimage[3]=m1r[3];
      }
    }
  }
  __my_little_xoshiro256starstar_unseeded_init();
  for (uint64_t i = 0; i < 1<<(48-ram); i++){
    m2_0 = xoshiro256starstar_random();
    m2_1 = xoshiro256starstar_random();
    m2r[0] =m2_0&0xFFFFFF;
    m2r[1] =(m2_0>>24)&0xFFFFFF;
    m2r[2] =m2_1&0xFFFFFF;
    m2r[3] =(m2_1>>24)&0xFFFFFF;
    uint64_t fp = get_cs48_dm_fp(m2r);
    f = root;
    int find=0;
    for(size_t j = 0; j<12; j++){
      uint8_t valHexa = (((fp<<4*j))>>44)&0xF;
      if(f->l!=NULL){
        if(f->l[valHexa]!=NULL){
          find++;
          f=f->l[valHexa];
        }else{
          break;
        }
      }else{
        break;
      }
    }

    if(find==12){
      for(int i=0; i<4; i++){
        m1[i]=f->preimage[i];
        m2[i]=m2r[i];
      }
      free_tree(root);
      return;
    }
  }
}

int test_em(int ram){
  uint32_t m1[4];
  uint32_t m2[4];
  find_exp_mess(m1,m2,ram);
  printf("\tMESSAGE: \n");
  printtab(m1,4);
  printf("\tFIXED POINT: \n");
  printtab(m2,4);
  printf("\tHASH OF MESSAGE: \n");
  uint64_t h1 = hs48(m1,1,0,0);
  printf("\t%" PRIx64 "\n",h1);

  srand((unsigned) time(NULL));
  int nb_fixed_point = rand() % 500;
  printf("\tHASH OF MESSAGE WITH %d(random) FIXED POINT: \n",nb_fixed_point);
  uint32_t *m = malloc((nb_fixed_point+1)*4*sizeof(uint32_t));
  for(int i = 0; i < 4; i++)
    m[i]=m1[i];
  for(int i = 1; i < nb_fixed_point+1; i++){
    m[i*4]=m2[0];
    m[i*4+1]=m2[1];
    m[i*4+2]=m2[2];
    m[i*4+3]=m2[3];
  }

  uint64_t h2 = hs48(m,nb_fixed_point+1,0,0);
  printf("\t%" PRIx64 "\n",h2);
  free(m);
  if(h1==h2)
    return 1;
  else
    return 0;
}

/*
We have a message mess.

First, find an expandable message (m1, fp)
So, we can construct a message m2 s.t h(m1||fp||fp||fp) == h(m1)

Then, find a collision s.t h(cm) == h(mess_X) with X an index of submessage mess

Finally, forge a new message nm s.t m1||(fp * X-1)||cm||mess_X||mess_X+1||mess
*/
void attack(int ram)
{
  uint64_t hash  = 0xFF3FD9D23B89;
  uint64_t h = IV;
  uint32_t m1[4], fp[4], mess[4];
  clock_t begin = clock();
  find_exp_mess(m1,fp,ram);
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("%s%f\n","TIME EXPANDABLE: ", time_spent);
  begin = clock();
  struct node_attack* root = calloc(1,sizeof(struct node_attack));
  struct node_attack* f;

  //On calcule et stock les différents hash possible pour le long message mess...
  for (int i = 0; i < (1 << 20); i+=4){
    mess[0] = i;
    mess[1] = 0;
    mess[2] = 0;
    mess[3] = 0;
    h=cs48_dm(mess, h);
    f = root;
    for(size_t j = 0; j<12; j++){
      uint8_t valHexa = (((h<<4*j))>>44)&0xF;
      if(f->l==NULL)
        f->l= calloc(16,sizeof(struct node_attack*));

      if(f->l[valHexa]==NULL)
        f->l[valHexa] = calloc(1,sizeof(struct node_attack*));

      f = f->l[valHexa];
      if(j==11)
        f->nb_block = i;
    }
  }

  uint64_t m2_0, m2_1;
  __my_little_xoshiro256starstar_unseeded_init();
  for (uint64_t i = 0; i < (1UL<<48); i++){
    f = root;
    m2_0 = xoshiro256starstar_random();
    m2_1 = xoshiro256starstar_random();
    mess[0] =m2_0&0xFFFFFF;
    mess[1] =(m2_0>>24)&0xFFFFFF;
    mess[2] =m2_1&0xFFFFFF;
    mess[3] =(m2_1>>24)&0xFFFFFF;

    h=cs48_dm(mess, get_cs48_dm_fp(fp));
    int find=0;
    for(size_t j = 0; j<12; j++){
      uint8_t valHexa = (((h<<4*j))>>44)&0xF;
      if(f->l!=NULL){
        if(f->l[valHexa]!=NULL){
          find++;
          f = f->l[valHexa];
        }else{
          break;
        }
      }else{
        break;
      }
    }
    if(find==12){
      printf("FIND_ATTACK\n");
      end = clock();
      time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
      printf("%s%f\n","TIME COL: ", time_spent);
      return;
    }
  }
}
