/****************************
 * Andrew Guinn
 * Project 1
 * lin.c
 ****************************/
#include <stdio.h>
#include <stdlib.h>

#include "lin.h"
#include "list.h"
#include "instr.h"
#include "label.h"

#define MAX_INSTR_LEN 13

instr_t *make_db_instr(uint32_t addr, unsigned char ch) {
  instr_t *instr;

  instr = malloc(sizeof(instr_t));
  if (instr == NULL) {
    return NULL;
  }

  instr_clear_instr(instr);

  instr->instr_bytes = malloc(1);
  if (instr->instr_bytes == NULL) {
    free(instr);
    return NULL;
  }

  instr->instr_bytes[0] = ch;
  instr->addr = addr;
  instr->size = 1;

  return instr;
}

int lin_parse_instructions(char *filename, ele_t **instrs, ele_t **labels) {
  FILE *fp;
  int i = 0;
  int res = 0;
  int of_in = 0;
  uint32_t addr = 0;
  ele_t *instr_list = NULL;
  ele_t *label_list = NULL;
  instr_t *tmp = NULL;
  label_t *label = NULL;
  unsigned char holding_buffer[MAX_INSTR_LEN] = {0};

  if (filename == NULL || instrs == NULL || labels == NULL) {
    return -1;
  }

  instr_list = calloc(1, sizeof(ele_t));
  if(instr_list == NULL) {
    return -1;
  }

  label_list = calloc(1, sizeof(ele_t));
  if(label_list == NULL) {
    return -1;
  }

  fp = fopen(filename, "rb");
  if (fp == NULL) {
    return -1;
  }

  //  printf("Starting loop!\n");

  while (fread(holding_buffer + of_in, 1, 1, fp) == 1) {
    res = instr_parse_instr(holding_buffer, of_in + 1, addr, &tmp);

    if (res < 0) {
      if (of_in + 1 >= MAX_INSTR_LEN) {
	printf("Gotta make db\n");
	tmp = make_db_instr(addr, holding_buffer[0]);
	if (tmp == NULL) {
	  printf("Failed to make db\n");
	  goto err;
	}

	//printf("Made DB\n");

	res = list_add_element(instr_list, tmp);
	if (res < 0) {
	  printf("Cannot add db to list\n");
	  goto err;
	}
	
	//I know this is gross
	//printf("Fseek?\n");
	res = fseek(fp, -1 * (MAX_INSTR_LEN - 1), SEEK_CUR);
	if (res < 0) {
	  printf("failed to seek\n");
	  goto err;
	}
	//printf("Fseek\n");

	of_in = 0;
	addr += 1;
      } else {
	//printf("doesn't make sense\n");
	of_in += 1;
      }
    } else {
      res = list_add_element(instr_list, tmp);
      addr += tmp->size;
      //printf("Made instr!\n");
      if (res < 0) {
	printf("Failed adding instr element\n");
	goto err;
      }

      if (tmp->dst_addr != UINT32_MAX) {
	printf("Making label for %s %"PRIu32"\n", tmp->label, tmp->dst_addr);
	label = label_make_label(tmp->dst_addr, tmp->label);
	if (label == NULL) {
	  printf("Can't make label\n");
	  goto err;
	}

	res = list_add_element(label_list, label);
	if (res < 0) {
	  printf("Failed adding element\n");
	  goto err;
	}
      }

      of_in = 0;
    }
  }

  printf("PAST LOOP %02X\n", *(holding_buffer + of_in));

  while(i <= of_in) {
    res = instr_parse_instr(holding_buffer, i + 1, addr, &tmp);

    if (res < 0) {
	tmp = make_db_instr(addr, holding_buffer[i]);
	if (tmp == NULL) {
	  goto err;
	}

	res = list_add_element(instr_list, tmp);
	if (res < 0) {
	  goto err;
	}

	i+=1;
	addr += 1;
    } else {
      res = list_add_element(instr_list, tmp);
      if (res < 0) {
	goto err;
      }

      if (tmp->dst_addr != UINT32_MAX) {
	printf("Making label for %s %"PRIu32"\n", tmp->label, tmp->dst_addr);
	label = label_make_label(tmp->dst_addr, tmp->label);
	if (label == NULL) {
	  goto err;
	}

	res = list_add_element(label_list, label);
	if (res < 0) {
	  goto err;
	}
      }

      break;
    }
  }

  //printf("Clean up!\n");

  fclose(fp);
  if (instr_list->value != NULL) {
    *instrs = instr_list;
  } else {
    *instrs = NULL;
    free(instr_list);
  }

  if (label_list->value != NULL) {
    *labels = label_list;
  } else {
    *labels = NULL;
    free(label_list);
  }
  
  return 0;
  
 err:
  fclose(fp);
  list_free_list(instr_list, instr_free_instr);
  list_free_list(label_list, label_free_label);
  return -1;
}
