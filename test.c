// cc *.c -o rsa_test && ./rsa_test
#include "rsa.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>	// exit(), atoi()

// prints each byte
void print_data(const void *data, size_t size, unsigned cols) {
	size_t i;
	unsigned pos = 0;

	for (i = 0; i < size; ++i) {
		pos += printf(" %02x", ((unsigned) *(const char *) (data+i)) & 0xffu);
		if (pos >= cols - 3) {
			printf("\n");
			pos = 0;
		}
	}
}

int main(int argc, char *argv[]) {
	struct RSAKey key;
	const char *text = "This is testing text.";
	size_t size = strlen(text) + 1;

	char *enc_text;
	size_t enc_size;

	char *dec_text;
	size_t dec_size;

	key = rsa_genkey();
	printf("pubkey = {n:%ld, e:%ld}\n", key.pub.n, key.pub.e);
	printf("privkey = {d:%ld}\n", key.priv.d);
	printf("data (%zu bytes):\n", size);
	print_data(text, size, 80);
	printf("\n");

	enc_size = size;
	enc_text = rsa_encrypt(key.pub, text, &enc_size);
	printf("encryption done\n");
	printf("data (%zu bytes):\n", enc_size);
	print_data(enc_text, enc_size, 80);
	printf("\n");

	dec_size = enc_size;
	dec_text = rsa_decrypt(key, enc_text, &dec_size);
	printf("decryption done\n");
	printf("data (%zu bytes):\n", dec_size);
	print_data(dec_text, dec_size, 80);
	printf("\n");

	if (memcmp(text, dec_text, size) == 0)
		printf("decryption successful\n");
	else
		printf("decryption failed\n");

	if (rsa_key_save(&key.pub, sizeof(key.pub), "key.pub") != 0)
		printf("failed to save public key\n");
	else
		printf("saved public key to key.pub\n");
	if (rsa_key_save(&key.priv, sizeof(key.priv), "key.priv") != 0)
		printf("failed to save private key\n");
	else
		printf("saved private key to key.priv\n");

	return 0;
}
