#include <stdio.h>

int main(void)
{
  printf("%d\n", ((FILE*)stdin)->_bf._size);
  printf("%d\n", ((FILE*)stdout)->_bf._size);
  return 0;
}
