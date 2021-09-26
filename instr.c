/****************************
 * Andrew Guinn
 * Project 1
 * instr.c
 ****************************/
#include <stdio.h>
#include <stdlib.h>

#include "instr.h"

int parse_prefix(unsigned char *buf, int max, instr_t *instr) {
  if (buf == NULL || max <= 0 || instr == NULL) {
    return 0;
  }

  instr->prefix = prefix_uchar_to_prefix(*buf);

  if (instr->prefix == repne) {
    return 1;
  } else {
    return 0;
  }
}

int parse_op(unsigned char *buf, int max, instr_t *instr) {
  if (buf == NULL || max <= 0 || instr == NULL) {
    return -1;
  }

  instr->op = op_uchar_to_op(*buf);
  if (instr->op == err_op) {
    return -1;
  }

  if(op_op_has_reg(instr->op) == OP_BOOL_YES) {
    instr->reg = op_op_get_reg(*buf);

    return 1;
  } else if (op_need_second_byte(*buf) == OP_BOOL_YES) {
    if (max < 2) {
      return -1; // Not enough bytes given to parse
    }

    instr->op = op_uchar_uchar_to_op(*buf, *(buf + 1));
    return 2;
  } else {
    return 1;
  }
  
}

int parse_modrm(unsigned char *buf, int max, op_t op, instr_t *instr) {
  int res = -1;

  if (buf == NULL || max < 0 || op == err_op || instr == NULL) {
    goto ret;
  }

  printf("Parsing MODRM byte %hx\n", *buf);

  if(op_has_modrm(op) == OP_BOOL_YES ||
     (op_has_modrm(op) == OP_BOOL_MAYBE &&
      op_need_modrm(op) == OP_BOOL_YES)) {

    if (max == 0) {
      return -1;
    }
    
    /* Either you know for sure you need an modrm
     * or you use a modrm to identify the OP */
    instr->mode = modrm_get_mod(*buf);
    instr->reg = modrm_get_reg(*buf);
    instr->rm = modrm_get_rm(*buf);

    printf("mod: %d reg: %d rm: %d\n", instr->mode, instr->reg, instr->rm);

    if (op_need_modrm(op) == OP_BOOL_YES) {
      char *tmp;
      tmp = op_to_str(op);
      printf("OP BEFORE RESOLUTON: %s\n", tmp);
      free(tmp);
      instr->op = op_op_modrm_to_op(op, instr->reg);
      tmp = op_to_str(instr->op);
      printf("AFTER RESOLUTION: %s\n", tmp);
      free(tmp);
      if (instr->op == err_op) {
	printf("Parse error\n");
	goto ret;
      }

      instr->reg = err_reg; //Reg was used to signal the OP, effectively does not exist here
    }

    res = 1;
  } else {
    printf("No modrm\n");
    res = 0;
  }

 ret:
  return res;
}

int parse_sib(unsigned char *buf, int max, op_t op, mod_t mode,
	      instr_t *instr) {
  if (buf == NULL || max < 0 || op == err_op || instr == NULL) {
    return -1;
  }

  printf("Past SIB check\n");

  if (sib_need_sib(op, mode, instr->rm)) {
    if (max == 0) {
      printf("Max too high\n");
      return -1;
    }
    
    instr->base = sib_get_base(*buf);
    instr->index = sib_get_index(*buf);
    instr->scale = sib_get_scale(*buf);

    /* Cannot scale esp */
    if (instr->base == esp && instr->index != 1) {
      printf("Scale ESP?\n");
      return -1;
    }

    printf("I return\n");
    return 1;
  } else {
    return 0;
  }
}

int parse_displacement(unsigned char *buf, int max, instr_t *instr) {
  int8_t b_field;
  int sz = 0;

  if (buf == NULL || max < 0) {
    return -1;
  }

  if (modrm_has_displacement(instr->mode, instr->rm)) {
    sz = modrm_get_displacement_size(instr->mode, instr->rm);
  }
  
  if (op_has_displacement(instr->op) == OP_BOOL_YES) {
    sz = op_get_displacement_size(instr->op);
  }

  if (sz == 0 && (op_has_displacement(instr->op) == OP_BOOL_YES ||
		  modrm_has_displacement(instr->mode, instr->rm))) {
    printf("Needs displacement yet no size %d %d\n", instr->mode, instr->rm);
    return -1; // Error if a displacement should be present but one isn't
  }

  if(sz == 4) {
    if (max < 4) {
      printf("Do not have 4 bytes to make displacement out of\n");
      return -1;
    }

    memcpy(&instr->disp, buf, 4);
  } else if(sz == 1) {
    memcpy(&b_field, buf, 1);
    instr->disp = b_field;
  }

  return sz;
}

