#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Shared state. Menu functions take no arguments, so they all use these. */
char debug_mode = 0;          /* 0 = off, 1 = on. Debug text goes to stderr. */
char file_name[128] = "";     /* Target file. Empty until the user sets it. */
int unit_size = 1;            /* Bytes in one unit: 1, 2, or 4. */
unsigned char mem_buf[10000]; /* Raw bytes loaded from the file. */
size_t mem_count = 0;         /* How many bytes in mem_buf are valid. */

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
  char input[1028];
  int size;

  printf("Please enter a number\n");
  if (fgets(input, sizeof(input), stdin) == NULL)
    return;
  if (sscanf(input, "%d", &size) != 1 || (size != 1 && size != 2 && size != 4)) {
    printf("Invalide unit size\n");
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
  bytes = (size_t)length * (size_t)unit_size;
  if (bytes > 10000) { /* mem_buf holds at most 10000 bytes. */
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

/* Placeholders. Each later task replaces one of these. */
void toggle_display_mode(void)
{
  printf("Not implemented yet\n");
}
void memory_display(void)
{
  printf("Not implemented yet\n");
}
void save_into_file(void)
{
  printf("Not implemented yet\n");
}
void memory_modify(void)
{
  printf("Not implemented yet\n");
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
      printf("Invalide choice\n");
      continue; /* Only Quit leaves the loop through exit. */
    }
    menu[choice].fun(); /* Dispatch through the pointer in the table. */
  }
  return 0;
}
