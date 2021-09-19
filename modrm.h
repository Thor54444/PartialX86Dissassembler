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

bool modrm_is_disp_mod(unsigned char uchar);

#endif //__MODRM_H__
