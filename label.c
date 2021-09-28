/****************************
 * Andrew Guinn
 * Project 1
 * label.c
 ****************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "label.h"

#define MAX_LABEL_LENGTH 16

label_t *label_make_label(uint32_t addr, char *label) {
  label_t *label_tmp;

  label_tmp = calloc(sizeof(label_t), 1);
  if (label_tmp) {
    label_tmp->addr = addr;
    label_tmp->label = calloc(MAX_LABEL_LENGTH + 1, 1);
    if (label_tmp->label != NULL) {
      memcpy(label_tmp->label, label, MAX_LABEL_LENGTH);
    } else {
      free(label_tmp);
      label_tmp = NULL;
    }
  }

  return label_tmp;
}

void label_free_label(void *label) {
  label_t *val = (label_t *)label;
  
  free(val->label);
  free(val);
}
