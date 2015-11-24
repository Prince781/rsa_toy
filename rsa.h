#ifndef _RSA_H
#define _RSA_H
#include <stddef.h>	// size_t

struct PubKey {
	long n;	// modulus (semiprime)
	long e;	// exponent
};

struct PrivKey {
	// not necessary; just for debugging
	long p, q;	// two primes (n = pq)
	long d;		// decryption exponent
};

struct RSAKey {
	struct PubKey	pub;
	struct PrivKey	priv;
};

struct RSAKey rsa_genkey(void);

int rsa_key_save(void *key, size_t size, const char *filename);

/**
 * given @key, @buf, and a reference to @sz,
 * returns a new (encrypted) buffer of (*sz) bytes long
 */
void *rsa_encrypt(struct PubKey key, const void *buf, size_t *sz);

/**
 * returns a new decrypted buffer (*sz) bytes long
 */
void *rsa_decrypt(struct RSAKey key, const void *buf, size_t *sz);

#endif
