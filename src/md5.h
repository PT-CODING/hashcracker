#ifndef MD5_H
#define MD5_H

/* Compute the MD5 of a NUL-terminated string.
 * `out` must have room for at least 33 bytes (32 hex chars + NUL). */
void md5_hex(const char *s, char *out);

#endif /* MD5_H */
