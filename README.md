# Lab 4 — ELF

Extended System Programming Laboratory.

32-bit ELF: find fields in the file, then read or patch the bytes.

## Build

Needs a 32-bit toolchain (`gcc -m32`).

```bash
make
./hexeditplus
./digits
make clean
```

`make` builds both programs. `hexeditplus` comes from `task1.c`.
`digits` comes from `task4.c`, with `-fno-pie -fno-stack-protector -O0`.

## Tasks

`task1.c` is Task 0b and Task 1.
`task4.c` is Task 4.

## hexeditplus

Menu of actions on a file and on a memory buffer.
Unit size is 1, 2, or 4 bytes. The default is 1.

```
Choose action:
0-Toggle Debug Mode
1-Set File Name
2-Set Unit Size
3-Load Into Memory
4-Toggle Display Mode
5-Memory Display
6-Save Into File
7-Memory Modify
8-Quit
> 
```

Debug mode prints `unit_size`, `file_name`, and `mem_count` to stderr before the menu.

- Option 3 reads a hex file offset and a decimal length, then copies that many units into `mem_buf`.
- Option 4 switches printing between hexadecimal and decimal.
- Option 5 prints units from an address. The address is hexadecimal and the length is decimal. Address 0 is the start of `mem_buf`.
- Option 6 writes units from a source address to a target offset in the file. The source address and the target offset are hexadecimal, and the length is decimal. Source address 0 is the start of `mem_buf`. The rest of the file stays. A target past the end of the file is an error.
- Option 7 replaces one unit in `mem_buf`. Location and value are both hex.

## digits

`count_digits` counts digit characters in a string.
`main` prints the count for `aabbaba123baacca`, `1112111`, and `0123456789`.

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

`i`, `s`, and `b` are the only arguments. Anything else is an error.
