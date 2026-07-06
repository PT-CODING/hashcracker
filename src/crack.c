/* The cracking engine.  Notice it never mentions MD5 or SHA-256 by name --
 * it just calls whatever hash function `h` it was handed.  This is the
 * "engine vs fuel" idea: one engine, swappable hash. */
#include "crack.h"
#include <stdio.h>
#include <string.h>

int dict_attack(const char *target, const char *wordlist,
                hash_fn h, long *tries, char *found) {
    FILE *f = fopen(wordlist, "r");
    if (!f) return -2;
    char line[256], hex[65];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;      /* strip trailing newline */
        h(line, hex);
        (*tries)++;
        if (strcmp(hex, target) == 0) {
            strcpy(found, line);
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int brute_attack(const char *target, int maxlen, const char *charset,
                 hash_fn h, long *tries, char *found) {
    int clen = (int)strlen(charset);
    char guess[32], hex[65];
    for (int len = 1; len <= maxlen; len++) {
        int idx[32];
        for (int i = 0; i < len; i++) idx[i] = 0;
        for (;;) {
            for (int i = 0; i < len; i++) guess[i] = charset[idx[i]];
            guess[len] = 0;
            h(guess, hex);
            (*tries)++;
            if (strcmp(hex, target) == 0) { strcpy(found, guess); return 1; }

            int pos = len - 1;                 /* odometer: ++ from the right */
            while (pos >= 0) { if (++idx[pos] < clen) break; idx[pos] = 0; pos--; }
            if (pos < 0) break;                /* every combo of this length tried */
        }
    }
    return 0;
}
