/****************************
 * Andrew Guinn
 * Project 1
 * label.h
 ****************************/
#ifndef __LABEL_H__
#define __LABEL_H__

#include <stdint.h>

typedef struct label_t {
  uint32_t addr;
  char *label;
} label_t;

label_t *label_make_label(uint32_t addr, char *label);

void label_free_label(void *label); 

#endif //__LABEL_H__
