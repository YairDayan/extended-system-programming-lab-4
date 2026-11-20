/* Counts digit characters. No calls and no globals, so its bytes can move. */
int count_digits(char *s)
{
  int count;

  count = 0;
  while (*s != '\0') {
    if (*s >= '0' && *s <= '9')
      count++;
    s++;
  }
  return count;
}

/* A program needs main so it can be compiled. The bytes copied later are only count_digits. */
int main(void)
{
  return 0;
}
