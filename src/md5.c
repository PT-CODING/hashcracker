/* MD5 (RFC 1321) -- compact single-block implementation for short inputs.
 * Passwords are short, so we only handle messages that fit in one 512-bit
 * block (<= 55 bytes), which keeps it allocation-free and fast. */
#include "md5.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define ROTL(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

static const uint32_t K[64] = {
    0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
    0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,0x6b901122,0xfd987193,0xa679438e,0x49b40821,
    0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
    0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
    0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
    0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
    0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
    0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391 };

static const int S[64] = {
    7,12,17,22, 7,12,17,22, 7,12,17,22, 7,12,17,22,
    5, 9,14,20, 5, 9,14,20, 5, 9,14,20, 5, 9,14,20,
    4,11,16,23, 4,11,16,23, 4,11,16,23, 4,11,16,23,
    6,10,15,21, 6,10,15,21, 6,10,15,21, 6,10,15,21 };

static void md5_block(const uint8_t *msg, size_t len, uint8_t out[16]) {
    uint8_t block[64];
    memset(block, 0, 64);
    memcpy(block, msg, len);
    block[len] = 0x80;
    uint64_t bits = (uint64_t)len * 8;
    for (int i = 0; i < 8; i++) block[56 + i] = (uint8_t)(bits >> (8 * i));

    uint32_t M[16];
    for (int i = 0; i < 16; i++)
        M[i] =  (uint32_t)block[i*4]         | ((uint32_t)block[i*4+1] << 8)
             | ((uint32_t)block[i*4+2] << 16) | ((uint32_t)block[i*4+3] << 24);

    uint32_t A = 0x67452301, B = 0xefcdab89, C = 0x98badcfe, D = 0x10325476;
    uint32_t a = A, b = B, c = C, d = D;
    for (int i = 0; i < 64; i++) {
        uint32_t F; int g;
        if      (i < 16) { F = (b & c) | (~b & d); g = i; }
        else if (i < 32) { F = (d & b) | (~d & c); g = (5*i + 1) & 15; }
        else if (i < 48) { F = b ^ c ^ d;          g = (3*i + 5) & 15; }
        else             { F = c ^ (b | ~d);       g = (7*i)     & 15; }
        F = F + a + K[i] + M[g];
        a = d; d = c; c = b;
        b = b + ROTL(F, S[i]);
    }
    A += a; B += b; C += c; D += d;

    uint32_t v[4] = { A, B, C, D };
    for (int i = 0; i < 4; i++) {
        out[i*4]   =  v[i]        & 0xff;
        out[i*4+1] = (v[i] >> 8)  & 0xff;
        out[i*4+2] = (v[i] >> 16) & 0xff;
        out[i*4+3] = (v[i] >> 24) & 0xff;
    }
}

void md5_hex(const char *s, char *out) {
    uint8_t d[16];
    md5_block((const uint8_t *)s, strlen(s), d);
    for (int i = 0; i < 16; i++) sprintf(out + i*2, "%02x", d[i]);
    out[32] = 0;
}
