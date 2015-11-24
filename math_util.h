#ifndef _MATH_UTIL_H
#define _MATH_UTIL_H
#include <stdbool.h>

bool is_prime(long n);

// "high-quality" random number
long get_rand(bool keepopen);

long gcd(long a, long b);

long modinv(long x, long m);

// (x^e) mod m
long modpow(long x, long e, long m);

// computes (a*b) mod m
long mod_mult(long a, long b, long m);

// compues (a+b) mod m without overflow
long mod_add(long a, long b, long m);

#endif
