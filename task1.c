#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Shared state. Menu functions take no arguments, so they all use these. */
char debug_mode = 0;          /* 0 = off, 1 = on. Debug text goes to stderr. */
char file_name[128] = "";     /* Target file. Empty until the user sets it. */
int unit_size = 1;            /* Bytes in one unit: 1, 2, or 4. */
unsigned char mem_buf[10000]; /* Raw bytes loaded from the file. */
size_t mem_count = 0;         /* How many bytes in mem_buf are valid. */
char display_mode = 0;        /* 0 = hexadecimal, 1 = decimal. */

/* Named here so the menu table below can store their addresses. */
void toggle_debug_mode(void);
void set_file_name(void);
void set_unit_size(void);
void load_into_memory(void);
void toggle_display_mode(void);
void memory_display(void);
void save_into_file(void);
void memory_modify(void);
void quit(void);

/* One menu row: the label shown to the user, and the function it runs. */
struct fun_desc {
  char *name;
  void (*fun)(void);
};

/* The last row is the sentinel. main stops when name is NULL. */
struct fun_desc menu[] = {{"Toggle Debug Mode", toggle_debug_mode},
                          {"Set File Name", set_file_name},
                          {"Set Unit Size", set_unit_size},
                          {"Load Into Memory", load_into_memory},
                          {"Toggle Display Mode", toggle_display_mode},
                          {"Memory Display", memory_display},
                          {"Save Into File", save_into_file},
                          {"Memory Modify", memory_modify},
                          {"Quit", quit},
                          {NULL, NULL}};

/* Flip the debug flag and report the new state on stderr. */
void toggle_debug_mode(void)
{
  if (debug_mode) {
    fprintf(stderr, "Debug flag now off\n");
    debug_mode = 0;
  } else {
    debug_mode = 1;
    fprintf(stderr, "Debug flag now on\n");
  }
}

/* Store a file name. The file is not opened here. */
void set_file_name(void)
{
  char input[128];

  printf("Please enter file name\n");
  if (fgets(input, sizeof(input), stdin) == NULL)
    return;
  input[strcspn(input, "\n")] = '\0'; /* fgets keeps the newline. */
  strncpy(file_name, input, 100);     /* A name is at most 100 characters. */
  file_name[100] = '\0';              /* strncpy writes no NUL when it fills the limit. */
  if (debug_mode)
    fprintf(stderr, "Debug: file name set to '%s'\n", file_name);
}

/* Set the unit size. Only 1, 2, and 4 are valid; other input is ignored. */
void set_unit_size(void)
{
  char input[128];
  int size;

  printf("Please enter a number\n");
  if (fgets(input, sizeof(input), stdin) == NULL)
    return;
  if (sscanf(input, "%d", &size) != 1 || (size != 1 && size != 2 && size != 4)) {
    printf("Invalid unit size\n");
    return;
  }
  unit_size = size;
  if (debug_mode) {
    fprintf(stderr, "Debug: set size to %d\n", unit_size);
  }
}

/* Copy length units from file_name, starting at a hex file offset, into mem_buf. */
void load_into_memory(void)
{
  char input[128];
  unsigned int location;
  int length;
  size_t bytes;
  FILE *file;

  if (file_name[0] == '\0') {
    printf("No file name set\n");
    return;
  }
  file = fopen(file_name, "rb"); /* Binary read: keep every byte as it is. */
  if (file == NULL) {
    printf("Failed to open file\n");
    return;
  }
  printf("Please enter <location> <length>\n");
  if (fgets(input, sizeof(input), stdin) == NULL) {
    fclose(file);
    return;
  }
  /* First number is a hex offset. Second number is a decimal unit count. */
  if (sscanf(input, "%x %d", &location, &length) != 2 || length <= 0) {
    printf("Invalid location or length\n");
    fclose(file);
    return;
  }
  bytes = (size_t)length * (size_t)unit_size; /* Unit count to a byte count. */
  if (bytes > 10000) {                        /* mem_buf holds at most 10000 bytes. */
    printf("Requested size is too large\n");
    fclose(file);
    return;
  }
  if (debug_mode) {
    fprintf(stderr, "Debug: file name: %s, location: %#x, length: %d\n", file_name, location,
            length);
  }
  /* fread counts units, as read_units_to_memory does in units.c. */
  if (fseek(file, (long)location, SEEK_SET) != 0 ||
      fread(mem_buf, unit_size, length, file) != (size_t)length) {
    printf("Failed to read file\n");
    fclose(file);
    return;
  }
  fclose(file); /* Opened in this function, so closed here on every path. */
  mem_count = bytes;
  printf("Loaded %d units into memory\n", length);
}

/* Flip the display flag. The bytes in memory are not printed here. */
void toggle_display_mode(void)
{
  if (display_mode) {
    printf("Decimal display flag now off, hexadecimal representation\n");
    display_mode = 0;
  } else {
    display_mode = 1;
    printf("Decimal display flag now on, decimal representation\n");
  }
}

