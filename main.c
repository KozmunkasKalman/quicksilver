#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>



void tokenize(char* content) {
  if (content == NULL) {
    printf("Error: File empty or failed to read.");
    assert(1);
  }

  // PLACEHOLDER - ai code, very bad
  // TODO: write own code for this shit
  for (int i = 0; content[i] != '\0'; i++) {
    if (content[i] == ' ') {
      content[i] = '\n';
    }
  }

  // TODO: actually tokenize it

  printf("%s", content);
}



void readfile(char *argv) {
  char *filename = argv;
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    printf("Error: Failed to open file \"%s\".\nFile either doesn't exist or can't be read.\n", filename);
    fclose(file);
  } else {
    fseek(file, 0, SEEK_END);
    long n = ftell(file);
    rewind(file);
    char *content = malloc(n + 1);
    fread(content, 1, n, file);
    content[n] = '\0';
    tokenize(content);
    fclose(file);
  }
}



int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Error: Incorrect usage, no input file given.\n> qsc <input.qsr>\n");
    return 1;
  }

  readfile(argv[1]);

  return 0;
}
