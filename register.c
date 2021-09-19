/****************************
 * Andrew Guinn
 * Project 1
 * Register.c
 ****************************/
#include <string.h>
#include <stdlib.h>

#include "register.h"

#define REG_EAX_STR "eax"
#define REG_ECX_STR "ecx"
#define REG_EDX_STR "edx"
#define REG_EBX_STR "ebx"
#define REG_ESP_STR "esp"
#define REG_EBP_STR "ebp"
#define REG_ESI_STR "esi"
#define REG_EDI_STR "edi"
#define REG_ERR_STR "err"
#define REG_STR_SIZE 3

reg_t uchar_to_reg(unsigned char uchar) {
  switch(uchar) {
  case 0:
    return eax;
  case 1:
    return ecx;
  case 2:
    return edx;
  case 3:
    return ebx;
  case 4:
    return esp;
  case 5:
    return ebp;
  case 6:
    return esi;
  case 7:
    return edi;
  default:
    return err;
  }
}

reg_t uchar_sect_to_reg(unsigned char uchar, int start_bit) {
  unsigned char mask;

  switch(start_bit) {
  case 0:
    mask = 0x07;
    break;
  case 1:
    mask = 0x0E;
    break;
  case 2:
    mask = 0x1C;
    break;
  case 3:
    mask = 0x38;
    break;
  case 4:
    mask = 0x70;
    break;
  case 5:
    mask = 0xE0;
    break;
  default:
    return err;
  }

  return uchar_to_reg((uchar & mask) >> start_bit);
}

char *reg_to_str(reg_t reg) {
  char *buf = calloc(REG_STR_SIZE + 1, 1), *str;

  if (buf == NULL) {
    goto end;
  }

  switch(reg) {
  case eax:
    str = REG_EAX_STR;
    break;
  case ecx:
    str = REG_ECX_STR;
    break;
  case edx:
    str = REG_EDX_STR;
    break;
  case ebx:
    str = REG_EBX_STR;
    break;
  case esp:
    str = REG_ESP_STR;
    break;
  case ebp:
    str = REG_EBP_STR;
    break;
  case esi:
    str = REG_ESI_STR;
    break;
  case edi:
    str = REG_EDI_STR;
    break;
  case err:
    str = REG_ERR_STR;
    break;
  }

  memcpy(buf, str, REG_STR_SIZE);

 end:
  return buf;
}

/*#include <stdio.h>

int main() {
  unsigned char chr = 0x03;
  reg_t reg = uchar_to_reg(chr);
  char *tmp;

  if (reg != ebx) {
    printf("Bad conversion\n");
  }

  tmp = reg_to_str(reg);

  printf("Reg str: %s\n", tmp);

  free(tmp);

  reg = uchar_sect_to_reg(chr, 0);

  if (reg != ebx) {
    printf("Bad conversion\n");
  }

  chr = 0xE0;
  reg = uchar_sect_to_reg(chr, 5);

  if (reg != edi) {
    printf("Bad conversion\n");
  }

  return 0;
  }*/
