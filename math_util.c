#include "math_util.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

bool is_prime(long n) {
	long f = 2;

	if (n <= 1)
		return false;
	while (f*f <= n) {
		if (n % f == 0)
			return false;
		if (f == 2)
			++f;
		else f += 2;
	}

	return true;
}

long get_rand(bool keepopen) {
	static int fd = -1;
	long val;

	if (fd == -1)
		fd = open("/dev/random", O_RDONLY);

	read(fd, &val, sizeof(val));

	if (!keepopen) {
		close(fd);
		fd = -1;
	}
	return val;
}

long gcd(long a, long b) {
	ldiv_t d;

	while (b != 0) {
		d = ldiv(a, b);
		a = b;
		b = d.rem;
	}

	return a;
}

long modinv(long x, long m) {
	ldiv_t d;

	long p0 = 0;
	long p1 = 1;
	long temp;

	long a = m;
	long b = x;

	do {
		d = ldiv(a, b);

		a = b;
		b = d.rem;

		// temp = (p0 - p1*d.quot) mod m
		temp = mod_mult(m-1, mod_mult(p1, d.quot, m), m);
		temp = mod_add(p0, temp, m);
		p0 = p1;
		p1 = temp;
	} while (b != 1);

	return p1;
}

long modpow(long x, long e, long m) {
	long res = 1;
	long val = x % m;

	for (; e != 0; e /= 2) {
		if (e & 1)
			res = mod_mult(res, val, m);	// x^i * x^j = x^(i + j)
		val = mod_mult(val, val, m);	// val^2 mod m
	}

	return res;
}

long mod_mult(long a, long b, long m) {
	long res = 0;

	while (a != 0) {
		if (a & 1)	// add to res each b*2^i (ith bit in a)
			res = mod_add(res, b, m);
		b = mod_add(b, b, m);
		a /= 2;
	}

	return res;
}

long mod_add(long a, long b, long m) {
	a %= m;
	b %= m;

	if (a+b <= 0l) {
		if (a >= b)
			return (b - (m - a)) % m;
		else
			return (a - (m - b)) % m;
	} else
		return (a + b) % m;
}

