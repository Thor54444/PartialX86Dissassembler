/****************************
 * Andrew Guinn
 * Project 1
 * instr.c
 ****************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instr.h"

#define LABEL_MAX_LEN 17
#define JMP_LABEL_FMT "offset_%08" PRIx32 "h"

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

    if (op_need_modrm(op) == OP_BOOL_YES) {
      instr->op = op_op_modrm_to_op(op, instr->reg);

      if (instr->op == err_op) {
	goto ret;
      }

      instr->reg = err_reg; //Reg was used to signal the OP, effectively does not exist here
    }

    res = 1;
  } else {
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

  if (sib_need_sib(op, mode, instr->rm)) {
    if (max == 0) {
      return -1;
    }
    
    instr->base = sib_get_base(*buf);
    instr->index = sib_get_index(*buf);
    instr->scale = sib_get_scale(*buf);

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
    return -1; // Error if a displacement should be present but one isn't
  }

  if(sz == 4) {
    if (max < 4) {
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
    return -1; // Error if an immediate should be present but none is
  }
  
  if (sz == 4) {
    if (max < 4) {
      return -1;
    }

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

int parse_jmp(instr_t *instr) {
  uint64_t mask = 0x00000000FFFFFFFF;
  uint64_t tmp = instr->addr + instr->size + instr->disp;
  char *label;
  int amt;

  // Only the instructions that inheritly need a
  // displacement have jmp labels
  if (op_has_displacement(instr->op) == OP_BOOL_YES) {
    instr->dst_addr = (uint32_t)(tmp & mask);

    label = calloc(LABEL_MAX_LEN, 1);
    if (label == NULL) {
      return -1;
    }

    amt = snprintf(label, LABEL_MAX_LEN, JMP_LABEL_FMT, instr->dst_addr);
    if (amt != LABEL_MAX_LEN - 1) {
      return -1;
    }
  } else {
    return 0;
  }

  instr->label = label;
  return 0;
}

void instr_clear_instr(instr_t *instr) {
  instr->addr = 0;
  instr->size = 0;
  instr->instr_bytes = NULL;
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
  instr->dst_addr = UINT32_MAX;
}

void instr_free_instr(void *instr_ptr) {
  if (instr_ptr == NULL) {
    return;
  }

  instr_t *instr = (instr_t *)instr_ptr;
  
  free(instr->label);
  free(instr->instr_bytes);
  free(instr);
}

int instr_parse_instr(unsigned char *buf, int len, uint32_t start_addr, instr_t **instr) {
  instr_t inner_instr;
  int cur = 0;
  int moved = 0;

  if (instr == NULL || buf == NULL) {
    return INSTR_BOOL_NO;
  }

  instr_clear_instr(&inner_instr);

  inner_instr.addr = start_addr;

  cur += parse_prefix(buf, len, &inner_instr);

  moved = parse_op(buf + cur, len - cur, &inner_instr);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;
  
  moved = parse_modrm(buf + cur, len - cur, inner_instr.op, &inner_instr);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;

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

  moved = parse_displacement(buf + cur, len - cur, &inner_instr);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;

  moved = parse_immediate(buf + cur, len - cur, &inner_instr);
  if (moved < 0) {
    return INSTR_BOOL_NO;
  }
  cur += moved;

  if (len - cur == 0) {
    inner_instr.size = len;
    moved = parse_jmp(&inner_instr);
    if(moved < 0) {
      return INSTR_BOOL_NO;
    }

    inner_instr.instr_bytes = malloc(len);
    if (inner_instr.instr_bytes == NULL) {
      free(inner_instr.label);
      return INSTR_BOOL_NO;
    }

    memcpy(inner_instr.instr_bytes, buf, len);
    
    *instr = calloc(sizeof(instr_t), 1);
    if(*instr == NULL) {
      free(inner_instr.label);
      free(inner_instr.instr_bytes);
      return INSTR_BOOL_NO;
    }

    memcpy(*instr, &inner_instr, sizeof(instr_t));
  }
  
  return len - cur;
}

#define REG_ALONE_FORMAT "%s"
#define REG_ACCESS_FORMAT "dword [ %s ]"
#define REG_BYTE_FORMAT "[%s+0x%02"PRIu8"]"
#define REG_DWORD_FORMAT "[%s+0x%08"PRIx32"]"
#define REG_SPECIAL_FORMAT "[0x%08"PRIx32"]"
#define REG_SIB_BASE_NO_SCALE_FORMAT "[%s+%s+0x%08"PRIx32"]"
#define REG_SIB_BASE_FORMAT "[%s+%s]"
#define REG_SCALE_INDEX_FORMAT "[%s*%d+%s]"
#define REG_FULL_SIB_FORMAT "[%s*%d+%s+0x%08"PRIx32"]"

#define MAX_RM_LINE_LENGTH 50

int rm_to_str(instr_t reg, char **str_ptr) {
  int res = 0;
  char *rm = NULL;
  char *base = NULL;
  char *index = NULL;
  char *buf = NULL;
  char tmp[MAX_RM_LINE_LENGTH] = {0};

  if (str_ptr == NULL) {
    goto err;
  }

  if (reg.rm == err_reg) {
    return 0;
  }

  rm = register_reg_to_str(reg.rm);
  if (rm == NULL) {
    goto err;
  }

  if (modrm_is_special_displacement_mod(reg.mode, reg.rm)) {
    res = sprintf(tmp, REG_SPECIAL_FORMAT, reg.disp);

    goto str_check;
  }
  
  if (sib_need_sib(reg.op, reg.mode, reg.rm)) {
    base = register_reg_to_str(reg.base);
    if (base == NULL) {
      goto err_with_rm;
    }

    if (reg.index == esp) {
      res = sprintf(tmp, REG_DWORD_FORMAT, base, reg.disp);
    } else {
      index = register_reg_to_str(reg.index);
      if (index == NULL) {
	free(base);
	goto err_with_rm;
      }
      
      if (reg.scale > 1) {
	if (reg.disp != UINT32_MAX) {
	  res = sprintf(tmp, REG_FULL_SIB_FORMAT, index, reg.scale, base, reg.disp);
	}  else {
	  res = sprintf(tmp, REG_SCALE_INDEX_FORMAT, index, reg.scale, base);
	}
      } else {
	if (reg.disp != UINT32_MAX) {
	  res = sprintf(tmp, REG_SIB_BASE_NO_SCALE_FORMAT, index, base, reg.disp);
	} else {
	  res = sprintf(tmp, REG_SIB_BASE_FORMAT, index, base);
	}
      }
      
      free(base);
    }

    goto str_check;
  }
 

  if (reg.mode == mem_access) {
    res = sprintf(tmp, REG_ACCESS_FORMAT, rm);
  } else if (reg.mode == byte_disp) {
    //res = sprintf(tmp, REG_BYTE_FORMAT, rm, (uint8_t)reg.disp);
    //The byte format is printed like the dword for some reason
    res = sprintf(tmp, REG_DWORD_FORMAT, rm, reg.disp);
  } else if (reg.mode == dword_disp) {
    res = sprintf(tmp, REG_DWORD_FORMAT, rm, reg.disp);
  } else if (reg.mode == direct) {
    res = sprintf(tmp, REG_ALONE_FORMAT, rm);
  } else {
    goto err_with_rm;
  }

 str_check:
  if (res < 0) {
    goto err_with_rm;
  }

  buf = calloc(strlen(tmp) + 1, 1);
  if (buf == NULL) {
    goto err_with_rm;
  }

  memcpy(buf, tmp, strlen(tmp));
  free(rm);

  *str_ptr = buf;

  return 0;

 err_with_rm:
  free(rm);
 err:
  return -1;
}

#define INSTR_BYTES_SEC_MAX 18
#define MAX_INSTR_LINE_SIZE 256
#define DB_FORMAT_STR "db %hx"
#define EAX_ARG_STR "eax, "
#define EAX_LAST_ARG_STR ", eax"

char *instr_reg_to_str(instr_t reg) {
  int i;
  int res;
  int loc = 0;
  char *addr = NULL;
  char *prefix = NULL;
  char *op = NULL;
  char *rm = NULL;
  char *re = NULL;
  char *imm = NULL;
  char *buf = NULL;
  char tmp[MAX_INSTR_LINE_SIZE] = {0};

  addr = calloc(8 + 1, 1);
  if (addr == NULL) {
    goto err;
  }

  res = sprintf(addr, "%08"PRIx32"", reg.addr);
  if (res < 0) {
    goto err_with_addr;
  }

  printf("ADDRESS: %s\n", addr);
  
  if (reg.op == err_op) {
    // db: byte
    goto make_instr;
  }

  if (reg.prefix != err_prefix) {
    prefix = prefix_to_str(reg.prefix);
    if (prefix == NULL) {
      goto err_with_addr;
    }
  }

  op = op_to_str(reg.op);
  if (op == NULL) {
    goto err_with_prefix;
  }

  printf("HERE IS MY OP: %s\n", op);

  res = rm_to_str(reg, &rm);
  if (res < 0) {
    goto err_with_op;
  }

  printf("HERE IS MY RM %s\n", rm);
  
  if (reg.reg != err_reg) {
    re = register_reg_to_str(reg.reg);
    if (re == NULL) {
      goto err_with_rm;
    }
  }

  if (reg.imm != UINT32_MAX) {
    imm = calloc(8 + 2 + 1, 1);
    if (imm == NULL) {
      goto err_with_reg;
    }

    //Instructions say to only handle immediates as 32bit or 16
    if (reg.op == retni || reg.op == retfi) {
      res = sprintf(imm, "0x%04"PRIx16"", (uint16_t)reg.imm);
    } else if (false && reg.op == out) {
      res = sprintf(imm, "0x%02"PRIx8"", (uint8_t)reg.imm);
    } else {
      res = sprintf(imm, "0x%08"PRIx32"", reg.imm);
    }

    if (res < 0) {
      goto err_with_imm;
    }
  }

  printf("After immediate\n");

 make_instr:  
  memcpy(tmp, addr, 8);
  loc += 8;

  tmp[loc++] = ':';

  memset(tmp + loc, ' ', 2);
  loc += 2;

  for(i = 0; i < reg.size; i++) {
    res = sprintf(tmp + loc, "%02X", reg.instr_bytes[i]);
    loc += 2;

    if (res < 0) {
      goto err_with_addr;
    }
  }

  memset(tmp + loc, ' ', INSTR_BYTES_SEC_MAX - (reg.size * 2));
  loc = loc + (INSTR_BYTES_SEC_MAX - (reg.size * 2));

  if (prefix != NULL) {
    memcpy(tmp + loc, prefix, strlen(prefix));
    loc += strlen(prefix);
    tmp[loc++] = ' ';
  }
  
  if (op == NULL) {
    res = sprintf(tmp + loc, DB_FORMAT_STR, reg.instr_bytes[0]);
    loc += 5;
    
    if (res < 0) {
      // Only got here with db condition
      goto err_with_addr;
    }

    goto make_return;
  } else {
    memcpy(tmp + loc, op, strlen(op));
    loc += strlen(op);
    tmp[loc++] = ' ';
  }

  printf("LABEL: %s\n", reg.label);
  
  if (reg.label != NULL) {
    memcpy(tmp + loc, reg.label, strlen(reg.label));
    loc += strlen(reg.label);
    printf("Skipping...\n");
    goto make_return;
  }

  res = op_op_arg_polarity(reg.op);

  printf("After pol func %d\n", res);
  if (res == OP_ARG_POL_RIGHT) {
    printf("RIGHT\n");
    memcpy(tmp + loc, re, strlen(re));
    loc += strlen(re);
    tmp[loc++] = ',';
    //tmp[loc++] = ' ';

    memcpy(tmp + loc, rm, strlen(rm));
    loc += strlen(rm);

    if (imm != NULL) {
      tmp[loc++] = ',';
      //tmp[loc++] = ' ';

      memcpy(tmp + loc, imm, strlen(imm));
      loc += strlen(imm);
    }
  } else if (res == OP_ARG_POL_LEFT) {
    printf("LEFT\n");
    memcpy(tmp + loc, rm, strlen(rm));
    loc += strlen(rm);

    tmp[loc++] = ',';
    //tmp[loc++] = ' ';

    // Either the immediate or reg field or 1 will be here
    if (re != NULL) {
      memcpy(tmp + loc, re, strlen(re));
      loc += strlen(re);
    } else if (imm != NULL){
      memcpy(tmp + loc, imm, strlen(imm));
      loc += strlen(imm);
    } else {
      //sal, sar, shr
      tmp[loc++] = '1';
    }
  } else if (res == OP_ARG_POL_ONLY) {
    printf("ONLY\n");
    memcpy(tmp + loc, rm, strlen(rm));
    loc += strlen(rm);

    if (imm != NULL) {
      tmp[loc++] = ',';
      //tmp[loc++] = ' ';
 
      memcpy(tmp + loc, imm, strlen(imm));
      loc += strlen(imm);
    }
  } else if (res == OP_ARG_POL_NONE) {
    printf("Here?\n");
    /* Instructions where the register is hardcoded eax */
    if (reg.op == addd || reg.op == andd || reg.op == cmpd || reg.op == movd ||
	reg.op == ord || reg.op == sbbd || reg.op == subd || reg.op == testd ||
	reg.op == xord) {
      memcpy(tmp + loc, EAX_ARG_STR, strlen(EAX_ARG_STR));
      loc += strlen(EAX_ARG_STR);

      memcpy(tmp + loc, imm, strlen(imm));
      loc += strlen(imm);
    } else if (reg.op == decr || reg.op == incr || reg.op == popr || reg.op == pushr) {
      memcpy(tmp + loc, re, strlen(re));
      loc += strlen(re);
    } else if (imm != NULL) {
      printf("Here?\n");
      memcpy(tmp + loc, imm, strlen(imm));
      loc += strlen(imm);
      
      if (reg.op == out) {
	memcpy(tmp + loc, EAX_LAST_ARG_STR, strlen(EAX_LAST_ARG_STR));
	loc += strlen(EAX_LAST_ARG_STR);
      }
    }
  } else {
    printf("Not a match???");
    goto err_with_imm;
  }

 make_return:
  printf("TMP CONTAINS: %s\n", tmp);
  buf = calloc(strlen(tmp) + 1, 1);
  if (buf != NULL) {
    memcpy(buf, tmp, strlen(tmp));
  }
 err_with_imm:
  free(imm);
 err_with_reg:
  free(re);
 err_with_rm:
  free(rm);
 err_with_op:
  free(op);
 err_with_prefix:
  free(prefix);
 err_with_addr:
  free(addr);
 err:
  return buf;
}

/*int main() {
  int res;
  char *tmp, *instr_str;
  unsigned char instruction[] = {0x5D};
  instr_t *result;

  res = instr_parse_instr(instruction, sizeof(instruction), 0, &result);

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

  if (result->label != NULL) {
    printf("LABEL: %s\n", result->label);
  }

  instr_str = instr_reg_to_str(*result);
  printf("INSTRUCTION LINE: \n%s\n", instr_str);
  free(instr_str);
  
  instr_free_instr(&result);
  
  return 0;
  }*/

