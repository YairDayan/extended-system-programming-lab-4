#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char debug_mode = 0;
char file_name[128] = "";
int unit_size = 1;
unsigned char mem_buf[10000];
size_t mem_count = 0;
