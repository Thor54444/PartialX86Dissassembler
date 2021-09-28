/****************************
 * Andrew Guinn
 * Project 1
 * main.c
 ****************************/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "label.h"
#include "list.h"
#include "lin.h"
#include "instr.h"

void main_print_output(ele_t *instr_list, ele_t *label_list) {
  char *buf;
  instr_t *instr;
  label_t *label;
  ele_t* cur_instr = instr_list;
  ele_t* cur_label;

  if (instr_list == NULL) {
    return;
  }

  do {
    instr = (instr_t *)cur_instr->value;

    if (label_list != NULL && label_list->value != NULL) {
      cur_label = label_list;
      do {
	label = (label_t *)cur_label->value;

	if (instr->addr == label->addr) {
	  printf("%s:\n", label->label);
	  break;
	}

      } while((cur_label = list_get_next(cur_label)) != NULL);
    }

    buf = instr_reg_to_str(*instr);
    if (buf == NULL) {
      return;
    }

    printf("%s\n", buf);

    free(buf);

  } while((cur_instr = list_get_next(cur_instr)) != NULL);
}

int main(int argc, char *argv[]) {
  int res;
  int opt;
  ele_t *instrs;
  ele_t *labels;
  char *filename = NULL;

  while ((opt = getopt(argc, argv, "i:")) != -1) {
    switch (opt) {
    case 'i':
      filename = optarg;
      break;
    default:
      fprintf(stderr, "Usage: %s [-i]\n", argv[0]);
      return 1;
    }
  }

  if (filename == NULL) {
    fprintf(stderr, "Usage: %s [-i]\n", argv[0]);
    return 2;
  }
  
  res = lin_parse_instructions(filename, &instrs, &labels);
  if (res < 0) {
    return 3;
  }

  main_print_output(instrs, labels);
  
  list_free_list(instrs, instr_free_instr);
  list_free_list(labels, label_free_label);

  return 0;
}
