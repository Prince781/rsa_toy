#include "rsa.h"
#include "math_util.h"	// gcd(), modinv(), modpow(), get_rand()
#include <stdlib.h>	// calloc(), malloc()
#include <assert.h>	// assert()
#include <string.h>	// memcpy()
#include <stdio.h>	// printf(), perror()
#include <errno.h>	// errno

static void gen_primes(long *p, long *q, long min_n) {
	long f = 5;
	long f2; 
	long max_n;
	long arb_n;
	ldiv_t d;

	*p = 2;
	*q = 3;
	while (f*f <= min_n)
		f += 2;
	f2 = f;
	while (f*f > 0)
		f += 2;
	max_n = (f-2) * (f-2);
	f = f2;

	arb_n = get_rand(false) % (max_n - min_n);
	if (arb_n < 0)
		arb_n += (max_n - min_n);
	arb_n += min_n;
#ifdef DEBUG
	printf("max_n = %ld\n", max_n);
	printf("arb_n = %ld\n", arb_n);
#endif
	while ((*p)*(*q) <= arb_n) {
		if (is_prime(f))
			*p = f;
		d = ldiv(arb_n, *p);
		if (is_prime(d.quot))
			*q = d.quot;
		f += 2;
	}

#ifdef DEBUG
	printf("p = %ld, q = %ld\n", *p, *q);
#endif
}

/* steps:
 * 1. find two primes (p,q) and semiprime (n=pq)
 * 2. find an exponent (e) that is coprime with (p-1)(q-1)
 * 3. find (d), the inverse of (e) modulo (p-1)(q-1)
 */
struct RSAKey rsa_genkey(void) {
	struct RSAKey key;
	long p, q;
	long n, e;
	long d;
	long t;	// totient of n

	// 1. gen primes (TODO: improve speed)
	gen_primes(&p, &q, 0x00FF00FF00FF00FFL);

	key.priv.p = p;
	key.priv.q = q;
	key.pub.n = n = p * q;
	t = (p - 1) * (q - 1);	// t(pq) = t(p)t(q) = (p-1)(q-1)

	// 2. pick e coprime with (p-1)(q-1)
	// TODO: find better method
	do {	// slow
		e = get_rand(true) % t;
		if (e < 0)
			e += t;
	} while (gcd(e, t) != 1);
	key.pub.e = e;
	get_rand(false);	// close fd to /dev/random

	// 3. find inverse of e mod (p-1)(q-1)
	d = modinv(e, t);
	key.priv.d = d;

	return key;
}

int rsa_key_save(void *key, size_t size, const char *filename) {
	FILE *file;

	if ((file = fopen(filename, "w")) == NULL) {
		perror(strerror(errno));
		return -1;
	}

	fwrite(key, size, 1, file);

	return fclose(file);
}

/**
 * 1. break into i-bit blocks (i = 32) with padding
 * 2. encrypt each block (c = m^e (mod n))
 */
void *rsa_encrypt(struct PubKey key, const void *buf, size_t *sz) {
	long *enc;
	// number of bytes (N) of data per block of 2(N)
	size_t bdata = sizeof(*enc) / 2;
	size_t blcks = (*sz + bdata-1) / bdata;

	assert (blcks > 0);
	enc = calloc(blcks, sizeof(*enc));

	// copy data with padding:
	// _ _ _ _ -> 0 _ 0 _ 0 _ 0 _
	size_t i, j, k;
	const unsigned char *pbuf = buf;
	unsigned long v;
	for (i = 0; i < blcks; ++i) {
		j = bdata * i;
		v = enc[i];
		for (k = 0; k < bdata && (j+k) < *sz; ++k)
			v |= ((unsigned long) pbuf[j + k] & 0xffu) << (16ul*k);
		enc[i] = v;
	}
	
	// encrypt: m^e (mod n)
	for (i = 0; i<blcks; ++i) {
#ifdef DEBUG
		printf("%016lx |->", enc[i]);
#endif
		enc[i] = modpow(enc[i], key.e, key.n);
#ifdef DEBUG
		printf(" %016lx\n", enc[i]);
#endif
	}

	*sz = blcks * sizeof(*enc);
	return enc;
}

void *rsa_decrypt(struct RSAKey key, const void *buf, size_t *sz) {
	long *lbuf;
	unsigned char *dec;
	size_t bdata = sizeof(*lbuf) / 2;
	size_t blcks = *sz / sizeof(*lbuf);

	lbuf = malloc(*sz);
	memcpy(lbuf, buf, *sz);
	dec = calloc(blcks, bdata);

	// decrypt each block (8-bytes to 4-bytes) and remove padding:
	// _ _ _ _ _ _ _ _ -> 0 _ 0 _ 0 _ 0 _ -> _ _ _ _
	// c^d (mod n)
	size_t i, j;
	for (i = 0; i < blcks; ++i) {
#ifdef DEBUG
		printf("%016lx |->", lbuf[i]);
#endif
		lbuf[i] = modpow(lbuf[i], key.priv.d, key.pub.n);
		for (j = 0; j < bdata; ++j)
			dec[bdata*i + j] = (unsigned char) 
				(((unsigned long)lbuf[i] >> (16ul*j)) & 0xffu);
#ifdef DEBUG
		printf(" %016lx\n", lbuf[i]);
#endif
	}

	*sz = blcks * bdata;
	free(lbuf);
	return dec;
}
