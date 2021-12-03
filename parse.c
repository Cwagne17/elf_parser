#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ELF_FILE "wget"
#define PARSE_OUT "parse_out.txt"

#define CALL_E8 0xe8

#define EI_NIDENT 16
struct Elf64_Ehdr {
  unsigned char e_ident[EI_NIDENT]; /* Magic number and other info */
  uint16_t      e_type;             /* Object file type */
  uint16_t      e_machine;          /* Architecture */
  uint32_t      e_version;          /* Object file version */
  uint64_t      e_entry;            /* Entry point virtual address */
  uint64_t      e_phoff;            /* Program header table file offset */
  uint64_t      e_shoff;            /* Section header table file offset */
  uint32_t      e_flags;            /* Processor-specific flags */
  uint16_t      e_ehsize;           /* ELF header size in bytes */
  uint16_t      e_phentsize;        /* Program header table entry size */
  uint16_t      e_phnum;            /* Program header table entry count */
  uint16_t      e_shentsize;        /* Section header table entry size */
  uint16_t      e_shnum;            /* Section header table entry count */
  uint16_t      e_shstrndx;         /* Section header string table index */
};

struct Elf64_Shdr {
    uint32_t   sh_name;
    uint32_t   sh_type;
    uint64_t   sh_flags;
    uint64_t   sh_addr;
    uint64_t   sh_offset;
    uint64_t   sh_size;
    uint32_t   sh_link;
    uint32_t   sh_info;
    uint64_t   sh_addralign;
    uint64_t   sh_entsize;
};

int main(int argc, char** argv) {
  FILE* fin; //ptr for infile
  FILE* fout; //ptr for outfile
  struct Elf64_Ehdr header; //first 64B of elf file
  long addr=0; //counter for fout

  // Open file
  printf("Opening file, %s\n", ELF_FILE);
  if((fin=fopen(ELF_FILE,"rb"))==NULL) {
    perror("Error");
    return -1;
  }

  long count = fread(&header, 1, sizeof(header), fin); //read elf hdr

  struct Elf64_Shdr shdrs[header.e_shnum]; //Stores section headers of elf file
  fseek(fin, header.e_shoff, SEEK_SET);
  count = fread(&shdrs, header.e_shnum, sizeof(struct Elf64_Ehdr), fin);
  struct Elf64_Shdr SH_TEXT = shdrs[16]; // get .text section

  // fseek(fin, shdrs[header.e_shstrndx].sh_addr, SEEK_SET);

  fseek(fin, SH_TEXT.sh_offset, SEEK_SET); //mov to begin of .text
  unsigned char buff[SH_TEXT.sh_size]; //alloc mem
  count = fread(&buff, sizeof(unsigned char), SH_TEXT.sh_size, fin); //read size of .text
  fclose(fin); //close file
  if(count<SH_TEXT.sh_size) { //error if count is not same as size of .text
    printf("Error: %llx bytes missing durring fread", (SH_TEXT.sh_size-count));
  }

  printf("Opening file, %s\n", PARSE_OUT);
  if((fout=fopen(PARSE_OUT, "wb"))==NULL) {
    perror("Error");
    return -1;
  }

  // iter over .text bytes for OPcodes
  fprintf(fout, "ADDR  OP Operands\n--------------------\n");
  addr=SH_TEXT.sh_offset;
  printf("Finding %02X opcodes\n", CALL_E8);
  for(long i=0; i<SH_TEXT.sh_size-4; i++) {
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