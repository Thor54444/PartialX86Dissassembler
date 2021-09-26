/****************************
 * Andrew Guinn
 * Project 1
 * modrm.h
 ****************************/
#ifndef __MODRM_H__
#define __MODRM_H__

#include <stdbool.h>

#include "register.h"

typedef enum mod_t {
  mem_access,
  byte_disp,
  dword_disp,
  direct,
  err_mod
} mod_t;

mod_t modrm_get_mod(unsigned char uchar);

reg_t modrm_get_reg(unsigned char uchar); 

reg_t modrm_get_rm(unsigned char uchar);

bool modrm_is_special_disp_mod(mod_t mod, reg_t rm);

bool modrm_use_disp(mod_t mod, reg_t rm);

int modrm_get_disp_size(mod_t mod, reg_t rm);

#endif //__MODRM_H__
