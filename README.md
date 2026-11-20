# Lab 4 — ELF

Extended System Programming Laboratory.

32-bit ELF: find fields in the file, then read or patch the bytes.
The program we build for that is `hexeditplus`.

## units

Example from the lab files. `units.c` reads 4 units from `input`,
prints them, and writes the same bytes to `output`.

```bash
cd "Lab 4 Auxiliary Files-20261119/units"
make
./units      # int, 4 bytes
./units s    # short, 2 bytes
./units b    # byte
```

`make` runs `gcc -Wall -g -m32`, so this needs a 32-bit toolchain.
`i`, `s`, and `b` are the only arguments. Anything else is an error.
