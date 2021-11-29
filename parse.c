#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELF_FILE "wget"

int main(int argc, char** argv) {
  FILE* fin;
  long size_b;
  
  // Open file
  printf("Opening file, %s\n", ELF_FILE);
  fin = fopen(ELF_FILE, "rb");
  if(fin==NULL) {
    printf("File, %s does not exist\n", ELF_FILE);
    return 1;
  }

  // Get byte size of the file
  fseek(fin, 0, SEEK_END);
  size_b = ftell(fin);
  fseek(fin, 0, SEEK_SET);
  printf("Size of the file: %ld bytes\n", size_b);

  // Allocate buff mem
  char buff[size_b];
  int count = fread(&buff, sizeof(char), size_b, fin);
  for(int i=546648; i<size_b; i++) {
    printf("%02X ", buff[i]);
    if(i!=0 && i%32==0) {
      printf("\n");
    }
  }

  return 0;
}