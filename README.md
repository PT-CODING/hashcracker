# hashcrack

A small **hash cracker written in pure C** — no external libraries. It recovers
the text behind an MD5 or SHA-256 hash using a **dictionary** or **brute-force**
attack, and reports how many hashes per second it churns through.

Built as a learning project to show why C is the right tool for this kind of
work: cracking is a raw-speed game, and compiled C hashes **millions** of
candidates per second.

> ⚠️ **For education and CTFs only.** Use it on hashes you created yourself or
> are authorized to test. Both MD5 and SHA-256 are implemented from scratch,
> from their specifications (RFC 1321 / FIPS 180-4).

## Features

- 🔓 **MD5** and **SHA-256**, both implemented from scratch (no OpenSSL, no deps)
- 📖 **Dictionary attack** from a wordlist file
- 💣 **Brute-force attack** over `a-z0-9` up to a chosen length
- ⏱️ Live **speed report** (hashes/second)
- 🧩 Clean modular layout — one generic engine, swappable hash function

## Build

With `make`:

```sh
make
```

Or directly with gcc:

```sh
gcc -O2 -Wall src/main.c src/md5.c src/sha256.c src/crack.c -o hashcrack
```

## Usage

```
hashcrack <algo> hash  <text>             print the hash of <text>
hashcrack <algo> dict  <hash> <wordlist>  dictionary attack
hashcrack <algo> brute <hash> <maxlen>    brute force (a-z0-9)

<algo> = md5 | sha256
```

### Examples

Hash a word:

```sh
$ hashcrack md5 hash banana
md5("banana") = 72b302bf297a228a75730123efef7c41
```

Dictionary attack (against the bundled 10,000-password list):

```sh
$ hashcrack md5 dict 0d107d09f5bbe40cade3de5c71e9e9b7 wordlists/common.txt

+-----------------------------------------+
|  CRACKED!  password = "letmein"
|  tries : 11
|  time  : 0.000 s
+-----------------------------------------+
```

Brute force:

```sh
$ hashcrack md5 brute 098f6bcd4621d373cade4e832627b4f6 4

+-----------------------------------------+
|  CRACKED!  password = "test"
|  tries : 940304
|  time  : 0.80 s
|  speed : ~1,170,000 hashes/sec
+-----------------------------------------+
```

## Wordlists

`wordlists/common.txt` ships with **10,000 real, most-common passwords**
(from the [SecLists](https://github.com/danielmiessler/SecLists) project).

For serious cracking, point the tool at a bigger list such as the famous
**rockyou.txt** (~14 million leaked passwords). It ships with Kali Linux at
`/usr/share/wordlists/rockyou.txt.gz`, or download it from SecLists, then:

```sh
hashcrack md5 dict <hash> path/to/rockyou.txt
```

## How it works

A hash is **one-way**: you can compute `md5("banana")` easily, but you can't run
it backwards to get `"banana"` from the digest. So cracking isn't "decrypting" —
it's **guess & check**:

```
for each candidate:
    if hash(candidate) == target:  ->  found it
```

- **Dictionary** — candidates come from a wordlist (fast; only finds passwords in
  the list).
- **Brute force** — candidates are generated automatically, every combination in
  order (finds anything eventually, but the count explodes with length).

## Why long passwords win

Each extra character multiplies the brute-force space by the charset size.
At ~1.2M hashes/sec over `a-z0-9`:

| length | combinations | time to crack |
|:------:|-------------:|:-------------:|
| 4      | 1.7 million  | ~1.4 seconds  |
| 6      | 2.1 billion  | ~30 minutes   |
| 8      | 2.8 trillion | ~27 days      |
| 10     | 3.6 quadrillion | ~96 years  |

Add uppercase and symbols and it grows far faster. This is exactly why real
systems store **salted**, deliberately **slow** hashes (bcrypt, Argon2) — to
drop that millions/sec down to a crawl.

## Project structure

```
hashcrack/
├── src/
│   ├── main.c      CLI
│   ├── md5.c/.h    MD5 implementation
│   ├── sha256.c/.h SHA-256 implementation
│   └── crack.c/.h  generic dictionary + brute-force engine
├── wordlists/
│   └── common.txt  sample wordlist
├── Makefile
└── LICENSE
```

The cracking engine in `crack.c` takes a **function pointer** to a hash function,
so it never needs to know which algorithm it's attacking — the same code cracks
MD5 and SHA-256 alike.

## License

MIT — see [LICENSE](LICENSE).
