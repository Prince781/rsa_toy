// cc *.c -o rsa_test && ./rsa_test
#include "rsa.h"
#include <stdio.h>	// perror()
#include <string.h>	// strerror(), memcmp()
#include <stdlib.h>	// exit(), atoi()
#include <errno.h>	// errno

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

void test_load_key(void) {
	struct RSAKey key = KEY_NULL;

	key = rsa_key_load("key.pub", "key.priv");
	if (memcmp(&key, &KEY_NULL, sizeof(key)) == 0)
		fprintf(stderr, "could not load RSA key\n");
	else
		printf("loaded RSA key\n");
}

void test_genkey(void) {
	struct RSAKey key;
	const char *text = "This is testing text.";
	size_t size = strlen(text) + 1;

	char *enc_text;
	size_t enc_size;

	char *dec_text;
	size_t dec_size;

	key = rsa_genkey();
	printf("key = %s\n", rsa_key_tostr(key));
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
}

int main(int argc, char *argv[]) {
	struct RSAKey key;
	const char *cmd = argv[1] ? argv[1] : "";
	const char *fname = argv[2];
	const char *oname = argv[3];
	FILE *file, *output;
	unsigned char buffer[4096];
	size_t bufsize = 0;

    if (strcmp(cmd, "test") == 0) {
		test_genkey();
    } else if (strcmp(cmd, "encrypt") == 0 || strcmp(cmd, "decrypt") == 0) {
		char *outbuf;
		key = rsa_key_load("key.pub", "key.priv");
		if (memcmp(&key, &KEY_NULL, sizeof(key)) == 0)
			return 1;

		if ((file = fopen(fname, "r")) == NULL) {
			fprintf(stderr, "error opening %s\n", fname);
			perror("fopen");
			return 1;
		}

		if ((output = fopen(oname, "w")) == NULL) {
			fprintf(stderr, "error opening %s\n", oname);
			perror("fopen");
			fclose(file);
			return 1;
		}

		while ((bufsize = fread(buffer, 1, sizeof(buffer), file)) > 0) {
			printf("input (%s):\n", fname);
			print_data(buffer, bufsize, 80);
			printf("\n");

			if (*cmd == 'e')
				outbuf = rsa_encrypt(key.pub, buffer, &bufsize);
			else
				outbuf = rsa_decrypt(key, buffer, &bufsize);

			printf("output (%s):\n", oname);
			print_data(outbuf, bufsize, 80);
			printf("\n");

			fwrite(outbuf, 1, bufsize, output);
			free(outbuf);
		}

		fclose(file);
		fclose(output);
	} else {
        int n1, n2;
        fprintf(stderr, "usage: %n%s%n <cmd> [args...]\n", &n1, argv[0], &n2);
        fprintf(stderr, "       %2$*1$s test\n" 
                        "       %2$*1$s encrypt <input> <output.enc>\n"
                        "       %2$*1$s decrypt <input.enc> <output>\n"
                        "(key.pub and key.priv must exist)\n", 
                        n2-n1, " ");
		return 1;
	}

	return 0;
}
