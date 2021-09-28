/****************************
 * Andrew Guinn
 * Project 1
 * op.h
 ****************************/
#ifndef __OP_H__
#define __OP_H__

#include "register.h"

#define OP_BOOL_MAYBE 1
#define OP_BOOL_YES 0
#define OP_BOOL_NO -1

#define OP_ARG_POL_ONLY 0
#define OP_ARG_POL_LEFT 1
#define OP_ARG_POL_RIGHT 2
#define OP_ARG_POL_UNSURE 3
#define OP_ARG_POL_NONE 4

typedef enum op_t {
  /* Obviously, each opcode isn't technically
   * duplicated like this, but this helps have
   * a record of which specific operation is
   * being performed once enough info is collected
   */
  addd,
  addi,
  addm,
  addr,
  andd,
  andi,
  andm,
  andr,
  callr,
  callm,
  clflush,
  cmpd,
  cmpi,
  cmpm,
  cmpr,
  decm,
  decr,
  idiv,
  imulm,
  imulr,
  imuli,
  incm,
  incr,
  jmp8,
  jmpr,
  jmpm,
  jz8,
  jz32,
  jnz8,
  jnz32,
  lea,
  movd,
  movi,
  movm,
  movr,
  movsd,
  mul,
  neg,
  nop,
  not,
  ord,
  ori,
  orm,
  orr,
  out,
  popm,
  popr,
  pushm,
  pushr,
  pushi,
  cmpsd,
  retf,
  retfi,
  retn,
  retni,
  sal,
  sar,
  shr,
  sbbd,
  sbbi,
  sbbm,
  sbbr,
  subd,
  subi,
  subm,
  subr,
  testd,
  testi,
  testm,
  xord,
  xori,
  xorm,
  xorr,
  /*These obviously are not true opcodes, this just represents the ambiguity with
   * parsing some opcodes in isolation */
  clflush_u, //clflush technically has an aditional value in the modrm required to confirm it
  pop_u,
  mov_u,
  inc_dec_call_jmp_push, //0xFF
  idiv_imul_mul_neg_not_test, //0xF7
  add_and_cmp_or_sbb_sub_xor, //0x81
  sal_sar_shr, //0xD1
  clflush_imul_jz_jnz, //0x0F
  err_op
} op_t;

op_t op_uchar_to_op(unsigned char uchar);

op_t op_uchar_uchar_to_op(unsigned char op1, unsigned char op2);

op_t op_op_modrm_to_op(op_t op, reg_t modrm_reg_field);

op_t op_uchar_modrm_to_op(unsigned char op, reg_t modrm_reg_field);

op_t op_uchar_uchar_modrm_to_op(unsigned char op1, unsigned char op2, reg_t modrm_reg_field);

char *op_to_str(op_t op);

int op_need_second_byte(op_t op);

/* Instructions that need the modrm byte to be fully identified */
int op_need_modrm(op_t op);

/* Instructions that have a modrm byte */
int op_has_modrm(op_t op);

int op_has_immediate(op_t op);

int op_get_immediate_size(op_t op);

int op_has_displacement(op_t op);

int op_get_displacement_size(op_t op);

int op_op_has_reg(op_t op);

reg_t op_op_get_reg(unsigned char uchar);

int op_op_arg_polarity(op_t op);
#endif //__OP_H__
