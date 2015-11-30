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

extern const struct RSAKey KEY_NULL;

struct RSAKey rsa_genkey(void);

// I/O

int rsa_key_save(void *key, size_t size, const char *filename);

struct RSAKey rsa_key_load(const char *pub_file, const char *priv_file);

// I/O

const char *rsa_key_tostr(struct RSAKey key);

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
