/****************************
 * Andrew Guinn
 * Project 1
 * instr.h
 ****************************/
#ifndef __INSTR_H__
#define __INSTR_H__

#include <string.h>
#include <inttypes.h>

#include "prefix.h"
#include "op.h"
#include "modrm.h"
#include "sib.h"
#include "register.h"

#define INSTR_BOOL_UNSURE 1
#define INSTR_BOOL_YES 0
#define INSTR_BOOL_NO -1

typedef struct instr_t {
  uint32_t addr;
  prefix_t prefix;
  op_t op;
  mod_t mode;
  reg_t reg;
  reg_t rm;
  int scale;
  reg_t index;
  reg_t base;
  union {
    uint8_t disp_8;
    uint32_t disp_32;
  };
  union {
    uint8_t imm_8;
    uint16_t imm_16;
    uint32_t imm_32;
  };
  char *label;
} instr_t;

int instr_parse_instr(unsigned char *buf, int len, instr_t *instr);

char *instr_reg_to_str(instr_t reg);

#endif //__INSTR_H__
