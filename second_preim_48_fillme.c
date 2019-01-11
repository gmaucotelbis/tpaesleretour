#include "second_preim_48_fillme.h"
#define ROTL24_16(x) ((((x) << 16) ^ ((x) >> 8)) & 0xFFFFFF)
#define ROTL24_3(x) ((((x) << 3) ^ ((x) >> 21)) & 0xFFFFFF)

#define ROTL24_8(x) ((((x) << 8) ^ ((x) >> 16)) & 0xFFFFFF)
#define ROTL24_21(x) ((((x) << 21) ^ ((x) >> 3)) & 0xFFFFFF)

#define IV 0x010203040506ULL

void printtab(uint32_t* tab, int size){
  int i;
  for(i=0;i<size;i++){
    printf(" %08x",tab[i]);
  }
  printf("\n");
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
	printtab(result, 2);
	printf("-----------Expected-------------\n");
	printtab(ciphertext, 2);
	return 1;
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
	printf("-----------Plaintext-------------\n");
	printtab(plaintext, 2);
	printf("-----------Ciphered-------------\n");
	printtab(result, 2);
	printf("-----------(Expected)-------------\n");
	printtab(ciphertext, 2);
	printf("-----------Inversion-------------\n");
	speck48_96_inv(key, ciphertext, result);
	printtab(result, 2);
	return 1;
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
	uint64_t result;

	printf("%" PRIx64 "\n", cs48_dm(m, h));
	printf("\n-----------(Expected)-------------\n");
	printf("%" PRIx64 "\n", expected);
}

/* assumes message length is fourlen * four blocks of 24 bits store over 32
 * fourlen is on 48 bitscs48_dm(m,fp) == fp
 * simply add one block of padding with fourlen and zeros on higher pos */
uint64_t hs48(const uint32_t *m, uint64_t fourlen, int padding, int verbose)
{
	uint64_t h = IV;
	uint32_t *mp = m;

	for (uint64_t i = 0; i < fourlen; i++)
	{
		h = cs48_dm(mp, h);
		if (verbose)
			printf("@%llu : %06X %06X %06X %06X => %06llX\n", i, mp[0], mp[1], mp[2], mp[3], h);
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
			printf("@%llu : %06X %06X %06X %06X => %06llX\n", fourlen, pad[0], pad[1], pad[2], pad[3], h);
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
	printf("FIXED POINT: \n");
	printf("%" PRIx64 "\n", fixed_point);
	uint64_t res=cs48_dm(m, fixed_point);
	printf("RES CS48DM: \n");
	printf("%" PRIx64 "\n", res);
}

/* Finds a two-block expandable message for hs48, using a fixed-point
 * That is, computes m1, m2 s.t. hs48_nopad(m1||m2) = hs48_nopad(m1||m2^*),
 * where hs48_nopad is hs48 with no padding */
void find_exp_mess(uint32_t m1[4], uint32_t m2[4])
{
	/* FILL ME */
}


void attack(void)
{
	/* FILL ME */
}

int main(){
	//test_sp48();
	//test_inv_sp48();
	//test_cs48_dm();
	test_cs48_dm_fp();
}