/* Print length numbers from memory. Address 0 means the start of mem_buf. */
void memory_display(void)
{
  /* Index is unit_size - 1. Slot 2 is unused because a unit is never 3 bytes. */
  static char *hex_formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
  static char *dec_formats[] = {"%#hhd\n", "%#hd\n", "No such unit", "%#d\n"};
  char input[128];
  unsigned int addr;
  int length;
  unsigned char *start;
  char **formats;

  printf("Enter address and length\n");
  if (fgets(input, sizeof(input), stdin) == NULL)
    return;
  /* Address is hex. Length is a decimal count of units. */
  if (sscanf(input, "%x %d", &addr, &length) != 2 || length <= 0) {
    printf("Invalid address or length\n");
    return;
  }
  if (debug_mode)
    fprintf(stderr, "Debug: address: %#x, length: %d\n", addr, length);
  if (addr == 0)
    start = mem_buf; /* The copy loaded from the file. */
  else
    start = (unsigned char *)addr; /* A 32-bit lab address. int and pointer match there. */
  if (addr == 0 && (size_t)length * (size_t)unit_size > 10000) {
    printf("Requested size is too large\n");
    return;
  }
  if (display_mode) {
    printf("Decimal\n=======\n"); /* Underline matches the sample heading. */
    formats = dec_formats;
  } else {
    printf("Hexadecimal\n===========\n");
    formats = hex_formats;
  }
  for (int i = 0; i < length; i++) {
    int var = *((int *)start);           /* Always read 4 bytes, as print_units does. */
    printf(formats[unit_size - 1], var); /* The format keeps only this unit. */
    start += unit_size;
  }
}

/* Write length units from memory onto a file offset. The rest of the file stays. */
void save_into_file(void)
{
  char input[128];
  unsigned int source;
  unsigned int target;
  int length;
  unsigned char *start;
  long file_size;
  FILE *file;

  if (file_name[0] == '\0') {
    printf("No file name set\n");
    return;
  }
  printf("Please enter <source-address> <target-location> <length>\n");
  if (fgets(input, sizeof(input), stdin) == NULL)
    return;
  /* Two hex numbers, then a decimal unit count. */
  if (sscanf(input, "%x %x %d", &source, &target, &length) != 3 || length <= 0) {
    printf("Invalid source, target, or length\n");
    return;
  }
  if (debug_mode) {
    fprintf(stderr, "Debug: file name: %s, source: %#x, target: %#x, length: %d\n", file_name,
            source, target, length);
  }
  file = fopen(file_name, "r+b"); /* Update in place. Do not erase the file. */
  if (file == NULL) {
    printf("Failed to open file\n");
    return;
  }
  if (fseek(file, 0, SEEK_END) != 0) {
    printf("Failed to read file size\n");
    fclose(file);
    return;
  }
  file_size = ftell(file); /* Byte offset of the end, which is the file size. */
  if (file_size < 0 || target > (unsigned int)file_size) {
    printf("Target location is past the end of the file\n");
    fclose(file);
    return;
  }
  if (source == 0)
    start = mem_buf; /* The copy loaded from the file. */
  else
    start = (unsigned char *)source; /* A 32-bit lab address. int and pointer match there. */
  if (source == 0 && (size_t)length * (size_t)unit_size > 10000) {
    printf("Requested size is too large\n");
    fclose(file);
    return;
  }
  /* fwrite counts units, as write_units does in units.c. */
  if (fseek(file, (long)target, SEEK_SET) != 0 ||
      fwrite(start, unit_size, length, file) != (size_t)length) {
    printf("Failed to write file\n");
    fclose(file);
    return;
  }
  fclose(file); /* Opened in this function, so closed here on every path. */
}

/* Replace one unit inside mem_buf. location is an offset in the buffer. */
void memory_modify(void)
{
  char input[128];
  unsigned int location;
  unsigned int val;

  printf("Please enter <location> <val>\n");
  if (fgets(input, sizeof(input), stdin) == NULL)
    return;
  /* Both numbers are hexadecimal. */
  if (sscanf(input, "%x %x", &location, &val) != 2) {
    printf("Invalid location or value\n");
    return;
  }
  if (debug_mode)
    fprintf(stderr, "Debug: location: %#x, val: %#x\n", location, val);
  /* The unit must fit inside the 10000-byte buffer. */
  if (location > 10000 || (unsigned int)unit_size > 10000 - location) {
    printf("Location is out of range\n");
    return;
  }
  /* Low byte is stored first. Copy only unit_size bytes of val. */
  memcpy(mem_buf + location, &val, unit_size);
}
/* Exit. In debug mode, report that on stderr before leaving. */
void quit(void)
{
  if (debug_mode)
    fprintf(stderr, "quitting\n");
  exit(0);
}

/* Show the menu, read a number, and call the function stored for that number. */
int main(void)
{
  char input[128];
  int choice;
  int bound;

  bound = 0;
  while (menu[bound].name != NULL) /* Count rows up to the sentinel. */
    bound++;

  while (1) {
    if (debug_mode) { /* Parsed state, on stderr, before every menu. */
      fprintf(stderr, "unit_size: %d\n", unit_size);
      fprintf(stderr, "file_name: %s\n", file_name);
      fprintf(stderr, "mem_count: %zu\n", mem_count);
    }
    printf("Choose action:\n");
    for (int i = 0; i < bound; i++)
      printf("%d-%s\n", i, menu[i].name);
    printf("> ");
    if (fgets(input, sizeof(input), stdin) == NULL)
      break; /* No more input. */
    if (sscanf(input, "%d", &choice) != 1 || choice < 0 || choice >= bound) {
      printf("Invalid choice\n");
      continue; /* Only Quit leaves the loop through exit. */
    }
    menu[choice].fun(); /* Dispatch through the pointer in the table. */
  }
  return 0;
}
