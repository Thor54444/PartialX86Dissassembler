/****************************
 * Andrew Guinn
 * Project 1
 * instr.c
 ****************************/
#include "instr.h"

int parse_prefix(unsigned char *buf, int max, prefix_t *prefix) {
  if (prefix == NULL || buf == NULL || max <= 0) {
    return 0;
  }

  *prefix = prefix_uchar_to_prefix(*buf);

  if (prefix == repne) {
    return 1;
  } else {
    return 0;
  }
}

int parse_op(unsigned char *buf, unsigned max, op_t *op) {
  op_t tmp;

  if (buf == NULL || max <= 0 || op == NULL) {
    return -1;
  }

  tmp = op_uchar_to_op(*buf);
  if (tmp == err_op) {
    return -1;
  }

  // op_need_second_byte returns either yes or no
  if (op_need_second_byte(*buf) == OP_BOOL_YES) {
    if (max < 2) {
      return -1; // Not enough bytes given to parse
    }

    *op = op_uchar_uchar_to_op(*buf, *(buf + 1));
    return 2;
  } else {
    *op = tmp;
    return 1;
  }
  
}

int parse_modrm(unsigned char *buf, int max, op_t op, mod_t *mod,
		reg_t *reg, reg_t *rm, op_t *final_op) {
  mod_t tmp_mod;
  reg_t tmp_reg;
  reg_t tmp_rm;
  int res = -1;

  if (buf == NULL || max <= 0 || op == err_op || mod == NULL ||
      reg == NULL || rm == NULL || final_op == NULL) {
    goto ret;
  }

  if(op_has_modrm(op) == OP_BOOL_YES ||
     op_has_modrm(op) == OP_BOOL_MAYBE) {

    tmp_mod = modrm_get_mod(*buf);
    tmp_reg = modrm_get_reg(*buf);
    tmp_rm = modrm_get_rm(*buf);

    if (op_need_modrm(op)) {
      *final_op = op_op_modrm_to_op(op, tmp_reg);

      if (*final_op == err_op) {
	goto ret;
      }
    } else {
      *final_op = op;
    }

    res = 1;
  } else {
    tmp_mod = err_reg;
    tmp_mod = err_mod;
    tmp_rm = err_reg;
    res = 0;
  }

  *mod = tmp_mod;
  *reg = tmp_reg;
  *rm = tmp_rm;
  
 ret:
  return res;
}

int parse_sib(unsigned char *buf, int max, op_t op, mod_t mode,
	      int *scale, reg_t *index, reg_t *base) {
  if (buf == NULL || max <= 0 || scale == NULL || index == NULL ||
      base == NULL) {
    return -1;
  }

  if (sib_need_sib(op, mode)) {
    *base = sib_get_base(*buf);
    *index = sib_get_index(*buf);
    *scale = sib_get_scale(*buf);

    return 1;
  } else {
    return 0;
  }
}

int parse_displacement(unsigned char *buf, int max, instr_t instr) {
  int sz = 0;

  if (buf == NULL || max <= 0) {
    return -1;
  }

  sz = modrm_get_disp_size(instr.mode, instr.rm);

  if(sz == 4) {
    if (max < 4) {
      return -1;
    }

    memcpy(&instr.disp_32, buf, 4);
  } else if(sz == 1) {
    memcpy(&instr.disp_8, buf, 1);
  }

  return sz;
}

int parse_immediate(unsigned char *buf, int max, instr_t instr) {
  int sz = 0;

  if (buf == NULL || max <= 0) {
    return -1;
  }

  sz = op_get_immediate_size(instr.op);

  if (sz == 4) {
    if (max < 4) {
      return -1;
    }

    memcpy(&instr.imm_32, buf, 4);
  } else if (sz == 2) {
    if (max < 2) {
      return -1;
    }

    memcpy(&instr.imm_16, buf, 2);
  } else if (sz == 1) {
    memcpy(&instr.imm_8, buf, 1);
  }

  return sz;
}

int instr_parse_instr(unsigned char *buf, int len, instr_t *instr) {
  instr_t inner_instr = {0};
  int cur = 0;
  int moved = 0;
  op_t op_tmp;

  if (instr == NULL || buf == NULL) {
    return INSTR_BOOL_NO;
  }

  cur += parse_prefix(buf, len, &inner_instr.prefix);
  moved = parse_op(buf + cur, len - cur, &op_tmp);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;
  
  moved = parse_modrm(buf + cur, len - cur, op_tmp, &inner_instr.mode,
		      &inner_instr.reg, &inner_instr.rm, &inner_instr.op);

  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved; 

  moved = parse_sib(buf + cur, len - cur, inner_instr.op, inner_instr.mode,
		    &inner_instr.scale, &inner_instr.index,
		    &inner_instr.base);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;

  moved = parse_displacement(buf + cur, len - cur, inner_instr);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;

  moved = parse_immediate(buf + cur, len - cur, inner_instr);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;
  
  return cur;
}


int main() {
  return 0;
}

