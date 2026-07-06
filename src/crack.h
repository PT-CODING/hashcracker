#ifndef CRACK_H
#define CRACK_H

/* A hash function: takes text, writes its hex digest into `out`.
 * Both md5_hex and sha256_hex match this shape, so the cracking engine
 * is generic -- it works with ANY hash you plug in. */
typedef void (*hash_fn)(const char *input, char *out_hex);

/* Try every word in `wordlist`.  Returns 1 if cracked (password in `found`),
 * 0 if exhausted, -2 if the wordlist could not be opened. */
int dict_attack(const char *target, const char *wordlist,
                hash_fn h, long *tries, char *found);

/* Try every combination of `charset` up to `maxlen` chars.
 * Returns 1 if cracked (password in `found`), 0 if exhausted. */
int brute_attack(const char *target, int maxlen, const char *charset,
                 hash_fn h, long *tries, char *found);

#endif /* CRACK_H */