int parse_immediate(unsigned char *buf, int max, instr_t *instr) {
  int8_t b_field;
  int16_t tb_field;
  int sz = 0;

  if (buf == NULL || max < 0) {
    return -1;
  }

  sz = op_get_immediate_size(instr->op);

  if (sz == 0 && op_has_immediate(instr->op) == OP_BOOL_YES) {
    printf("I need an immediate?\n");
    return -1; // Error if an immediate should be present but none is
  }
  
  if (sz == 4) {
    if (max < 4) {
      return -1;
    }

    printf("I am copying the 32 bit immediate %"PRIu32"\n", *((uint32_t *)buf));

    memcpy(&instr->imm, buf, 4);
  } else if (sz == 2) {
    if (max < 2) {
      return -1;
    }

    memcpy(&tb_field, buf, 2);
    instr->imm = tb_field;
  } else if (sz == 1) {
    memcpy(&b_field, buf, 1);
    instr->imm = b_field;
  }

  return sz;
}

void clear_instr(instr_t *instr) {
  instr->addr = 0;
  instr->prefix = err_prefix;
  instr->op = err_op;
  instr->mode = err_mod;
  instr->reg = err_reg;
  instr->rm = err_reg;
  instr->scale = -1;
  instr->index = err_reg;
  instr->base = err_reg;
  memset(&instr->imm, 0xFF, 4);
  memset(&instr->disp, 0xFF, 4);
  instr->label = NULL;
}

int instr_parse_instr(unsigned char *buf, int len, instr_t **instr) {
  instr_t inner_instr;
  int cur = 0;
  int moved = 0;
  char *tmp;

  clear_instr(&inner_instr);

  if (instr == NULL || buf == NULL) {
    return INSTR_BOOL_NO;
  }

  cur += parse_prefix(buf, len, &inner_instr);

  printf("Parse prefix: %d bytes\n", cur);
  
  moved = parse_op(buf + cur, len - cur, &inner_instr);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;
  tmp = op_to_str(inner_instr.op);
  printf("Parse op %s: %d bytes\n", tmp, cur);
  free(tmp);
  
  moved = parse_modrm(buf + cur, len - cur, inner_instr.op, &inner_instr);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;

  tmp = op_to_str(inner_instr.op);
  printf("Parse modrm %s: %d bytes\n", tmp, cur);
  free(tmp);

  if ((inner_instr.op == clflush || inner_instr.op == lea)
      && inner_instr.mode == direct) {
    return INSTR_BOOL_NO; //Specific case written into the spec
  }

  moved = parse_sib(buf + cur, len - cur, inner_instr.op, inner_instr.mode,
		    &inner_instr);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;
  printf("Parse sib: %d bytes\n", cur);

  moved = parse_displacement(buf + cur, len - cur, &inner_instr);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;
  printf("Parse disp: %d bytes\n", cur);

  moved = parse_immediate(buf + cur, len - cur, &inner_instr);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;
  printf("Parse imm: %d bytes\n", cur);

  *instr = calloc(1, sizeof(instr_t));
  if(*instr == NULL) {
    return INSTR_BOOL_NO;
  }

  memcpy(*instr, &inner_instr, sizeof(instr_t));
  
  return len - cur;
}



int main() {
  int res;
  char *tmp;
  unsigned char instruction[] = {0x8B, 0x55, 0x08};
  instr_t *result;

  res = instr_parse_instr(instruction, sizeof(instruction), &result);

  if(res < 0) {
    printf("Instruction parsing error\n");
    return -1;
  } else if (res > 0) {
    printf("More bytes remaining: %d\n", res);
    return 1;
  } else {
    printf("It parsed!\n");
  }

  printf("PRINTING INSTR INFO: \n");
  tmp = op_to_str(result->op);
  printf("Mneumonic CODE: %s\n", tmp);
  free(tmp);

  printf("Mode: %d\n", result->mode);

  tmp = register_reg_to_str(result->rm);
  printf("RM: %d %s\n", result->rm, tmp);
  free(tmp);
  
  tmp = register_reg_to_str(result->reg);
  printf("Reg: %d %s\n", result->reg, tmp);
  free(tmp);

  tmp = register_reg_to_str(result->base);
  printf("BASE: %d %s\n", result->base, tmp);
  free(tmp);

  tmp = register_reg_to_str(result->index);
  printf("INDEX: %d %s\n", result->index, tmp);
  free(tmp);

  printf("Scale: %d\n", result->scale);
  
  printf("Displacement %"PRId32"\n", result->disp);
  printf("Immediate: %"PRId32"\n", result->imm);
  
  free(result);
  
  return 0;
}

