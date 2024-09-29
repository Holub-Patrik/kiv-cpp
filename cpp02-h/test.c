#include <stdio.h>

int main() {
  printf("Starting ...\n");

  FILE* file = fopen("file1.txt", "w");
  fprintf(file, "%s", "Hello Wolrd\n");
  fclose(file);

  printf("Ending\n");
}
