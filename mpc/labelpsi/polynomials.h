#pragma once

#include <cstdint>
#include <vector>

using namespace std;

/*
The functions in this file implement some operations on polynomials, interpreted
as vectors of coefficients.
*/

// if p is at most 32 bits, arithmetic modulo p can be implemented by directly
// using 64-bit arithmetic and reducing mod p. but if it's bigger,
// multiplication will overflow, so we have to use a slower multiplication
// algorithm.
#ifdef MODULUS_IS_SMALL
    #define MUL_MOD(a, b, m) (((a)*(b)) % (m))
#else
    // using __uint128_t, which is a GCC-specific extension
    #define MUL_MOD(a, b, m) ((((__uint128_t) (a)) * ((__uint128_t) (b))) % (m))
#endif

/* modexp(a, b, m) computes a^b mod m in O(log b) time. */
uint64_t modexp(uint64_t base, uint64_t exponent, uint64_t modulus);

/* modinv(a, m) computes a^-1 mod m in O(log m) time. */
uint64_t modinv(uint64_t x, uint64_t modulus);

/*
polynomial_from_roots(l) computes the coefficients of the polynomial
(x - l[0]) * (x - l[1]) * ...

time complexity: O(n²), where n is the size of l
*/
void polynomial_from_roots(vector<uint64_t> &roots, vector<uint64_t> &coeffs, uint64_t modulus);

/*
polynomial_from_points(xs, ys) computes the coefficients of a
(xs.size() - 1)-degree polynomial f with f(xs[i]) = ys[i] for each i.

if two points with the same x exist, the behavior is undefined, even if their y
are the same.

time complexity: O(n²), where n is the size of xs
*/
void polynomial_from_points(vector<uint64_t> &xs,
                            vector<uint64_t> &ys,
                            vector<uint64_t> &coeffs,
                            uint64_t modulus);
