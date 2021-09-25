/****************************
 * Andrew Guinn
 * Project 1
 * modrm.c
 ****************************/
#include "modrm.h"

#define MOD_MASK 0xC0

#define MOD_SHIFT 6
#define REG_SHIFT 3
#define RM_SHIFT 0

mod_t modrm_get_mod(unsigned char uchar) {
  uchar = (uchar & MOD_MASK) >> MOD_SHIFT;

  switch(uchar) {
  case 0:
    return mem_access;
  case 1:
    return byte_disp;
  case 2:
    return dword_disp;
  case 3:
    return direct;
  default:
    return err_mod;
  }
}

bool modrm_use_disp(mod_t mod) {
  return mod == byte_disp || mod == dword_disp;
}

reg_t modrm_get_reg(unsigned char uchar) {
  return register_uchar_sect_to_reg(uchar, REG_SHIFT);
}

reg_t modrm_get_rm(unsigned char uchar) {
  return register_uchar_sect_to_reg(uchar, RM_SHIFT);
}

bool modrm_is_disp_mod(unsigned char uchar) {
  return modrm_get_mod(uchar) == mem_access && modrm_get_rm(uchar) == ebp;
}

/*#include <stdio.h>
#include <stdlib.h>

int main() {
  bool special;
  unsigned char uchar = 0xC2;
  reg_t reg, rm;
  mod_t mod;
  char *reg_str, *rm_str;

  mod = modrm_get_mod(uchar);
  
  reg = modrm_get_reg(uchar);
  rm = modrm_get_rm(uchar);

  reg_str = register_reg_to_str(reg);
  rm_str = register_reg_to_str(rm);

  special = modrm_is_disp_mod(uchar);

  printf("Mode: %d Reg: %s RM: %s Special: %d\n", mod, reg_str,
	 rm_str, special);

  free(reg_str);
  free(rm_str);

  return 0;
  }*/
