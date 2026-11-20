#include <stdio.h>

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

/* Runs count_digits on a few strings and prints the counts. Only count_digits is copied later. */
int main(void)
{
  printf("%d\n", count_digits("aabbaba123baacca"));
  printf("%d\n", count_digits("1112111"));
  printf("%d\n", count_digits("0123456789"));
  return 0;
}
