/* hashcrack -- command-line interface.
 * Picks a hash function based on <algo>, then runs the requested attack. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "md5.h"
#include "sha256.h"
#include "crack.h"

#define CHARSET "abcdefghijklmnopqrstuvwxyz0123456789"

static void usage(void) {
    printf("hashcrack -- a pure-C hash cracker (MD5 / SHA-256)\n\n");
    printf("Usage:  hashcrack <algo> <mode> ...\n");
    printf("        <algo> = md5 | sha256      (ALWAYS comes first)\n\n");
    printf("Modes:\n");
    printf("  hashcrack <algo> hash  <text>             print the hash of <text>\n");
    printf("  hashcrack <algo> dict  <hash> <wordlist>  crack using a wordlist\n");
    printf("  hashcrack <algo> brute <hash> <maxlen>    crack by brute force (a-z0-9)\n\n");
    printf("Examples:\n");
    printf("  hashcrack md5    hash  letmein\n");
    printf("  hashcrack md5    dict  0d107d09f5bbe40cade3de5c71e9e9b7 wordlists/common.txt\n");
    printf("  hashcrack sha256 brute <hash> 4\n");
}

static void report(int hit, const char *found, long tries, double secs) {
    printf("\n+-----------------------------------------+\n");
    if (hit) printf("|  CRACKED!  password = \"%s\"\n", found);
    else     printf("|  not found.\n");
    printf("|  tries : %ld\n", tries);
    printf("|  time  : %.3f s\n", secs);
    if (secs > 0) printf("|  speed : %.0f hashes/sec\n", tries / secs);
    printf("+-----------------------------------------+\n");
}

int main(int argc, char **argv) {
    if (argc < 2) { usage(); return 1; }

    /* friendly hint: people often forget the algorithm and write the mode first */
    if (strcmp(argv[1], "hash") == 0 || strcmp(argv[1], "dict") == 0 ||
        strcmp(argv[1], "brute") == 0) {
        printf("The ALGORITHM must come first, then the mode.\n");
        printf("You put the mode ('%s') first. Try:\n\n", argv[1]);
        printf("    hashcrack md5 %s ...\n\n", argv[1]);
        return 1;
    }

    if (argc < 3) { usage(); return 1; }

    /* choose the hash function -- the only place the algorithm matters */
    hash_fn H;
    if      (strcmp(argv[1], "md5")    == 0) H = md5_hex;
    else if (strcmp(argv[1], "sha256") == 0) H = sha256_hex;
    else { printf("Unknown algorithm '%s'. Use 'md5' or 'sha256'.\n\n", argv[1]); usage(); return 1; }

    const char *mode = argv[2];

    if (strcmp(mode, "hash") == 0) {
        if (argc < 4) { printf("Missing text.  Usage: hashcrack %s hash <text>\n", argv[1]); return 1; }
        char hex[65];
        H(argv[3], hex);
        printf("%s(\"%s\") = %s\n", argv[1], argv[3], hex);
        return 0;
    }
    if (strcmp(mode, "dict") == 0) {
        if (argc < 5) { printf("Usage: hashcrack %s dict <hash> <wordlist>\n", argv[1]); return 1; }
        char found[256] = ""; long tries = 0;
        clock_t t0 = clock();
        int r = dict_attack(argv[3], argv[4], H, &tries, found);
        double secs = (double)(clock() - t0) / CLOCKS_PER_SEC;
        if (r == -2) { printf("Could not open wordlist '%s'.\n", argv[4]); return 1; }
        report(r == 1, found, tries, secs);
        return r == 1 ? 0 : 2;
    }
    if (strcmp(mode, "brute") == 0) {
        if (argc < 5) {
            printf("Missing max length.  Usage: hashcrack %s brute <hash> <maxlen>\n", argv[1]);
            printf("  e.g.  hashcrack %s brute %s 4\n", argv[1], (argc >= 4 ? argv[3] : "<hash>"));
            return 1;
        }
        int maxlen = atoi(argv[4]);
        if (maxlen < 1) { printf("maxlen must be a positive number (e.g. 4).\n"); return 1; }
        char found[256] = ""; long tries = 0;
        printf("brute forcing %s (charset a-z0-9, up to %d chars)...\n", argv[1], maxlen);
        clock_t t0 = clock();
        int r = brute_attack(argv[3], maxlen, CHARSET, H, &tries, found);
        double secs = (double)(clock() - t0) / CLOCKS_PER_SEC;
        report(r == 1, found, tries, secs);
        return r == 1 ? 0 : 2;
    }

    printf("Unknown mode '%s'. Use: hash | dict | brute\n\n", mode);
    usage();
    return 1;
}
