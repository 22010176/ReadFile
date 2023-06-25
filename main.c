
// C program for the above approach
#include <stdio.h>
typedef int (*ptr)(int);
typedef ptr(*pm)(int);


// Function that return type ptr
ptr fun(int z) {
  printf("Hello ");
  int fun1(int y) {
    printf("geeks!!\n");
    return y + z;
  }

  return fun1;
}

// Driver Code
int main()
{
  int a = 10, b = 10;

  int* (*u) = (*fun)(a);

  printf("%d", u(b));

  return 0;
}