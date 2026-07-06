#ifndef SHA256_H
#define SHA256_H

/* Compute the SHA-256 of a NUL-terminated string.
 * `out` must have room for at least 65 bytes (64 hex chars + NUL). */
void sha256_hex(const char *s, char *out);

#endif /* SHA256_H */
