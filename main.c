#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <regex.h>



typedef enum {
  EXIT,
} TypeKeyword;

typedef enum {
  INT,  
} TypeLiteral;

typedef enum {
  NEWLINE,
  OPEN_PARENTHESES,
  CLOSE_PARENTHESES,
} TypeSeparator;

typedef struct {
  TypeKeyword type;
} TokenKeyword;

typedef struct {
  TypeLiteral type;
  int value;
} TokenLiteral;

typedef struct {
  TypeSeparator type;
} TokenSeparator;



void tokenize(char current) {
  if (current == 0xA) { // newline
    printf("\n");
  } else if ( current == 0x20) { // space
    printf("\n");
  } else if ( current == 0x9) { // tabulator
    printf("\n");
  } else if ( current == 0x28) { // opening parentheses
    printf("\n(");
  } else if ( current == 0x29) { // closing parentheses
    printf("\n)");
  } else if ( current == 0x3A) { // colon
    printf("\n:");
  } else {
    printf("%c", current);
  }
}



void readfile(char *argv) {
  char *filename = argv;
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    printf("Error: Failed to open file \"%s\".\nFile either doesn't exist or can't be read.\n", filename);
  } else {
    char current = fgetc(file);

    while (current != EOF) {
      tokenize(current);
      current = fgetc(file);
    }

    fclose(file);
  }
}



int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Error: Incorrect usage, no input file given.\n> qsc <input.qs>\n");
    return 1;
  }

  readfile(argv[1]);

  return 0;
}
