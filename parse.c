#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELF_FILE "wget"
#define PARSE_OUT "parse_out.txt"

#define TEXT_START 0xf1b0
#define TEXT_SIZE 0x4e415
#define TEXT_END 0x5D5C5

#define CALL_E8 0xe8
#define CALL_FF15 0xff15
#define CALL_FF25 0xff25

int main(int argc, char** argv) {
  FILE* fin;
  FILE* fout;
  unsigned char buff[TEXT_SIZE]; //alloc mem
  long addr=0;

  // Open file
  printf("Opening file, %s\n", ELF_FILE);
  if((fin=fopen(ELF_FILE,"rb"))==NULL) {
    perror("Error");
    return -1;
  }

  fseek(fin, TEXT_START, SEEK_SET); //mov to begin of .text
  long count = fread(&buff, sizeof(unsigned char), TEXT_SIZE, fin); //read size of .text
  fclose(fin); //close file
  if(count<TEXT_SIZE) { //error if count is not same as size of .text
    printf("Error: %ld bytes missing durring fread", (TEXT_SIZE-count));
  }

  if((fout=fopen(PARSE_OUT, "wb"))==NULL) {
    perror("Error");
    return -1;
  }

  // iter over .text bytes for OPcodes
  fprintf(fout, "ADDR  OP  Operands\n--------------------");
  addr=TEXT_START;
  for(long i=0; i<TEXT_SIZE-4; i++) {
    if (buff[i]==CALL_E8) {
      fprintf(
        fout, 
        "%lx: %02X %02X %02X %02X %02X\n", 
        addr, buff[i], buff[i+1], buff[i+2], buff[i+3], buff[i+4]
      );
      i+=4; addr+=4;
    }
    addr++;
  }
  fclose(fout);

  return 0;
}