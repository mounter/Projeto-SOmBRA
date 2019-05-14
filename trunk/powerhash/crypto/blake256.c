/* 
 *
 * Blake256.c
 * Author: Curie Kief, Base Max
 * Purpose of review and rewrite is optimization.
 * Reffer : http://131002.net/blake/
 *
 */
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdint.h>

typedef struct
{
	uint32_t h[8], s[4], t[2];
	int buflen, nullt;
	uint8_t buf[64];
} state;

#define U8TO32(p) \
	(((uint32_t)((p)[0]) << 24) | ((uint32_t)((p)[1]) << 16) |    \
	 ((uint32_t)((p)[2]) <<  8) | ((uint32_t)((p)[3])      ))

#define U32TO8(p,v) \
	(p)[0] = (uint8_t)((v) >> 24);(p)[1] = (uint8_t)((v) >> 16);\
	(p)[2] = (uint8_t)((v) >>  8);(p)[3] = (uint8_t)((v)      );

const uint8_t sigma[][16] = {
	{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
	{14,10,4,8,9,15,13,6,1,12,0,2,11,7,5,3},
	{11,8,12,0,5,2,15,13,10,14,3,6,7,1,9,4},
	{ 7,9,3,1,13,12,11,14,2,6,5,10,4,0,15,8},
	{ 9,0,5,7,2,4,10,15,14,1,11,12,6,8,3,13},
	{ 2,12,6,10,0,11,8,3,4,13,7,5,15,14,1,9},
	{12,5,1,15,14,13,4,10,0,7,6,3,9,2,8,11},
	{13,11,7,14,12,1,3,9,5,0,15,4,8,6,2,10},
	{ 6,15,14,9,11,3,0,8,12,2,13,7,1,4,10,5},
	{10,2,8,4,7,6,1,5,15,11,9,14,3,12,13,0},
	{ 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
	{14,10,4,8,9,15,13,6,1,12,0,2,11,7,5,3},
	{11,8,12,0,5,2,15,13,10,14,3,6,7,1,9,4},
	{ 7,9,3,1,13,12,11,14,2,6,5,10,4,0,15,8}
};

const uint32_t cst[16] = {
	0x243F6A88,0x85A308D3,0x13198A2E,0x03707344,
	0xA4093822,0x299F31D0,0x082EFA98,0xEC4E6C89,
	0x452821E6,0x38D01377,0xBE5466CF,0x34E90C6C,
	0xC0AC29B7,0xC97C50DD,0x3F84D5B5,0xB5470917
};

static const uint8_t padding[] = {
	0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

void blake256_compress(state *S,const uint8_t *block)
{
	uint32_t v[16],m[16],i;
	#define ROT(x,n) (((x)<<(32-n))|((x)>>(n)))
	#define G(a,b,c,d,e)                                      \
		v[a] += (m[sigma[i][e]] ^ cst[sigma[i][e+1]]) + v[b];\
		v[d] = ROT(v[d] ^ v[a],16);                          \
		v[c] += v[d];                                        \
		v[b] = ROT(v[b] ^ v[c],12);                          \
		v[a] += (m[sigma[i][e+1]] ^ cst[sigma[i][e]])+v[b];  \
		v[d] = ROT(v[d] ^ v[a],8);                          \
		v[c] += v[d];                                        \
		v[b] = ROT(v[b] ^ v[c],7);

	for(i = 0;i < 16;++i)
		m[i] = U8TO32(block + i * 4);
	for(i = 0;i < 8; ++i)
		v[i] = S->h[i];
	v[8]  = S->s[0] ^ 0x243F6A88;
	v[9]  = S->s[1] ^ 0x85A308D3;
	v[10] = S->s[2] ^ 0x13198A2E;
	v[11] = S->s[3] ^ 0x03707344;
	v[12] = 0xA4093822;
	v[13] = 0x299F31D0;
	v[14] = 0x082EFA98;
	v[15] = 0xEC4E6C89;
	if(S->nullt == 0)
	{
		v[12] ^= S->t[0];
		v[13] ^= S->t[0];
		v[14] ^= S->t[1];
		v[15] ^= S->t[1];
	}
	for(i = 0;i < 14;++i)
	{
		G(0,4,8,12,0);
		G(1,5,9,13,2);
		G(2,6,10,14,4);
		G(3,7,11,15,6);
		G(3,4,9,14,14);
		G(2,7,8,13,12);
		G(0,5,10,15,8);
		G(1,6,11,12,10);
	}
	for(i = 0;i < 16;++i)
		S->h[i % 8] ^= v[i];
	for(i = 0;i < 8;++i)
		S->h[i] ^= S->s[i % 4];
}
void blake256_hash(uint8_t *out,const uint8_t *in)
{
	state S;
	S.h[0] = 0x6A09E667;
	S.h[1] = 0xBB67AE85;
	S.h[2] = 0x3C6EF372;
	S.h[3] = 0xA54FF53A;
	S.h[4] = 0x510E527F;
	S.h[5] = 0x9B05688C;
	S.h[6] = 0x1F83D9AB;
	S.h[7] = 0x5BE0CD19;
	S.t[0] = S.t[1] = S.buflen = S.nullt = 0;
	S.s[0] = S.s[1] = S.s[2] = S.s[3] = 0;
	S.t[0] += 512;
	blake256_compress(&S,in);
	in += 64;
	S.t[0] += 512;
	blake256_compress(&S,in);
	in += 64;
	S.t[0] += 512;
	blake256_compress(&S,in);
	in += 64;
	memcpy((void *) (S.buf),(void *)in, 8);
	memcpy((void *) (S.buf + 8),(void *)padding,47);
	S.buf[55] = 0x01;
	U32TO8(S.buf + 56,S.t[1]);
	U32TO8(S.buf + 60,S.t[0] + 64);
	S.t[0] += 64;
	blake256_compress(&S,S.buf);
	U32TO8(out +  0,S.h[0]);
	U32TO8(out +  4,S.h[1]);
	U32TO8(out +  8,S.h[2]);
	U32TO8(out + 12,S.h[3]);
	U32TO8(out + 16,S.h[4]);
	U32TO8(out + 20,S.h[5]);
	U32TO8(out + 24,S.h[6]);
	U32TO8(out + 28,S.h[7]);
}
