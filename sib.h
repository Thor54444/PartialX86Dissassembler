/****************************
 * Andrew Guinn
 * Project 1
 * sib.h
 ****************************/
#ifndef __SIB_H__
#define __SIB_H__

#include <stdbool.h>

#include "register.h"
#include "modrm.h"
#include "op.h"

int sib_get_scale(unsigned char uchar);

reg_t sib_get_index(unsigned char uchar); 

reg_t sib_get_base(unsigned char uchar);

bool sib_need_sib(op_t op, mod_t mod, reg_t reg);

#endif //__SIB_H__
