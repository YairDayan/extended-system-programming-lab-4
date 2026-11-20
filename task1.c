#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char debug_mode = 0;
char file_name[128] = "";
int unit_size = 1;
unsigned char mem_buf[10000];
size_t mem_count = 0;

void toggle_debug_mode(void);
void set_file_name(void);
void set_unit_size(void);
void load_into_memory(void);
void toggle_display_mode(void);
void memory_display(void);
void save_into_file(void);
void memory_modify(void);
void quit(void);

struct fun_desc {
  char *name;
  void (*fun)(void);
};

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

void set_file_name(void)
{
  char input[128];

  printf("Please enter file name\n");
  if (fgets(input, sizeof(input), stdin) == NULL)
    return;
  input[strcspn(input, "\n")] = '\0';
  strncpy(file_name, input, 100);
  file_name[100] = '\0';
  if (debug_mode)
    fprintf(stderr, "Debug: file name set to '%s'\n", file_name);
}

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

void load_into_memory(void)
{
  printf("Not implemented yet\n");
}
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
void quit(void)
{
  if (debug_mode)
    fprintf(stderr, "quitting\n");
  exit(0);
}

int main(void)
{
  char input[128];
  int choice;
  int bound;

  bound = 0;
  while (menu[bound].name != NULL)
    bound++;

  while (1) {
    if (debug_mode) {
      fprintf(stderr, "unit_size: %d\n", unit_size);
      fprintf(stderr, "file_name: %s\n", file_name);
      fprintf(stderr, "mem_count: %zu\n", mem_count);
    }
    printf("Choose action:\n");
    for (int i = 0; i < bound; i++)
      printf("%d-%s\n", i, menu[i].name);
    printf("> ");
    if (fgets(input, sizeof(input), stdin) == NULL)
      break;
    if (sscanf(input, "%d", &choice) != 1 || choice < 0 || choice >= bound) {
      printf("Invalide choice\n");
      continue;
    }
    menu[choice].fun();
  }
  return 0;
}
